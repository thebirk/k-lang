#include "typesystem.h"

void typesys_init(TypeSystem *pool)
{
    pool->count = 0;
    pool->types = 0;
}

void typesys_add_type(TypeSystem *pool, TypeDef type)
{
    pool->count++;
    pool->types = realloc(pool->types, sizeof(TypeDef)*pool->count);
    pool->types[pool->count-1] = type;
}

 TypeDef typesys_get_type(TypeSystem *pool, char *name)
{
    for(int i = 0; i < pool->count; i++) {
        TypeDef def = pool->types[i];
        if(strcmp(def.name, name) == 0) {
            return def;
        }
    }
    
    static TypeDef def = {TYPE_UNKNWON};
    return def;
}

static TypeDef create_int_type(char *name, int bits, int sign)
{
    TypeDef def = {0};
    
    def.type = TYPE_INTEGER;
    def.name = strdup(name);
    def.i.bits = bits;
    def.i.sign = sign;
    
    return def;
}

static TypeDef create_alias_type(char *alias, char *type)
{
    TypeDef def = {0};
    
    def.type = TYPE_ALIAS;
    def.name = strdup(alias);
    def.a.type_name = strdup(type);
    
    return def;
}

static TypeDef create_float_type()
{
    TypeDef def = {0};
    
    def.type = TYPE_FLOAT;
    def.name = strdup("f32");
    def.f.bits = 32;
    
    return def;
}

static TypeDef create_double_type()
{
    TypeDef def = {0};
    
    def.type = TYPE_FLOAT;
    def.name = strdup("f64");
    def.f.bits = 64;
    
    return def;
}

void add_default_types(TypeSystem *ts)
{
    typesys_add_type(ts, create_int_type("u8", 8, 0));
    typesys_add_type(ts, create_int_type("u16", 16, 0));
    typesys_add_type(ts, create_int_type("u32", 32, 0));
    typesys_add_type(ts, create_int_type("u64", 64, 0));
    
    typesys_add_type(ts, create_int_type("s8", 8, 1));
    typesys_add_type(ts, create_int_type("s16", 16, 1));
    typesys_add_type(ts, create_int_type("s32", 32, 1));
    typesys_add_type(ts, create_int_type("s64", 64, 1));
    
    typesys_add_type(ts, create_alias_type("i8", "u8"));
    typesys_add_type(ts, create_alias_type("i16", "u16"));
    typesys_add_type(ts, create_alias_type("i32", "u32"));
    typesys_add_type(ts, create_alias_type("i64", "u64"));
    
    typesys_add_type(ts, create_float_type());
    typesys_add_type(ts, create_alias_type("float", "f32"));
    
    typesys_add_type(ts, create_double_type());
    typesys_add_type(ts, create_alias_type("double", "f64"));
}
