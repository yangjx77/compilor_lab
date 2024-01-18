#include "intercode.h"

//声明了两个外部变量，分别用于存储符号表的条目和指向符号表中各个作用域的链表的头节点
extern Entry symbolTable[HASH_SIZE];
extern Entry layersHead;

// 存储中间代码指令的双向链表的链表头节点
InterCode interCodes;

// 临时变量，标记的全局编号
int tmpVarNo = 1;     //生成临时变量的唯一标识符
int labelNo = 1;      //生成标签的唯一标识符

// 初始化双向链表
void initInterCodes() {}

// 向指令链表尾部插入多条指令组成的双向链表
void insertInterCode(InterCode code, InterCode interCodes) {
    //首先，检查参数interCodes是否为NULL，如果是，则输出错误信息并返回,这是为了确保操作的链表不为空
    if (interCodes == NULL) {
        printf("Cannot insert code to a null interCodes!\n");
        return;
    }
    //接下来，检查参数code是否为 NULL，如果是，则输出提示信息并返回,这是为了避免将空代码插入到链表中
    if (code == NULL) {
        printf("Inserting a null code to interCodes has nothing to do.\n");
        return;
    }
    //如果interCodes的下一个节点为NULL，说明链表中只有一个节点
    if (interCodes->next == NULL) {
        //在这种情况下，将code插入到interCodes的下一个位置，并进行相应的链接操作
        interCodes->next = code;
        //检查code的下一个节点是否为NULL
        //如果是，说明code即为链表的最后一个节点
        if (code->next == NULL) {
            //将code的下一个节点指向interCodes
            code->next = interCodes;
            //然后进行双向链接，将code的前一个节点指向interCodes
            code->pre = interCodes;
            //同时将interCodes的前一个节点指向code
            interCodes->pre = code;
        }
        //如果code的下一个节点不为NULL，说明code不是链表的最后一个节点
        else {        
            //将code前一个节点的下一个节点指向interCodes
            code->pre->next = interCodes;
            //然后进行双向链接，将interCodes的前一个节点指向code前一个节点
            interCodes->pre = code->pre;
            //同时将code的前一个节点指向interCodes
            code->pre = interCodes;
        }
    }
    //如果interCodes的下一个节点不为 NULL，说明链表中有多个节点
    else {
        //将code插入到interCodes前一个节点的下一个位置，并进行相应的链接操作
        interCodes->pre->next = code;
        //检查code的下一个节点是否为NULL
        //如果是，说明code即为链表的最后一个节点
        if (code->next == NULL) {
            //将code的下一个节点指向interCodes
            code->next = interCodes;
            //然后进行双向链接，将code的前一个节点指向interCodes前一个节点
            code->pre = interCodes->pre;
            //同时将interCodes的前一个节点指向code
            interCodes->pre = code;
        }
        //如果code的下一个节点不为NULL，说明code不是链表的最后一个节点

        else {
            //将 code前一个节点的下一个节点指向interCodes
            code->pre->next = interCodes;
            //然后进行双向链接，将code的前一个节点保存到临时变量codePre中
            InterCode codePre = code->pre;
            //将code的前一个节点指向interCodes前一个节点
            code->pre = interCodes->pre;
            //同时将interCodes的前一个节点指向codePre
            interCodes->pre = codePre;
        }
    }
    return;
}

// 用于向指定文件中打印中间代码
void printInterCodes(char* name) {
    //打开指定名称的文件以供写入，创建一个指向文件的指针fp
    FILE* fp = fopen(name, "w");
    //如果文件打开失败（返回值为 NULL），则输出错误信息并返回
    if (fp == NULL) {
        printf("Cannot open file %s", name);
        return;
    }
    //创建一个指向中间代码链表的当前节点的指针curr，将其初始化为链表的头节点interCodes
    InterCode curr = interCodes;
    //创建一个标志变量flag，用于控制循环的终止条件
    int flag = 1;
    //进入循环，循环条件为flag为1或者curr不等于链表的头节点interCodes
    //这样可以确保循环至少执行一次，并遍历整个链表
    while (flag == 1 || curr != interCodes) {
        //将flag设置为0，表示当前循环已经执行过一次
        flag = 0;
        //使用switch语句根据当前节点的指令类型kind执行相应的操作
        switch(curr->kind) {
            //根据不同的指令类型，使用fputs()函数将相应的中间代码字符串写入文件

            case LABEL_IR:            //标签类型
                fputs("LABEL ", fp);
                printOperand(curr->ops[0], fp);
                fputs(" :", fp);
                break;
            case FUNC_IR:             //函数类型
                fputs("FUNCTION ", fp);
                printOperand(curr->ops[0], fp);
                fputs(" :", fp);
                break;
            case ASSIGN_IR:           //赋值类型
                printOperand(curr->ops[0], fp);
                fputs(" := ", fp);
                printOperand(curr->ops[1], fp);
                break;
            case PLUS_IR:              //加法类型
                printOperand(curr->ops[0], fp);
                fputs(" := ", fp);
                printOperand(curr->ops[1], fp);
                fputs(" + ", fp);
                printOperand(curr->ops[2], fp);
                break;
            case SUB_IR:               //减法类型
                printOperand(curr->ops[0], fp);
                fputs(" := ", fp);
                printOperand(curr->ops[1], fp);
                fputs(" - ", fp);
                printOperand(curr->ops[2], fp);
                break;
            case MUL_IR:               //乘法类型
                printOperand(curr->ops[0], fp);
                fputs(" := ", fp);
                printOperand(curr->ops[1], fp);
                fputs(" * ", fp);
                printOperand(curr->ops[2], fp);
                break;
            case DIV_IR:                //除法类型
                printOperand(curr->ops[0], fp);
                fputs(" := ", fp);
                printOperand(curr->ops[1], fp);
                fputs(" / ", fp);
                printOperand(curr->ops[2], fp);
                break;
            case TO_MEM_IR:             //存储到内存类型
                fputs("*", fp);
                printOperand(curr->ops[0], fp);
                fputs(" := ", fp);
                printOperand(curr->ops[1], fp);
                break;
            case GOTO_IR:               //无条件跳转类型
                fputs("GOTO ", fp);
                printOperand(curr->ops[0], fp);
                break;
            case IF_GOTO_IR:            //有条件跳转类型
                fputs("IF ", fp);
                printOperand(curr->ops[0], fp);
                fputs(" ", fp);
                fputs(curr->relop, fp);
                fputs(" ", fp);
                printOperand(curr->ops[1], fp);
                fputs(" GOTO ", fp);
                printOperand(curr->ops[2], fp);
                break;
            case RETURN_IR:             //返回类型
                fputs("RETURN ", fp);
                printOperand(curr->ops[0], fp);
                break;
            case DEC_IR:                //变量声明类型
                fputs("DEC ", fp);
                printOperand(curr->ops[0], fp);
                char str[32];
                sprintf(str, " %d", curr->size);
                fputs(str, fp);
                break;
            case ARG_IR:                //参数类型
                fputs("ARG ", fp);
                printOperand(curr->ops[0], fp);
                break;
            case CALL_IR:               //函数调用类型
                printOperand(curr->ops[0], fp);
                fputs(" := CALL ", fp);
                printOperand(curr->ops[1], fp);
                break;
            case PARAM_IR:              //函数参数类型
                fputs("PARAM ", fp);
                printOperand(curr->ops[0], fp);
                break;
            case READ_IR:               //读指令类型
                fputs("READ ", fp);
                printOperand(curr->ops[0], fp);
                break;
            case WRITE_IR:              //写指令类型
                fputs("WRITE ", fp);
                printOperand(curr->ops[0], fp);
                break;
            default:                     //其它(空指令)类型
                break;
        }
        //空指令什么也不输出，也不需要换行
        //对于非空指令类型，输出换行符，以便下一行指令在新的一行显示
        if (curr->kind != NULL_IR)
            fputs("\n", fp);
        //刷新文件缓冲区，确保数据被写入文件
        fflush(fp);
        //将curr指针移动到链表的下一个节点，继续处理下一条指令
        curr = curr->next;
    }
    //关闭打开的文件，释放资源
    fclose(fp);
}

// 向指定文件中打印操作数
void printOperand(Operand op, FILE* fp) {
    //首先检查操作数是否为NULL
    //如果操作数为NULL，说明没有有效的操作数，将字符串"null"写入文件中，然后返回
    if (op == NULL) {
        fputs("null", fp);
        return;
    }
    //如果操作数不为NULL
    //创建一个长度为32的字符数组out，用于存储操作数的字符串表示
    char out[32];
    //根据操作数的类型kind执行相应的操作
    switch (op->kind) {
        //根据不同的操作数类型，使用sprintf()将操作数转换为字符串格式，并将其存储在out数组中
        //使用fputs()函数将out数组中的字符串写入文件
        case VARIABLE_OP:                  //变量类型
            sprintf(out, "%s", op->name);
            fputs(out, fp);
            break;
        case TEMP_VAR_OP:                  //临时变量类型
            sprintf(out, "t%d", op->no);
            fputs(out, fp);
            break;
        case CONSTANT_OP:                  //常量类型
            sprintf(out, "#%d", op->value);
            fputs(out, fp);
            break;
        case LABEL_OP:                     //标签类型
            sprintf(out, "label%d", op->no);
            fputs(out, fp);
            break;
        case FUNCTION_OP:                   //函数类型
            sprintf(out, "%s", op->name);
            fputs(out, fp);
            break;
        case GET_ADDR_OP:                   //取地址操作数类型
            fputs("&", fp);
            printOperand(op->opr, fp);
            break;
        case GET_VAL_OP:                    //解引用操作数类型
            fputs("*", fp);
            printOperand(op->opr, fp);
            break;
        default:                            //其它操作数类型
            break;
    }
    return;
}

