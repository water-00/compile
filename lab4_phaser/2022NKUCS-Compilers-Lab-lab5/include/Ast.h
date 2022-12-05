#ifndef __AST_H__
#define __AST_H__

#include <fstream>
#include <vector>

class SymbolEntry;
class AssignStmt;

class Node
{
private:
    static int counter;
    int seq;
public:
    Node();
    int getSeq() const {return seq;};
    virtual void output(int level) = 0;
};

class ExprNode : public Node
{
protected:
    SymbolEntry *symbolEntry;
public:
    ExprNode(SymbolEntry *symbolEntry) : symbolEntry(symbolEntry){};
};

class UnaryExpr : public ExprNode
{
private:
    int op;
    ExprNode *expr;
public:
    enum {NOT, UNARY_PLUS, UNARY_MINUS};
    UnaryExpr(SymbolEntry *se, int op, ExprNode* expr) : ExprNode(se), op(op), expr(expr){};
    void output(int level);
};

class BinaryExpr : public ExprNode
{
private:
    int op;
    ExprNode *expr1, *expr2;
public:
    enum {OR, AND, EQUAL, NOTEQUAL, LESS, LESSEQUAL, GREAT, GREATEQUAL, ADD, SUB, MOD, ASSIGN, MUL, DIV};
    BinaryExpr(SymbolEntry *se, int op, ExprNode*expr1, ExprNode*expr2) : ExprNode(se), op(op), expr1(expr1), expr2(expr2){};
    void output(int level);
};

class Constant : public ExprNode
{
public:
    Constant(SymbolEntry *se) : ExprNode(se){};
    void output(int level);
};

class Id : public ExprNode
{
public:
    Id(SymbolEntry *se) : ExprNode(se){};
    void output(int level);
};

class ConstId : public ExprNode
{
public:
    ConstId(SymbolEntry *se) : ExprNode(se){};
    void output(int level);
};






class ListNode : public Node 
{};

class IdList : public ListNode 
{
public:
    std::vector<Id*> idlist;
    std::vector<AssignStmt*> assignList;
    IdList(std::vector<Id*> idlist, std::vector<AssignStmt*> assignList) : idlist(idlist), assignList(assignList) {};
    void output(int level);
};

class ConIdList : public ListNode
{
public:
    std::vector<ConstId*> conidlist; // 因为const语句中必须要给初值，所以实际上不用conidlist（不过也许后面函数的参数列表会用到？）
    std::vector<AssignStmt*> assignList;
    ConIdList(std::vector<ConstId*> conidlist, std::vector<AssignStmt*> assignList) : conidlist(conidlist), assignList(assignList) {};
    void output(int level);
};

class FuncFParam : public ExprNode
{
public:
    FuncFParam(SymbolEntry *se) : ExprNode(se){};
    void output(int level);
};

class FuncRParams : public ListNode
{
public:
    std::vector<ExprNode*> Exprs;
    FuncRParams(std::vector<ExprNode*> Exprs) : Exprs(Exprs){};
    void output(int level);
};

class FuncFParams : public ListNode
{
public:
    std::vector<FuncFParam*> FPs;
    std::vector<AssignStmt*> Assigns;
    FuncFParams(std::vector<FuncFParam*> FPs, std::vector<AssignStmt*> Assigns) : FPs(FPs), Assigns(Assigns) {};
    void output(int level);
};








class StmtNode : public Node
{};

class Empty : public StmtNode
{
public:
    void output(int level);
};

class SingleStmt : public StmtNode
{
private:
    ExprNode* expr;
public:
    SingleStmt(ExprNode* expr) : expr(expr){};
    void output(int level);
};

class CompoundStmt : public StmtNode
{
private:
    StmtNode *stmt;
public:
    CompoundStmt(StmtNode *stmt) : stmt(stmt) {};
    void output(int level);
};

class SeqNode : public StmtNode
{
private:
    StmtNode *stmt1, *stmt2;
public:
    SeqNode(StmtNode *stmt1, StmtNode *stmt2) : stmt1(stmt1), stmt2(stmt2){};
    void output(int level);
};

class DeclStmt : public StmtNode
{
private:
    IdList *ids;
public:
    DeclStmt(IdList *ids) : ids(ids){};
    void output(int level);
};

class DefStmt : public StmtNode
{
private:
    IdList *id;
    ExprNode *expr;
public:
    DefStmt(IdList *id) : id(id){};
    void output(int level);
};

class ConstDeclStmt : public StmtNode
{
private:
    ConIdList *cids;
public:
    ConstDeclStmt(ConIdList *cids) : cids(cids){};
    void output(int level);
};

class ConstDefStmt : public StmtNode
{
private:
    ConstId *cid;
    ExprNode *expr;
public:
    ConstDefStmt(ConstId *cid) : cid(cid){};
    void output(int level);
};

class IfStmt : public StmtNode
{
private:
    ExprNode *cond;
    StmtNode *thenStmt;
public:
    IfStmt(ExprNode *cond, StmtNode *thenStmt) : cond(cond), thenStmt(thenStmt){};
    void output(int level);
};

class IfElseStmt : public StmtNode
{
private:
    ExprNode *cond;
    StmtNode *thenStmt;
    StmtNode *elseStmt;
public:
    IfElseStmt(ExprNode *cond, StmtNode *thenStmt, StmtNode *elseStmt) : cond(cond), thenStmt(thenStmt), elseStmt(elseStmt) {};
    void output(int level);
};

class WhileStmt : public StmtNode
{
private:
    ExprNode *cond;
    StmtNode *whileStmt;
public:
    WhileStmt(ExprNode *cond, StmtNode *whileStmt) : cond(cond), whileStmt(whileStmt) {};
    void output(int level);
};

class ReturnStmt : public StmtNode
{
private:
    ExprNode *retValue;
public:
    ReturnStmt(ExprNode*retValue) : retValue(retValue) {};
    void output(int level);
};

class AssignStmt : public StmtNode
{
private:
    ExprNode *lval;
    ExprNode *expr;
public:
    AssignStmt(ExprNode *lval, ExprNode *expr) : lval(lval), expr(expr) {};
    void output(int level);
};

class FunctionDef : public StmtNode
{
private:
    SymbolEntry *se;
    FuncFParams *FPs;
    StmtNode *stmt;
public:
    FunctionDef(SymbolEntry *se, FuncFParams *FPs, StmtNode *stmt) : se(se), FPs(FPs), stmt(stmt){};
    void output(int level);
};

class FunctionCall : public ExprNode
{
public:
    FuncRParams *RPs;
    FunctionCall(SymbolEntry*se, FuncRParams *RPs) : ExprNode(se), RPs(RPs){};
    void output(int level);
};

class Ast
{
private:
    Node* root;
public:
    Ast() {root = nullptr;}
    void setRoot(Node*n) {root = n;}
    void output();
};

#endif
