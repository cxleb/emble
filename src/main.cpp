#include <optional>
#include <vector>
#include <stdio.h>

#include "tree_sitter/api.h"

#include "ast.h"
#include "ir.h"
#include "irgen.h"
#include "codegen.h"

extern "C" const TSLanguage *tree_sitter_emble(void);

std::optional<std::vector<char>> slerp(const std::string& path) {
    auto file = fopen(path.c_str(), "rb");
    if (!file) {
        return std::nullopt;
    }

    fseek(file, 0, SEEK_END);
    auto size = ftell(file);
    rewind(file);

    std::vector<char> contents(size);

    auto read = fread(&contents[0], 1, size, file);
    
    return std::move(contents);
}

int main(int argc, const char** argv) {
    auto language = tree_sitter_emble();
    auto parser = ts_parser_new();
    ts_parser_set_language(parser, language);
    
    if (argc == 1) {
        printf("Expected `emblec <root file>`\n");
        return 1;
    }
    
    printf("starting\n");
    
    auto path = argv[1];
    auto maybe_file = slerp(path);
    if (!maybe_file.has_value()) {
        printf("Could not find input file: %s\n", path);
        return 1;
    }
    auto file = *maybe_file;

    printf("parsing\n");
    auto tree = ts_parser_parse_string(parser, nullptr, file.data(), file.size());
    if (tree == nullptr) {
        printf("Parser failed\n");
        return 1;
    }

    printf("ir gen\n");
    auto module = ir_gen(tree, file);

    printf("done\n");

    print_module(module);

    codegen_init();
    codegen(module);

    ts_tree_delete(tree);
    ts_parser_delete(parser);

    return 0;
}
