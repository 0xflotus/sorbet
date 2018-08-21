#include "ast/treemap/treemap.h"
#include "common/Timer.h"
#include "core/Errors.h"
#include "core/Files.h"
#include "core/Names/resolver.h"
#include "core/Unfreeze.h"
#include "core/errors/internal.h"
#include "core/errors/namer.h"
#include "core/errors/resolver.h"
#include "core/lsp/DefLocSaver.h"
#include "core/lsp/QueryResponse.h"
#include "lsp.h"
#include "main/pipeline/pipeline.h"
#include "namer/namer.h"
#include "resolver/resolver.h"
#include "spdlog/fmt/ostr.h"
#include <unordered_set>

using namespace std;

namespace sorbet {
namespace realmain {
namespace lsp {

bool LSPLoop::isTestFile(const shared_ptr<core::File> &file) {
    if (file->path().find("/test/") != file->path().npos) {
        return true;
    } else if (file->path().find_first_of("test/") == 0) {
        return true;
    } else {
        return false;
    }
}

core::FileRef LSPLoop::addNewFile(const shared_ptr<core::File> &file) {
    core::FileRef fref;
    if (!file)
        return fref;
    fref = initialGS->findFileByPath(file->path());
    if (fref.exists()) {
        initialGS = core::GlobalState::replaceFile(move(initialGS), fref, move(file));
    } else {
        fref = initialGS->enterFile(move(file));
    }

    std::vector<std::string> emptyInputNames;
    auto t = pipeline::indexOne(opts, *initialGS, fref, kvstore, logger);
    int id = t->loc.file().id();
    if (id >= indexed.size()) {
        indexed.resize(id + 1);
    }
    indexed[id] = move(t);
    return fref;
}

vector<unique_ptr<ast::Expression>> incrementalResolve(core::GlobalState &gs, vector<unique_ptr<ast::Expression>> what,
                                                       const options::Options &opts,
                                                       shared_ptr<spdlog::logger> logger) {
    try {
        int i = 0;
        for (auto &tree : what) {
            auto file = tree->loc.file();
            try {
                unique_ptr<ast::Expression> ast;
                core::MutableContext ctx(gs, core::Symbols::root());
                logger->trace("Naming: {}", file.data(gs).path());
                core::ErrorRegion errs(gs, file);
                tree = sorbet::namer::Namer::run(ctx, move(tree));
                i++;
            } catch (SRubyException &) {
                if (auto e = gs.beginError(sorbet::core::Loc::none(file), core::errors::Internal::InternalError)) {
                    e.setHeader("Exception naming file: `{}` (backtrace is above)", file.data(gs).path());
                }
            }
        }

        core::MutableContext ctx(gs, core::Symbols::root());
        {
            Timer timeit(logger, "Incremental resolving");
            logger->trace("Resolving (incremental pass)...");
            core::ErrorRegion errs(gs, sorbet::core::FileRef());
            what = sorbet::resolver::Resolver::runTreePasses(ctx, move(what));
        }
    } catch (SRubyException &) {
        if (auto e = gs.beginError(sorbet::core::Loc::none(), sorbet::core::errors::Internal::InternalError)) {
            e.setHeader("Exception resolving (backtrace is above)");
        }
    }

    return what;
}

vector<unsigned int> LSPLoop::computeStateHashes(const vector<shared_ptr<core::File>> &files) {
    std::vector<unsigned int> res(files.size());
    shared_ptr<ConcurrentBoundedQueue<int>> fileq = make_shared<ConcurrentBoundedQueue<int>>(files.size());
    for (int i = 0; i < files.size(); i++) {
        auto copy = i;
        fileq->push(move(copy), 1);
    }

    logger->debug("Computing state hashes for {} files", files.size());

    res.resize(files.size());

    shared_ptr<BlockingBoundedQueue<vector<std::pair<int, unsigned int>>>> resultq =
        make_shared<BlockingBoundedQueue<vector<std::pair<int, unsigned int>>>>(files.size());
    workers.multiplexJob([fileq, resultq, files, logger = this->logger]() {
        vector<std::pair<int, unsigned int>> threadResult;
        int processedByThread = 0;
        int job;
        options::Options emptyOpts;
        emptyOpts.runLSP = true;

        {
            for (auto result = fileq->try_pop(job); !result.done(); result = fileq->try_pop(job)) {
                if (result.gotItem()) {
                    processedByThread++;

                    if (!files[job]) {
                        threadResult.emplace_back(make_pair(job, 0));
                        continue;
                    }
                    shared_ptr<core::GlobalState> lgs = make_shared<core::GlobalState>(
                        (std::make_shared<realmain::ConcurrentErrorQueue>(*logger, *logger)));
                    lgs->initEmpty();
                    lgs->silenceErrors = true;
                    core::UnfreezeFileTable fileTableAccess(*lgs);
                    core::UnfreezeSymbolTable symbolTable(*lgs);
                    core::UnfreezeNameTable nameTable(*lgs);
                    auto fref = lgs->enterFile(files[job]);
                    vector<unique_ptr<ast::Expression>> single;
                    unique_ptr<KeyValueStore> kvstore;
                    single.emplace_back(pipeline::indexOne(emptyOpts, *lgs, fref, kvstore, logger));
                    pipeline::resolve(*lgs, move(single), emptyOpts, logger, true);
                    threadResult.emplace_back(make_pair(job, lgs->hash()));
                }
            }
        }

        if (processedByThread > 0) {
            resultq->push(move(threadResult), processedByThread);
        }
    });

    {
        std::vector<std::pair<int, unsigned int>> threadResult;
        for (auto result = resultq->wait_pop_timed(threadResult, pipeline::PROGRESS_REFRESH_TIME_MILLIS);
             !result.done(); result = resultq->wait_pop_timed(threadResult, pipeline::PROGRESS_REFRESH_TIME_MILLIS)) {
            if (result.gotItem()) {
                for (auto &a : threadResult) {
                    res[a.first] = a.second;
                }
            }
        }
    }
    return res;
}

void LSPLoop::reIndexFromFileSystem() {
    indexed.clear();
    unordered_set<string> fileNamesDedup(opts.inputFileNames.begin(), opts.inputFileNames.end());
    for (int i = 1; i < initialGS->filesUsed(); i++) {
        core::FileRef f(i);
        if (f.data(*initialGS, true).sourceType == core::File::Type::Normal) {
            fileNamesDedup.insert((string)f.data(*initialGS, true).path());
        }
    }
    std::vector<string> fileNames(make_move_iterator(fileNamesDedup.begin()), make_move_iterator(fileNamesDedup.end()));
    std::vector<core::FileRef> emptyInputFiles;
    for (auto &t : pipeline::index(initialGS, fileNames, emptyInputFiles, opts, workers, kvstore, logger)) {
        int id = t->loc.file().id();
        if (id >= indexed.size()) {
            indexed.resize(id + 1);
        }
        indexed[id] = move(t);
    }
}

void LSPLoop::invalidateAllErrors() {
    updatedErrors.clear();
    for (auto &e : errorsAccumulated) {
        updatedErrors.push_back(e.first);
    }
    errorsAccumulated.clear();
}

void LSPLoop::invalidateErrorsFor(const vector<core::FileRef> &vec) {
    for (auto f : vec) {
        auto fnd = errorsAccumulated.find(f);
        if (fnd != errorsAccumulated.end()) {
            errorsAccumulated.erase(fnd);
            updatedErrors.push_back(f);
        }
    }
}

void LSPLoop::tryApplyDefLocSaver(unique_ptr<core::GlobalState> &finalGs,
                                  vector<unique_ptr<ast::Expression>> &indexedCopies) {
    if (finalGs->lspInfoQueryLoc == core::Loc::none()) {
        return;
    }
    for (auto &t : indexedCopies) {
        sorbet::lsp::DefLocSaver defLocSaver;
        core::Context ctx(*finalGs, core::Symbols::root());
        t = ast::TreeMap::apply(ctx, defLocSaver, move(t));
    }
}

void LSPLoop::runSlowPath(const std::vector<shared_ptr<core::File>>

                              &changedFiles) {
    logger->debug("Taking slow path");

    invalidateAllErrors();

    std::vector<core::FileRef> changedFileRefs;
    indexed.reserve(indexed.size() + changedFiles.size());
    for (auto &t : changedFiles) {
        if (!isTestFile(t)) {
            addNewFile(t);
        }
    }

    vector<unique_ptr<ast::Expression>> indexedCopies;
    for (const auto &tree : indexed) {
        if (tree) {
            indexedCopies.emplace_back(tree->deepCopy());
        }
    }

    finalGs = initialGS->deepCopy(true);
    tryApplyDefLocSaver(finalGs, indexedCopies);
    pipeline::typecheck(finalGs, pipeline::resolve(*finalGs, move(indexedCopies), opts, logger), opts, workers, logger);
}
void LSPLoop::tryFastPath(std::vector<shared_ptr<core::File>>

                              &changedFiles) {
    logger->debug("Trying to see if happy path is available after {} file changes", changedFiles.size());
    bool good = true;
    auto hashes = computeStateHashes(changedFiles);
    ENFORCE(changedFiles.size() == hashes.size());
    vector<core::FileRef> subset;
    int i = -1;
    for (auto &f : changedFiles) {
        ++i;
        if (!f || isTestFile(f)) {
            continue;
        }
        auto wasFiles = initialGS->filesUsed();
        auto fref = addNewFile(f);
        if (wasFiles != initialGS->filesUsed()) {
            logger->debug("Taking sad path because {} is a new file", changedFiles[i]->path());
            good = false;
            if (globalStateHashes.size() <= fref.id()) {
                globalStateHashes.resize(fref.id() + 1);
                globalStateHashes[fref.id()] = hashes[i];
            }
        } else {
            if (hashes[i] != globalStateHashes[fref.id()]) {
                logger->debug("Taking sad path because {} has changed definitions", changedFiles[i]->path());
                good = false;
                globalStateHashes[fref.id()] = hashes[i];
            }
            if (good) {
                finalGs = core::GlobalState::replaceFile(move(finalGs), fref, changedFiles[i]);
            }

            subset.emplace_back(fref);
        }
    }

    if (good) {
        invalidateErrorsFor(subset);
        logger->debug("Taking happy path");

        std::vector<std::unique_ptr<ast::Expression>> updatedIndexed;
        for (auto &f : subset) {
            auto t = pipeline::indexOne(opts, *finalGs, f, kvstore, logger);
            int id = t->loc.file().id();
            indexed[id] = move(t);
            updatedIndexed.emplace_back(indexed[id]->deepCopy());
        }

        tryApplyDefLocSaver(finalGs, updatedIndexed);
        pipeline::typecheck(finalGs, incrementalResolve(*finalGs, move(updatedIndexed), opts, logger), opts, workers,
                            logger);
    } else {
        runSlowPath(changedFiles);
    }
}
} // namespace lsp
} // namespace realmain
} // namespace sorbet