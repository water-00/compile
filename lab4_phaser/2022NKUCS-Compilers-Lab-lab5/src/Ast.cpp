#include "Ast.h"
#include "SymbolTable.h"
#include <string>
#include "Type.h"

extern FILE *yyout;
int Node::counter = 0;

Node::Node()
{
    seq = counter++;
}

void Ast::output()
{
    fprintf(yyout, "program\n");
    if(root != nullptr)
        root->output(4); // root的类型开始是Node*，但是它被实例化了，于是会调用具体类型的output()
}

void UnaryExpr::output(int level)
{
    std::string op_str;
    switch (op)
    {
    case NOT:
        op_str = "not";
        break;
    case UNARY_PLUS:
        op_str = "unary plus";
        break;
    case UNARY_MINUS:
        op_str = "unary minus";
        break;
    }
    fprintf(yyout, "%*cUnaryExpr\top: %s\n", level, ' ', op_str.c_str());
    expr->output(level + 4);
}

void BinaryExpr::output(int level)
{
    std::string op_str;
    // TODO here!!
    // 多加点运算符的token
    switch(op)
    {
        case OR:
            op_str = "or";
            break;
        case AND:
            op_str = "and";
            break;
        case EQUAL:
            op_str = "equal";
            break;
        case NOTEQUAL:
            op_str = "notEqual";
            break;
        case LESS:
            op_str = "less";
            break;
        case LESSEQUAL:
            op_str = "lessEqual";
            break;
        case GREAT:
            op_str = "great";
            break;
        case GREATEQUAL:
            op_str = "greatEqual";
            break;
        case ADD:
            op_str = "add";
            break;
        case SUB:
            op_str = "sub";
            break;
        case MOD:
            op_str = "mod";
            break;
        case ASSIGN:
            op_str = "assign";
            break;
        case MUL:
            op_str = "mul";
            break;
        case DIV:
            op_str = "div";
            break;
    }
    fprintf(yyout, "%*cBinaryExpr\top: %s\n", level, ' ', op_str.c_str());
    expr1->output(level + 4);
    expr2->output(level + 4);
}

void Constant::output(int level)
{
    std::string type, value;
    type = symbolEntry->getType()->toStr();
    value = symbolEntry->toStr();
    fprintf(yyout, "%*cIntegerLiteral\tvalue: %s\ttype: %s\n", level, ' ',
            value.c_str(), type.c_str());
}

void Id::output(int level)
{
    std::string name, type;
    int scope;
    name = symbolEntry->toStr();
    type = symbolEntry->getType()->toStr();
    scope = dynamic_cast<IdentifierSymbolEntry*>(symbolEntry)->getScope();
    fprintf(yyout, "%*cId\tname: %s\tscope: %d\ttype: %s\n", level, ' ',
            name.c_str(), scope, type.c_str());
}

void ConstId::output(int level)
{
    std::string name, type;
    int scope;
    name = symbolEntry->toStr();
    type = symbolEntry->getType()->toStr();
    scope = dynamic_cast<IdentifierSymbolEntry*>(symbolEntry)->getScope();
    fprintf(yyout, "%*cConstId\tname: %s\tscope: %d\ttype: %s\n", level, ' ',
            name.c_str(), scope, type.c_str());
}

void FuncFParam::output(int level)
{
    std::string name, type;
    int scope;
    name = symbolEntry -> toStr();
    type = symbolEntry -> getType() -> toStr();
    scope = dynamic_cast<IdentifierSymbolEntry*>(symbolEntry) -> getScope();
    fprintf(yyout, "%*cFuncFParam\tname:%s\tscope:%d\ttype:%s\n", level, ' ',
            name.c_str(), scope, type.c_str());
}



void CompoundStmt::output(int level)
{
    fprintf(yyout, "%*cCompoundStmt\n", level, ' ');
    stmt->output(level + 4);
}

void SeqNode::output(int level)
{
    // fprintf(yyout, "%*cSequence\n", level, ' ');
    stmt1->output(level);
    stmt2->output(level);
}

