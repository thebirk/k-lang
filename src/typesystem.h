#ifndef KOS_TYPESYSTEM_H
#define KOS_TYPESYSTEM_H

ENUM(int, TypeDefType)
{
    TYPE_UNKNWON = -1,
    TYPE_INTEGER = 0,
    TYPE_FLOAT,
    TYPE_ALIAS,
    TYPE_VOID,
    TYPE_STRUCT,
};

STRUCT(IntegerType)
{
    int bits;
    int sign;
};

STRUCT(FloatType)
{
    int bits;
};

STRUCT(AliasType)
{
    char *type_name;
};

STRUCT(StructType)
{
    int field_count;
    char *type_names;
};

STRUCT(TypeDef)
{
    TypeDefType type;
    char *name;
    
    union {
        IntegerType i;
        FloatType f;
        AliasType a;
        StructType s;
    };
};

STRUCT(TypeSystem)
{
    int count;
    TypeDef *types;
};

void typesys_init(TypeSystem *pool);
void typesys_add_type(TypeSystem *pool, TypeDef type);
TypeDef typesys_get_type(TypeSystem *pool, char *name);

void add_default_types();

#endif /* KOS_TYPESYSTEM_H */