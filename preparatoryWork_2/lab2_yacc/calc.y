%{
#include <ctype.h>
#include <iostream>
#include <stdio.h>
#include <string>
#include <vector>
#include <map>
using namespace std;

char idStr[50];
vector<pair<string, double>> symbolTable;
void addElement(vector<pair<string, double>>& vec, string str, double value); // 遇到赋值语句时，往符号表里添加元素，若没有则添加，若有则覆盖
double findTable(vector<pair<string, double>>& vec, string str); // 遇到含ID的表达式时，查找ID，找得到返回value，否则执行yyerror()
int yylex();
int yyerror(string s);
%}

%token ADD SUB MUL DIV LPARE RPARE ASSIGN

%union {
	double doubleVal;
	char* str;
}

%token <doubleVal> NUMBER
%token <str> ID

%right ASSIGN
%left ADD SUB
%left MUL DIV
%right UMINUS

%type <doubleVal> lines
%type <doubleVal> expr
%%

lines			: lines ID ASSIGN expr			{ printf("%s = %g\n", $2, $4); addElement(symbolTable, $2, $4); }	
					| lines expr ';'						{ printf("%g\n", $2); } // 为什么line的产生式是这样的？ --这样可以一次性计算多个;结尾的expr，一次给出多个答案
					| lines ';'
					|
	 	 			;

 expr			: expr ADD expr							{ $$ = $1 + $3; }
					| expr SUB expr							{ $$ = $1 - $3; }
					| expr MUL expr							{ $$ = $1 * $3; }
					| expr DIV expr							{ $$ = $1 / $3; }
					| LPARE expr RPARE					{ $$ = $2; }
					| SUB expr %prec UMINUS			{ $$ = -$2; }
					| ID												{ $$ = findTable(symbolTable, $1); }
					| NUMBER
		 	 		;  
%%

int main()
{      
	return yyparse();
}  

int yylex()
{ 
	int c;
	while ((c = getchar()) == ' ' || c == '\t' || c == '\n'); // 这一行是什么意思？ 
																														// --yylex()作为词法分析器，在每次读取函数时都会被调用，于是就用这行代码去除这次读取时的' ','\t','\n'，直到读到有意义的字符为止
	  if (c == '.' || isdigit(c)) {
			ungetc(c, stdin); // 这一行是什么意思？ --就是把c刚才get到的字符推回到输入流里
			scanf("%lf", &yylval.doubleVal); // 这一行？ --"%lf"=double
			return NUMBER; // yacc在return的时候把yylval的值隐式赋值给终结符
	 	}
	 	else if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_') {
			int i = 0;
			while (((c >= 'a' && c <= ' z') || (c >= 'A' && c <= 'Z') || c == '_' || (c >= '0') && (c <= '9')) && c != '=') {
				idStr[i] = c;
				i++;
				c = getchar();
			}
			idStr[i] = '\0';
			yylval.str = idStr;
			ungetc(c, stdin);
			return ID;
		}
		else if (c == '+') { return ADD; }
		else if (c == '-') { return SUB; } // 怎么区分减号和负号？ --好像不用在词法分析阶段区分，得到-后丢给产生式就行了
		else if (c == '*') { return MUL; }
		else if (c == '/') { return DIV; }
		else if (c == '(') { return LPARE; }
		else if (c == ')') { return RPARE; }
		else if (c == '=') { return ASSIGN; }

	return c;
}   

int yyerror(string s)
{  
	cout << "Error message: " << s << endl;
	return 1;
}

void addElement(vector<pair<string , double>>& vec, string str, double value) {
	for (int i = 0; i < vec.size(); i++) {
		if (vec[i].first == str) {
			vec[i].second = value;
			return;
		 }
	} 
	vec.push_back(make_pair(str, value));
}

double findTable(vector<pair<string, double>>& vec, string str) {
	for (int i = 0; i < vec.size(); i++) {
		if (vec[i].first == str) {
			return vec[i].second;
	 	}
	} 
	yyerror(str);
	return 0;
}
