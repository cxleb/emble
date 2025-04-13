#include <vector>
#include <stdio.h>

#include "tree_sitter/tree-sitter-emble.h"
#include "tree_sitter/api.h"

#include "ir.h"
#include "irgen.h"
#include "codegen.h"

std::vector<char> slerp(const char* path) {
    auto file = fopen(path, "rb");
    
    fseek(file, 0, SEEK_END);
    auto size = ftell(file);
    rewind(file);

    std::vector<char> contents(size);

    auto read = fread(&contents[0], 1, size, file);
    
    return std::move(contents);
}

int main(int argc, const char** argv) {
    printf("starting\n");
    auto language = tree_sitter_emble();
    auto parser = ts_parser_new();
    ts_parser_set_language(parser, language);

    auto path = "../demo.emble";
    printf("reading file %s\n", path);
    auto file = slerp(path);

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
