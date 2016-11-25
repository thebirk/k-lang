#include <stdio.h>
#include <stdlib.h>
#define STRUCT(name) typedef struct name name; struct name
#define ENUM(type, name) typedef type name; enum name##_
#include "../ast.h"
#include <string.h>
#include <jni.h>
#ifdef __WIN32
#define _strdup strdup
#endif

char* new_string(const char *str)
{
    return strdup(str);
}

static jclass node_class;
static jclass arraylist_class;
static jfieldID node_field_parent;
static jfieldID node_field_value;
static jfieldID node_field_children;
static jmethodID arraylist_add;

void setup_java_node(JNIEnv *env)
{
    node_class = (*env)->FindClass(env, "Node");
    if(node_class == 0) printf("Failed to find Node\n");
    
    arraylist_class = (*env)->FindClass(env, "java/util/ArrayList");
    if(arraylist_class == 0) printf("Failed to find ArrayList\n");
    
    arraylist_add = (*env)->GetMethodID(env, arraylist_class, "add", "(Ljava/lang/Object;)Z");
    if(arraylist_add == 0) printf("Failed to get ArrayList.add()\n");
    
     node_field_value = (*env)->GetFieldID(env, node_class, "value", "Ljava/lang/String;");
    if(node_field_value == 0) printf("Failed to find Node.value field");
    node_field_parent = (*env)->GetFieldID(env, node_class, "parent", "LNode;");
    if(node_field_parent == 0) printf("Failed to find Node.parent field");
    node_field_children = (*env)->GetFieldID(env, node_class, "children", "Ljava/util/ArrayList;");
    if(node_field_children == 0) printf("Failed to find Node.children field");
}

jobject create_java_node(JNIEnv *env)
{
    jmethodID constructor = (*env)->GetMethodID(env, node_class, "<init>", "()V");
    if(constructor == 0) printf("Failed to find Node constructor\n");
    jobject obj = (*env)->NewObject(env, node_class, constructor);
    if(obj == 0) printf("Failed to create Node\n");
    
    return obj;
}

void set_java_node_value(JNIEnv *env, jobject jnode, char *value)
{
    jstring java_value = (*env)->NewStringUTF(env, value);
    (*env)->SetObjectField(env, jnode, node_field_value, java_value);
}

void set_java_node_parent(JNIEnv *env, jobject jnode, jobject parent)
{
    (*env)->SetObjectField(env, jnode, node_field_parent, parent);
}

void add_java_node_child(JNIEnv *env, jobject jnode, jobject child)
{
    jobject ar = (*env)->GetObjectField(env, jnode, node_field_children);
    (*env)->CallBooleanMethod(env, ar, arraylist_add, child);
}

char binop_type_to_c(BinOpType type)
{
    switch(type) {
        case BINOP_ADD: return '+';
        case BINOP_SUB: return '-';
        case BINOP_MUL: return '*';
        case BINOP_DIV: return '/';
    }
    return '?';
}

