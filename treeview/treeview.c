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

STRUCT(TreeNode) {
    TreeNode *parent;
    char *value;
    TreeNode **children;
    int child_count;
};

 TreeNode* make_tnode()
{
    TreeNode *n = (TreeNode*) malloc(sizeof(TreeNode));
    n->parent = 0;
    n->value = 0;
    n->children = 0;
    n->child_count = 0;
    return n;
}

char* new_string(const char *str)
{
    return strdup(str);
}

void add_child(TreeNode *parent, TreeNode *child)
{
    child->parent = parent;
    parent->child_count++;
    parent->children = (TreeNode**) realloc(parent->children, sizeof(TreeNode*)*parent->child_count);
    parent->children[parent->child_count-1] = child;
}

void add_child_value(TreeNode *parent, char *value)
{
    TreeNode *child = make_tnode();
    child->value = new_string(value);
    add_child(parent, child);
}

void do_node(Node *n, TreeNode* parent)
{
    switch(n->type) {
        case NODE_BINOP: {
            BinOpNode op = n->binop;
            
            TreeNode *tn = make_tnode();
            tn->parent = parent;
            tn->value = new_string("BinOp");
            add_child_value(tn, "Type 0");
            
            TreeNode *lhs = make_tnode();
            lhs->value = new_string("LHS");
            do_node(op.lhs, lhs);
            TreeNode *rhs = make_tnode();
            rhs->value = new_string("RHS");
            do_node(op.rhs, rhs);
            
            add_child(parent, tn);
        } break;
            case NODE_CONSTANT: {
                add_child_value(parent, "Constant");
            } break;
        case NODE_IDENT: {
            add_child_value(parent, "Ident");
        } break;
        case NODE_FUNCDEF: {
            add_child_value(parent, "Funcdef");
        } break;
        case NODE_FUNCDECL: {
            add_child_value(parent, "Funcdecl");
        } break;
        case NODE_FUNCCALL: {
            add_child_value(parent, "Funccall");
        } break;
        case NODE_VARDECL: {
            add_child_value(parent, "Vardecl");
        } break;
        case NODE_VARASSIGN: {
            add_child_value(parent, "Varassign");
        } break;
        case NODE_PROGRAM: {
            add_child_value(parent, "Program");
        } break;
    }
}

TreeNode* ast_to_tnode(Node *ast)
{
    TreeNode *root = make_tnode();
    root->value = new_string("root");
    
    do_node(ast, root);
    
    return root;
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

void do_node_2(JNIEnv *env, Node *n, jobject parent)
{
    char buffer[2048];
    switch(n->type) {
        case NODE_BINOP: {
            BinOpNode op = n->binop;
            
            jobject tn = create_java_node(env);
            set_java_node_value(env, tn, "BinOp");
            set_java_node_parent(env, tn, parent);
            
            jobject type = create_java_node(env);
            set_java_node_parent(env, type, tn);
            set_java_node_value(env, type, "Type: +");
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
            snprintf(buffer, 2048, "Constant: %s", n->constant.value);
            set_java_node_value(env, obj, buffer);
            set_java_node_parent(env, obj, parent);
            add_java_node_child(env, parent, obj);
        } break;
        case NODE_IDENT: {
            jobject obj = create_java_node(env);
            set_java_node_value(env, obj, "Ident");
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
        } break;
        case NODE_VARASSIGN: {
            jobject obj = create_java_node(env);
            set_java_node_value(env, obj, "Varassign");
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
    options.optionString = "-Djava.class.path=./";
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
