#include "llvm-c/Core.h"
#include "llvm-c/ExecutionEngine.h"
#include "llvm-c/Analysis.h"
#include "llvm-c/BitWriter.h"

STRUCT(LLVM)
{
    LLVMContextRef context;
    LLVMBuilderRef builder;
    LLVMModuleRef module;
};

LLVMTypeRef type_string_to_ref(LLVM *llvm, char *type_name)
{
    // NOTE: Do type lookup here, structs etc.
    if(strcmp(type_name, "int") == 0) {
        return LLVMInt32TypeInContext(llvm->context);
    } else if(strcmp(type_name, "u8") == 0) {
        return LLVMInt8TypeInContext(llvm->context);
    } else if(strcmp(type_name, "void") == 0) {
        return LLVMVoidTypeInContext(llvm->context);
    }
    
    printf("Unknown type string: %s\n", type_name);
    exit(-1);
    return 0;
}

LLVMTypeRef node_to_type(LLVM *llvm, Node *n)
{
    if(n->type != NODE_TYPE) {
    printf("Node passed to node_to_type is not of type NODE_TYPE!\n");
    exit(-1);
    }
    TypeNode t = n->ntype;
    LLVMTypeRef type;
    
    char *type_name = t.ident->ident.value;
    type = type_string_to_ref(llvm, type_name);
    
    for(int i = 0; i < t.pointer_count; i++) {
        type = LLVMPointerType(type, 0);
    }
    
    // TOOD: Implement static arrays
    
    return type;
}

LLVMValueRef generate_expression(LLVM *llvm, Node *n)
{
    switch(n->type) {
        case NODE_CONSTANT: {
            ConstantNode c = n->constant;
            
            if(c.type == CONST_INTEGER) {
                long val = strtol(c.value, 0, 0);
                return LLVMConstInt(LLVMInt8TypeInContext(llvm->context), val, 1);
            } else {
                printf("Unimplemented constant type in generate_expression()!\n");
                exit(-1);
            }
        
        } break;
        
        case NODE_BINOP: {
            BinOpNode b = n->binop;
            
            // TODO: Difference between float, signed, unsigned
            
            switch(b.type) {
                case BINOP_ADD: {
                    LLVMValueRef lhs = generate_expression(llvm, b.lhs);
                    LLVMValueRef rhs = generate_expression(llvm, b.rhs);
                    return LLVMBuildAdd(llvm->builder, lhs, rhs, "wat is this value?!");
                } break;
                case BINOP_SUB: {
                    LLVMValueRef lhs = generate_expression(llvm, b.lhs);
                    LLVMValueRef rhs = generate_expression(llvm, b.rhs);
                    return LLVMBuildSub(llvm->builder, lhs, rhs, "wat is this value?!");
                } break;
                case BINOP_MUL: {
                    LLVMValueRef lhs = generate_expression(llvm, b.lhs);
                    LLVMValueRef rhs = generate_expression(llvm, b.rhs);
                    return LLVMBuildMul(llvm->builder, lhs, rhs, "wat is this value?!");
                } break;
                case BINOP_DIV: {
                    // TODO: Signed unsigned difference
                    LLVMValueRef lhs = generate_expression(llvm, b.lhs);
                    LLVMValueRef rhs = generate_expression(llvm, b.rhs);
                    return LLVMBuildSDiv(llvm->builder, lhs, rhs, "wat is this value?!");
                } break;
            } break;
            
        } break;
    }
    
    printf("Unimplemented expression node in generate_expression()!\n");
    exit(1);
}

void generate_statement(LLVM *llvm, Node *n)
{
    switch(n->type) {
        case NODE_RETURN: {
            LLVMBuildRet(llvm->builder, generate_expression(llvm, n->nreturn.expr));
        } break;
        
        case NODE_FUNCCALL: {
            FuncCallNode fc = n->funccall;
            LLVMValueRef f = LLVMGetNamedFunction(llvm->module, fc.func_name->ident.value);
            if(f == 0) {
                printf("Cant find function named %s!\n", fc.func_name->ident.value);
                exit(-1);
            }
            
            LLVMValueRef *args = 0;
            int arg_count = 0;
            
            for(int i = 0; i < fc.argument_count; i++) {
                LLVMValueRef expr = generate_expression(llvm, fc.arguments[i]);
                arg_count++;
                args = realloc(args, sizeof(LLVMValueRef)*arg_count);
                args[arg_count-1] = expr; 
            }
            LLVMBuildCall(llvm->builder, f, args, arg_count, "");
        } break;
        
        default: {
            printf("Unimplemented statement in generate_statement()! Type: %d\n", n->type);
            exit(1);
        } break;
    }
}

