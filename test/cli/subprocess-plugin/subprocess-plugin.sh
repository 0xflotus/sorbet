#!/bin/bash

set -e

# Check that the subprocess is getting good arguments
echo ------ Arguments
main/sorbet --silence-dev-message --dsl-plugins test/cli/subprocess-plugin/echo_argv.yaml --print plugin-generated-code test/cli/subprocess-plugin/permute.rb
echo ------ Multi file generated code
main/sorbet --silence-dev-message --dsl-plugins test/cli/subprocess-plugin/multi_empty.yaml --print plugin-generated-code test/cli/subprocess-plugin/multi*.rb
echo ------ Multi file symbol table
main/sorbet --silence-dev-message --dsl-plugins test/cli/subprocess-plugin/multi.yaml --print symbol-table test/cli/subprocess-plugin/multi*.rb