// 下面开始创建不同类型的操作数变量
// 创建临时变量
Operand newTemp() {
    //使用malloc()函数为临时变量分配内存空间，大小为Operand_结构体的大小，tmpVar是一个指向临时变量操作数对象的指针
    Operand tmpVar = (Operand)malloc(sizeof(Operand_));
    //将临时变量的类型设置为临时变量操作数类型
    tmpVar->kind = TEMP_VAR_OP;
    //将临时变量的编号设置为全局的tmpVarNo
    tmpVar->no = tmpVarNo;
    //将全局的tmpVarNo++，以便为下一个临时变量分配不同的编号
    tmpVarNo++;
    //返回创建的临时变量的操作数对象
    return tmpVar;
}

// 创建临时标记
Operand newLabel() {
    //使用malloc()函数为临时标记分配内存空间，大小为Operand_结构体的大小，其中label是一个指向标签操作数对象的指针
    Operand label = (Operand)malloc(sizeof(Operand_));
    //将临时标记的类型设置为标签操作数类型
    label->kind = LABEL_OP;
    //将临时标记的编号设置为labelNo
    label->no = labelNo;
    //将labelNo++，以便为下一个标签变量分配不同的编号
    labelNo++;
    //返回创建的临时标签的操作数对象
    return label;
}

// 创建常量
Operand getValue(int num) {
    //使用malloc()函数为常量操作数分配内存空间，大小为Operand_结构体的大小，其中cons是一个指向常量操作数对象的指针
    Operand cons = (Operand)malloc(sizeof(Operand_));
    //将常量操作数的类型设置为常量操作数类型
    cons->kind = CONSTANT_OP;
    //将常量操作数的值设置为传入的整数参数num
    cons->value = num;
    //返回创建的常量操作数的操作数对象
    return cons;
}

// 创建变量操作数
Operand getVar(char* name) {
    //使用malloc()函数为变量操作数分配内存空间，大小为Operand_结构体的大小，其中var是一个指向变量操作数对象的指针
    Operand var = (Operand)malloc(sizeof(Operand_));
    //将变量操作数的类型设置为变量操作数类型
    var->kind = VARIABLE_OP;
    //在变量操作数名前面加上一个v，防止某些名字和临时变量名重名
    sprintf(var->name, "v%s", name);
    //返回创建的变量操作数的操作数对象
    return var;
}

// 创建函数操作数
Operand getFunc(char* name) {
    //使用malloc()函数为函数操作数分配内存空间，大小为Operand_结构体的大小，其中func是一个指向函数操作数对象的指针
    Operand func = (Operand)malloc(sizeof(Operand_));
    //将函数操作数的类型设置为函数操作数类型
    func->kind = FUNCTION_OP;
    //使用strcpy()函数将传入的字符串参数name复制到函数操作数的名称中
    strcpy(func->name, name);
    //返回创建的函数操作数的操作数对象
    return func;
}

// 对某个操作数取地址
Operand getAddr(Operand op) {
    //使用malloc()函数为取地址操作数分配内存空间，大小为Operand_结构体的大小，其中addr是一个指向对某个操作数取地址的操作数对象的指针
    Operand addr = (Operand)malloc(sizeof(Operand_));
    //将取地址操作数的类型设置为取地址操作数类型
    addr->kind = GET_ADDR_OP;
    //将取地址操作数的操作数对象设置为传入的操作数对象op
    addr->opr = op;
    //返回创建的取地址操作数的操作数对象
    return addr;
}

// 对某个操作数解引用
Operand getVal(Operand op) {
    //使用 malloc()函数为解引用操作数分配内存空间，大小为Operand_结构体的大小，其中val是一个指向对某个操作数解引用的操作数对象的指针
    Operand val = (Operand)malloc(sizeof(Operand_));
    //将解引用操作数的类型设置为解引用操作数类型
    val->kind = GET_VAL_OP;
    //将解引用操作数的操作数对象设置为传入的操作数对象op
    val->opr = op;
    //返回创建的解引用操作数的操作数对象
    return val;
}

// 将src操作数拷贝给dest操作数
void operandCpy(Operand dest, Operand src) {
    //将目标操作数dest的类型（kind）设置为源操作数src的类型
    dest->kind = src->kind;
    //如果目标操作数dest的类型是临时变量操作数类型或标签操作数类型，将目标操作数的编号（no）设置为源操作数的编号
    if (dest->kind == TEMP_VAR_OP || dest->kind == LABEL_OP)
        dest->no = src->no;
    //如果目标操作数dest的类型是常量操作数类型，将目标操作数的值（value）设置为源操作数的值
    else if (dest->kind == CONSTANT_OP)
        dest->value = src->value;
    //如果目标操作数dest的类型是变量操作数类型或函数操作数类型，使用strcpy()函数将源操作数的名称复制到目标操作数的名称中
    else if (dest->kind == VARIABLE_OP || dest->kind == FUNCTION_OP)
        strcpy(dest->name, src->name);
    //对于其他类型的操作数，将目标操作数的操作数对象（opr）设置为源操作数的操作数对象
    else
        dest->opr = src->opr;
    //将目标操作数的类型（type）设置为源操作数的类型
    dest->type = src->type;
    //将目标操作数的下一个操作数（next）设置为源操作数的下一个操作数
    dest->next = src->next;
    return;
}

// 计算结构体或数组变量的大小（字节数）
int getSize(Type type) {
    //如果类型对象type的类型种类为基本类型且为整型（int），返回4，表示整型占据4个字节
    if (type->kind == ENUM_BASIC && type->basic == INT_TYPE)
        return 4;
    //如果类型对象type的类型种类为数组
    else if (type->kind == ENUM_ARRAY)
        //返回数组的大小（size）乘以数组元素的大小（通过递归调用 getSize() 函数计算）
        return type->array.size * getSize(type->array.elem);
    //如果类型对象type的类型种类为结构体
    else if (type->kind == ENUM_STRUCT) {
        //创建一个指向结构体字段链表头部的指针head
        FieldList head = type->structure->head;
        //初始化一个变量sum，用于累计字段大小的总和
        int sum = 0;
        //进入循环，遍历结构体的字段链表，直到链表末尾（即 head 为 NULL）
        while (head != NULL) {
            //获取当前字段的类型对象head->type的大小（通过递归调用getSize()函数计算）
            int tmp = getSize(head->type);
            // 按四字节对齐
            if (tmp % 4 != 0)
                tmp = ((tmp / 4) + 1) * 4;
            //将调整后的字段大小加到总和sum中
            sum += tmp;
            //将指针head移动到下一个字段
            head = head->next;
        }
        //将计算得到的变量大小返回
        return sum;
    }
}

// 获取一条空指令
InterCode getNullInterCode() {
    //使用malloc()函数为InterCode结构体分配内存空间，大小为InterCode_结构体的大小，其中code1是一个指向InterCode结构体的指针
    InterCode code1 = (InterCode)malloc(sizeof(InterCode_));
    //将InterCode结构体的类型（kind）设置为NULL_IR，表示空指令类型
    code1->kind = NULL_IR;
    //返回创建的空指令
    return code1;
}

// 优化加法
// *** todo ***
/*
You need to finish Plus operation. You can refer to other subtraction and similar operations
*/
InterCode optimizePLUSIR(Operand dest, Operand src1, Operand src2) {
    //如果src1和src2都是常量操作数类型
    if(src1->kind==CONSTANT_OP&&src2->kind==CONSTANT_OP){
        //调用operandCpy()函数，将计算得到的结果（src1+src2）拷贝给目标操作数dest，而getValue()函数用于获取常量操作数的值
        operandCpy(dest,getValue(src1->value+src2->value));
        return getNullInterCode();
    }
    //如果src1是常量且为0，并且src2不为获取地址操作数类型或获取值操作数类型
    else if(src1->kind==CONSTANT_OP&&src1->value==0&&
            src2->kind!=GET_ADDR_OP&&src2->kind!=GET_VAL_OP){
        //直接将src2拷贝到目标操作数dest
        operandCpy(dest,src2);
        return getNullInterCode();
    }
    else if(src2->kind==CONSTANT_OP&&src2->value==0&&
            src1->kind!=GET_ADDR_OP&&src1->kind!=GET_VAL_OP){
        //直接将src1拷贝到目标操作数dest
        operandCpy(dest,src1);
        return getNullInterCode();
    }
    //其它情况
    else{
        //使用malloc()函数为InterCode结构体分配内存空间，大小为InterCode_结构体的大小，而code1是一个指向InterCode结构体的指针
        InterCode code1 = (InterCode)malloc(sizeof(InterCode_));
        //将InterCode结构体的类型（kind）设置为PLUS_IR，表示加法指令类型
        code1->kind = PLUS_IR;
        //将目标操作数dest存储在InterCode结构体的第一个操作数位置
        code1->ops[0] = dest;
        //将源操作数src1存储在InterCode结构体的第二个操作数位置
        code1->ops[1] = src1;
        //将源操作数src2存储在InterCode结构体的第三个操作数位置
        code1->ops[2] = src2;
        //返回创建的加法指令
        return code1;
    }
}

// 优化减法
InterCode optimizeSUBIR(Operand dest, Operand src1, Operand src2) {
    //如果src1和src2都是常量操作数类型
    if (src1->kind == CONSTANT_OP && src2->kind == CONSTANT_OP) {
        //调用operandCpy()函数，将计算得到的结果（src1-src2）拷贝给目标操作数dest，而getValue()函数用于获取常量操作数的值
        operandCpy(dest, getValue(src1->value - src2->value));
        //返回一条空指令，表示该优化后的减法指令不需要执行实际的减法操作
        return getNullInterCode();
    }
    //如果src2是常量操作数类型且值为0，并且src1不是获取地址操作数类型或获取值操作数类型
    else if (src2->kind == CONSTANT_OP && src2->value == 0 &&
             src1->kind != GET_ADDR_OP && src1->kind != GET_VAL_OP) {
        //直接将src1拷贝给目标操作数dest
        operandCpy(dest, src1);
        //返回一条空指令，表示该优化后的减法指令不需要执行实际的减法操作
        return getNullInterCode();
    }
    //其它情况
    else {
        //使用malloc()函数为InterCode结构体分配内存空间，大小为InterCode_结构体的大小，而code1是一个指向InterCode结构体的指针
        InterCode code1 = (InterCode)malloc(sizeof(InterCode_));
        //将InterCode结构体的类型（kind）设置为SUB_IR，表示减法指令类型
        code1->kind = SUB_IR;
        //将目标操作数dest存储在InterCode结构体的第一个操作数位置
        code1->ops[0] = dest;
        //将源操作数src1存储在InterCode结构体的第二个操作数位置
        code1->ops[1] = src1;
        //将源操作数src2存储在InterCode结构体的第三个操作数位置
        code1->ops[2] = src2;
        //返回创建的减法指令
        return code1;
    }
}

