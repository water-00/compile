%code top{
    #include <iostream>
    #include <assert.h>
    #include "parser.h"
    extern Ast ast;
    int yylex();
    int yyerror( char const * );
    int test = 1;
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
%token LESS LESSEQUAL GREATER GREATEREQUAL
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
        // std::cout << "parser.y  Progrom: Stmts" << std::endl;
        ast.setRoot($1);
    }
    ;
Stmts:
    Stmt {
        // std::cout << "parser.y  Stmts: Stmt" << std::endl;
        $$=$1;}
    | Stmts Stmt{
        $$ = new SeqNode($1, $2);
    }
    ;
Stmt:
    SEMICOLON {
        // std::cout << "parser.y  Stmt: SEMICOLON" << std::endl;
        $$ = new Empty();}
    | SingleStmt {
        // std::cout << "parser.y  Stmt: SingleStmt" << std::endl;
        $$ = $1;}
    | AssignStmt {
        // std::cout << "parser.y  Stmt: AssignStmt" << std::endl;
        $$=$1;}
    | BlockStmt {
        // std::cout << "parser.y  Stmt: BlockStmt" << std::endl;
        $$=$1;}
    | IfStmt {
        // std::cout << "parser.y  Stmt: IfStmt" << std::endl;
        $$=$1;}
    | WhileStmt {
        // std::cout << "parser.y  Stmt: WhileStmt" << std::endl;
        $$=$1;}
    | ReturnStmt {
        // std::cout << "parser.y  Stmt: ReturnStmt" << std::endl;
        $$=$1;}
    | DeclStmt {
        // std::cout << "parser.y  Stmt: DeclStmt" << std::endl;
        $$=$1;}
    // | DefStmt {$$=$1;}
    | ConstDeclStmt {
        // std::cout << "parser.y  Stmt: ConstDeclStmt" << std::endl;
        $$ = $1;}
    // | ConstDefStmt {$$ = $1;}
    | FuncDef {
        // std::cout << "parser.y  Stmt: FuncDef" << std::endl;
        $$=$1;}
    ;


LVal:
    ID {
        // std::cout << "parser.y" << "  LVal: ID" << std::endl;
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
        // std::cout << "parser.y" << "  SingleStmt: Exp SEMICOLON" << std::endl;
        $$ = new SingleStmt($1);
    };
AssignStmt:
    LVal ASSIGN Exp SEMICOLON {
        // std::cout << "parser.y" << "  AssignStmt: LVal ASSIGN Exp SEMICOLON" << std::endl;
        $$ = new AssignStmt($1, $3);
    }
    ;
BlockStmt:
    LBRACE {
        // std::cout << "parser.y" << "  BlockStmt: LBRACE Stmts RBACE" << std::endl;
        identifiers = new SymbolTable(identifiers);
    } 
    Stmts RBRACE {
        $$ = new CompoundStmt($3);
        SymbolTable *top = identifiers;
        identifiers = identifiers->getPrev();
        delete top;
    }
    | LBRACE RBRACE {
        // std::cout << "parser.y" << "  BlockStmt: LBRACE RBRACE" << std::endl;
        $$ = new Empty();
    }
    ;
IfStmt:
    IF LPAREN Cond RPAREN Stmt %prec THEN {
        // std::cout << "parser.y" << "  IfStmt: IF LPAREN Cond RPAREN Stmt %%prec THEN" << std::endl;
        $$ = new IfStmt($3, $5);
    }
    | IF LPAREN Cond RPAREN Stmt ELSE Stmt {
        // std::cout << "parser.y" << "  IfStmt: IF LPAREN Cond RPAREN Stmt ELSE Stmt" << std::endl;
        $$ = new IfElseStmt($3, $5, $7);
    }
    ;
WhileStmt:
    WHILE LPAREN Cond RPAREN Stmt {
        // std::cout << "parser.y" << "  WhileStmt: WHILE LPAREN Cond RPAREN Stmt" << std::endl;
        $$ = new WhileStmt($3, $5);
    }
    ;
