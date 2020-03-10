#include "backend.h"
#include <string>
#include <sstream>
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
            stream << "extern _" << name << '\n';
        }
        for(ir::func* current = prog->funcs; current; current = current->next)
        {
            stream << "global _" << current->name << "\n";
        }

        stream << "section .data\n"; 
        for(ir::global_const* current = prog->global_consts; current; current = current->next)
        {
            if(current->value->type == ir::value_type::string_t)
            {
                stream << current->name << " db \"" << current->value->the_value.string << "\", 0\n";
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
        push(&stream, "_" + func->name + ":");
        
        // setup stack frame asm
        stream << "push rbp\n";
        stream << "mov rbp, rsp\n";
        if(func->locals)
        {
            stream << "sub ebp, " << std::to_string(func->locals) << "\n";
        }   

        // translate each block
        ir::block* current = func->blocks;
        for (current; current; current = current->next) {
            stream << translate_block(current, func->name);
        }

        stream << "_" << func->name << "_ret:\n";
        stream << "mov rsp, rbp\n";
        stream << "pop rbp\n";
        stream << "ret\n";

        return stream.str();
    }
    
    std::string translate_block(ir::block* block, std::string func_name )
    {
        std::stringstream stream;
        stream << "_" << func_name << "_b"<< std::to_string(block->number) << ":\n";
        
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
                        {
                            push(&stream, "mov al, " + std::to_string(value->the_value.number));
                            //push(&stream, "push al");
                        }
                        break;
                        case ir::value_type::uint16:
                        case ir::value_type::int16:
                        {
                            push(&stream, "mov ax, " + std::to_string(value->the_value.number));
                            //push(&stream, "push ax");
                        }
                        break;
                        case ir::value_type::uint32:
                        case ir::value_type::int32:
                        {
                            push(&stream, "mov eax, " + std::to_string(value->the_value.number));
                            //push(&stream, "push eax");
                        }
                        break;
                        case ir::value_type::uint64:
                        case ir::value_type::int64:
                        {
                            push(&stream, "mov rax, " + std::to_string(value->the_value.number));
                            //push(&stream, "push rax");
                        }
                        break;
                        case ir::value_type::global_t:
                        {
                            push(&stream, "push " + std::string(value->the_value.string));
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
                            push(&stream, "mov al, BYTE[ebp - "+std::to_string(value->the_value.number + 4)+"]");
                            push(&stream, "push al");
                        }
                        break;
                        case ir::value_type::uint16:
                        case ir::value_type::int16:
                        {
                            push(&stream, "mov ax, WORD[ebp - "+std::to_string(value->the_value.number + 4)+"]");
                            push(&stream, "push ax");
                        }
                        break;
                        case ir::value_type::uint32:
                        case ir::value_type::int32:
                        {
                            push(&stream, "mov eax, DWORD[ebp - "+std::to_string(value->the_value.number + 4)+"]");
                            push(&stream, "push eax");
                
                        }
                        break;
                        case ir::value_type::uint64:
                        case ir::value_type::int64:
                        {
                            push(&stream, "mov rax, QWORD[ebp - "+std::to_string(value->the_value.number + 4)+"]");
                            push(&stream, "push rax");
                
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
                            push(&stream, "pop al");
                            push(&stream, "mov BYTE[ebp - "+std::to_string(value->the_value.number + 4)+"], al");
                        }
                        break;
                        case ir::value_type::uint16:
                        case ir::value_type::int16:
                        {
                            push(&stream, "pop ax");
                            push(&stream, "mov WORD[ebp - "+std::to_string(value->the_value.number + 4)+"], ax");
                        }
                        break;
                        case ir::value_type::uint32:
                        case ir::value_type::int32:
                        {
                            push(&stream, "pop eax");
                            push(&stream, "mov DWORD[ebp - "+std::to_string(value->the_value.number + 4)+"], eax");
                        }
                        break;
                        case ir::value_type::uint64:
                        case ir::value_type::int64:
                        {
                            push(&stream, "pop rax");
                            push(&stream, "mov QWORD[ebp - "+std::to_string(value->the_value.number + 4)+"], rax");
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
                    push(&stream, "call _" + std::string(value->the_value.string));
                }
                break;
                case ir::value_instruction::i_ret:
                {
                    switch(value->type)
                    {
                        case ir::value_type::uint8:
                        case ir::value_type::int8:
                        {
                            push(&stream, "pop al");
                            push(&stream, "jmp _" + func_name + "_ret" );
                        }
                        break;
                        case ir::value_type::uint16:
                        case ir::value_type::int16:
                        {
                            push(&stream, "pop ax");
                            push(&stream, "jmp _" + func_name + "_ret" );
                        }
                        break;
                        case ir::value_type::uint32:
                        case ir::value_type::int32:
                        {
                            push(&stream, "pop eax");
                            push(&stream, "jmp _" + func_name + "_ret" );
                        }
                        break;
                        case ir::value_type::uint64:
                        case ir::value_type::int64:
                        {
                            push(&stream, "pop rax");
                            push(&stream, "jmp _" + func_name + "_ret" );
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
                default:
                push(&stream, "whoops unimplemented value type");
                break;
            }
        }
        
        return stream.str();
    }
}
