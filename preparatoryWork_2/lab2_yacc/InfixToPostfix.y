%{
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define YYSTYPE char*
char idStr[50];
char numStr[50];
%}

%token NUMBER // 声明终结符
%token ID
%token ADD
%token SUB
%token MUL
%token DIV
%token LPARE
%token RPARE

%left ADD SUB
%left MUL DIV
%right UMINUS

%%

lines			: lines expr ';'						{ printf("%s\n", $2); }
					| lines ';'
					|
 	 	 			; 

expr			: expr ADD expr							{ $$ = (char*)malloc(50*sizeof(char)); strcpy($$, $1); strcat($$, $3); strcat($$, "+ "); }
					| expr SUB expr							{ $$ = (char*)malloc(50*sizeof(char)); strcpy($$, $1); strcat($$, $3); strcat($$, "- "); }
					| expr MUL expr							{ $$ = (char*)malloc(50*sizeof(char)); strcpy($$, $1); strcat($$, $3); strcat($$, "* "); }
					| expr DIV expr							{ $$ = (char*)malloc(50*sizeof(char)); strcpy($$, $1); strcat($$, $3); strcat($$, "/ "); }
					| LPARE expr RPARE					{ $$ = (char*)malloc(50*sizeof(char)); strcpy($$, $2); strcat($$, ""); }
					//| SUB expr %prec UMINUS			{ $$ = -$2; }
 					| NUMBER										{ $$ = (char*)malloc(50*sizeof(char)); strcpy($$, $1); strcat($$, " "); }
 					| ID												{ $$ = (char*)malloc(50*sizeof(char)); strcpy($$, $1); strcat($$, " "); }
		 	 		; 
%%

int main()
{      
	return yyparse(); // yacc提供的语法分析的接口
 }  

int yylex() // yacc提供的词法分析的接口
{     
	int c;
	while ((c = getchar()) == ' ' || c == '\t' || c == '\n'); 
		if (c >= '0' && c <= '9') {
			int i = 0;
			while (c >= '0' && c <= '9') {
				numStr[i] = c;
				i++;
				c = getchar();
			}
			numStr[i] = '\0';
			yylval = numStr;
			ungetc(c, stdin);
			return NUMBER;
		}
		else if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_') {
			int i = 0;
			while ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_' || (c >= '0' && c <= '9')) {
				idStr[i] = c;
				i++;
				c = getchar();
			}
			idStr[i] = '\0';
			yylval = idStr;
			ungetc(c, stdin);
			return ID;
		}
		else if (c == '+') { return ADD; }
		else if (c == '-') { return SUB; }
		else if (c == '*') { return MUL; }
		else if (c == '/') { return DIV; }
		else if (c == '(') { return LPARE; }
		else if (c == ')') { return RPARE; }

	return c;
}    
 
int yyerror(char *s)
{  
	printf("Error Message:%s\n",s);
	return 1;
} 

