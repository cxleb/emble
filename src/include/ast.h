#pragma once
#include <string>
#include <deque>
#include <vector>
#include <cstdint>

#include <ir.h>

namespace frontend {
    
    namespace ast {
        
        enum node_type{
            expression,
            return_,
            integer,
            string,
            variable_reference,
            func_call,
            variable,
            math,
            extern_func,
            func,
            block,
            root
        };
        
        enum math_op {
            unused,
            add,
            sub,
            div,
            mult
        };
        
        struct expression_node {
            virtual ~expression_node() { }
            virtual void print();
            int line;
            node_type type;
            ir::value_type marked_value;
        };

        struct return_node : expression_node
        {
            ~return_node() { }
            virtual void print() override;
            expression_node* return_value;
        };
        
        struct integer_node : expression_node {
            ~integer_node() { }
            virtual void print() override;
            int64_t value;
        };

        struct string_node : expression_node {
            ~string_node() { }
            virtual void print() override;
            std::string value;
        };
        
        struct variable_reference_node : expression_node {
            ~variable_reference_node() { }
            virtual void print() override;
            std::string name;
        };
        
        struct func_call_node : expression_node {
            ~func_call_node() { }
            virtual void print() override;
            std::string name;
            std::deque<expression_node*> arguements;
        };
        
        struct variable_node : expression_node {
            ~variable_node() { }
            virtual void print() override;
            std::string name;
            std::string variable_type;
            expression_node* assignment;
        };
        
        struct math_node : expression_node {
            ~math_node() { }
            virtual void print() override;
            expression_node* lhs;
            int op;
            expression_node* rhs;
        };
        
        struct extern_func_node : expression_node {
            ~extern_func_node() { }
            virtual void print() override;
            std::string name;
            std::string return_type;
            std::deque<expression_node*> arguements;
        };
        
        
        struct func_node : expression_node {
            ~func_node() { }
            virtual void print() override;
            std::string name;
            std::string return_type;
            std::deque<expression_node*> arguements;
            expression_node* statement;
        };
        
        struct block_node : expression_node {
            ~block_node() { }
            virtual void print() override;
            std::deque<expression_node*> statements;
        };
        
        struct root_node : expression_node{
            ~root_node() { }
            virtual void print() override;
            std::vector<expression_node*> functions;
        };
        
    }
    
}
