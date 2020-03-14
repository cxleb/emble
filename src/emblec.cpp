#include <iostream>
#include <fstream>
#include <sstream>

#include "frontend.h"
#include "ir.h"
#include "backend.h"
#include "codegen.h"

int main(int argc, char** argv) {
    printf("Emble Compiler (C++ version)\n");
    char* file_path = 0;
    char debug = 0;
    char run = 0;
    if (argc < 2 )
    {
        std::cout << "Please specify an emble file.\n";
        return 0;
    }
    for(int i = 1; i < argc; i++)
    {
        char first = argv[i][0];
        char second = argv[i][1];
        switch(first)
        {
            case '-':
            switch(second)
            {
                case 'd':
                debug = 1;
                break;
                case 'r':
                run = 1;
                break;
            }
            break;
            default:
            file_path = argv[i];
            break;
        }
    }
    
    std::ifstream file;
    std::stringstream stream;
    file.open(file_path);
    stream << file.rdbuf();
    std::string src = stream.str();
    file.close();

    frontend::compiler_state* state = new frontend::compiler_state();
    if(debug)
        std::cout << "parsing... ";
    frontend::parse(src, state);
    if(debug)
        std::cout << "type checking... ";
    frontend::type_check(state);
    //state->root->print();
    if(debug)
        std::cout << "codegen... ";
    ir::prog* prog = codegen::codegen(state);
    if(debug)
        std::cout << "assembly... ";
    std::string code = backend::translate_prog(prog);
    if(debug)
        std::cout << "done.\n";
    
    std::ofstream out("output.asm");
    out << code;
    out.close();

    system("compile");

    if(run)
        system("output");
/*
    ir::prog* prog = new ir::prog();

    ir::func* main = new ir::func();
    main->name = "main";
    // gen some ir
    ir::block* block = new ir::block();
    //block->number = ir::request_block(main);
    ir::request_block(main, block);
    //ir::add_value_to_block(block, ir::create_get_local(ir::value_type::uint64, bit_var));
    ir::add_value_to_block(block, ir::create_global_const("gbl_0"));
    ir::add_value_to_block(block, ir::create_call("printf"));
    
    ir::add_func_to_prog(prog, main);


    ir::func* addint32 = new ir::func();
    addint32->name = "addint32";
    index num1 = ir::request_local(addint32, 4);
    index num2 = ir::request_local(addint32, 4);
    // gen some ir
    ir::block* block2 = new ir::block();
    //block->number = ir::request_block(main);
    ir::request_block(addint32, block2);
    //ir::add_value_to_block(block, ir::create_get_local(ir::value_type::uint64, bit_var));
    ir::add_value_to_block(block2, ir::create_get_local(ir::value_type::int32, num1));
    ir::add_value_to_block(block2, ir::create_get_local(ir::value_type::int32, num2));
    ir::add_value_to_block(block2, ir::create_custom(ir::value_instruction::i_add, ir::value_type::int32));
    ir::add_value_to_block(block2, ir::create_custom(ir::value_instruction::i_ret, ir::value_type::int32));
    
    ir::add_func_to_prog(prog, addint32);



    // gen the asm
    //std::cout<<backend::translate_block(block, "test_func");
    //std::cout<<backend::translate_func(main);
    std::cout<<backend::translate_prog(prog);

    // cleeeeen
    ir::clean_up_block(block);
    ir::clean_up_block(block2);
/*
    
    */

    return 0;
    
}
