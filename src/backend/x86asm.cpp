#include "backend.h"
#include <string>
#include <sstream>
#include <iostream>
namespace backend{
    void error()
    {
        fprintf(stderr, "x86asm: found an error");
    }
    
    void push(std::stringstream* strm, std::string cmd) {
        (*strm) << cmd << "\n";
    }

    std::string translate_prog(ir::prog* prog)
    {
        std::stringstream stream;
        //global and public defines
        for(ir::extern_func* current = prog->extern_funcs; current; current = current->next)
        {
            std::string name = current->name;
            stream << "extern " << name << '\n';
        }
        for(ir::func* current = prog->funcs; current; current = current->next)
        {
            stream << "global " << current->name << "\n";
        }

        stream << "section .data\n"; 
        for(ir::global_const* current = prog->global_consts; current; current = current->next)
        {
            if(current->value->type == ir::value_type::string_t)
            {
                stream << current->name << " db \"";
                char c = 0;
                for (int i = 0; (c = current->value->the_value.string[i]) != 0; i++)
                {
                    switch(c)
                    {
                    case 13:
                        stream << "\", 10, 13, \"";
                        break;
                    default:
                        stream << c;
                    }
                }
                stream << "\", 0\n";
            }
        }

        stream << "section .text\n"; 
        for(ir::func* current = prog->funcs; current; current = current->next)
        {
            stream << translate_func(current);
        }

        return stream.str();
    }

    std::string translate_func(ir::func* func)
    {
        std::stringstream stream;
        // put name of function at top
        push(&stream, func->name + ":");
        
        // setup stack frame asm
        int locals = func->locals + ((4 - func->parameter_count) * 8);
        stream << "sub rsp, " << std::to_string(locals) << "\n";

        //load params
        if(func->parameter_count >= 1)
            stream << "mov QWORD [rsp + 0], rcx\n";
        if(func->parameter_count >= 2)
            stream << "mov QWORD [rsp + 8], rdx\n";
        if(func->parameter_count >= 3)
            stream << "mov QWORD [rsp + 16], r8\n";
        if(func->parameter_count >= 4)
            stream << "mov QWORD [rsp + 24], r9\n";        

        // translate each block
        ir::block* current = func->blocks;
        for (current; current; current = current->next) {
            stream << translate_block(current, func->name);
        }

        stream << func->name << "_ret:\n";
        stream << "add rsp, " << std::to_string(locals) << "\n";
        stream << "ret\n";

        return stream.str();
    }
    