void do_node_2(JNIEnv *env, Node *n, jobject parent)
{
    #define BSIZE 2048
    char buffer[BSIZE];
    switch(n->type) {
        case NODE_BINOP: {
            BinOpNode op = n->binop;
            
            jobject tn = create_java_node(env);
            set_java_node_value(env, tn, "BinOp");
            set_java_node_parent(env, tn, parent);
            
            jobject type = create_java_node(env);
            set_java_node_parent(env, type, tn);
            snprintf(buffer, BSIZE, "Type: %c", binop_type_to_c(op.type));
            set_java_node_value(env, type, buffer);
            add_java_node_child(env, tn, type);
            
            jobject lhs = create_java_node(env);
            set_java_node_parent(env, lhs, tn);
            set_java_node_value(env, lhs, "LHS");
            do_node_2(env, op.lhs, lhs);
            add_java_node_child(env, tn, lhs);
            
            jobject rhs = create_java_node(env);
            set_java_node_parent(env, rhs, tn);
            set_java_node_value(env, rhs, "RHS");
            do_node_2(env, op.rhs, rhs);
            add_java_node_child(env, tn, rhs);
            
            add_java_node_child(env, parent, tn);
        } break;
        case NODE_CONSTANT: {
            jobject obj = create_java_node(env);
            snprintf(buffer, BSIZE, "Constant: %s", n->constant.value);
            set_java_node_value(env, obj, buffer);
            set_java_node_parent(env, obj, parent);
            add_java_node_child(env, parent, obj);
        } break;
        case NODE_IDENT: {
            jobject obj = create_java_node(env);
            snprintf(buffer, BSIZE, "Ident: %s", n->ident.value);
            set_java_node_value(env, obj, buffer); 
            set_java_node_parent(env, obj, parent);
            add_java_node_child(env, parent, obj);
        } break;
        case NODE_FUNCDEF: {
            jobject obj = create_java_node(env);
            set_java_node_value(env, obj, "Funcdef");
            set_java_node_parent(env, obj, parent);
            add_java_node_child(env, parent, obj);
        } break;
        case NODE_FUNCDECL: {
            jobject obj = create_java_node(env);
            set_java_node_value(env, obj, "Funcdecl");
            set_java_node_parent(env, obj, parent);
            add_java_node_child(env, parent, obj);
        } break;
        case NODE_FUNCCALL: {
            jobject obj = create_java_node(env);
            set_java_node_value(env, obj, "Funccall");
            set_java_node_parent(env, obj, parent);
            add_java_node_child(env, parent, obj);
        } break;
        case NODE_VARDECL: {
            jobject obj = create_java_node(env);
            set_java_node_value(env, obj, "Vardecl");
            set_java_node_parent(env, obj, parent);
            add_java_node_child(env, parent, obj);
            
            if(n->vardecl.type == 0) {
                jobject type = create_java_node(env);
                set_java_node_value(env, type, "Type: To be inferred");
                set_java_node_parent(env, type, obj);
                add_java_node_child(env, obj, type);
            } else {
                do_node_2(env, n->vardecl.type, obj);
            }
            
            jobject var = create_java_node(env);
            snprintf(buffer, BSIZE, "Var: %s", n->vardecl.var);
            set_java_node_value(env, var, buffer);
            set_java_node_parent(env, var, obj);
            add_java_node_child(env, obj, var);
        } break;
        case NODE_VARASSIGN: {
            jobject obj = create_java_node(env);
            set_java_node_value(env, obj, "Varassign");
            set_java_node_parent(env, obj, parent);
            add_java_node_child(env, parent, obj);
            
            jobject type = create_java_node(env);
            snprintf(buffer, BSIZE, "Var: %s", n->varassign.var);
            set_java_node_value(env, type, buffer);
            set_java_node_parent(env, type, obj);
            add_java_node_child(env, obj, type);
            
            jobject expr = create_java_node(env);
            set_java_node_value(env, expr, "Expr");
            set_java_node_parent(env, expr, obj);
            do_node_2(env, n->varassign.expr, expr);
            add_java_node_child(env, obj, expr);
        } break;
        case NODE_VARDECLASSIGN: {
            jobject obj = create_java_node(env);
            set_java_node_value(env, obj, "Vardeclassign");
            set_java_node_parent(env, obj, parent);
            add_java_node_child(env, parent, obj);
            
            if(n->vardeclassign.type == 0) {
                jobject type = create_java_node(env);
            set_java_node_value(env, type, "Type: To be inferred");
            set_java_node_parent(env, type, obj);
            add_java_node_child(env, obj, type);
            } else {
            do_node_2(env, n->vardeclassign.type, obj);
            }
            
            jobject var = create_java_node(env);
            snprintf(buffer, BSIZE, "Var: %s", n->vardeclassign.var);
            set_java_node_value(env, var, buffer);
            set_java_node_parent(env, var, obj);
            add_java_node_child(env, obj, var);
            
            jobject expr = create_java_node(env);
            set_java_node_value(env, expr, "Expr");
            set_java_node_parent(env, expr, obj);
            do_node_2(env, n->vardeclassign.expr, expr);
            add_java_node_child(env, obj, expr);
        } break;
        case NODE_BLOCK: {
            jobject obj = create_java_node(env);
            set_java_node_value(env, obj, "Block");
            set_java_node_parent(env, obj, parent);
            add_java_node_child(env, parent, obj);
            
            BlockNode b = n->block;
            jobject count = create_java_node(env);
            snprintf(buffer, BSIZE, "Count: %d", b.count);
            set_java_node_value(env, count, buffer);
            set_java_node_parent(env, count, obj);
            add_java_node_child(env, obj, count);
            
            for(int i = 0; i < b.count; i++) {
                jobject stmt = create_java_node(env);
                set_java_node_parent(env, stmt, obj);
                add_java_node_child(env, obj, stmt);
                set_java_node_value(env, stmt, "Stmt");
                do_node_2(env, b.stmts[i], stmt);
            }
        } break;
        case NODE_WHILE: {
            jobject obj = create_java_node(env);
            set_java_node_value(env, obj, "While");
            set_java_node_parent(env, obj, parent);
            add_java_node_child(env, parent, obj);
            
            WhileNode b = n->nwhile;
            jobject cond = create_java_node(env);
            set_java_node_value(env, cond, "Cond");
            set_java_node_parent(env, cond, obj);
            add_java_node_child(env, obj, cond);
            do_node_2(env, b.condition, cond);
            
            jobject block = create_java_node(env);
            set_java_node_value(env, block, "Block");
            set_java_node_parent(env, block, obj);
            add_java_node_child(env, obj, block);
            do_node_2(env, b.block, block);
        } break;
        case NODE_IF: {
            jobject obj = create_java_node(env);
            set_java_node_value(env, obj, "If");
            set_java_node_parent(env, obj, parent);
            add_java_node_child(env, parent, obj);
            
            IfNode b = n->nif;
            jobject cond = create_java_node(env);
            set_java_node_value(env, cond, "Cond");
            set_java_node_parent(env, cond, obj);
            add_java_node_child(env, obj, cond);
            do_node_2(env, b.condition, cond);
            
            jobject block = create_java_node(env);
            set_java_node_value(env, block, "Block");
            set_java_node_parent(env, block, obj);
            add_java_node_child(env, obj, block);
            do_node_2(env, b.block, block);
            
            
            jobject eblock = create_java_node(env);
            set_java_node_value(env, eblock, "Else Block");
            set_java_node_parent(env, eblock, obj);
            add_java_node_child(env, obj, eblock);
            if(b.else_block) {
            do_node_2(env, b.else_block, eblock);
            }
        } break;
        case NODE_TYPE: {
            jobject obj = create_java_node(env);
            
            int offs = 0;
            
            offs += snprintf(buffer+offs, BSIZE-offs, "Type: ");
            for(int i = 0; i < n->ntype.pointer_count; i++) {
                offs += snprintf(buffer+offs, BSIZE-offs, "*");
            }
            offs += snprintf(buffer+offs, BSIZE-offs, "%s", n->ntype.ident->ident.value);
            for(int i = 0; i < n->ntype.array_count; i++) {
                offs += snprintf(buffer+offs, BSIZE-offs, "[]");
            }
            
            offs += snprintf(buffer+offs, BSIZE-offs, " - Not fully implemented");
            set_java_node_value(env, obj, buffer);
            set_java_node_parent(env, obj, parent);
            add_java_node_child(env, parent, obj);
        } break;
        case NODE_PROGRAM: {
            jobject obj = create_java_node(env);
            set_java_node_value(env, obj, "Program");
            set_java_node_parent(env, obj, parent);
            add_java_node_child(env, parent, obj);
        } break;
    }
}