void generate_block(LLVM *llvm, Node *n)
{
    BlockNode block = n->block;
    for(int i = 0; i < block.count; i++) {
        generate_statement(llvm, block.stmts[i]);
    }
}

void generate_function(LLVM *llvm, Node *n)
{
    if(n->type == NODE_FUNCDEF) {
    // If function already exist just add the body
        // the types should match as this is going to be
        // checked before we get here
        FuncDefNode f = n->funcdef;
        LLVMTypeRef ret_type = node_to_type(llvm, f.type);
        LLVMTypeRef *arg_types = 0;
        int arg_type_count = 0;
        
        for(int i = 0; i < f.argument_count; i++) {
            arg_type_count++;
            arg_types = realloc(arg_types, arg_type_count*sizeof(LLVMTypeRef));
            arg_types[arg_type_count-1] = node_to_type(llvm, f.argument_types[i]);
        }
        
        // NOTE: Last arg: isVarArg
        LLVMTypeRef func_type = LLVMFunctionType(ret_type, arg_types, arg_type_count, 0);
        LLVMValueRef function = LLVMAddFunction(llvm->module, f.name->ident.value, func_type);
        
        LLVMBasicBlockRef main_block = LLVMAppendBasicBlockInContext(llvm->context, function, "main_block");
        LLVMPositionBuilderAtEnd(llvm->builder, main_block);
        generate_block(llvm, f.block);
        //LLVMBuildRetVoid(llvm->builder);
    } else if(n->type == NODE_FUNCDECL){
        FuncDeclNode f = n->funcdecl;
        LLVMTypeRef ret_type = node_to_type(llvm, f.type);
        LLVMTypeRef *arg_types = 0;
        int arg_type_count = 0;
        
        for(int i = 0; i < f.argument_count; i++) {
            arg_type_count++;
            arg_types = realloc(arg_types, arg_type_count*sizeof(LLVMTypeRef));
            arg_types[arg_type_count-1] = node_to_type(llvm, f.argument_types[i]);
        }
        
        LLVMTypeRef func_type = LLVMFunctionType(ret_type, arg_types, arg_type_count, 0);
        LLVMValueRef function = LLVMAddFunction(llvm->module, f.name->ident.value, func_type);
    }
}

void llvm_gen(Node *n, char *path)
{
    LLVM llvm = {0};
    llvm.context = LLVMContextCreate();
    llvm.builder = LLVMCreateBuilderInContext(llvm.context);
    llvm.module = LLVMModuleCreateWithNameInContext("main", llvm.context);
    
    ProgramNode program = n->program;
    for(int i = 0; i < program.func_count; i++) {
        generate_function(&llvm, program.functions[i]);
    }
    
    char *error = 0;
    int ret = LLVMVerifyModule(llvm.module, LLVMAbortProcessAction, &error);
    if(ret) {
        printf("Verify error:\n%s\n", error);
        exit(-1);
    }
    LLVMDisposeMessage(error);
    
    LLVMWriteBitcodeToFile(llvm.module, path);
    
    LLVMExecutionEngineRef engine;
    error = 0;
    LLVMInitializeNativeTarget();
    LLVMLinkInInterpreter();
    ret = LLVMCreateExecutionEngineForModule(&engine, llvm.module, &error);
    if(ret != 0) {
        printf("Failed to create execution engine!\n");
        exit(-1);
    }
    if(error) {
        printf("LLVM error:\n%s\n", error);
        LLVMDisposeMessage(error);
        exit(-1);
    }
    
    LLVMValueRef main = LLVMGetNamedFunction(llvm.module, "main");
    LLVMGenericValueRef result = LLVMRunFunction(engine, main, 0, 0);
    printf("%lld\n", LLVMGenericValueToInt(result, 0));
     
    LLVMDisposeModule(llvm.module);
    LLVMDisposeBuilder(llvm.builder);
    LLVMContextDispose(llvm.context);
}
