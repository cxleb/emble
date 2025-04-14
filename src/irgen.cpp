#include "irgen.h"

TSNode ts_node_child_by_field_name(TSNode self, const std::string& name) {
    return ts_node_child_by_field_name(self, name.c_str(), name.length());
}

std::string ts_node_source(TSNode self, const std::vector<char>& source) {
    auto start = ts_node_start_byte(self);
    auto end = ts_node_end_byte(self);
    std::string content(end-start, ' ');
    for (uint32_t i = 0; i < (end-start); i++) {
        content[i] = source[i + start];
    }
    return content;
}

struct Generator {
    const std::vector<char>& source;
    ref<Module> module;
    TSTree* tree;

    Generator(TSTree* tree, const std::vector<char>& src) : source(src) {
        this->tree = tree;
        module = make_ref<Module>();
    }

    ref<Module> generate() {
        auto root = ts_tree_root_node(tree);
        auto count = ts_node_child_count(root);

        auto sexpr = ts_node_string(root);
        printf("%s\n", sexpr);
        free(sexpr);

        for (uint32_t i = 0; i < count; i++) {
            top_level_definition(ts_node_child(root, i));
        }

        return module;
    }

    void top_level_definition(TSNode n) {
        n = ts_node_child(n, 0);
        auto type = ts_node_type(n);
        if(strcmp(type, "func") == 0) {
            module->funcs.push_back(func(n));
        }
    }

    ref<Func> func(TSNode n) {
        auto func = make_ref<Func>();
        // Exported
        auto exported_node = ts_node_child_by_field_name(n, "exported");
        func->is_exported = !ts_node_is_null(exported_node);
        // Name
        auto name_node = ts_node_child_by_field_name(n, "name");
        func->name = ts_node_source(name_node, source);
        // TODO(caleb): insert params
        // Return 
        auto return_node = ts_node_child_by_field_name(n, "return");
        func->return_type = to_type(return_node);
        // Block
        func->block_count = 1;
        auto block_node = ts_node_child_by_field_name(n, "block");
        auto root = make_ref<Block>();
        root->func = func.get();
        block(root, block_node); 
        func->root = root;

        return func;
    }

    void statement(ref<Block> b, TSNode n) {
        auto statement_type = ts_node_type(n);
	    n = ts_node_child(n, 0);
        auto type = ts_node_type(n);
        if (strcmp(type, "block") == 0) {
            auto new_block = b->new_child_block();
            block(new_block, n);
        } else if (strcmp(type, "return") == 0) {
            return_(b, n);
        } else if (strcmp(type, "if") == 0) {
            if_(b, n);
        } else if (strcmp(type, "variable") == 0) {
            var(b, n);
        } else if (strcmp(type, "function_call") == 0) {
            call(b, n);
        } else {
            printf("Unimplemented statement type: %s\n", type);
        }
    }

    void block(ref<Block> b, TSNode n) {
        auto type = ts_node_type(n);
        auto count = ts_node_child_count(n);
        // the first and last nodes are { and }, we can ignore them
        for (uint32_t i = 1; i < count - 1; i++) {
            statement(b, ts_node_child(n, i));
        }
    }
    
    void return_(ref<Block> b, TSNode n) {
        auto value = ts_node_child_by_field_name(n, "value");
        if (ts_node_is_null(value)) {
            b->ret();
        } else {
            b->retval(expression(b, value));
        }
    }

    void if_(ref<Block> b, TSNode n) {
        auto condition = expression(b, ts_node_child_by_field_name(n, "condition"));
        auto then_block = b->new_child_block();
        block(then_block, ts_node_child_by_field_name(n, "then"));
        auto else_node = ts_node_child_by_field_name(n, "else");
        if (!ts_node_is_null(else_node)) {
            auto else_block = b->new_child_block();
            block(else_block, else_node);
            b->if_(condition, then_block, else_block);
        } else {
            b->if_(condition, then_block, std::nullopt);
        }
    }

