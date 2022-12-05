%code top{
    #include <iostream>
    #include <assert.h>
    #include "parser.h"
    extern Ast ast;
    int yylex();
    int yyerror( char const * );
}

%code requires {
    #include "Ast.h"
    #include "SymbolTable.h"
    #include "Type.h"
}

%union {
    // 返回类型
    int itype;
    char* strtype;
    StmtNode* stmttype;
    ExprNode* exprtype;
    Type* type;
    IdList* idlist;
    ConIdList* conidlist;
    FuncFParams* Fstype;
    FuncRParams* FRtype;
}

%start Program
%token <strtype> ID 
%token <itype> INTEGER
%token CONST
%token IF ELSE WHILE
%token INT VOID CHAR
%token LPAREN RPAREN LBRACE RBRACE SEMICOLON COMMA
%token NOT OR AND
%token EQUAL NOTEQUAL
%token LESS LESSEQUAL GREAT GREATEQUAL
%token ADD SUB ASSIGN
%token MUL DIV MOD
%token RETURN

%nterm <stmttype> Stmts Stmt SingleStmt AssignStmt BlockStmt IfStmt WhileStmt ReturnStmt ConstDeclStmt DeclStmt FuncDef /* DefStmt ConstDefStmt*/ 
%nterm <exprtype> Exp AddExp MulExp Cond PrimaryExp UnaryExp LVal RelExp LAndExp LOrExp EqExp 
%nterm <type> Type
%nterm <idlist> IdList
%nterm <conidlist> ConIdList
%nterm <Fstype> FuncFParams
%nterm <FRtype> FuncRParams

%precedence THEN
%precedence ELSE
%%
Program:
    Stmts {
        ast.setRoot($1);
    }
    ;
Stmts:
    Stmt {$$=$1;}
    | Stmts Stmt{
        $$ = new SeqNode($1, $2);
    }
    ;
Stmt:
    SEMICOLON {$$ = new Empty();}
    | SingleStmt {$$ = $1;}
    | AssignStmt {$$=$1;}
    | BlockStmt {$$=$1;}
    | IfStmt {$$=$1;}
    | WhileStmt {$$=$1;}
    | ReturnStmt {$$=$1;}
    | DeclStmt {$$=$1;}
    // | DefStmt {$$=$1;}
    | ConstDeclStmt {$$ = $1;}
    // | ConstDefStmt {$$ = $1;}
    | FuncDef {$$=$1;}
    ;


LVal:
    ID {
        SymbolEntry *se;
        se = identifiers->lookup($1);
        if(se == nullptr)
        {
            fprintf(stderr, "identifier \"%s\" is undefined\n", (char*)$1);
            delete [](char*)$1;
            assert(se != nullptr);
        }
        $$ = new Id(se);
        delete []$1;
    }
SingleStmt: 
    Exp SEMICOLON {
        $$ = new SingleStmt($1);
    };
AssignStmt:
    LVal ASSIGN Exp SEMICOLON {
        $$ = new AssignStmt($1, $3);
    }
    ;
BlockStmt:
    LBRACE {identifiers = new SymbolTable(identifiers);} 
    Stmts RBRACE {
        $$ = new CompoundStmt($3);
        SymbolTable *top = identifiers;
        identifiers = identifiers->getPrev();
        delete top;
    }
    | LBRACE RBRACE {
        $$ = new Empty();
    }
    ;
IfStmt:
    IF LPAREN Cond RPAREN Stmt %prec THEN {
        $$ = new IfStmt($3, $5);
    }
    | IF LPAREN Cond RPAREN Stmt ELSE Stmt {
        $$ = new IfElseStmt($3, $5, $7);
    }
    ;
WhileStmt:
    WHILE LPAREN Cond RPAREN Stmt {
        $$ = new WhileStmt($3, $5);
    }
    ;
ReturnStmt:
    RETURN Exp SEMICOLON {
        $$ = new ReturnStmt($2);
    }
    ;

Exp:
    AddExp {$$ = $1;}; // 无法从Exp开始找到Cond，但可以从Cond开始找到Exp中的每种类型
Cond:
    LOrExp {$$ = $1;};