JNIEnv* create_vm(JavaVM **jvm)
{
    JNIEnv *env;
    JavaVMInitArgs args;
    JavaVMOption options;
    args.version = JNI_VERSION_1_6;
    args.nOptions = 1;
    options.optionString = "-Djava.class.path=./treeview";
    args.options = &options;
    args.ignoreUnrecognized = 0;
    int rv;
    rv = JNI_CreateJavaVM(jvm, (void**)&env, &args);
    if (rv < 0 || !env)
        printf("Unable to Launch JVM %d\n",rv);
    else
        printf("Launched JVM! :)\n");
    return env;
}

void view_ast(Node *n)
{
    JNIEnv *env;
    JavaVM *vm;
    env = create_vm(&vm);
    if(env == 0) {
        return;
    }
    setup_java_node(env);
    
    jobject tn = create_java_node(env);
    set_java_node_value(env, tn, "c_root");
    do_node_2(env, n, tn);
    
    jclass cls = (*env)->FindClass(env, "TreeView");
    if(cls == 0) printf("Cant find TreeView class\n");
    jmethodID create_tree = (*env)->GetStaticMethodID(env, cls, "create_tree", "(LNode;)V");
    if(create_tree == 0) printf("Failed to find TreeView.create_tree");
     (*env)->CallStaticVoidMethod(env, cls, create_tree, tn);
}

#if 0
int main()
{
    view_ast();
    return 0;
}
#endif /* 0 */