// 优化乘法
// *** todo ***
/*
You need to finish Mul operation. You can refer to other Div and similar operations
*/
InterCode optimizeMULIR(Operand dest, Operand src1, Operand src2) {
    //如果src1和src2都是常量操作数类型
    if(src1->kind==CONSTANT_OP&&src2->kind==CONSTANT_OP){
        //调用operandCpy()函数，将计算得到的结果（src1*src2）拷贝给目标操作数dest，而getValue()函数用于获取常量操作数的值
        operandCpy(dest, getValue(src1->value * src2->value));
        return getNullInterCode();
    }
    //如果src1是常量操作数类型且值为1，并且src2不是获取地址操作数类型或获取值操作数类型
    else if((src1->kind==CONSTANT_OP&&src1->value==0)||
            (src2->kind==CONSTANT_OP&&src2->value==0)){
        //直接将0拷贝给目标操作数dest
        operandCpy(dest,getValue(0));
        return getNullInterCode();
    }
    //如果src2是常量操作数类型且值为1，并且src1不是获取地址操作数类型或获取值操作数类型
    else if(src2->kind==CONSTANT_OP&&src2->value==1&&
            src1->kind!=GET_ADDR_OP&&src1->kind!=GET_VAL_OP){
        //直接将src1拷贝到目标操作数dest
        operandCpy(dest,src1);
        return getNullInterCode();
    }
    else if(src1->kind==CONSTANT_OP&&src1->value==1&&
            src2->kind!=GET_ADDR_OP&&src2->kind!=GET_VAL_OP){
        //直接将src2拷贝到目标操作数dest中
        operandCpy(dest,src2);
        return getNullInterCode();
    }
    else{
        //使用malloc()函数为InterCode结构体分配内存空间，大小为InterCode_结构体的大小，而code1是一个指向InterCode结构体的指针
        InterCode code1 = (InterCode)malloc(sizeof(InterCode_));
        //将InterCode结构体的类型（kind）设置为MUL_IR，表示乘法指令类型
        code1->kind = MUL_IR;
        //将目标操作数dest存储在InterCode结构体的第一个操作数位置
        code1->ops[0] = dest;
        //将源操作数src1存储在InterCode结构体的第二个操作数位置
        code1->ops[1] = src1;
        //将源操作数src2存储在InterCode结构体的第三个操作数位置
        code1->ops[2] = src2;
        //返回创建的乘法指令
        return code1;
    }
}

// 优化除法
InterCode optimizeDIVIR(Operand dest, Operand src1, Operand src2) {
    //如果src1和src2都是常量操作数类型
    if (src1->kind == CONSTANT_OP && src2->kind == CONSTANT_OP) {
        //调用operandCpy()函数，将计算得到的结果（src1/src2）拷贝给目标操作数dest，而getValue()函数用于获取常量操作数的值
        operandCpy(dest, getValue(src1->value / src2->value));
        //返回一条空指令，表示该优化后的减法指令不需要执行实际的减法操作
        return getNullInterCode();
    }
    //如果src1是常量操作数类型且值为0
    else if (src1->kind == CONSTANT_OP && src1->value == 0) {
        //直接将0拷贝给目标操作数dest
        operandCpy(dest, getValue(0));
        //返回一条空指令，表示该优化后的减法指令不需要执行实际的减法操作
        return getNullInterCode();
    }
    //如果src2是常量操作数类型且值为1，并且src1不是获取地址操作数类型或获取值操作数类型
    else if (src2->kind == CONSTANT_OP && src2->value == 1 && 
             src1->kind != GET_ADDR_OP && src1->kind != GET_VAL_OP) {
        //直接将src1拷贝给目标操作数dest
        operandCpy(dest, src1);
        //返回一条空指令，表示该优化后的减法指令不需要执行实际的减法操作
        return getNullInterCode();
    }
    //其它情况
    else {
        //使用malloc()函数为InterCode结构体分配内存空间，大小为InterCode_结构体的大小，而code1是一个指向InterCode结构体的指针
        InterCode code1 = (InterCode)malloc(sizeof(InterCode_));
        //将InterCode结构体的类型（kind）设置为DIV_IR，表示除法指令类型
        code1->kind = DIV_IR;
        //将目标操作数dest存储在InterCode结构体的第一个操作数位置
        code1->ops[0] = dest;
        //将源操作数src1存储在InterCode结构体的第二个操作数位置
        code1->ops[1] = src1;
        //将源操作数src2存储在InterCode结构体的第三个操作数位置
        code1->ops[2] = src2;
        //返回创建的除法指令
        return code1;
    }
}

// 找到语句链表中的最后一条非空语句或者空语句（如果没有非空语句的话）   
InterCode findLastInterCode(InterCode code) {
    //创建一个指向code的前一个语句的指针last
    InterCode last = code->pre;
    //进入循环，条件是last的类型（kind）为NULL_IR（空语句）且last不等于code（表示还未到达链表的起始位置）
    while (last->kind == NULL_IR && last != code)
        //将last移动到前一个语句的位置，即更新last指针为上一条语句
        last = last->pre;
    //返回找到的最后一条非空语句或空语句（如果没有非空语句）
    return last;
}

// 优化后面跟着GOTO或LABEL的LABEL语句
void optimizeLABELBeforeGOTO(InterCode code, Operand label) {
    //调用findLastInterCode()函数，查找给定语句链表中的最后一条非空语句或空语句，并将其赋值给last
    InterCode last = findLastInterCode(code);
    //如果last的类型（kind）为LABEL_IR
    if (last->kind == LABEL_IR) {
        //获取LABEL语句的标号（编号）
        int no = last->ops[0]->no;
        //将LABEL语句的类型设置为NULL_IR，表示将其优化为空语句
        last->kind = NULL_IR;
        //创建一个指向last的指针curr，用于遍历语句链表
        InterCode curr = last;
        //进入循环，条件是curr不等于code（表示还未到达链表的起始位置）
        while (curr != code) {
            // 如果curr的类型为GOTO_IR（无条件跳转语句）且跳转目标的标号与no相同
            if (curr->kind == GOTO_IR && curr->ops[0]->no == no)
                //将跳转目标的标号更新为新的标号label->no
                curr->ops[0]->no = label->no;
            //如果curr的类型为IF_GOTO_IR（条件跳转语句）且跳转目标的标号与no相同
            else if (curr->kind == IF_GOTO_IR && curr->ops[2]->no == no)
                //将跳转目标的标号更新为新的标号label->no
                curr->ops[2]->no = label->no;
            //将curr移动到前一个语句的位置，即更新curr指针为上一条语句
            curr = curr->pre;
        }
    }
}