PrimaryExp:
    LPAREN Exp RPAREN {
        $$ = $2;
    }
    | LVal {
        $$ = $1;
    }
    | INTEGER {
        SymbolEntry *se = new ConstantSymbolEntry(TypeSystem::intType, $1);
        $$ = new Constant(se);
    };
UnaryExp:
    PrimaryExp {$$ = $1;}
    | ID LPAREN RPAREN {
        // 先找function，再调用FunctionCall
        SymbolEntry *se;
        se = identifiers->lookup($1);
        if(se == nullptr)
        {
            fprintf(stderr, "Function \"%s\" is undefined\n", (char*)$1);
            delete [](char*)$1;
            assert(se != nullptr);
        }
        $$ = new FunctionCall(se, nullptr);
        delete []$1;
    }
    | ID LPAREN FuncRParams RPAREN{
        SymbolEntry *se;
        se = identifiers->lookup($1);
        if(se == nullptr)
        {
            fprintf(stderr, "Function \"%s\" is undefined\n", (char*)$1);
            delete [](char*)$1;
            assert(se != nullptr);
        }
        $$ = new FunctionCall(se, $3);
        delete []$1;
    }
    | NOT UnaryExp {
        SymbolEntry *se = new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel());
        $$ = new UnaryExpr(se, UnaryExpr::NOT, $2);
    }
    | ADD UnaryExp {
        SymbolEntry *se = new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel());
        $$ = new UnaryExpr(se, UnaryExpr::UNARY_PLUS, $2);
    }
    | SUB UnaryExp {
        SymbolEntry *se = new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel());
        $$ = new UnaryExpr(se, UnaryExpr::UNARY_MINUS, $2);
    };
MulExp:
    UnaryExp {$$ = $1;}
    | MulExp MUL UnaryExp {
        SymbolEntry *se = new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel());
        $$ = new BinaryExpr(se, BinaryExpr::MUL, $1, $3);
    }
    | MulExp DIV UnaryExp {
        SymbolEntry *se = new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel());
        $$ = new BinaryExpr(se, BinaryExpr::DIV, $1, $3);
    }
    | MulExp MOD UnaryExp {
        SymbolEntry *se = new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel());
        $$ = new BinaryExpr(se, BinaryExpr::MOD, $1, $3);
    };
AddExp:
    MulExp {$$ = $1;}
    | AddExp ADD MulExp {
        SymbolEntry *se = new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel());
        $$ = new BinaryExpr(se, BinaryExpr::ADD, $1, $3);
    }
    | AddExp SUB MulExp {
        SymbolEntry *se = new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel());
        $$ = new BinaryExpr(se, BinaryExpr::SUB, $1, $3);
    };
RelExp:
    AddExp {$$ = $1;}
    | RelExp LESS AddExp {
        SymbolEntry *se = new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel());
        $$ = new BinaryExpr(se, BinaryExpr::LESS, $1, $3);
    }
    | RelExp GREAT AddExp {
        SymbolEntry *se = new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel());
        $$ = new BinaryExpr(se, BinaryExpr::GREAT, $1, $3);
    }
    | RelExp LESSEQUAL AddExp {
        SymbolEntry *se = new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel());
        $$ = new BinaryExpr(se, BinaryExpr::LESSEQUAL, $1, $3);
    }
    | RelExp GREATEQUAL AddExp {
        SymbolEntry *se = new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel());
        $$ = new BinaryExpr(se, BinaryExpr::GREATEQUAL, $1, $3);
    };
EqExp:
    RelExp {$$ = $1;}
    | EqExp EQUAL RelExp {
        SymbolEntry *se = new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel());
        $$ = new BinaryExpr(se, BinaryExpr::EQUAL, $1, $3);
    }
    | EqExp NOTEQUAL RelExp {
        SymbolEntry *se = new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel());
        $$ = new BinaryExpr(se, BinaryExpr::NOTEQUAL, $1, $3);
    };
LAndExp:
    EqExp {$$ = $1;}
    | LAndExp AND EqExp
    {
        SymbolEntry *se = new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel());
        $$ = new BinaryExpr(se, BinaryExpr::AND, $1, $3);
    };
    ;
LOrExp:
    LAndExp {$$ = $1;}
    | LOrExp OR LAndExp
    {
        SymbolEntry *se = new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel());
        $$ = new BinaryExpr(se, BinaryExpr::OR, $1, $3);
    };



