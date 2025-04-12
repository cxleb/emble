#include <vector>
#include <stdio.h>

#include "tree_sitter/tree-sitter-emble.h"
#include "tree_sitter/api.h"

#include "ir.h"

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

    auto path = "..\\demo.emble";
    printf("reading file %s\n", path);
    auto file = slerp(path);

    printf("parsing\n");
    auto tree = ts_parser_parse_string(parser, nullptr, file.data(), file.size());
    if (tree == nullptr) {
        printf("Parser failed\n");
        return 1;
    }

    auto root = ts_tree_root_node(tree);

    printf("root name: %s\n", ts_node_type(root));

    ts_tree_delete(tree);
    ts_parser_delete(parser);
    
    printf("done\n");

    Inst inst;
    inst = InstAdd {
        .lhs = Ref(10),
        .rhs = Ref(10),
    };

    //auto inst = Inst{
    //    .ty = InstType::Add,
    //    .bin_op = {
    //        .lhs = Ref(10),
    //        .rhs = Ref(20),
    //    },
    //};
    

    return 0;
}
