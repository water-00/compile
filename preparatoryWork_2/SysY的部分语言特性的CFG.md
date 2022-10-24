SysY的部分语言特性的CFG：

注：符号'...'表示单引号内包含的为终结符，符号(...)表示括号内包含的部分在产生式中需要先行计算

1. 变量声明
   $$
   \begin{align}
   VarDecl &\to Type \;Idlist\\
   Idlist &\to Idlist,\text{Id\;|\;Id}\\
   Type &\to \text{int\;|\;float}
   \end{align}
   $$

2. 赋值语句
   $$
   \begin{align}
   AssignStmt &\to LeftVal=Stmt\;|\;LeftVal=Expr;\\
   LeftVal &\to \text{Id}\;|\;\text{Id}[Expr]\\
   Expr &\to AddExpr\;|\;MulExpr\;|\;RelationExpr\;|\;EqualExpr\;|\;LogicalExpr\;|\;Digit\;|\;\text{Id}\\
   AddExpr &\to \text{Id}(+|-)\text{Id}\;|\;\text{Id}(+|-)Expr\\
   MulExpr &\to \text{Id}(*|/|\%)\text{Id}\;|\;\text{Id}(*|/|\%)Expr\\
   RelationExpr &\to \text{Id}(< | > | <= | >=)\text{Id}\;|\;\text{Id}(< | > | <= | >=)Expr\\
   EqualExpr &\to \text{Id}(== | !=)\text{Id}\;|\;\text{Id}(== | !=)Expr\\
   LogicalExpr &\to \text{Id}('\&\&'|'||')\text{Id}\;|\;\text{Id}('\&\&'|'||')Expr\\
   Digit&\to Number \;digit \;|\; Number\\
   Number&\to 0\;|\;1\;|\;2\;|\;3\;|\;4\;|\;5\;|\;6\;|\;7\;|\;8\;|\;9
   \end{align}
   $$

3. if语句
   $$
   \begin{align}
   Stmt &\to \text{if}\;(Expr)\;Stmt\\
   &|\;\text{if}\;(Expr)\;Stmt \;\text{else} \; Stmt\\
   &|\;\{Stmts\}\\
   Stmts&\to Stmts\;Stmt\\
   &|\;\epsilon
   \end{align}
   $$

4. while语句
   $$
   \begin{align}
   Stmt\to&|\;\text{while}\;(Expr)\;Stmt\\
   &|\;\{Stmts\}\\
   Stmts&\to Stmts\;Stmt\\
   &|\;\epsilon
   \end{align}
   $$

5. 函数定义
   $$
   \begin{align}
   FuncDef &\to Type\;FuncName(Paralist) \;Stmt\\
   Paralist&\to Paralist,Paradef\;|\;Paradef\;|\;\epsilon\\
   Paradef&\to \text{float Id}\;|\;\text{int Id}\\
   Type&\to \text{int\;|\;float\;|\;void}
   \end{align}
   $$
   
6. 算术运算

   感觉在赋值语句部分写过了。。所以这两部分不知道该怎么写（抄文档）了

7. 逻辑运算





## 2. 变量与常量

常量/变量基本类型
$$
\begin{align*}
    BType & \to \; \text{'int'} \\
    &| \quad \text{'float'}
\end{align*}
$$
常量声明
$$
\begin{align*}
    ConstDecl & \to \; \text{'const'} \; BType \; ConstDef\\
    &| \quad ConstDecl\; \text{','} \; ConstDef
\end{align*}
$$
常数定义
$$
\begin{align*}
    ConstDef & \to \; \textbf{Ident}\{ '[' ConstExp ']' \} \;  \text{'='} \; ConstInitVal \\
    &| \quad \text{'float'}
\end{align*}
$$
常量初值
$$
\begin{align*}
    ConstInitVal & \to \; ConstExp\\
    &| \quad \text{'\{'\;'\}'}\\
    &| \quad \text{'\{'} InitValList \text{'\}'}\\
    InitValList &\to InitValList \;\text{','}\; ConstExp\\
    &| \quad ConstExp
\end{align*}
$$
变量声明
$$
\begin{align*}
    VarDecl & \to BType\; VarDef\\
    &| \quad VarDecl \;\text{','} \; VarDef\\
    &| \quad 
\end{align*}
$$
变量定义
$$
\begin{align*}
    VarDecl & \to \textbf{Ident} \;\{\text{'['\;} ConstExp \; \text{']'}\}\; \\
    &| \quad \textbf{Ident}  \;\{\text{'['\;} ConstExp \; \text{']'}\}\;  \text{'='}\; InitVal\\
    &| \quad 
\end{align*}
$$
变量初值
$$
\begin{align*}
    InitVal & \to \; Exp\\
    &| \quad \text{'\{'\;'\}'}\\
    &| \quad \text{'\{'} [\;InitVal\;\{\;\text{','\;}InitVal\;\}\;]\text{'\}'}
\end{align*}
$$
这里的变量初值不需要是常量值。

## 3. 函数