// 基本表达式的翻译
InterCode translateExp(Node* root, Operand place) {
    //赋值表达式
    if (root->childNum == 3 && strcmp(root->children[1]->name, "ASSIGNOP") == 0) {
        // 单个变量作为左值
        // *** todo ***
        /*
        You need to finish expression translation if the exp is just one variable
        You need to add a temporary variable, 
        then assign the expression to the temporary variable, and finally assign the temporary variable.
        */
        if (root->children[0]->childNum == 1 && 
            strcmp(root->children[0]->children[0]->name, "ID") == 0) {
			    Entry sym = findSymbolAll(root->children[0]->children[0]->strVal);
			    Operand var = getVar(sym->name);
			    // tmp1 是右侧表达式的运算结果
			    Operand tmp1 = newTemp();
			    InterCode code1 = translateExp(root->children[2], tmp1);
			    InterCode code2 = (InterCode)malloc(sizeof(InterCode_));
			    code2->kind = ASSIGN_IR;
			    code2->ops[0] = var;
			    code2->ops[1] = tmp1;
			    insertInterCode(code2, code1);
			    //将运算结果存回place
			    if (place != NULL)
			         operandCpy(place, var);
			    return code1;
        }

        //数组元素作为左值
        //判断当前节点的第一个子节点的子节点数是否为4，且第一个子节点的第二个子节点的名称为"LB"（表示左方括号）
        else if (root->children[0]->childNum == 4 && 
            strcmp(root->children[0]->children[1]->name, "LB") == 0) {
            // tmp1应当是数组的地址，tmp2应当是一个整型，tmp3是所取元素的偏移量
            Operand tmp1 = newTemp();           //用于存储数组的地址
            Operand tmp2 = newTemp();           //用于存储一个整数值
            Operand tmp3 = newTemp();           //用于存储元素的偏移量
            //调用translateExp函数，分别将数组的表达式节点翻译为中间代码
            //并将结果存储在code1和code2中，同时使用tmp1和tmp2作为结果的临时变量
            InterCode code1 = translateExp(root->children[0]->children[0], tmp1);
            InterCode code2 = translateExp(root->children[0]->children[2], tmp2);
            //获取数组元素的大小
            int size = getSize(tmp1->type->array.elem);
            //调用optimizeMULIR函数，生成乘法运算的中间代码，将tmp2与size相乘，并将结果存储在tmp3中
            InterCode code3 = optimizeMULIR(tmp3, tmp2, getValue(size));
            insertInterCode(code2, code1);       //将code2中的中间代码插入到code1的中间代码之前
            insertInterCode(code3, code1);       //将code3中的中间代码插入到code1的中间代码之前
            //tmp4存储所取元素的首地址
            Operand tmp4 = newTemp();
            //调用optimizePLUSIR函数，生成加法运算的中间代码，将tmp1与tmp3相加，并将结果存储在tmp4中
            InterCode code4 = optimizePLUSIR(tmp4, tmp1, tmp3);
            insertInterCode(code4, code1);        //将code4中的中间代码插入到code1的中间代码之前
            //tmp5存储的是右侧表达式的运算结果
            Operand tmp5 = newTemp();
            //调用translateExp函数，将右侧表达式节点root->children[2]翻译为中间代码
            //并将结果存储在code5中，同时使用tmp5作为结果的临时变量
            InterCode code5 = translateExp(root->children[2], tmp5);
            //分配一个新的中间代码节点code6的内存空间
            InterCode code6 = (InterCode)malloc(sizeof(InterCode_));
            code6->kind = TO_MEM_IR;              //将code6的类型设置为TO_MEM_IR（存储到内存）
            code6->ops[0] = tmp4;                 //将tmp4设置为code6的第一个操作数，表示存储的目标地址
            code6->ops[1] = tmp5;                 //将tmp5设置为code6的第二个操作数，表示要存储的值
            insertInterCode(code5, code1);        //将code5中的中间代码插入到code1的中间代码之前
            insertInterCode(code6, code1);        //将code6中的中间代码插入到code1的中间代码之前
            //将运算结果tmp4存储回place中
            if (place != NULL)
                operandCpy(place, getVal(tmp4));
            return code1;                         //返回生成的中间代码code1
        }
        //结构体特定域作为左值
        //判断当前节点的第一个子节点的子节点数是否为3，且第一个子节点的第二个子节点的名称是否为"DOT"（表示点操作符）
        else if (root->children[0]->childNum == 3 &&
            strcmp(root->children[0]->children[1]->name, "DOT") == 0) {
            //创建一个字符数组name，用于存储结构体域的名称
            char name[32];
            //将节点的第一个子节点的第三个子节点的字符串值赋值给name
            strcpy(name, root->children[0]->children[2]->strVal);
            //创建一个临时变量tmp1，用于存储结构体的首地址
            Operand tmp1 = newTemp();
            //tmp1返回的是一个结构体的首地址，并且带有type属性
            //调用translateExp函数，将结构体的表达式节点root->children[0]->children[0]翻译为中间代码
            //并将结果存储在code1中，同时使用tmp1作为结果的临时变量
            InterCode code1 = translateExp(root->children[0]->children[0], tmp1);
            //获取域的偏移量和类型
            int offset = 0;
            //创建一个指向结构体域链表头部的指针head，通过tmp1的类型信息获取
            FieldList head = tmp1->type->structure->head;
            //进入循环，循环条件是当前结构体域的名称与目标域的名称不相等
            while (strcmp(head->name, name) != 0) {
                int tmp = getSize(head->type);
                //按四字节对齐
                if (tmp % 4 != 0)
                    tmp = ((tmp / 4) + 1) * 4;
                //将调整后的域的大小累加到offset中，更新偏移量
                offset += tmp;
                //将指针head移动到下一个结构体域
                head = head->next;
            }
            //tmp2中存储域的首地址
            Operand tmp2 = newTemp();
            //调用optimizePLUSIR函数，生成加法运算的中间代码，将tmp1与offset相加，并将结果存储在tmp2中
            InterCode code2 = optimizePLUSIR(tmp2, tmp1, getValue(offset));
            insertInterCode(code2, code1);                  //将code2中的中间代码插入到code1的中间代码之前
            //tmp3存储的是右侧表达式的运算结果
            Operand tmp3 = newTemp();
            //调用translateExp函数，将右侧表达式节点root->children[2]翻译为中间代码
            //并将结果存储在code3中，同时使用tmp3作为结果的临时变量
            InterCode code3 = translateExp(root->children[2], tmp3);
            //分配一个新的中间代码节点code4的内存空间
            InterCode code4 = (InterCode)malloc(sizeof(InterCode_));
            code4->kind = TO_MEM_IR;                 //将code4的类型设置为TO_MEM_IR（存储到内存）
            code4->ops[0] = tmp2;                    //将tmp2设置为code4的第一个操作数，即存储域的首地址
            code4->ops[1] = tmp3;                    //将tmp3设置为code4的第二个操作数，即右侧表达式的运算结果
            insertInterCode(code3, code1);           //将code3中的中间代码插入到code1的中间代码之前
            insertInterCode(code4, code1);           //将code4中的中间代码插入到code1的中间代码之前
            // 将运算结果tmp2存回place中
            if (place != NULL)
                operandCpy(place, getVal(tmp2));
            return code1;                            //返回生成的中间代码code1
        }
    }

    //加减乘除表达式
    //判断当前节点是否符合加减乘除表达式的条件，即节点的子节点数为3，并且第二个子节点的名称为"PLUS"、"MINUS"、"STAR"或"DIV"
    else if (root->childNum == 3 && (
             strcmp(root->children[1]->name, "PLUS") == 0 ||
             strcmp(root->children[1]->name, "MINUS") == 0 ||
             strcmp(root->children[1]->name, "STAR") == 0 ||
             strcmp(root->children[1]->name, "DIV") == 0)) {
        Operand tmp1 = newTemp();                    //创建一个临时变量tmp1，用于存储表达式的第一个操作数
        Operand tmp2 = newTemp();                    //创建一个临时变量tmp2，用于存储表达式的第二个操作数
        //调用translateExp函数，分别将表达式的第一个子节点和第三个子节点翻译为中间代码
        //并将结果分别存储在code1和code2中，同时使用tmp1和tmp2作为结果的临时变量
        InterCode code1 = translateExp(root->children[0], tmp1);         
        InterCode code2 = translateExp(root->children[2], tmp2);
        insertInterCode(code2, code1);                //将code2中的中间代码插入到code1的中间代码之前
        InterCode code3 = getNullInterCode();         //创建一个空的中间代码节点code3
        switch (root->children[1]->name[0]) {         //根据表达式的操作符进行判断，进入相应的case分支
            case 'P': code3 = optimizePLUSIR(place, tmp1, tmp2); break;        //加法
            case 'M': code3 = optimizeSUBIR(place, tmp1, tmp2); break;         //减法
            case 'S': code3 = optimizeMULIR(place, tmp1, tmp2); break;         //乘法
            case 'D': code3 = optimizeDIVIR(place, tmp1, tmp2); break;         //除法
        }
        insertInterCode(code3, code1);                                         //将code3中的中间代码插入到code1的中间代码之前
        return code1;                                                          //返回生成的中间代码code1
    }

    //取负表达式
    //判断当前节点是否符合取负表达式的条件，即第一个子节点的名称为"MINUS"
    else if (strcmp(root->children[0]->name, "MINUS") == 0) {
        Operand tmp1 = newTemp();                                              //创建一个临时变量tmp1，用于存储表达式的操作数
        //调用translateExp函数，将表达式的第二个子节点root->children[1]翻译为中间代码
        //并将结果存储在code1中，同时使用tmp1作为结果的临时变量
        InterCode code1 = translateExp(root->children[1], tmp1);
        //调用optimizeSUBIR函数生成减法运算的中间代码，将常数0和tmp1作为操作数
        //将结果存储在code2中，同时使用place作为结果的目标变量
        InterCode code2 = optimizeSUBIR(place, getValue(0), tmp1);
        insertInterCode(code2, code1);                                         //将code2中的中间代码插入到code1的中间代码之前
        return code1;                                                          //返回生成的中间代码code1
    }

    //括号表达式
    //判断当前节点是否符合括号表达式的条件，即第一个子节点的名称为"LP"，表示左括号
    else if (strcmp(root->children[0]->name, "LP") == 0) {
        Operand tmp1 = newTemp();                                               //创建一个临时变量tmp1，用于存储括号内表达式的结果
        //调用translateExp函数，将括号内的表达式的子节点root->children[1]翻译为中间代码
        //并将结果存储在code1中，同时使用tmp1作为结果的临时变量
        InterCode code1 = translateExp(root->children[1], tmp1);
        // 优化：直接把place修改为tmp1
        if (place != NULL)
            operandCpy(place, tmp1);
        return code1;                                                            //返回生成的中间代码code1
    }

    // 条件表达式
    // *** todo ***
    /*
    You need to finish expression translation if the exp is an conditional expression
    "You need to make appropriate use of the translateCond function.
    */
    else if (root->childNum >= 2 && (
             strcmp(root->children[0]->name, "NOT") == 0 ||
             strcmp(root->children[1]->name, "RELOP") == 0 ||
             strcmp(root->children[1]->name, "AND") == 0 ||
             strcmp(root->children[1]->name, "OR") == 0)) {
                Operand label1 = newLabel();                               // 创建一个标签label1，用于条件跳转
                Operand label2 = newLabel();                               // 创建一个标签label2，用于条件跳转
                InterCode code1=(InterCode)malloc(sizeof(InterCode_));
                code1->kind=ASSIGN_IR;
                code1->ops[0]=place;
                code1->ops[1]=getValue(0);

                InterCode code2 = translateCond(root, label1, label2);     // 调用translateCond函数翻译条件表达式

                InterCode code3 = (InterCode)malloc(sizeof(InterCode_));   // 创建一个标签中间代码节点code2
                code3->kind=LABEL_IR;
                code3->ops[0] = label1;                                    // 设置code2的操作数为label1

                InterCode code4 = (InterCode)malloc(sizeof(InterCode_));   // 创建一个标签中间代码节点code3
                code4->kind=ASSIGN_IR;
                code4->ops[0] = place;                                    // 设置code3的操作数为label2
                code4->ops[1]=getValue(1);

                InterCode code5=(InterCode)malloc(sizeof(InterCode_));
                code5->kind=LABEL_IR;
                code5->ops[0]=label2;


                insertInterCode(code2, code1);                             // 将code2插入到code1之前
                insertInterCode(code3, code1);                             // 将code3插入到code1之前
                insertInterCode(code4, code1);
                insertInterCode(code5, code1);
                return code1;                                              // 返回生成的中间代码code1
    }

    else if (strcmp(root->children[0]->name, "ID") == 0) {
        //单变量表达式
        //判断当前节点是否符合单变量表达式的条件，即子节点数为1
        if (root->childNum == 1) {
            //根据子节点的字符串值，在符号表中查找对应的符号项sym，该符号项表示变量
            Entry sym = findSymbolAll(root->children[0]->strVal);
            //根据符号项的名称，在操作数表中查找对应的变量操作数var，用于存储变量的值
            Operand var = getVar(sym->name);
            // 数组类型和结构体类型并且不是函数参数是局部变量——需要取地址指令
            if (sym->type->kind != ENUM_BASIC && sym->isArg == 0) {
                operandCpy(place, getAddr(var));         //将var的地址操作数复制给place，表示需要取变量的地址
                place->type = sym->type;                 //将place的类型设置为变量的类型
                return getNullInterCode();               //返回一个空的中间代码节点作为结果
            }
            // 优化：不需要取地址指令，直接修改place
            else {
                operandCpy(place, var);                  //将var的值直接复制给place，表示直接使用变量的值作为结果
                place->type = sym->type;                 //将place的类型设置为变量的类型
                return getNullInterCode();               //返回一个空的中间代码节点作为结果
            }
        }
        // 函数调用表达式
        else {
            //根据函数调用表达式的函数名，在符号表中查找对应的函数项sym
            Entry sym = findSymbolFunc(root->children[0]->strVal);
            //根据函数项的名称，在操作数表中查找对应的函数操作数func，用于表示函数调用
            Operand func = getFunc(sym->name);
            //无参函数
            if (root->childNum == 3) {
                // read函数
                if (strcmp(func->name, "read") == 0) {                          //判断调用的函数是否为read函数
                    InterCode code1 = (InterCode)malloc(sizeof(InterCode_));    //创建一个中间代码节点code1，用于表示read函数的调用
                    code1->kind = READ_IR;                                      //将code1的操作码设置为READ_IR，表示读操作
                    code1->ops[0] = place;                                      //将place设置为code1的操作数，表示读取的结果存储在place中
                    return code1;                                               //返回生成的中间代码code1
                }
                InterCode code1 = (InterCode)malloc(sizeof(InterCode_));        //创建一个中间代码节点code1，用于表示函数调用
                code1->kind = CALL_IR;                                          //将code1的操作码设置为CALL_IR，表示函数调用
                code1->ops[0] = place;                                          //将place设置为code1的操作数，表示函数调用的结果存储在place中
                code1->ops[1] = func;                                           //将func设置为code1的操作数，表示调用的函数
                return code1;                                                   //返回生成的中间代码code1
            }
            // 带参函数
            else if (root->childNum == 4) {
                Operand argList = (Operand)malloc(sizeof(Operand_));             //创建一个操作数节点argList，用于存储参数列表
                //调用translateArgs函数，将参数列表的子节点root->children[2]翻译为中间代码
                //并将结果存储在code1中，同时使用argList作为参数列表的操作数
                InterCode code1 = translateArgs(root->children[2], argList);     
                // write函数
                if (strcmp(func->name, "write") == 0) {                          //判断调用的函数是否为 write 函数
                    InterCode code2 = (InterCode)malloc(sizeof(InterCode_));     //创建一个中间代码节点code2，用于表示write函数的调用
                    code2->kind = WRITE_IR;                                      //将code2的操作码设置为WRITE_IR，表示写操作
                    code2->ops[0] = argList->next;                               //将参数列表中的第一个参数设置为code2的操作数，表示需要写入的值
                    insertInterCode(code2, code1);                               //将code2插入到code1的中间代码之前
                    if (place != NULL)                                           
                        operandCpy(place, getValue(0));                          //将getValue(0)的值复制给place，表示写操作的结果
                    return code1;                                                //返回生成的中间代码code1
                }
                Operand curr = argList->next;                                    //创建一个指针curr，指向参数列表中的第一个参数
                while (curr != NULL) {                                           //进入循环，遍历参数列表中的每个参数
                    InterCode code2 = (InterCode)malloc(sizeof(InterCode_));     //创建一个中间代码节点code2，用于表示参数传递的操作
                    code2->kind = ARG_IR;                                        //将code2的操作码设置为ARG_IR，表示参数传递
                    code2->ops[0] = curr;                                        //将当前参数设置为code2的操作数
                    insertInterCode(code2, code1);                               //将code2插入到code1的中间代码之前
                    curr = curr->next;                                           //将指针curr指向下一个参数
                }
                InterCode code3 = (InterCode)malloc(sizeof(InterCode_));         //创建一个中间代码节点code3，用于表示函数调用
                code3->kind = CALL_IR;                                           //将code3的操作码设置为CALL_IR，表示函数调用
                code3->ops[0] = place;                                           //将place设置为code3的操作数，表示函数调用的结果存储在place中
                code3->ops[1] = func;                                            //将func设置为code3的操作数，表示调用的函数
                insertInterCode(code3, code1);                                   //将code3插入到code1的中间代码之前
                return code1;                                                    //返回生成的中间代码code1
            }
        }
    }

    //整型表达式
    else if (strcmp(root->children[0]->name, "INT") == 0) {
        //优化：直接把place改成一个常量操作数，不需要多加一条赋值指令
        Operand value = getValue(root->children[0]->intVal);                     //获取整型字面量节点的值，并将其存储在操作数value中
        operandCpy(place, value);                                                //将操作数value的值复制给操作数place，即将整型字面量的值存储在place中
        //输出一条空指令来避免空指针错误
        return getNullInterCode();                                               //返回一个表示空指令的中间代码，这样做是为了避免空指针错误
    }

    //数组元素表达式
    //判断表达式的子节点数是否为4，并且第二个子节点的名称是否为"LB"，即中括号
    else if (root->childNum == 4 && strcmp(root->children[1]->name, "LB") == 0) {
        //tmp1应当是地址，tmp2应当是一个整型
        Operand tmp1 = newTemp();                                                //创建一个临时变量操作数tmp1，用于存储数组元素的地址
        Operand tmp2 = newTemp();                                                //创建一个临时变量操作数tmp2，用于存储数组索引的值
        Operand tmp3 = newTemp();                                                //创建一个临时变量操作数tmp3，用于存储数组索引与元素大小的乘积
        //调用translateExp函数，将数组名的两个子节点翻译为中间代码
        //并将结果存储在code1和code2中，同时使用tmp1和tmp2作为数组名的操作数
        InterCode code1 = translateExp(root->children[0], tmp1);                 
        InterCode code2 = translateExp(root->children[2], tmp2);
        insertInterCode(code2, code1);                                           //将code2插入到code1的中间代码之前
        // place为NULL没必要继续算
        if (place != NULL) {
            int size = getSize(tmp1->type->array.elem);                          //调用函数getSize获取数组元素的大小
            InterCode code3 = optimizeMULIR(tmp3, tmp2, getValue(size));         //生成乘法操作的中间代码，将数组索引的值tmp2与元素大小的值相乘，并将结果存储在tmp3中
            insertInterCode(code3, code1);                                       //将code3插入到code1的中间代码之前
            // 如果取出的数组元素还是一个数组，或者是一个结构体，则返回地址
            if (tmp1->type->array.elem->kind == ENUM_ARRAY || 
                tmp1->type->array.elem->kind == ENUM_STRUCT) {                   //判断取出的数组元素是否是数组或结构体
                //优化加法
                //生成加法操作的中间代码，将数组地址tmp1与乘积结果tmp3相加，并将结果存储在place中
                InterCode code4 = optimizePLUSIR(place, tmp1, tmp3);             
                insertInterCode(code4, code1);                                   //将code4插入到code1的中间代码之前
                place->type = tmp1->type->array.elem;                            //将place的类型设置为取出的数组元素的类型
            }
            // 取出的数组元素是一个基本类型，则返回值
            else {
                Operand tmp4 = newTemp();                                        //创建一个临时变量操作数tmp4，用于存储数组元素的值
                //生成加法操作的中间代码，将数组地址tmp1与乘积结果tmp3相加，并将结果存储在tmp4中
                InterCode code4 = optimizePLUSIR(tmp4, tmp1, tmp3);             
                insertInterCode(code4, code1);                                   //将code4插入到code1的中间代码之前
                operandCpy(place, getVal(tmp4));                                 //将tmp4的值复制给place，表示取出的数组元素的值存储在place中
            }
            return code1;                                                        //返回生成的中间代码code1
        }
        return code1;                                                            //返回生成的中间代码code1
    }

    //取结构体域
    //判断表达式的子节点数是否为3，并且第二个子节点的名称是否为"DOT"，即点操作符
    else if (root->childNum == 3 && strcmp(root->children[1]->name, "DOT") == 0) {
        //获取域名
        char name[32];                                                           //创建一个名为name的字符数组，用于存储结构体域的名字
        strcpy(name, root->children[2]->strVal);                                 //将子节点root->children[2]中的字符串值复制到name数组中，获取结构体域的名字
        Operand tmp1 = newTemp();                                                //创建一个临时变量操作数tmp1，用于存储结构体的首地址，并且带有type属性
        //tmp1返回的是一个结构体的首地址，并且带有type属性
        //调用translateExp函数，将结构体变量的子节点root->children[0]翻译为中间代码
        //并将结果存储在code1中，同时使用tmp1作为结构体变量的操作数
        InterCode code1 = translateExp(root->children[0], tmp1);
        // 获取域的偏移量和类型
        int offset = 0;                                                           //创建一个整型变量offset，用于存储结构体域的偏移量，初始值为0
        //创建一个指向结构体域链表头部的指针head，从tmp1->type->structure获取结构体的域信息
        FieldList head = tmp1->type->structure->head;
        while (strcmp(head->name, name) != 0) {
            int tmp = getSize(head->type);                                        //获取当前域的类型大小
            // 按四字节对齐
            if (tmp % 4 != 0)
                tmp = ((tmp / 4) + 1) * 4;
            offset += tmp;                                                        //将调整后的当前域大小加到偏移量offset上
            head = head->next;                                                    //将指针head移动到下一个结构体域
        }
        // place为NULL没必要算
        if (place != NULL) {
            InterCode code2 = getNullInterCode();                                 //创建一个表示空指令的中间代码code2
            if (head->type->kind == ENUM_BASIC) {                                 //判断当前域的类型是否为基本类型 
                Operand tmp2 = newTemp();                                         //创建一个临时变量操作数tmp2，用于存储取出的结构体域的值
                //生成加法操作的中间代码，将结构体首地址tmp1与偏移量offset相加，并将结果存储在tmp2中
                code2 = optimizePLUSIR(tmp2, tmp1, getValue(offset));           
                insertInterCode(code2, code1);                                     //将code2插入到code1的中间代码之前
                operandCpy(place, getVal(tmp2));                                   //将tmp2的值复制给place，表示取出的结构体域的值存储在place中
            }
            else {
                code2 = optimizePLUSIR(place, tmp1, getValue(offset));             //生成加法操作的中间代码，将结构体首地址tmp1与偏移量offset相加，并将结果存储在place中
                place->type = head->type;                                          //place的类型设置为当前域的类型
                insertInterCode(code2, code1);                                     //将code2插入到code1的中间代码之前
            }
        }
        return code1;                                                              //返回生成的中间代码code1
    }
    return getNullInterCode();                                                     //返回一个表示空指令的中间代码，这样做是为了避免空指针错误
}

