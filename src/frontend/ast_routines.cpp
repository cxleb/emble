#include "ast.h"
#include <iostream>
namespace frontend {
    namespace ast {
        void expression_node::print() {
            
        }

        void return_node::print()
        {
            std::cout << "Return, value :" << std::endl;
            this->return_value->print();
        }

        void string_node::print() {
            std::cout << "String, Value: " << this->value << std::endl;
        }
        
        void integer_node::print() {
            std::cout << "Int, Value: " << this->value << std::endl;
        }
        
        void variable_reference_node::print() {
            std::cout << "Var Ref to " << this->name << std::endl;
        }
        
        void func_call_node::print() {
            std::cout << "Func Call to " << this->name << std::endl;
            for (expression_node* e : this->arguements)
            {
                printf("\tARG ---> ");
                e->print();
            }
        }
        
        void variable_node::print() {
            std::cout << "Variable, Name: " << this->name << " Type: " << this->variable_type << std::endl;
            if (this->assignment != nullptr) {
                printf("\tEQL ---> ");
                this->assignment->print();
            }
        }
        
        void math_node::print() {
            std::cout << "Math OP, OP: " << this->op << std::endl;
            printf("\tLHS ---> ");
            this->lhs->print();
            printf("\tRHS ---> ");
            this->rhs->print();
        }
        
        void block_node::print() {
            printf("in block:\n");
            for (expression_node* e : this->statements)
            {
                printf("\t");
                e->print();
            }
        }
        
        void func_node::print() {
            std::cout << "Function, Name: " << this->name << std::endl;
            std::cout << "Return Type: " << this->return_type << std::endl;
            for (expression_node* e : this->arguements)
            {
                printf("\tARG ---> ");
                e->print();
            }
            this->statement->print();
        }
        
        void extern_func_node::print() {
            std::cout << "External Function, Name: " << this->name << std::endl;
            for (expression_node* e : this->arguements)
            {
                printf("\tARG ---> ");
                e->print();
            }
        }
        
        void root_node::print() {
            std::cout << "Root Node" << std::endl;
            for (expression_node* e : this->functions)
            {
                e->print();
                printf("\n");
            }
        }
    }
}