Type:
    INT {
        $$ = TypeSystem::intType;
    }
    | VOID {
        $$ = TypeSystem::voidType;
    }
DeclStmt:
    Type IdList SEMICOLON {
        $$ = new DeclStmt($2);
    };
// 用了idlist后就不用区分是DeclStmt还是DefStmt了，因为idlist里既有id也有assignExpr
/* DefStmt:
    Type ID ASSIGN Exp SEMICOLON {
        SymbolEntry *se;
        se = new IdentifierSymbolEntry($1, $2, identifiers->getLevel());
        identifiers->install($2, se);
        $$ = new DefStmt(new Id(se), $4);
        delete []$2;
    };  */
ConstDeclStmt:
    CONST Type ConIdList SEMICOLON {
        $$ = new ConstDeclStmt($3);
    };
/* ConstDefStmt:
    CONST Type ID ASSIGN Exp SEMICOLON {
        SymbolEntry *se;
        se = new IdentifierSymbolEntry($2, $3, identifiers->getLevel());
        identifiers->install($3, se);
        $$ = new DefStmt(new Id(se), $5);
        delete []$3;
    } */
IdList:
    ID {
        // 大致思路：
        // 1. 把id放到符号表里去        
        SymbolEntry *se;
        se = new IdentifierSymbolEntry(TypeSystem::intType, $1, identifiers->getLevel());
        identifiers->install($1, se);
        // 2. 定义一个类IdList的成员temp，操作IdList，把这一次产生式得到的id和expression分别放到IdList中的两个vector里去
        std::vector<Id*> Ids;
        std::vector<AssignStmt*> Assigns;
        IdList *temp = new IdList(Ids, Assigns);
        temp->idlist.push_back(new Id(se));
        // 3. 非终结符IdList指向的就是这一个语句产生的所有ids和expressions
        $$ = temp;
        delete []$1;
    }
    | IdList COMMA ID {
        SymbolEntry *se;
        se = new IdentifierSymbolEntry(TypeSystem::intType, $3, identifiers->getLevel());
        identifiers->install($3, se);

        IdList *temp = $1; // 已经有IdList了
        temp->idlist.push_back(new Id(se));

        $$ = temp;
        delete []$3;

    }
    | ID ASSIGN Exp{
        SymbolEntry *se;
        se = new IdentifierSymbolEntry(TypeSystem::intType, $1, identifiers->getLevel());
        identifiers->install($1, se);

        std::vector<Id*> Ids;
        std::vector<AssignStmt*> Assigns;
        IdList *temp = new IdList(Ids, Assigns);
        Id* t = new Id(se); // 这里必须定义一个t的原因是，$ 1只是一个char*（这是在%union段定义的），没法作为AssignStmt的第一个参数
        temp->idlist.push_back(t);
        temp->assignList.push_back(new AssignStmt(t, $3)); // 这俩vector是IdList里的成员变量

        $$ = temp;
        delete []$1;
    }
    | IdList COMMA ID ASSIGN Exp {
        SymbolEntry *se;
        se = new IdentifierSymbolEntry(TypeSystem::intType, $3, identifiers->getLevel());
        identifiers->install($3, se);

        IdList *temp = $1;
        Id* t = new Id(se);
        temp->idlist.push_back(t);
        temp->assignList.push_back(new AssignStmt(t, $5));

        $$ = temp;
        delete []$3;
    };
ConIdList:
    ID ASSIGN Exp {
        SymbolEntry *se;
        se = new IdentifierSymbolEntry(TypeSystem::intType, $1, identifiers->getLevel());
        identifiers->install($1, se); 

        std::vector<ConstId*> Conids;
        std::vector<AssignStmt*> Assigns;
        ConIdList *temp = new ConIdList(Conids, Assigns);
        ConstId *t = new ConstId(se);
        temp->conidlist.push_back(t);
        temp->assignList.push_back(new AssignStmt(t, $3));

        $$ = temp;
        delete []$1;
    }
    | ConIdList COMMA ID ASSIGN Exp {
        SymbolEntry *se;
        se = new IdentifierSymbolEntry(TypeSystem::intType, $3, identifiers->getLevel());
        identifiers->install($3, se);

        ConIdList *temp = $1;
        ConstId *t = new ConstId(se);
        temp->conidlist.push_back(t);
        temp->assignList.push_back(new AssignStmt(t, $5));

        $$ = temp;
        delete []$3;
    };