// 函数参数的翻译模式
InterCode translateArgs(Node* root, Operand argList) {
    if (root->childNum == 1) {                                                     //判断节点root的子节点数是否为1
        Operand tmp1 = newTemp();                                                  //创建一个临时变量操作数tmp1，用于存储参数的值
        //调用translateExp函数，将参数的子节点root->children[0]翻译为中间代码
        //并将结果存储在code1中，同时使用tmp1作为参数的操作数
        InterCode code1 = translateExp(root->children[0], tmp1);                   
        tmp1->next = argList->next;                                                 //将tmp1的next指针指向argList的next指针，实现将tmp1插入到argList链表的头部
        argList->next = tmp1;                                                       //将argList的next指针指向tmp1，更新argList链表的头部
        return code1;                                                               //返回生成的中间代码code1
    }
    else if (root->childNum == 3) {
        Operand tmp1 = newTemp();                                                   //创建一个临时变量操作数tmp1，用于存储参数的值
        //调用translateExp函数，将参数的子节点root->children[0]翻译为中间代码
        //并将结果存储在code1中，同时使用tmp1作为参数的操作数
        InterCode code1 = translateExp(root->children[0], tmp1);                 
        tmp1->next = argList->next;                                                 //将tmp1的next指针指向argList的next指针，实现将tmp1插入到argList链表的头部
        argList->next = tmp1;                                                       //将argList的next指针指向tmp1，更新argList链表的头部
        InterCode code2 = translateArgs(root->children[2], argList);                //递归调用translateArgs函数，处理剩余的参数，并将返回的中间代码存储在code2中
        insertInterCode(code2, code1);                                              //将code2插入到code1的中间代码之前
        return code1;                                                               //返回生成的中间代码code1
    }
}

