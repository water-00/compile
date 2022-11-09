%option noyywrap
%{
    #define YY_NO_UNPUT
    #define YY_NO_INPUT

    #include "parser.h"
    #include <ostream>
    #include <fstream>
    using namespace std;

    extern FILE *yyin; 
    extern FILE *yyout;
    extern bool dump_tokens;

    void DEBUG_FOR_LAB4(std::string s){
        std::string DEBUG_INFO = "[DEBUG LAB4]: \t" + s + "\n";
        fputs(DEBUG_INFO.c_str(), yyout);
    }
%}

OCTAL (0[0-7]*[1-7])
HEXADECIMAL (0[xX][0-9a-fA-F]+)
DECIMAL ([1-9][0-9]*|0)
ID [[:alpha:]_][[:alpha:][:digit:]_]*
EOL (\r\n|\n|\r)
WHITE [\t ]
blockCommentBegin "/*"
blockCommentElement .|\n
blockCommentEnd "*/"
lineCommentBegin "//"
lineCommentEnd "\n"
lineCommentElement .

%x BLOCKCOMMENT
%x LINECOMMENT

%%
{blockCommentBegin} {
    BEGIN BLOCKCOMMENT;
}
<BLOCKCOMMENT>{blockCommentElement} {}
<BLOCKCOMMENT>{blockCommentEnd} {BEGIN INITIAL;}


{lineCommentBegin} {
    BEGIN LINECOMMENT;
}
<LINECOMMENT>{lineCommentElement} {}
<LINECOMMENT>{lineCommentEnd} {BEGIN INITIAL;}

"int" {
    if(dump_tokens)
        DEBUG_FOR_LAB4("INT\tint");
    return INT;
}
"void" {
    if(dump_tokens)
        DEBUG_FOR_LAB4("VOID\tvoid");
    return VOID;
}
"if" {
    if(dump_tokens)
        DEBUG_FOR_LAB4("IF\tif");
    return IF;
};
"else" {
    if(dump_tokens)
        DEBUG_FOR_LAB4("ELSE\telse");
    return ELSE;
};
"return" {
    if(dump_tokens)
        DEBUG_FOR_LAB4("RETURN\treturn");
    return RETURN;
}
"while" {
    if(dump_tokens)
        DEBUG_FOR_LAB4("WHILE\twhile");
    return WHILE;
}

"=" {
    if(dump_tokens)
        DEBUG_FOR_LAB4("ASSIGN\t=");
    return ASSIGN;
}
"+" {
    if(dump_tokens)
        DEBUG_FOR_LAB4("ADD\t+");
    return ADD;
}
"-" {
    if(dump_tokens)
        DEBUG_FOR_LAB4("SUB\t+");
    return SUB;
}
"*" {
    if(dump_tokens)
        DEBUG_FOR_LAB4("MUL\t+");
    return MUL;
}
"/" {
    if(dump_tokens)
        DEBUG_FOR_LAB4("DIV\t+");
    return DIV;
}
"%" {
    if(dump_tokens)
        DEBUG_FOR_LAB4("MOD\t+");
    return MOD;
}

"<" {
    if(dump_tokens)
        DEBUG_FOR_LAB4("LESS\t<");
    return LESS;
}
"<=" {
    if(dump_tokens)
        DEBUG_FOR_LAB4("LESSEQUAL\t<=");
    return LESSEQUAL;
}
">" {
    if(dump_tokens)
        DEBUG_FOR_LAB4("GREAT\t>");
    return GREAT;
}
">=" {
    if(dump_tokens)
        DEBUG_FOR_LAB4("GREATEQUAL\t>=");
    return GREATEQUAL;
}
"==" {
    if(dump_tokens)
        DEBUG_FOR_LAB4("EQUAL\t==");
    return EQUAL;
}
"!=" {
    if(dump_tokens)
        DEBUG_FOR_LAB4("NOTEQUAL\t!=");
    return NOTEQUAL;
}

"!" {
    if(dump_tokens)
        DEBUG_FOR_LAB4("NOT\t!");
    return NOT;
}
"&&" {
    if(dump_tokens)
        DEBUG_FOR_LAB4("AND\t&&");
    return AND;
}
"||" {
    if(dump_tokens)
        DEBUG_FOR_LAB4("OR\t||");
    return OR;
}

";" {
    if(dump_tokens)
        DEBUG_FOR_LAB4("SEMICOLON\t;");
    return SEMICOLON;
}
"(" {
    if(dump_tokens)
        DEBUG_FOR_LAB4("LPAREN\t(");
    return LPAREN;
}
")" {
    if(dump_tokens)
        DEBUG_FOR_LAB4("RPAREN\t)");
    return RPAREN;
}
"{" {
    if(dump_tokens)
        DEBUG_FOR_LAB4("LBRACE\t{");
    return LBRACE;
}
"}" {
    if(dump_tokens)
        DEBUG_FOR_LAB4("RBRACE\t}");
    return RBRACE;
}

"getint" {
    if(dump_tokens)
        DEBUG_FOR_LAB4("GETINT\tgetint");
    return GETINT;
}
"getch" {
    if(dump_tokens)
        DEBUG_FOR_LAB4("GETCH\tgetch");
    return GETCH;
}
"getfloat" {
    if(dump_tokens)
        DEBUG_FOR_LAB4("GETFLOAT\tgetfloat");
    return GETFLOAT;
}
"getarray" {
    if(dump_tokens)
        DEBUG_FOR_LAB4("GETARRAY\tgetarray");
    return GETARRAY;
}
"getfarray" {
    if(dump_tokens)
        DEBUG_FOR_LAB4("GETFARRAY\tgetfarray");
    return GETFARRAY;
}
"putint" {
    if(dump_tokens)
        DEBUG_FOR_LAB4("PUTINT\tputint");
    return PUTINT;
}
"putch" {
    if(dump_tokens)
        DEBUG_FOR_LAB4("PUTCH\tputch");
    return PUTCH;
}
"putfloat" {
    if(dump_tokens)
        DEBUG_FOR_LAB4("PUTFLOAT\tputfloat");
    return PUTFLOAT;
}
"putarray" {
    if(dump_tokens)
        DEBUG_FOR_LAB4("PUTARRAY\tputarray");
    return PUTARRAY;
}
"putfarray" {
    if(dump_tokens)
        DEBUG_FOR_LAB4("PUTFARRAY\tputfarray");
    return PUTFARRAY;
}
"putf" {
    if(dump_tokens)
        DEBUG_FOR_LAB4("PUTF\tputf");
    return PUTF;
}
"starttime" {
    if(dump_tokens)
        DEBUG_FOR_LAB4("STARTTIME\tstarttime");
    return STARTTIME;
}
"stoptime" {
    if(dump_tokens)
        DEBUG_FOR_LAB4("STOPTIME\tstoptime");
    return STOPTIME;
}

{DECIMAL} {
    if(dump_tokens)
        DEBUG_FOR_LAB4(yytext);
    yylval.itype = atoi(yytext);
    return INTEGER;
}

{ID} {
    if(dump_tokens)
        DEBUG_FOR_LAB4(yytext);
    char *lexeme;
    lexeme = new char[strlen(yytext) + 1];
    strcpy(lexeme, yytext);
    yylval.strtype = lexeme;
    return ID;
}
{EOL} yylineno++;
{WHITE}
%%