void DeclStmt::output(int level)
{
    fprintf(yyout, "%*cDeclStmt\n", level, ' ');
    ids->output(level + 4);
}

void DefStmt::output(int level) 
{
    fprintf(yyout, "%*cDefStmt\n", level, ' ');
    id->output(level + 4);
    expr->output(level + 4);
}


void ConstDeclStmt::output(int level)
{
    fprintf(yyout, "%*cConstDeclStmt\n", level, ' ');
    cids->output(level + 4);
}

void ConstDefStmt::output(int level) 
{
    fprintf(yyout, "%*cConstDefStmt\n", level, ' ');
    cid->output(level + 4);
    expr->output(level + 4);
}

void IfStmt::output(int level)
{
    fprintf(yyout, "%*cIfStmt\n", level, ' ');
    cond->output(level + 4);
    thenStmt->output(level + 4);
}

void IfElseStmt::output(int level)
{
    fprintf(yyout, "%*cIfElseStmt\n", level, ' ');
    cond->output(level + 4);
    thenStmt->output(level + 4);
    elseStmt->output(level + 4);
}

void WhileStmt::output(int level)
{
    fprintf(yyout, "%*cWhileStmt\n", level, ' '); // 打印level个'*'
    cond->output(level + 4); // 调用的是ExprNode子类中的BinaryExpr（这也是cond的类型，且仅在本程序中是BinaryExpr）中的output()
    whileStmt->output(level + 4); // 调用的是StmtNode子类中的CompoundStmt中的output（因为本程序中的whileStmt是CompoundStmt类型的）
}

void ReturnStmt::output(int level)
{
    fprintf(yyout, "%*cReturnStmt\n", level, ' ');
    retValue->output(level + 4);
}

void AssignStmt::output(int level)
{
    fprintf(yyout, "%*cAssignStmt\n", level, ' ');
    lval->output(level + 4);
    expr->output(level + 4);
}

void FunctionDef::output(int level)
{
    std::string name, type;
    name = se->toStr();
    type = se->getType()->toStr();
    fprintf(yyout, "%*cFunctionDefine function name: %s, type: %s\n", level, ' ', 
            name.c_str(), type.c_str());
    if(FPs != nullptr){
        FPs -> output(level + 4);
    }
    stmt->output(level + 4);
}

void FunctionCall::output(int level)
{
    std::string name, type;
    name = symbolEntry->toStr();
    type = symbolEntry->getType()->toStr();
    fprintf(yyout, "%*cFuncCall\tname: %s\ttype: %s\n", level, ' ',
            name.c_str(), type.c_str());
    if(RPs != nullptr)
    {
        RPs -> output(level + 4);
    }
}

void IdList::output(int level)
{
    for (long unsigned int i = 0; i < idlist.size(); i++) {
        idlist.at(i)->output(level + 4);
    }
    for (long unsigned int i = 0; i < assignList.size(); i++) {
        assignList.at(i)->output(level + 4);
    }
}

void ConIdList::output(int level)
{
    for (long unsigned int i = 0; i < conidlist.size(); i++) {
        conidlist.at(i)->output(level + 4);
    }
    for (long unsigned int i = 0; i < assignList.size(); i++) {
        assignList.at(i)->output(level + 4);
    }
}

void FuncFParams::output(int level)
{
    fprintf(yyout, "%*cFuncFParams\n", level, ' ');
    for(long unsigned int i = 0; i < FPs.size(); i++)
    {
        FPs[i] -> output(level + 4);
    }
    for(long unsigned int i = 0; i < Assigns.size(); i++)
    {
        Assigns[i] -> output(level + 4);
    }
}

void FuncRParams::output(int level)
{
    fprintf(yyout, "%*cFuncRParams\n", level, ' ');
    for(long unsigned int i = 0; i < Exprs.size(); i++)
    {
        Exprs[i] -> output(level + 4);
    }
}

void Empty::output(int level)
{
    fprintf(yyout, "%*cEmpty Statement\n", level, ' ');
}

void SingleStmt::output(int level)
{
    fprintf(yyout, "%*cSingle Statement\n", level, ' ');
    expr->output(level + 4);
}