//语句的翻译模式
InterCode translateStmt(Node* root) {
    //如果root的第一个子节点的名称为 "Exp"，则调用translateExp函数翻译该表达式，并返回其生成的中间代码
    if (strcmp(root->children[0]->name, "Exp") == 0) {
        return translateExp(root->children[0], NULL);
    }
    //如果root的第一个子节点的名称为"CompSt"
    else if (strcmp(root->children[0]->name, "CompSt") == 0) {
        //则先创建一个新的作用域（调用pushLayer函数）
        pushLayer();
        //然后调用translateCompSt函数翻译复合语句，并返回其生成的中间代码
        InterCode code1 = translateCompSt(root->children[0], NULL);    
        //最后，退出当前作用域（调用popLayer函数）
        popLayer();
        return code1;
    }
    //如果root的第一个子节点的名称为"RETURN"
    else if (strcmp(root->children[0]->name, "RETURN") == 0) {    
        //则创建一个临时变量（调用newTemp函数）
        Operand tmp1 = newTemp();
        //然后调用translateExp函数翻译返回值表达式，并将结果存储到临时变量中
        InterCode code1 = translateExp(root->children[1], tmp1);
        InterCode code2 = (InterCode)malloc(sizeof(InterCode_));
        code2->kind = RETURN_IR;
        code2->ops[0] = tmp1;
        //接下来，创建一个表示返回语句的中间代码，   
        //并插入到之前生成的中间代码序列中（调用insertInterCode函数）
        insertInterCode(code2, code1);    
        //最后，返回之前生成的中间代码序列
        return code1;
    }
    //如果root的第一个子节点的名称为"IF"，并且子节点数量为5，表示是一个单独的IF语句
    else if (strcmp(root->children[0]->name, "IF") == 0 && root->childNum == 5) {
        //创建两个标号（label1和label2）作为条件语句的跳转目标
        Operand label1 = newLabel();
        Operand label2 = newLabel();
        //调用translateCond函数翻译条件表达式，并生成相应的中间代码
        InterCode code1 = translateCond(root->children[2], label1, label2);
        InterCode code2 = (InterCode)malloc(sizeof(InterCode_));
        //创建表示label1的中间代码，并将其插入到之前生成的中间代码序列中
        code2->kind = LABEL_IR;
        code2->ops[0] = label1;
        //调用translateStmt函数翻译IF语句的执行部分，并生成相应的中间代码序列
        InterCode code3 = translateStmt(root->children[4]);
        InterCode code4 = (InterCode)malloc(sizeof(InterCode_));
        //创建表示label2的中间代码，并将其插入到之前生成的中间代码序列中
        code4->kind = LABEL_IR;
        code4->ops[0] = label2;
        //插入条件表达式和执行部分的中间代码序列到之前生成的中间代码序列中
        insertInterCode(code2, code1);
        if (code3 != NULL)
            insertInterCode(code3, code1);
        insertInterCode(code4, code1);
        //返回之前生成的中间代码序列
        return code1;
    }
    else if (strcmp(root->children[0]->name, "IF") == 0 && root->childNum == 7) {
        //首先，创建三个新的操作数（Operand）label1、label2和label3，这些操作数用于表示条件语句中的标签（label）
        Operand label1 = newLabel();
        Operand label2 = newLabel();
        Operand label3 = newLabel();
        //调用函数translateCond，将条件表达式（root->children[2]）翻译为中间代码，并且翻译结果存储在变量code1中
        InterCode code1 = translateCond(root->children[2], label1, label2);
        // code1的最后一条语句为goto labelfalse并且code1中只有这一条向labelflase的跳转语句，此时可以优化
        InterCode last = findLastInterCode(code1);
        //进行优化的过程是统计code1中跳转目标为label2的语句数量
        //如果只有一条，说明label2只被条件表达式的跳转语句引用，可以进行优化
        if (last->kind == GOTO_IR && last->ops[0]->no == label2->no) {
            int count = 0;
            if (code1->kind == GOTO_IR && code1->ops[0]->no == label2->no)
                count++;
            InterCode curr = code1->pre;
            while (curr != code1) {
                if (curr->kind == GOTO_IR && curr->ops[0]->no == label2->no)
                    count++;
                curr = curr->pre;
            }
            // 优化：把labelFalse对应的语句提到条件表达式的IFGOTO语句后面，可以消除一个冗余的label
            if (count == 1) {
                //将last的中间代码类型（kind）设置为NULL_IR，表示将其删除
                last->kind = NULL_IR;
                //然后，调用函数translateStmt，将if语句的主体部分（root->children[6]）翻译为中间代码，结果存储在变量code2中
                InterCode code2 = translateStmt(root->children[6]);
                // 优化：如果code2的最后一句是LABEL语句，那么将code2中的所有GOTO语句中的该LABEL替换为LABEL3
                optimizeLABELBeforeGOTO(code2, label3);
                //code3为跳转到label3的无条件跳转语句（GOTO_IR）
                //code4为表示条件为真时的标签语句（LABEL_IR，使用label1）
                //code5为if语句条件为假时的中间代码
                //将条件语句的语句块（root->children[4]）翻译为中间代码
                InterCode code3 = (InterCode)malloc(sizeof(InterCode_));
                code3->kind = GOTO_IR;
                code3->ops[0] = label3;
                InterCode code4 = (InterCode)malloc(sizeof(InterCode_));
                code4->kind = LABEL_IR;
                code4->ops[0] = label1;
                InterCode code5 = translateStmt(root->children[4]);
                //进行另一项优化，如果code5的最后一条语句是一个标签语句（LABEL_IR）
                //将code5中所有跳转语句（GOTO_IR）中的该标签替换为label3
                optimizeLABELBeforeGOTO(code5, label3);
                //创建两个新的中间代码
                //code6为表示条件为假时的标签语句（LABEL_IR，使用label3）
                //code7为if语句结束后的标签语句（LABEL_IR，使用label3）
                InterCode code6 = (InterCode)malloc(sizeof(InterCode_));
                code6->kind = LABEL_IR;
                code6->ops[0] = label3;
                insertInterCode(code2, code1);
                insertInterCode(code3, code1);
                insertInterCode(code4, code1);
                insertInterCode(code5, code1);
                insertInterCode(code6, code1);
                return code1;
            }
        }
        InterCode code2 = (InterCode)malloc(sizeof(InterCode_));
        code2->kind = LABEL_IR;
        code2->ops[0] = label1;
        InterCode code3 = translateStmt(root->children[4]);
        optimizeLABELBeforeGOTO(code3, label3);
        InterCode code4 = (InterCode)malloc(sizeof(InterCode_));
        code4->kind = GOTO_IR;
        code4->ops[0] = label3;
        InterCode code5 = (InterCode)malloc(sizeof(InterCode_));
        code5->kind = LABEL_IR;
        code5->ops[0] = label2;
        InterCode code6 = translateStmt(root->children[6]);
        optimizeLABELBeforeGOTO(code6, label3);
        InterCode code7 = (InterCode)malloc(sizeof(InterCode_));
        code7->kind = LABEL_IR;
        code7->ops[0] = label3;
        //将中间代码的各个部分按正确的顺序插入到code1中
        insertInterCode(code2, code1);
        insertInterCode(code3, code1);
        insertInterCode(code4, code1);
        insertInterCode(code5, code1);
        insertInterCode(code6, code1);
        insertInterCode(code7, code1);
        //返回code1作为整个if语句的中间代码表示
        return code1;
    }
    else if (strcmp(root->children[0]->name, "WHILE") == 0) {
        //首先，创建三个新的操作数（Operand）label1、label2和label3，这些操作数用于表示循环语句中的标签（label）
        Operand label1 = newLabel();
        Operand label2 = newLabel();
        Operand label3 = newLabel();
        //创建一个新的中间代码code1，表示循环开始的标签语句（LABEL_IR），使用label1作为操作数
        InterCode code1 = (InterCode)malloc(sizeof(InterCode_));
        code1->kind = LABEL_IR;
        code1->ops[0] = label1;
        //调用函数translateCond，将循环条件表达式（root->children[2]）翻译为中间代码。翻译结果存储在变量code2中
        //同时，将label2作为条件为真时跳转的目标标签，将label3作为条件为假时跳转的目标标签
        InterCode code2 = translateCond(root->children[2], label2, label3);
        //创建一个新的中间代码code3，表示条件为真时的标签语句（LABEL_IR），使用label2作为操作数
        InterCode code3 = (InterCode)malloc(sizeof(InterCode_));
        code3->kind = LABEL_IR;
        code3->ops[0] = label2;
        //调用函数translateStmt，将循环体（root->children[4]）翻译为中间代码，结果存储在变量code4中
        InterCode code4 = translateStmt(root->children[4]);
        //进行优化，如果code4的最后一条语句是一个标签语句（LABEL_IR）
        //将code4中所有跳转语句（GOTO_IR）中的该标签替换为label1，以实现循环跳转
        optimizeLABELBeforeGOTO(code4, label1);
        //创建一个新的中间代码code5，表示无条件跳转回循环开始的标签语句（GOTO_IR），使用label1作为操作数
        InterCode code5 = (InterCode)malloc(sizeof(InterCode_));
        code5->kind = GOTO_IR;
        code5->ops[0] = label1;
        //创建一个新的中间代码code6，表示循环结束的标签语句（LABEL_IR），使用label3作为操作数
        InterCode code6 = (InterCode)malloc(sizeof(InterCode_));
        code6->kind = LABEL_IR;
        code6->ops[0] = label3;
        //将中间代码的各个部分按正确的顺序插入到code1中
        insertInterCode(code2, code1);
        insertInterCode(code3, code1);
        insertInterCode(code4, code1);
        insertInterCode(code5, code1);
        insertInterCode(code6, code1);
        //返回生成的中间代码code1
        return code1;
    }
    //返回一个表示空指令的中间代码，这样做是为了避免空指针错误
    return getNullInterCode();
}