ReturnStmt:
    RETURN Exp SEMICOLON {
        // std::cout << "parser.y" << "  ReturnStmt: RETURN Exp SEMICOLON" << std::endl;
        $$ = new ReturnStmt($2);
    }
    ;

Exp:
    AddExp {
        // std::cout << "parser.y" << "  Exp: AddExp" << std::endl;
        $$ = $1;}; // 无法从Exp开始找到Cond，但可以从Cond开始找到Exp中的每种类型
Cond:
    LOrExp {
        // std::cout << "parser.y" << "  Cond: LOrExp" << std::endl;
        $$ = $1;};
PrimaryExp:
    LPAREN Exp RPAREN {
        // std::cout << "parser.y" << "  PrimaryExp: LPAREN Exp RPAREN" << std::endl;
        $$ = $2;
    }
    | LVal {
        // std::cout << "parser.y" << "  PrimaryExp: LVal" << std::endl;
        $$ = $1;
    }
    | INTEGER {
        // std::cout << "parser.y" << "  PrimaryExp: INTEGER" << std::endl;
        SymbolEntry *se = new ConstantSymbolEntry(TypeSystem::intType, $1);
        $$ = new Constant(se);
    };
UnaryExp:
    PrimaryExp {
        // std::cout << "parser.y" << "  UnaryExp: PrimaryExp" << std::endl;
        $$ = $1;}
    | ID LPAREN RPAREN {
        // std::cout << "parser.y" << "  UnaryExp: ID LPAREN RPAREN" << std::endl;
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
        // std::cout << "parser.y" << "  UnaryExp: ID LPAREN FuncRParams RPAREN" << std::endl;
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
        // std::cout << "parser.y" << "  UnaryExp: NOT UnaryExp" << std::endl;
        SymbolEntry *se = new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel());
        $$ = new UnaryExpr(se, UnaryExpr::NOT, $2);
    }
    | ADD UnaryExp {
        // std::cout << "parser.y" << "  UnaryExp: ADD UnaryExp" << std::endl;
        SymbolEntry *se = new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel());
        $$ = new UnaryExpr(se, UnaryExpr::UNARY_PLUS, $2);
    }
    | SUB UnaryExp {
        // std::cout << "parser.y" << "  UnaryExp: SUB UnaryExp" << std::endl;
        SymbolEntry *se = new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel());
        $$ = new UnaryExpr(se, UnaryExpr::UNARY_MINUS, $2);
    };
MulExp:
    UnaryExp {
        // std::cout << "parser.y" << "  MulExp: UnaryExp" << std::endl;
        $$ = $1;}
    | MulExp MUL UnaryExp {
        // std::cout << "parser.y" << "  MulExp: MulExp MUL UnaryExp" << std::endl;
        SymbolEntry *se = new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel());
        $$ = new BinaryExpr(se, BinaryExpr::MUL, $1, $3);
    }
    | MulExp DIV UnaryExp {
        // std::cout << "parser.y" << "  MulExp: MulExp DIV UnaryExp" << std::endl;
        SymbolEntry *se = new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel());
        $$ = new BinaryExpr(se, BinaryExpr::DIV, $1, $3);
    }
    | MulExp MOD UnaryExp {
        // std::cout << "parser.y" << "  MulExp: MulExp MOD UnaryExp" << std::endl;
        SymbolEntry *se = new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel());
        $$ = new BinaryExpr(se, BinaryExpr::MOD, $1, $3);
    };
AddExp:
    MulExp {
        // std::cout << "parser.y" << "  AddExp: MulExp" << std::endl;
        $$ = $1;}
    | AddExp ADD MulExp {
        // std::cout << "parser.y" << "  AddExp: AddExp ADD MulExp" << std::endl;
        SymbolEntry *se = new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel());
        $$ = new BinaryExpr(se, BinaryExpr::ADD, $1, $3);
    }
    | AddExp SUB MulExp {
        // std::cout << "parser.y" << "  AddExp: AddExp SUB MulExp" << std::endl;
        SymbolEntry *se = new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel());
        $$ = new BinaryExpr(se, BinaryExpr::SUB, $1, $3);
    };