函数定义
$$
\begin{align*}
    FuncDef & \to  FuncType \; \textbf{Ident}\;\text{'('\;')'}\; Block \\
    &| \quad FuncType \; \textbf{Ident}\;\text{'('\;} FuncFParams\;\text{')'}\; Block\\
\end{align*}
$$
函数类型
$$
\begin{align*}
    FuncType & \to \text{'void'} \\
    &| \quad \text{'int'}\\
    &| \quad \text{'float'}
\end{align*}
$$
函数形参表
$$
\begin{align*}
    FuncFParams & \to FuncFParam\\
    &| \quad FuncFParams \;\text{','}\; FuncFParam\\
\end{align*}
$$
函数形参
$$
\begin{align*}
    FuncFParams & \to Btype \;\textbf{Ident}\\
    &| \quad Btype \;\textbf{Ident} \; \text{'['\;']'}\\
    &| \quad FuncFParam \; \text{'['} \; Exp \; \text{']'}\\
    &| \quad \epsilon
\end{align*}
$$

## 4. 语句

语句块
$$
\begin{align*}
    Block & \to \text{'\{'\; \{\;BlockItem\;\}\;'\}'} \; 
\end{align*}
$$
语句块项
$$
\begin{align*}
    BlockItem & \to Decl\\
    &| \quad Stmt\\
\end{align*}
$$
语句
$$
\begin{align*}
    Stmt & \to \epsilon \\
    &| \quad Lval \;\text{'='}\; Exp \;\text{';'}\\
    &| \quad Exp \;\text{';'}\\
    &| \quad Block\\
    &| \quad \text{'if'}\;\text{'('}\; Cond \;\text{')'}\;Stmt\\
    &| \quad \text{'if'}\;\text{'('}\; Cond \;\text{')'}\;Stmt\; \text{'else'}\;Stmt\\
    &| \quad \text{'while'}\;\text{'('}\; Cond \;\text{')'}\;Stmt\\
    &| \quad \text{'break'}\;\text{';'}\\
    &| \quad \text{'continue'}\;\text{';'}\\
    &| \quad \text{'return'}\;\text{';'}\\
    &| \quad \text{'return'}\;Exp\;\text{';'}
\end{align*}
$$

## 5. 表达式

表达式
$$
\begin{align*}
    Exp & \to AddExpr
\end{align*}
$$
条件表达式
$$
Cond \to LOrExp
$$
左值表达式
$$
\begin{align*}
    Lval & \to \textbf{Ident}\; \\
    &| \quad  Lval \;\text{'['}\;Exp \;\text{']'}\\
\end{align*}
$$
基本表达式
$$
\begin{align*}
    PrimaryExp & \to \text{'('}\; Exp \;\text{')'} \; \\
    &| \quad Lval \\
    &| \quad Number
\end{align*}
$$
数值
$$
Number \to \textbf{IntConst}
$$
一元表达式
$$
\begin{align*}
    UnaryExp & \to PrimaryExp \; \\
    &| \quad \textbf{Ident} \;\text{'('\;')'}\\
    &| \quad \textbf{Ident} \;\text{'('\;}FuncRParams\;\text{')'}\\
    &| \quad UnaryOp \;UnaryExp
\end{align*}
$$
单目运算符
$$
\begin{align*}
    UnaryOp & \to \text{'+'}\\
    &| \quad \text{'-'}\\
    &| \quad \text{'!'}
\end{align*}
$$
函数实参表
$$
\begin{align*}
    FuncRParams & \to Exp\; \\
    &| \quad FuncRparams\; \text{','}\; Exp\\
\end{align*}
$$
乘除模表达式
$$
\begin{align*}
    MulExp & \to MulExp \;\text{'\%'} \;UnaryExp \\
    &| \quad MulExp \;\text{'*'} \;UnaryExp\\
    &| \quad MulExp \;\text{'/'} \;UnaryExp
\end{align*}
$$
加减表达式
$$
\begin{align*}
    AddExp & \to MulExp\\
    &| \quad AddExp \;\text{'}+\text{'} \;MulExp\\
    &| \quad AddExp \;\text{'}-\text{'} \;MulExp
\end{align*}
$$
关系表达式
$$
\begin{align*}
    RelExp & \to AddExp \; \\
    &| \quad RelExp \;\text{'<'}\;AddExp\\
    &| \quad RelExp \;\text{'<='}\;AddExp\\
    &| \quad RelExp \;\text{'>='}\;AddExp\\
\end{align*}
$$
相等性表达式
$$
\begin{align*}
    EqExp & \to RelExp \; \\
    &| \quad EqExp \;\text{'=='}\;RelExp\\
    &| \quad EqExp \;\text{'!='}\;RelExp\\
\end{align*}
$$
逻辑表达式
$$
\begin{align*}
    LAndExp & \to EqExp\\
    &| \quad LAndExp \;\text{'\&\&'}\; EqExp\\
    LOrExp & \to LAndExp\\
    &| \quad LOrExp \;\text{'||'}\; LAndExp\\
\end{align*}
$$
常量表达式
$$
ConstExp \to AddExp
$$
