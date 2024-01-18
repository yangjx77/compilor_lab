#ifndef SEMANTIC_H
#define SEMANTIC_H

#include <stdio.h>
#include <stdlib.h>
#include "Tree.h"

// 两个基本类型
#define INT_TYPE 0
#define FLOAT_TYPE 1

// 哈希表大小
#define HASH_SIZE 1024

typedef struct Type_d Type_;
typedef Type_* Type;
typedef struct FieldList_d FieldList_;
typedef FieldList_* FieldList;
typedef struct Structure_d Structure_;
typedef Structure_* Structure;
typedef struct Function_d Function_;
typedef Function_* Function;
typedef struct Entry_d Entry_;
typedef Entry_* Entry;

// 定义了类型的种类
typedef enum {
    ENUM_BASIC,//基本类型
    ENUM_ARRAY,//数组类型
    ENUM_STRUCT,//结构体类型
    ENUM_STRUCT_DEF,//结构体定义类型
    ENUM_FUNC//函数类型
} Kind;

//定义了标识符（变量或字段）的种类
typedef enum {
    ENUM_VAR,//变量
    ENUM_FIELD//字段
} IdType;

//定义了类型的信息
struct Type_d {
    Kind kind;
    union {
        // 基本类型
        int basic;
        // 数组类型包括元素类型与数组大小构成
        struct {
            Type elem;
            int size;
        } array;
        // 结构体类型是一个链表
        Structure structure;
        // 函数类型
        Function func;
    };
};

// 结构体域链表节点
struct FieldList_d {
    // 域的名字
    char name[32];
    // 域的类型
    Type type;
    // 指向下一个域的指针
    FieldList next;
};

// 结构体类型
struct Structure_d {
    char name[32];
    FieldList head;
};

// 函数类型
struct Function_d {
    // 函数的名称
    char name[32];
    // 返回值类型
    Type returnType;
    // 参数个数
    int parmNum;
    // 参数链表头节点指针
    FieldList head;
    // 是否已经定义
    int hasDefined;
    // 所在行数
    int lineno;
};

// 符号表条目类型
struct Entry_d {
    // 条目的名称
    char name[32];
    // 条目的类型
    Type type;
    // 指向同一槽位的下一个条目
    Entry hashNext;
    // 指向同一层次的下一个条目
    Entry layerNext;
};

void semantic_analyse(Node* root);//语义分析函数的入口，接受语法树的根节点作为参数
void Program(Node* root);//对整个程序进行语义分析的函数
void check();//执行类型检查和符号表处理的函数
void ExtDefList(Node* root);//处理外部定义列表的函数
void ExtDef(Node* root);//处理外部定义的函数
Type Specifier(Node* root);//处理类型说明符的函数
void ExtDecList(Node* root, Type type);//处理外部变量声明列表的函数
Function FunDec(Node* root);//处理变量声明的函数
void CompSt(Node* root, char* funcName, Type reType);//处理复合语句的函数
Type StructSpecifier(Node* root);//处理结构体的定义或声明
FieldList DefList(Node* root, IdType class);//处理变量定义列表
FieldList Def(Node* root, IdType class);//处理变量定义
FieldList DecList(Node* root, Type type, IdType class);//处理便变量声明列表
FieldList Dec(Node* root, Type type, IdType class);//处理变量声明
FieldList VarDec(Node* root, Type type, IdType class);//处理变量声明中的变量名部分的函数
FieldList VarList(Node* root);//处理函数参数列表的函数
FieldList ParamDec(Node* root);//处理函数参数声明的函数
void StmtList(Node* root, Type retype);//处理语句列表的函数
void Stmt(Node* root, Type reType);//处理语句的函数
Type Exp(Node* root);//处理表达式的函数
FieldList Args(Node* root);//处理函数调用参数列表的函数
void printArgs(FieldList head);//打印参数列表的函数。
void printType(Type type);//打印类型的函数

#endif