FuncDef:
    Type ID LPAREN {
        Type *funcType;
        funcType = new FunctionType($1,{});
        SymbolEntry *se = new IdentifierSymbolEntry(funcType, $2, identifiers->getLevel());
        identifiers->install($2, se);
        identifiers = new SymbolTable(identifiers);
    }
    RPAREN
    BlockStmt
    {
        // 在符号表中找到id
        SymbolEntry *se;
        se = identifiers->lookup($2);
        assert(se != nullptr);
        // 定义函数，并把它加到符号表里？
        $$ = new FunctionDef(se, nullptr,$6);
        SymbolTable *top = identifiers;
        identifiers = identifiers->getPrev();
        delete top;
        delete []$2;
    }
    | Type ID LPAREN {
        Type *funcType;
        funcType = new FunctionType($1,{});
        SymbolEntry *se = new IdentifierSymbolEntry(funcType, $2, identifiers->getLevel());
        identifiers->install($2, se);
        identifiers = new SymbolTable(identifiers);
    } FuncFParams RPAREN
    BlockStmt {
        SymbolEntry *se;
        se = identifiers->lookup($2);
        assert(se != nullptr);

        $$ = new FunctionDef(se, $5 ,$7);
        SymbolTable *top = identifiers;
        identifiers = identifiers->getPrev();
        delete top;
        delete []$2;
    };

FuncFParams: 
    Type ID 
    {
        // 1. 把id加入符号表
        SymbolEntry *se;
        se = new IdentifierSymbolEntry($1, $2, identifiers->getLevel());
        identifiers->install($2, se);
        // 2. 把id和expression加入FuncFParams的vector
        std::vector<FuncFParam*> FPs;
        std::vector<AssignStmt*> Assigns;
        FuncFParams *temp = new FuncFParams(FPs, Assigns);
        temp->FPs.push_back(new FuncFParam(se));
        // 3. 非终结符FuncFParams指向这一个语句中的ids和expressions
        $$ = temp;
        delete []$2;
    }
    | FuncFParams COMMA Type ID
    {
        SymbolEntry *se;
        se = new IdentifierSymbolEntry($3, $4, identifiers->getLevel());
        identifiers->install($4, se);

        FuncFParams *temp = $1;
        temp -> FPs.push_back(new FuncFParam(se));

        $$ = temp;
        delete []$4;
    }
    | Type ID ASSIGN Exp
    {
        SymbolEntry *se;
        se = new IdentifierSymbolEntry($1, $2, identifiers->getLevel());
        identifiers->install($2, se);

        std::vector<FuncFParam*> FPs;
        std::vector<AssignStmt*> Assigns;
        FuncFParams *temp = new FuncFParams(FPs, Assigns);
        FuncFParam* t = new FuncFParam(se); // 需要一个t，理由同idlist，因为$ 2是char*
        temp -> FPs.push_back(t);
        temp -> Assigns.push_back(new AssignStmt(t, $4));

        $$ = temp;
        delete []$2;
    }
    | FuncFParams COMMA Type ID ASSIGN Exp
    {
        SymbolEntry *se;
        se = new IdentifierSymbolEntry($3, $4, identifiers->getLevel());
        identifiers->install($4, se);

        FuncFParams *temp = $1;
        FuncFParam* t = new FuncFParam(se);
        temp -> FPs.push_back(t);
        temp -> Assigns.push_back(new AssignStmt(t, $6));

        $$ = temp;
        delete []$4;
    };

FuncRParams:
    Exp
    {
        std::vector<ExprNode*> t;
        t.push_back($1);
        FuncRParams *temp = new FuncRParams(t);
        $$ = temp;
    }
    | FuncRParams COMMA Exp
    {
        FuncRParams *temp = $1;
        temp->Exprs.push_back($3);
        $$ = temp;
    }
    ;
%%

int yyerror(char const* message)
{
    std::cerr<<message<<std::endl;
    return -1;
}