    void var(ref<Block> b, TSNode n) {
        bool is_const = false;
        if(ts_node_source(ts_node_child_by_field_name(n, "specifier"), source) == "const") {
            is_const = true;
        }

        auto name = ts_node_source(ts_node_child_by_field_name(n, "name"), source);

        auto type = unknown_type();
        auto type_node = ts_node_child_by_field_name(n, "type");
        if(!ts_node_is_null(type_node)) {
            type = to_type(type_node);
        }

        auto equals = expression(b, ts_node_child_by_field_name(n, "equals"));

        b->var(name, type, is_const, equals);
    }


    Ref expression(ref<Block> b, TSNode n) {
        n = ts_node_child(n, 0);
        auto type = ts_node_type(n);
        if (strcmp(type, "binary_expression") == 0) {
            return bin_expr(b, n);
        } else if (strcmp(type, "integer") == 0) {
            return integer(b, n);
        } else if (strcmp(type, "identifier") == 0) {
            return identifier(b, n);
        } else if (strcmp(type, "function_call") == 0) {
            return call(b, n);
        } else {
            printf("Unimplemented expression type: %s\n", type);
            return Ref(0);
        }
    }

    Ref bin_expr(ref<Block> b, TSNode n) {
        auto left = expression(b, ts_node_child_by_field_name(n, "left"));
        auto op = ts_node_source(ts_node_child_by_field_name(n, "operator"), source);
        auto right = expression(b, ts_node_child_by_field_name(n, "right"));
        if (op == "+") {
            return b->add(left, right);
        } else if (op == "==") {
            return b->eq(left, right);
        } else if (op == "!=") {
            return b->noteq(left, right);            
        } else {
            printf("Unimplemented operator %s\n", op.c_str());
            return b->add(left, right);
        }
    }

    Ref integer(ref<Block> b, TSNode n) {
        auto value = ts_node_source(n, source);
        return b->int_(atoi(value.c_str()));
    }

    Ref identifier(ref<Block> b, TSNode n) {
        auto name = ts_node_source(n, source);
        return b->load(name);
    }

    Ref call(ref<Block> b, TSNode n) {
        auto name = ts_node_source(ts_node_child_by_field_name(n, "name"), source);
        return b->call(name, std::vector<Ref>());
    }

    ref<Type> to_type(TSNode n) {
        auto t = make_ref<Type>();
        n = ts_node_child(n, 0);
        auto type = ts_node_type(n);
        t->type = TypeUnknown;
        if (strcmp(type, "array_type") == 0 ) {
            auto count = ts_node_child_by_field_name(n, "count");
            if (!ts_node_is_null(count)) {
                auto value = ts_node_source(count, source);
                t->count = atoi(value.c_str());
            }
            t->type = TypeArray;
            t->inner = to_type(ts_node_child(n, 1));
        } else if (strcmp(type, "reference_type") == 0 ) {
            t->type = TypeReference;
            t->inner = to_type(ts_node_child(n, 1));
        } else if (strcmp(type, "pointer_type") == 0 ) {
            t->type = TypePointer;
            t->inner = to_type(ts_node_child(n, 1));
        } else if (strcmp(type, "identifier") == 0 ) {
            auto name = ts_node_source(n, source);
            t->inner = name;
            if(name == "i8") { t->type = TypeInt8; }
            else if(name == "u8") { t->type = TypeUInt8; }
            else if(name == "i16") { t->type = TypeInt16; }
            else if(name == "u16") { t->type = TypeUInt16; }
            else if(name == "i32") { t->type = TypeInt32; }
            else if(name == "u32") { t->type = TypeUInt32; }
            else if(name == "i64") { t->type = TypeInt64; }
            else if(name == "u64") { t->type = TypeUInt64; }
            else if(name == "float") { t->type = TypeFloat; }
            else if(name == "double") { t->type = TypeDouble; }
            else { t->type = TypeIdentifier; }
        }
        return t;
    }
    
    ref<Type> unknown_type() {
        auto type = make_ref<Type>();
        type->type = TypeUnknown;
        return type;
    }
};

ref<Module> ir_gen(TSTree* tree, const std::vector<char>& source) {
    Generator gen(tree, source);
    return gen.generate();
}