// 条件表达式的翻译模式
// *** todo ***
/*
You need to translate conditional expressions. 
You can refer to the lab manual and textbooks. 
Pay attention to how to implement short-circuit translation.
*/
InterCode translateCond(Node* root, Operand labelTrue, Operand labelFalse) {
    //如果为Relop
    if (root->childNum==3 && strcmp(root->children[1]->name, "RELOP") == 0) {
        Operand tmp1 = newTemp();
        Operand tmp2 = newTemp();
        InterCode code1 = translateExp(root->children[0], tmp1);
        InterCode code2 = translateExp(root->children[2], tmp2);
        InterCode code3=(InterCode)malloc(sizeof(InterCode_));
        code3->kind=IF_GOTO_IR;
        strcpy(code3->relop,root->children[1]->strVal);
        code3->ops[0]=tmp1;
        code3->ops[1]=tmp2;
        code3->ops[2]=labelTrue;
        InterCode code4=(InterCode)malloc(sizeof(InterCode_));
        code4->kind=GOTO_IR;
        code4->ops[0]=labelFalse;
        insertInterCode(code2,code1);
        insertInterCode(code3,code1);
        insertInterCode(code4,code1);
        return code1;
    } else if (root->childNum==2 && strcmp(root->children[0]->name, "NOT") == 0) {    //条件表达式为NOT
        return translateCond(root->children[1], labelFalse, labelTrue);
    } else if (root->childNum==3 && strcmp(root->children[1]->name, "AND") == 0) {    //条件表达式为AND
        Operand label1 = newLabel();
        InterCode code1 = translateCond(root->children[0], label1, labelFalse);
        InterCode code2 = translateCond(root->children[2], labelTrue, labelFalse);

        InterCode code3 = (InterCode)malloc(sizeof(InterCode_));
        code3->kind=LABEL_IR;
        code3->ops[0]=label1;
        insertInterCode(code3,code1);
        insertInterCode(code2,code1);
        return code1;
    } else if (root->childNum==3 && strcmp(root->children[1]->name, "OR") == 0) {     //条件表达式为OR
        Operand label1 = newLabel();
        InterCode code1 = translateCond(root->children[0], labelTrue, label1);
        InterCode code2 = translateCond(root->children[2], labelTrue, labelFalse);
        InterCode code3 = (InterCode)malloc(sizeof(InterCode_));
        code3->kind=LABEL_IR;
        code3->ops[0]=label1;
        insertInterCode(code3,code1);
        insertInterCode(code2,code1);
        return code1;
    } else {                                                    //其它情况
        Operand tmp1 = newTemp();
        InterCode code1 = translateExp(root, tmp1);
        InterCode code2=(InterCode)malloc(sizeof(InterCode_));
        code2->kind=IF_GOTO_IR;
        strcpy(code2->relop,"!=");
        code2->ops[0]=tmp1;
        code2->ops[1]=getValue(0);
        code2->ops[2]=labelTrue;
        InterCode code3 = (InterCode)malloc(sizeof(InterCode_));
        code3->kind=GOTO_IR;
        code3->ops[0]=labelFalse;
        insertInterCode(code2,code1);
        insertInterCode(code3,code1);
        return code1;
    }
}

//完整的程序翻译
void translateProgram(Node* root) {
    initSymbolTable();                 //调用initSymbolTable函数，用于初始化符号表
    initInterCodes();                  //调用initInterCodes的函数，用于初始化中间代码表
    //调用了一个名为translateExtDefList的函数，将root->children[0]作为参数进行调用
    //并且将结果存储在code中，用于翻译外部定义列表（external definition list）
    InterCode code = translateExtDefList(root->children[0]);
    //将code赋值给全局变量interCodes
    interCodes = code;
}

//用于翻译外部定义列表
InterCode translateExtDefList(Node* root) {
    if (root->childNum != 0) {
        //调用translateExtDef函数，将root->children[0]作为参数进行调用
        //用于翻译单个外部定义，同时将翻译结果存储在变量code1中
        InterCode code1 = translateExtDef(root->children[0]);
        //调用translateExtDefList函数，将root->children[1]作为参数进行递归调用
        //用于翻译剩余的外部定义列表，同时将翻译结果存储在变量code2中
        InterCode code2 = translateExtDefList(root->children[1]);
        if (code2 != NULL && code1 != NULL)
            insertInterCode(code2, code1);     //将code2插入到code1的中间代码之前
        if (code1 != NULL)
            return code1;                      //直接返回code1作为翻译结果
        else if (code2 != NULL) 
            return code2;                      //直接返回code2作为翻译结果
    }
    return getNullInterCode();                 //返回一个表示空指令的中间代码，这样做是为了避免空指针错误
}