RelExp:
    AddExp {
        // std::cout << "parser.y" << "  RelExp: AddExp" << std::endl;
        $$ = $1;}
    | RelExp LESS AddExp {
        // std::cout << "parser.y" << "  RelExp: RelExp LESS AddExp" << std::endl;
        SymbolEntry *se = new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel());
        $$ = new BinaryExpr(se, BinaryExpr::LESS, $1, $3);
    }
    | RelExp GREATER AddExp {
        // std::cout << "parser.y" << "  RelExp: RelExp GREATER AddExp" << std::endl;
        SymbolEntry *se = new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel());
        $$ = new BinaryExpr(se, BinaryExpr::GREATER, $1, $3);
    }
    | RelExp LESSEQUAL AddExp {
        // std::cout << "parser.y" << "  RelExp: RelExp LESSEQUAL AddExp" << std::endl;
        SymbolEntry *se = new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel());
        $$ = new BinaryExpr(se, BinaryExpr::LESSEQUAL, $1, $3);
    }
    | RelExp GREATEREQUAL AddExp {
        // std::cout << "parser.y" << "  RelExp: RelExp GREATEREQUAL AddExp" << std::endl;
        SymbolEntry *se = new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel());
        $$ = new BinaryExpr(se, BinaryExpr::GREATEREQUAL, $1, $3);
    };
EqExp:
    RelExp {
        // std::cout << "parser.y" << "  EqExp: RelExp" << std::endl;
        $$ = $1;}
    | EqExp EQUAL RelExp {
        // std::cout << "parser.y" << "  EqExp: EqExp EQUAL RelExp" << std::endl;
        SymbolEntry *se = new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel());
        $$ = new BinaryExpr(se, BinaryExpr::EQUAL, $1, $3);
    }
    | EqExp NOTEQUAL RelExp {
        // std::cout << "parser.y" << "  EqExp: EqExp NOTEQUAL RelExp" << std::endl;
        SymbolEntry *se = new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel());
        $$ = new BinaryExpr(se, BinaryExpr::NOTEQUAL, $1, $3);
    };
LAndExp:
    EqExp {
        // std::cout << "parser.y" << "  LAndExp: EqExp" << std::endl;
        $$ = $1;}
    | LAndExp AND EqExp
    {
        // std::cout << "parser.y" << "  LAndExp: LAndExp AND EqExp" << std::endl;
        SymbolEntry *se = new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel());
        $$ = new BinaryExpr(se, BinaryExpr::AND, $1, $3);
    };
    ;
LOrExp:
    LAndExp {
        // std::cout << "parser.y" << "  LOrExp: LAndExp" << std::endl;
        $$ = $1;}
    | LOrExp OR LAndExp
    {
        // std::cout << "parser.y" << "  LOrExp: LOrExp OR LAndExp" << std::endl;
        SymbolEntry *se = new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel());
        $$ = new BinaryExpr(se, BinaryExpr::OR, $1, $3);
    };



Type:
    INT {
        // std::cout << "parser.y" << "  INT" << std::endl;
        $$ = TypeSystem::intType;
    }
    | VOID {
        // std::cout << "parser.y" << "  VOID" << std::endl;
        $$ = TypeSystem::voidType;
    }
DeclStmt:
    Type IdList SEMICOLON {
        // std::cout << "parser.y" << "  DeclStmt" << std::endl;
        $$ = new DeclStmt($2);
    };