    std::string translate_block(ir::block* block, std::string func_name )
    {
        std::stringstream stream;
        stream << func_name << "_b"<< std::to_string(block->number) << ":\n";
        
        for(ir::value* value = block->values; value; value = value->next)
        {
            switch(value->instruction)
            {
                case ir::value_instruction::i_const:
                {
                    switch(value->type)
                    {
                        case ir::value_type::uint8:
                        case ir::value_type::int8:
                        case ir::value_type::uint16:
                        case ir::value_type::int16:
                        case ir::value_type::uint32:
                        case ir::value_type::int32:
                        case ir::value_type::uint64:
                        case ir::value_type::int64:
                        {
                            push(&stream, "mov rax, " + std::to_string(value->the_value.number));
                            //push(&stream, "push rax");
                        }
                        break;
                        case ir::value_type::global_t:
                        {
                            push(&stream, "mov rax, " + std::string(value->the_value.string));
                        }
                        break;
                        default:
                        break;
                    }
                }
                break;
                case ir::value_instruction::i_get:
                {
                    switch(value->type)
                    {
                        case ir::value_type::uint8:
                        case ir::value_type::int8:
                        {
                            push(&stream, "mov al, BYTE[rsp + "+std::to_string(value->the_value.number)+"]");
                        }
                        break;
                        case ir::value_type::uint16:
                        case ir::value_type::int16:
                        {
                            push(&stream, "mov ax, WORD[rsp + "+std::to_string(value->the_value.number)+"]");
                        }
                        break;
                        case ir::value_type::uint32:
                        case ir::value_type::int32:
                        {
                            push(&stream, "mov eax, DWORD[rsp + "+std::to_string(value->the_value.number)+"]");
                        }
                        break;
                        case ir::value_type::uint64:
                        case ir::value_type::int64:
                        {
                            push(&stream, "mov rax, QWORD[rsp + "+std::to_string(value->the_value.number)+"]");
                        }
                        break;
                        default:
                        break;
                    }
                    
                }
                break;
                case ir::value_instruction::i_set:
                {
                    switch(value->type)
                    {
                        case ir::value_type::uint8:
                        case ir::value_type::int8:
                        {
                            push(&stream, "mov BYTE[rsp + "+std::to_string(value->the_value.number)+"], al");
                        }
                        break;
                        case ir::value_type::uint16:
                        case ir::value_type::int16:
                        {
                            push(&stream, "mov WORD[rsp + "+std::to_string(value->the_value.number)+"], ax");
                        }
                        break;
                        case ir::value_type::uint32:
                        case ir::value_type::int32:
                        {
                            push(&stream, "mov DWORD[rsp + "+std::to_string(value->the_value.number)+"], eax");
                        }
                        break;
                        case ir::value_type::uint64:
                        case ir::value_type::int64:
                        {
                            push(&stream, "mov QWORD[rsp + "+std::to_string(value->the_value.number)+"], rax");
                        }
                        break;
                        default:
                        break;
                    }
                    
                }
                break;
                case ir::value_instruction::i_add:
                {
                    switch(value->type)
                    {
                        case ir::value_type::uint8:
                        case ir::value_type::int8:
                        {
                            push(&stream, "pop al");
                            push(&stream, "pop bl");
                            push(&stream, "add al, bl");
                            push(&stream, "push al");
                        }
                        break;
                        case ir::value_type::uint16:
                        case ir::value_type::int16:
                        {
                            push(&stream, "pop ax");
                            push(&stream, "pop bx");
                            push(&stream, "add ax, bx");
                            push(&stream, "push ax");
                        }
                        break;
                        case ir::value_type::uint32:
                        case ir::value_type::int32:
                        {
                            push(&stream, "pop eax");
                            push(&stream, "pop ebx");
                            push(&stream, "add eax, ebx");
                            push(&stream, "push eax");
                        }
                        break;
                        case ir::value_type::uint64:
                        case ir::value_type::int64:
                        {
                            push(&stream, "pop rax");
                            push(&stream, "pop rbx");
                            push(&stream, "add rax, rbx");
                            push(&stream, "push rax");
                        }
                        break;
                        default:
                        {
                            error();
                        }
                        break;
                    }
                    // end add type switch
                }
                break;
                case ir::value_instruction::i_call:
                {
                    push(&stream, "call " + std::string(value->the_value.string));
                }
                break;
                case ir::value_instruction::i_ret:
                {
                    switch(value->type)
                    {
                        case ir::value_type::uint8:
                        case ir::value_type::int8:
                        {
                            push(&stream, "jmp " + func_name + "_ret" );
                        }
                        break;
                        case ir::value_type::uint16:
                        case ir::value_type::int16:
                        {
                            push(&stream, "jmp " + func_name + "_ret" );
                        }
                        break;
                        case ir::value_type::uint32:
                        case ir::value_type::int32:
                        {
                            push(&stream, "jmp " + func_name + "_ret" );
                        }
                        break;
                        case ir::value_type::uint64:
                        case ir::value_type::int64:
                        {
                            push(&stream, "jmp " + func_name + "_ret" );
                        }
                        break;
                        default:
                        {
                            error();
                        }
                        break;
                    }
                }
                break;
                case ir::value_instruction::i_args:
                {
                    switch(value->the_value.number)
                    {
                        case 1:
                            push(&stream, "mov rcx, rax");
                        break;
                        case 2:
                            push(&stream, "mov rdx, rax");
                        break;
                        case 3:
                            push(&stream, "mov r8, rax");
                        break;
                        case 4:
                            push(&stream, "mov r9, rax");
                        break;
                        default:
                            fprintf(stderr, "x86asm: fast call convention cant be used exiting\n");
                            exit(0);
                        break;
                    }
                }
                break;
                default:
                push(&stream, "whoops unimplemented value type");
                break;
            }
        }
        
        return stream.str();
    }
}
