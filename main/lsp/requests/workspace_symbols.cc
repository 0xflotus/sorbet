#include "core/lsp/QueryResponse.h"
#include "main/lsp/lsp.h"

using namespace std;

namespace sorbet::realmain::lsp {

unique_ptr<SymbolInformation> LSPLoop::symbolRef2SymbolInformation(const core::GlobalState &gs,
                                                                   core::SymbolRef symRef) {
    auto sym = symRef.data(gs);
    if (!sym->loc().file().exists() || hideSymbol(gs, symRef)) {
        return nullptr;
    }

    auto result = make_unique<SymbolInformation>(sym->name.show(gs), symbolRef2SymbolKind(gs, symRef),
                                                 loc2Location(gs, sym->loc()));
    result->containerName = sym->owner.data(gs)->fullName(gs);
    return result;
}

unique_ptr<core::GlobalState> LSPLoop::handleWorkspaceSymbols(unique_ptr<core::GlobalState> gs, const MessageId &id,
                                                              const WorkspaceSymbolParams &params) {
    prodCategoryCounterInc("lsp.requests.processed", "workspace.symbols");

    vector<unique_ptr<JSONBaseType>> result;
    string_view searchString = params.query;

    auto finalGs = move(gs);
    for (u4 idx = 1; idx < finalGs->symbolsUsed(); idx++) {
        core::SymbolRef ref(finalGs.get(), idx);
        if (hasSimilarName(*finalGs, ref.data(*finalGs)->name, searchString)) {
            auto data = symbolRef2SymbolInformation(*finalGs, ref);
            if (data) {
                result.push_back(move(data));
            }
        }
    }
    sendResponse(id, result);
    return finalGs;
}
} // namespace sorbet::realmain::lsp