// 用了idlist后就不用区分是DeclStmt还是DefStmt了，因为idlist里既有id也有assignExpr
// 否则会有shfit/reduce错误
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
        // std::cout << "parser.y" << "  ConstDeclStmt" << std::endl;
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
        // std::cout << "parser.y" << "  IdList: ID" << std::endl;
        // 大致思路：
        // 1. 先进行类型检查，如果符号表没有则把id放到符号表里去
        SymbolEntry *se;
        se = new IdentifierSymbolEntry(TypeSystem::intType, $1, identifiers->getLevel());
        if (!identifiers->lookup($1)) {
            identifiers->install($1, se);
        } // else { //2012679 去掉重定义报错这部分
           // fprintf(stderr,"identifier \"%s\" is redefined\n",(char*)$1);
           // delete [](char*)$1;
           // exit(EXIT_FAILURE);
       // }
        
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
        // std::cout << "parser.y" << "  IdList: IdList COMMA ID" << std::endl;
        SymbolEntry *se;
        se = new IdentifierSymbolEntry(TypeSystem::intType, $3, identifiers->getLevel());
        if (!identifiers->lookup($3)) {
            identifiers->install($3, se);
        } // else {//2012679 去掉重定义报错这部分
           // fprintf(stderr,"identifier \"%s\" is redefined\n",(char*)$3);
           // delete [](char*)$3;
            //exit(EXIT_FAILURE);
        // }
        IdList *temp = $1; // 已经有IdList了
        temp->idlist.push_back(new Id(se));

        $$ = temp;
        delete []$3;

    }
    | ID ASSIGN Exp{
        // std::cout << "parser.y" << "  IdList: ID ASSIGN Exp" << std::endl;
        SymbolEntry *se;
        se = new IdentifierSymbolEntry(TypeSystem::intType, $1, identifiers->getLevel());
        identifiers->install($1, se);
        // 2012679 增 
        ((IdentifierSymbolEntry*)se)->setValue($3->getValue()); 
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
        // std::cout << "parser.y" << "  IdList: IdList COMMA ID ASSIGN Exp" << std::endl;
        SymbolEntry *se;
        se = new IdentifierSymbolEntry(TypeSystem::intType, $3, identifiers->getLevel());
        if (!identifiers->lookup($3)) {
            identifiers->install($3, se);
        } // else {  //2012679 去掉重定义报错这部分
           // fprintf(stderr,"identifier \"%s\" is redefined\n",(char*)$3);
         //   delete [](char*)$3;
         //   exit(EXIT_FAILURE);
     //   }
        // 2012679 增 
        ((IdentifierSymbolEntry*)se)->setValue($5->getValue());
        IdList *temp = $1;
        Id* t = new Id(se);
        temp->idlist.push_back(t);
        temp->assignList.push_back(new AssignStmt(t, $5));

        $$ = temp;
        delete []$3;
    };
ConIdList:
    ID ASSIGN Exp {
        // std::cout << "parser.y" << "  ConIdList: Id ASSIGN Exp" << std::endl;
        SymbolEntry *se;
        se = new IdentifierSymbolEntry(TypeSystem::intType, $1, identifiers->getLevel());
        if (!identifiers->lookup($1)) {
            identifiers->install($1, se);
        } // else { //2012679 去掉重定义报错这部分
          //  fprintf(stderr,"identifier \"%s\" is redefined\n",(char*)$1);
          //  delete [](char*)$1;
          //  exit(EXIT_FAILURE);
      //  }
        // 2012679 增
        ((IdentifierSymbolEntry*)se)->setConst();
        ((IdentifierSymbolEntry*)se)->setValue($3->getValue()); 
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
        // std::cout << "parser.y" << "  ConIdList: ConIdList COMMA ID ASSIGN Exp" << std::endl;
        SymbolEntry *se;
        se = new IdentifierSymbolEntry(TypeSystem::intType, $3, identifiers->getLevel());
        if (!identifiers->lookup($3)) {
            identifiers->install($3, se);
        }
        // 2012679 增
        ((IdentifierSymbolEntry*)se)->setValue($5->getValue()); 
        ((IdentifierSymbolEntry*)se)->setConst();
        ConIdList *temp = $1;
        ConstId *t = new ConstId(se);
        temp->conidlist.push_back(t);
        temp->assignList.push_back(new AssignStmt(t, $5));

        $$ = temp;
        delete []$3;
    };

