#ifndef INTERCODE_H
#define INTERCODE_H

#include <stdio.h>
#include <stdlib.h>
#include "semantic.h"

// 定义了Operand和InterCode这两种类型的别名
// 分别指向了结构体Operand_d和InterCode_d
typedef struct Operand_d Operand_;
typedef Operand_* Operand;
typedef struct InterCode_d InterCode_;
typedef InterCode_* InterCode;

// 中间代码的操作数（变量，临时变量，标记，函数）
struct Operand_d {
    enum {
        //变量、临时变量、常量、标签、函数、取地址操作数、解引用操作数
        VARIABLE_OP, TEMP_VAR_OP, CONSTANT_OP, LABEL_OP, 
        FUNCTION_OP, GET_ADDR_OP, GET_VAL_OP
    } kind;
    //用于存储具体的值
    union {
        int no;         // 临时变量的编号，标记的编号
        int value;      // 常量的值
        char name[32];  // 函数以及变量的名字
        Operand opr;    // 取地址和解引用指向的操作数
    };
    Type type;          // 存放数组/结构体类型变量的类型
    Operand next;       // 给argList链接操作数链表
};

// 中间代码的单条指令
struct InterCode_d {
    enum {
        //标签、函数、赋值、加法、减法、乘法
        //除法、存储到内存、无条件跳转
        //条件跳转、返回、变量声明、参数、函数调用、函数参数
        //读、写、空指令
        LABEL_IR, FUNC_IR, ASSIGN_IR, PLUS_IR, SUB_IR, MUL_IR, 
        DIV_IR, TO_MEM_IR, GOTO_IR,
        IF_GOTO_IR, RETURN_IR, DEC_IR, ARG_IR, CALL_IR, PARAM_IR,
        READ_IR, WRITE_IR, NULL_IR
    } kind;
    // 操作数指针数组，用于存储操作数
    Operand ops[3];
    // 额外信息
    union {
        char relop[32];    //条件跳转的关系运算符
        int size;          //条件跳转变量的大小
    };
    // 前一条和后一条指令
    InterCode pre;
    InterCode next;
};

void initInterCodes();      //初始化中间代码
void insertInterCode(InterCode code, InterCode interCodes);     //插入指令
void printInterCodes(char* name);                               //打印中间代码
void printOperand(Operand op, FILE* fp);                        //打印操作数

InterCode translateExp(Node* root, Operand place);              //翻译表达式节点，并将结果存储到place操作数中
InterCode translateArgs(Node* root, Operand argList);           //翻译参数节点，并且将参数列表存储到arglist中
InterCode translateStmt(Node* root);                            //翻译语句节点，
InterCode translateCond(Node* root, Operand labelTrue, Operand labelFalse);          //翻译条件表达式节点，并且生成条件跳转指令
void translateProgram(Node* root);                              //翻译整个程序的语法树
InterCode translateExtDefList(Node* root);                      //翻译外部定义列表节点 
InterCode translateExtDef(Node* root);                          //翻译外部定义节点
InterCode translateCompSt(Node* root, char* funcName);          //翻译复合语句节点，并且传递当前所在的函数名
InterCode translateStmtList(Node* root);                        //用于翻译语句列表节点
InterCode translateDefList(Node* root, IdType class);           //用于翻译定义列表节点
InterCode translateDef(Node* root, IdType class);               //翻译定义节点
FieldList translateDecList(Node* root, Type type, IdType class, InterCode code);      //翻译声明列表节点，并且根据类型和类别生成变量的符号表项
FieldList translateDec(Node* root, Type type, IdType class, InterCode code);          //翻译声明节点，并且根据类型和类别生成变量的符号表项

#endif