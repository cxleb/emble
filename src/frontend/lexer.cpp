#include "frontend.h"

#include <string>
#include <sstream>
#include <stdlib.h>
#include <iostream>

namespace frontend 
{
    
    int line = 1;
    
    void skip_whitespace(std::string& line)
    {
        size_t start = 0;
        while (line[start] == '\t' || line[start] == ' ')
            start++;
        if (std::string::npos != start)
            line = line.substr(start);
    }

    int isalnum_extra(int c)
    {
        return isalnum(c) || c == '_';
    }

    int isalpha_extra(int c)
    {
        return isalpha(c) || c == '_';
    }
    
    token* get_next_token (std::string &code)
    {
        if (code.length() == 0)
            return nullptr;
        
        token* tok = new token();
        
        int read = 0;
        
        skip_whitespace(code);
        
        while (code[0] == '\n') 
        {
            line++;
            code = code.substr(1);
        }
        
        skip_whitespace(code);
        
        tok->line = line;
        
        if (code.find("//") == 0) 
        {
            while (code[read] != '\n')
                read++;
            tok->type = token_type::comment;
        }
        else if (code[0] == '\"')
        {
            std::string id;
            
            std::stringstream str;
            str << code;
            str.get();
            int tmp = 0;
            read = 0;
            while ((tmp = str.get()) != '\"')
            {
                if (tmp == '\\')
                {
                    tmp = str.get();
                    switch(tmp)
                    {
                        case 'n':
                            read += 1;
                            tmp = 13;
                            break;
                        //case '\"':
                        //    tmp = '\"';
                        //    break;
                        //case 't':
                        //    tmp = "\t";
                        //case '\\':
                        //    tmp = "\\";
                        default:
                            id += '\\';
                            break;
                    }
                }
                id += (char)tmp;
            }
            
            read += id.length() + 2;
            tok->text = id;
            tok->type = token_type::string;
        }
        else if (isdigit(code[0])) 
        {
            std::string id;
            
            std::stringstream str;
            str << code;
            
            int tmp = 0;
            while (isdigit(tmp = str.get()))
                id += (char)tmp;
            
            read = id.length();
            tok->text = id;
            tok->type = token_type::number;
        }
        else if (isalpha_extra(code[0])) 
        {
            std::string id;
            
            std::stringstream str;
            str << code;
            
            int tmp = 0;
            while (isalnum_extra(tmp = str.get()))
                id += (char)tmp;
            
            read = id.length();
            tok->text = id;
            if (id == "func") tok->type = token_type::kw_func;
            else if (id == "if") tok->type = token_type::kw_if;
            else if (id == "else") tok->type = token_type::kw_else;
            else if (id == "elseif") tok->type = token_type::kw_elseif;
            else if (id == "extern") tok->type = token_type::kw_extern;
            else if (id == "return") tok->type = token_type::kw_return;
            else tok->type = token_type::identifier;
        }
        else if (!isalnum_extra(code[0])) 
        {
            switch (code[0])
            {
                case '+':
                tok->type = token_type::addition;
                break;
                case '-':
                tok->type = token_type::subtraction;
                break;
                case '/':
                tok->type = token_type::divison;
                break;
                case '*':
                tok->type = token_type::multilication;
                break;
                case '=':
                tok->type = token_type::equals;
                break;
                case '(':
                tok->type = token_type::left_paren;
                break;
                case ')':
                tok->type = token_type::right_paren;
                break;
                case '{':
                tok->type = token_type::left_curly;
                break;
                case '}':
                tok->type = token_type::right_curly;
                break;
                case '[':
                tok->type = token_type::left_bracket;
                break;
                case ']':
                tok->type = token_type::right_bracket;
                break;
                case ':':
                tok->type = token_type::colon;
                break;
                case ',':
                tok->type = token_type::comma;
                break;
                default:
                std::cout << "unexpected symbol found in the the lexer: " << code[0] << '\n';
                exit(0);
                break;
            }
            
            tok->text = code[0];
            read = 1;
            
        }
        
        code = code.substr(read);
        return tok;
    }
}

