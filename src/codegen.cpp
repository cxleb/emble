#include "codegen.h"
#include <unordered_map>
#include "llvm-c/Core.h"
#include "llvm-c/Target.h"
#include "llvm-c/Analysis.h"
#include "llvm-c/Types.h"

namespace detail {
    struct Func {
        LLVMValueRef val;
        LLVMTypeRef type;
        std::vector<LLVMValueRef> vars;    
    };
}

struct CodeGen {
    LLVMContextRef context;
    LLVMModuleRef mod;
    LLVMBuilderRef builder;
    std::unordered_map<std::string, detail::Func> funcs;

    void generate(ref<Module> m) {
        context = LLVMContextCreate();
        mod = LLVMModuleCreateWithNameInContext("root", context);
        builder = LLVMCreateBuilderInContext(context);

        for (auto f : m->funcs) {
            auto type = LLVMFunctionType(LLVMInt32Type(), nullptr, 0, false);
            auto val = LLVMAddFunction(mod, f->name.c_str(), type);
            funcs.insert_or_assign(f->name, detail::Func{
                .val = val,
                .type = type,
                .vars = std::vector<LLVMValueRef>(f->var_count)
            });
        }

        for (auto f : m->funcs) {
            func(f);
        }

        auto llir = LLVMPrintModuleToString(mod);
        printf("%s\n", llir);
        LLVMDisposeMessage(llir);

        char* msg;        
        if (!LLVMVerifyModule(mod, LLVMReturnStatusAction, &msg)) {
            printf("Module failed to verify: %s\n", msg);
            LLVMDisposeMessage(msg);
            return;
        }
    }

    void func(ref<Func> func) {
        auto f = funcs.at(func->name);
        auto b = LLVMAppendBasicBlockInContext(context, f.val, "");
        auto did_return = block(b, func->root, f);
        if(!did_return) {
            LLVMPositionBuilderAtEnd(builder, b);
            LLVMBuildRetVoid(builder);
        }
    }

    // Returns whether or not the block returned aka, if there was a ret instruction
    bool block(LLVMBasicBlockRef llvm_block, ref<Block> ir_block, detail::Func& f) {
        std::vector<LLVMValueRef> values(ir_block->insts.size());
        LLVMPositionBuilderAtEnd(builder, llvm_block);
        for(uint32_t i = 0; i < ir_block->insts.size(); i++) {
            Inst& inst = ir_block->insts[i];
            switch(inst.type) {
            case InstBlock: {
                auto data = std::get<DataBlock>(inst.data);
                if(block(llvm_block, data.block, f)) {
                    return true;
                }
                break;
            }
            case InstCall: {
                auto data = std::get<DataCall>(inst.data);
                auto f = funcs.at(data.name);
                values[i] = LLVMBuildCall2(builder, f.type, f.val, nullptr, 0, "");
                break;
            }
            case InstRetVal: {
                auto data = std::get<DataUnary>(inst.data);
                LLVMBuildRet(builder, values[data.value]);
                return true;
            }
            case InstRet: {
                LLVMBuildRetVoid(builder);
                return true;
            }
            case InstIf: {
                auto data = std::get<DataIf>(inst.data);
                auto then_block = LLVMAppendBasicBlockInContext(context, f.val, "");
                auto then_returned = block(then_block, data.then, f);

                LLVMBasicBlockRef else_block;
                LLVMBasicBlockRef new_block;
                bool else_returned = false;

                if(auto else_ = data.else_) {
                    else_block = LLVMAppendBasicBlockInContext(context, f.val, "");
                    else_returned = block(else_block, *else_, f);
                    new_block = LLVMAppendBasicBlockInContext(context, f.val, "");
                } else {
                    new_block = LLVMAppendBasicBlockInContext(context, f.val, "");
                    else_block = new_block;

                }
                LLVMPositionBuilderAtEnd(builder, llvm_block);
                LLVMBuildCondBr(builder, values[data.condition], then_block, else_block);

                if(then_returned && else_returned) {
                    return true;
                }
                
                if (!then_returned) {    
                    LLVMPositionBuilderAtEnd(builder, then_block);
                    LLVMBuildBr(builder, new_block);
                }

                if (!else_returned && data.else_.has_value()) {    
                    LLVMPositionBuilderAtEnd(builder, else_block);
                    LLVMBuildBr(builder, new_block);
                }

                llvm_block = new_block;
                break;
            }
            case InstVar: {
                auto data = std::get<DataVar>(inst.data);
                auto type = to_llvm_type(data.var->type);
                auto var = LLVMBuildAlloca(builder, type, "");
                f.vars[data.var->number] = var;
                LLVMBuildStore(builder, values[data.equals], var);
                break;
            }
            case InstStore: {
                auto data = std::get<DataStore>(inst.data);
                auto var = f.vars[data.var->number];
                LLVMBuildStore(builder, values[data.equals], var);
                break;
            }
            case InstLoad: {
                auto data = std::get<DataLoad>(inst.data);
                auto var = f.vars[data.var->number];
                auto type = to_llvm_type(data.var->type);
                values[i] = LLVMBuildLoad2(builder, type, var, "");
                break;
            }
            case InstAdd: {
                auto data = std::get<DataBinOp>(inst.data);
                values[i] = LLVMBuildAdd(builder, values[data.lhs], values[data.rhs], "");
                break;
            }
            case InstEq: {
                auto data = std::get<DataBinOp>(inst.data);
                values[i] = LLVMBuildICmp(builder, LLVMIntEQ, values[data.lhs], values[data.rhs], "");
                break;
            }
            case InstNotEq: {
                auto data = std::get<DataBinOp>(inst.data);
                values[i] = LLVMBuildICmp(builder, LLVMIntNE, values[data.lhs], values[data.rhs], "");
                break;
            }
            case InstInt: {
                auto data = std::get<DataInt>(inst.data);
                values[i] = LLVMConstInt(LLVMInt32Type(), data.value, false);
                break;
            }
            }
        }

        return false;
    }

    LLVMTypeRef to_llvm_type(ref<Type> type) {
        return LLVMInt32Type();
    }
};

void codegen_init() {
    LLVMInitializeAllTargets();
    LLVMInitializeAllTargetMCs();
    LLVMInitializeAllTargetInfos();
    LLVMInitializeAllAsmParsers();
    LLVMInitializeAllAsmPrinters();
}

void codegen(ref<Module> m) {
    CodeGen gen;
    gen.generate(m);
}