FuncDef:
    Type ID LPAREN {
        // std::cout << "parser.y" << "  FuncDef: Type ID LPAREN RPAREN BlockStmt" << std::endl;
        Type *funcType;
        funcType = new FunctionType($1,{});
        SymbolEntry *se = new IdentifierSymbolEntry(funcType, $2, identifiers->getLevel());
        if (!identifiers->lookup($2)) {
            identifiers->install($2, se);
        } else {
            fprintf(stderr,"function \"%s\" is redefined\n",(char*)$2);
            delete [](char*)$2;
            exit(EXIT_FAILURE);
        }
        identifiers = new SymbolTable(identifiers);
    }
    RPAREN
    BlockStmt
    {
        // 在符号表中找到id
        SymbolEntry *se;
        se = identifiers->lookup($2);
        assert(se != nullptr);
        // 定义函数，并把它加到符号表里
        $$ = new FunctionDef(se, nullptr, $6);
        SymbolTable *top = identifiers;
        identifiers = identifiers->getPrev();
        delete top;
        delete []$2;
    }
    | Type ID LPAREN {
        // std::cout << "parser.y" << "  FuncDef: Type ID LPAREN FuncFParams RPAREN BlockStmt" << std::endl;
        Type *funcType;
        funcType = new FunctionType($1,{});
        SymbolEntry *se = new IdentifierSymbolEntry(funcType, $2, identifiers->getLevel());
        if (!identifiers->lookup($2)) {
            identifiers->install($2, se);
        } else {
            fprintf(stderr,"function \"%s\" is redefined\n",(char*)$2);
            delete [](char*)$2;
            exit(EXIT_FAILURE);
        }
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
        // std::cout << "parser.y" << "  FuncFParams: Type ID" << std::endl;
        // 1. 把id加入符号表
        SymbolEntry *se;
        se = new IdentifierSymbolEntry($1, $2, identifiers->getLevel());
        identifiers->install($2, se);
        // 2. 把id和expression加入FuncFParams的vector
        std::vector<FuncFParam*> FPs;
        std::vector<AssignStmt*> Assigns;
        FuncFParams *temp = new FuncFParams(FPs, Assigns);
        temp->FPs.push_back(new FuncFParam(se)); // 问题：应该是要把FPs的元素（或者说把FuncFParam的构造函数的参数）改成IdentifierSymbolEntry而不是SymbolEntry的
        // 3. 非终结符FuncFParams指向这一个语句中的ids和expressions
        $$ = temp;
        delete []$2;
    }
    | FuncFParams COMMA Type ID
    {
        // std::cout << "parser.y" << "  FuncFParams: FuncFParams COMMA Type ID" << std::endl;
        SymbolEntry *se;
        se = new IdentifierSymbolEntry($3, $4, identifiers->getLevel());
        identifiers->install($4, se);

        FuncFParams *temp = $1;
        temp->FPs.push_back(new FuncFParam(se));

        $$ = temp;
        delete []$4;
    }
    | Type ID ASSIGN Exp
    {
        // std::cout << "parser.y" << "  FuncFParams: Type ID ASSIGN Exp" << std::endl;
        SymbolEntry *se;
        se = new IdentifierSymbolEntry($1, $2, identifiers->getLevel());
        identifiers->install($2, se);

        std::vector<FuncFParam*> FPs;
        std::vector<AssignStmt*> Assigns;
        FuncFParams *temp = new FuncFParams(FPs, Assigns);
        FuncFParam* t = new FuncFParam(se); // 需要一个t，理由同idlist，因为$ 2是char*
        temp->FPs.push_back(t);
        temp->Assigns.push_back(new AssignStmt(t, $4));

        $$ = temp;
        delete []$2;
    }
    | FuncFParams COMMA Type ID ASSIGN Exp
    {
        // std::cout << "parser.y" << "  FuncFParams: FuncFParams COMMA Type ID = ASSIGN Exp" << std::endl;
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
        // std::cout << "parser.y" << "  FuncRParams: Exp" << std::endl;
        std::vector<ExprNode*> t;
        t.push_back($1);
        FuncRParams *temp = new FuncRParams(t);
        $$ = temp;
    }
    | FuncRParams COMMA Exp
    {
        // std::cout << "parser.y" << "  FuncRParams: FuncRParams COMMA Exp" << std::endl;
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
