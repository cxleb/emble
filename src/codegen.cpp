#include "codegen.h"
#include <unordered_map>
#include "llvm-c/Core.h"
#include "llvm-c/Target.h"
#include "llvm-c/Linker.h"
#include "llvm-c/Analysis.h"

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
        auto did_return = block(b, func->root);
        if(!did_return) {
            LLVMPositionBuilderAtEnd(builder, b);
            LLVMBuildRetVoid(builder);
        }
    }

    // Returns whether or not the block returned aka, if there was a ret instruction
    bool block(LLVMBasicBlockRef llvm_block, ref<Block> ir_block) {
        std::vector<LLVMValueRef> values(ir_block->insts.size());
        LLVMPositionBuilderAtEnd(builder, llvm_block);
        for(uint32_t i = 0; i < ir_block->insts.size(); i++) {
            Inst& inst = ir_block->insts[i];
            switch(inst.type) {
            case InstBlock: {
                auto data = std::get<DataBlock>(inst.data);
                break;
            }
            case InstCall: {
                auto data = std::get<DataCall>(inst.data);
                auto f = funcs.at(data.name);
                LLVMBuildCall2(builder, f.type, f.val, nullptr, 0, "");
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
                break;
            }
            case InstVar: {
                auto data = std::get<DataVar>(inst.data);
                break;
            }
            case InstStore: {
                auto data = std::get<DataStore>(inst.data);
                break;
            }
            case InstLoad: {
                auto data = std::get<DataLoad>(inst.data);
                break;
            }
            case InstAdd: {
                auto data = std::get<DataBinOp>(inst.data);
                break;
            }
            case InstEq: {
                auto data = std::get<DataBinOp>(inst.data);
                break;
            }
            case InstNotEq: {
                auto data = std::get<DataBinOp>(inst.data);
                break;
            }
            case InstInt: {
                auto data = std::get<DataInt>(inst.data);
                break;
            }
            }
        }

        return false;
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