//翻译外部定义
InterCode translateExtDef(Node* root) {
    //声明了一个Type类型的变量type，并调用Specifier函数，将root->children[0]作为参数进行调用
    //用于解析类型说明符，返回表示该类型的Type对象
    Type type = Specifier(root->children[0]);
    // 结构体定义，是结构体，不是匿名类型，域定义没有产生错误
    if (type->kind == ENUM_STRUCT && type->structure->name != "" && type->structure->head != NULL) {
        //创建一个Entry类型的结构体指针res，用于存储结构体定义的符号表条目
        Entry res = (Entry)malloc(sizeof(Entry_));
        //使用strcpy函数将结构体名称复制到res->name中
        strcpy(res->name, type->structure->name);
        //需要保证对res->type->kind的改动不会影响到type
        res->type = (Type)malloc(sizeof(Type_));
        res->type->structure = type->structure;
        //将res->type->kind设置为ENUM_STRUCT_DEF，表示这是一个结构体定义
        res->type->kind = ENUM_STRUCT_DEF;
        //将结构体定义的符号表条目插入符号表中
        insertSymbol(res);
    }
    // 函数定义
    if (strcmp(root->children[1]->name, "FunDec") == 0) {
        //调用FunDec函数，将root->children[1]作为参数进行调用，该函数用于解析函数声明，返回表示函数的Function对象
        Function func = FunDec(root->children[1]);
        //生成FUNCTION和PARAM中间代码
        InterCode code1 = (InterCode)malloc(sizeof(InterCode_));
        //将code1的kind字段设置为FUNC_IR，表示这是一个函数声明的中间代码
        code1->kind = FUNC_IR;
        //通过getFunc函数获取函数名称对应的符号表条目，并将其存储在code1->ops[0]中
        code1->ops[0] = getFunc(func->name);
        FieldList head = func->head;
        //开始遍历函数的参数列表
        while (head != NULL) {
            //创建一个InterCode类型的指针code2，用于存储表示参数的中间代码
            InterCode code2 = (InterCode)malloc(sizeof(InterCode_));
            //将code2的kind字段设置为PARAM_IR，表示这是一个参数声明的中间代码
            code2->kind = PARAM_IR;
            //通过getVar函数获取参数名称对应的符号表条目，并将其存储在code2->ops[0]中
            code2->ops[0] = getVar(head->name);
            //将code2插入到code1的中间代码之前
            insertInterCode(code2, code1);
            //更新head指针
            head = head->next;
        }
        // 向符号表中添加符号
        func->returnType = type;
        func->hasDefined = 1;
        //创建一个新的Type对象newType，将其kind字段设置为ENUM_FUNC，表示这是一个函数类型
        Type newType = (Type)malloc(sizeof(Type_));
        newType->kind = ENUM_FUNC;
        newType->func = func;
        //创建一个Entry类型的结构体指针res，用于存储函数的符号表条目
        Entry res = (Entry)malloc(sizeof(Entry_));
        //使用strcpy函数将函数名称复制到res->name中
        strcpy(res->name, func->name);
        //将res->type指向新创建的newType
        res->type = newType;
        //调用insertSymbol函数，将函数的符号表条目插入符号表中。
        insertSymbol(res);
        //调用pushLayer函数，用于在符号表中创建新的作用域
        pushLayer();
        //调用translateCompSt函数，将root->children[2]和函数名称作为参数进行调用，该函数用于翻译函数体的复合语句
        InterCode code3 = translateCompSt(root->children[2], func->name);
        //调用popLayer函数，用于退出当前作用域
        popLayer();
        if (code3 != NULL)
            //将code3插入到code1的中间代码之前
            insertInterCode(code3, code1);
        return code1;
    }
    //返回一个表示空指令的中间代码，这样做是为了避免空指针错误
    return getNullInterCode();
}

//翻译复合语句
InterCode translateCompSt(Node* root, char* funcName) {
    //先把函数参数存进符号表
    if (funcName != NULL) {
        //通过findSymbolFunc函数查找符号表中存储函数符号的条目，并将其赋值给变量sym
        Entry sym = findSymbolFunc(funcName);
        //获取函数参数列表parms
        FieldList parms = sym->type->func->head;
        //开始遍历函数参数列表
        while (parms != NULL) {
            //创建一个Entry类型的结构体指针parm，用于存储函数参数的符号表条目
            Entry parm = (Entry)malloc(sizeof(Entry_));
            //使用strcpy函数将参数名称复制到parm->name中
            strcpy(parm->name, parms->name);
            //将parm->type指向参数的类型
            parm->type = parms->type;
            //标记该符号表条目为函数传入的参数
            parm->isArg = 1;
            //调用insertSymbol函数，将参数的符号表条目插入符号表中。
            insertSymbol(parm);
            //更新参数列表，遍历下一个参数
            parms = parms->next;
        }
    }
    //调用translateDefList函数，将root->children[1]和ENUM_VAR作为参数进行调用
    //该函数用于翻译变量定义列表，并返回表示中间代码的InterCode对象
    InterCode code1 = translateDefList(root->children[1], ENUM_VAR);
    //调用translateStmtList函数，将root->children[2]作为参数进行调用
    //该函数用于翻译语句列表，并返回表示中间代码的InterCode对象
    InterCode code2 = translateStmtList(root->children[2]);
    //将code2插入到code1的中间代码之前
    insertInterCode(code2, code1);
    //返回生成的中间代码code1
    return code1;
}

//语句列表
InterCode translateStmtList(Node* root) {
    if (root->childNum == 2) {
        //调用translateStmt函数，将root->children[0]作为参数进行调用
        //该函数用于翻译单个语句，并返回表示中间代码的InterCode对象
        InterCode code1 = translateStmt(root->children[0]);
        //调用translateStmtList函数，将root->children[1]作为参数进行调用
        //该函数用于翻译剩余的语句列表，并返回表示中间代码的InterCode对象
        InterCode code2 = translateStmtList(root->children[1]);
        if (code2 != NULL && code1 != NULL)
            //将code2插入到code1的中间代码之前
            insertInterCode(code2, code1);
        if (code1 != NULL)
            //直接返回code1作为整个语句列表的中间代码
            return code1;
        else if (code2 != NULL)
            //直接返回code2作为整个语句列表的中间代码。
            return code2;
    }
    //返回一个表示空指令的中间代码，这样做是为了避免空指针错误
    return getNullInterCode();
}

//函数体局部变量的翻译模式
InterCode translateDefList(Node* root, IdType class) {
    if (root->childNum == 0)
        //调用getNullInterCode函数，返回一个表示空的中间代码
        return getNullInterCode();
    else {
        //调用translateDef函数，将root->children[0]和class作为参数进行调用
        //该函数用于翻译单个变量定义，并返回表示中间代码的InterCode对象
        InterCode code1 = translateDef(root->children[0], class);
        //调用translateDefList函数，将root->children[1]和class作为参数进行递归调用
        //该函数用于翻译剩余的变量定义列表，并返回表示中间代码的InterCode对象
        InterCode code2 = translateDefList(root->children[1], class);
        //将code2插入到code1的中间代码之前
        insertInterCode(code2, code1);
        //返回生成的中间代码code1
        return code1;
    }
}

//翻译单个变量定义
InterCode translateDef(Node* root, IdType class) {
    //调用Specifier函数，将root->children[0]作为参数进行调用，以获取变量定义的类型type
    Type type = Specifier(root->children[0]);
    //创建一个表示空的中间代码对象code1
    InterCode code1 = getNullInterCode();
    //调用translateDecList函数，将root->children[1]、type、class和code1作为参数进行调用
    //该函数用于翻译变量定义列表，并返回表示中间代码的FieldList对象
    FieldList res = translateDecList(root->children[1], type, class, code1);
    //如果type是结构体或者数组，那么对每个定义的局部变量都要申请空间
    FieldList curr = res;
    while (curr != NULL) {
        //判断当前变量的类型是否为结构体或数组
        if (curr->type->kind == ENUM_ARRAY || curr->type->kind == ENUM_STRUCT) {
            //创建一个表示空的中间代码对象code2
            InterCode code2 = getNullInterCode();
            //将中间代码对象的类型设置为DEC_IR，表示变量的定义
            code2->kind = DEC_IR;
            //将变量的名称作为操作数存入中间代码对象的ops[0]中
            code2->ops[0] = getVar(curr->name);
            //将变量的大小（通过调用getSize函数获取）存入中间代码对象的size中
            code2->size = getSize(curr->type);
            //将code2插入到code1的中间代码之前
            insertInterCode(code2, code1);
        }
        //更新指针变量curr，指向下一个变量
        curr = curr->next;
    }
    //返回生成的中间代码code1
    return code1;
}

//翻译变量定义列表
FieldList translateDecList(Node* root, Type type, IdType class, InterCode code) {
    //调用translateDec函数，将root->children[0]、type、class和code作为参数进行调用
    //该函数用于翻译单个变量定义，并返回表示变量的FieldList对象。将返回值存储在变量res中。
    FieldList res = translateDec(root->children[0], type, class, code);
    if (root->childNum == 3) {
        if (res == NULL)
            //将变量定义列表的头部设置为下一个变量定义的翻译结果，即调用translateDecList函数递归处理下一个变量定义
            //并将type、class和code作为参数传递
            res = translateDecList(root->children[2], type, class, code);
        else {
            //创建一个临时指针变量tmp，并将其初始化为res，即变量定义列表的头部
            FieldList tmp = res;
            //循环遍历链表，找到链表的尾部
            while (tmp->next != NULL) tmp = tmp->next;
            //将下一个变量定义的翻译结果连接到链表的尾部，即调用translateDecList函数递归处理下一个变量定义
            //并将type、class和code作为参数传递
            tmp->next = translateDecList(root->children[2], type, class, code);
        }
    }
    //将变量定义列表的头部作为函数的返回值
    return res;
}

FieldList translateDec(Node* root, Type type, IdType class, InterCode code) {
    //调用VarDec函数，将root->children[0]、type和class作为参数进行调用
    //该函数用于处理变量声明，并返回表示变量的FieldList对象。将返回值存储在变量res中
    FieldList res = VarDec(root->children[0], type, class);
    // 局部变量在声明时初始化
    //判断变量的类别class是否为ENUM_VAR，且res不为空，同时判断变量定义的子节点数量是否为3，即判断是否包含初始化表达式
    if (class == ENUM_VAR && res != NULL && root->childNum == 3) {
        //创建一个临时操作数tmp1，用于存储初始化表达式的结果
        Operand tmp1 = newTemp();
        //调用translateExp函数，将root->children[2]和tmp1作为参数进行调用
        //该函数用于翻译表达式，并返回表示中间代码的InterCode对象code1
        InterCode code1 = translateExp(root->children[2], tmp1);
        //将code1插入到code的中间代码之前
        insertInterCode(code1, code);
        //创建一个表示赋值语句的中间代码对象code2
        InterCode code2 = (InterCode)malloc(sizeof(InterCode_));
        //将中间代码对象的类型设置为ASSIGN_IR，表示赋值语句
        code2->kind = ASSIGN_IR;
        //将变量的名称作为操作数存入中间代码对象的ops[0]中
        code2->ops[0] = getVar(res->name);
        //将临时操作数tmp1作为操作数存入中间代码对象的ops[1]中。
        code2->ops[1] = tmp1;
        //将code2插入到code的中间代码之前
        insertInterCode(code2, code);
    }
    //将变量的定义结果res作为函数的返回值
    return res;
}