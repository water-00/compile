%option noyywrap
%option yylineno
%{
    /*
    * You will need to comment this line in lab5.
    * 1. 注释 get
    * 2. 行号&偏移 get
    * 3. 符号表
    */
    #define ONLY_FOR_LEX
    
    #ifdef ONLY_FOR_LEX
    #else
    #define YYSTYPE void *
    #include "parser.h"
    #endif

    #define YY_NO_UNPUT
    #define YY_NO_INPUT
    #include <string>

    #ifdef ONLY_FOR_LEX
    #include <vector>
    #include <map>
    #include <math.h>
    #include <ostream>
    #include <fstream>
    #include <stack>
    using namespace std;
    extern FILE *yyin; 
    extern FILE *yyout;

    int offset = 0;

    int octalInChar_toDecimal(char* str) {
        int length = strlen(str);
        int result = 0;
        for (int i = 0; i < length - 1; i++) {
            result += (str[length - i - 1] - '0') * pow(8, i);
        }
        return result;
    }

    int HexadecimalInChar_toDecimal(char* str) {
        int length = strlen(str);
        int result = 0;
        for (int i = 0; i < length - 2; i++) {
            result += (str[length - i - 1] - '0') * pow(16, i);
        }
        return result;
    }

    void DEBUG_FOR_LAB4(std::string s, std::string other = ""){
        char* lineno_str = (char *)malloc(11*sizeof(char));
        char* offset_str = (char *)malloc(11*sizeof(char));
        sprintf(lineno_str, "%d", yylineno);
        sprintf(offset_str, "%d", offset);
        std::string DEBUG_INFO = s + "\t" + lineno_str + "\t" + offset_str + "\t" + other + "\n";
        fputs(DEBUG_INFO.c_str(), yyout);
        free(lineno_str);
    }

    // for SymbolTable
    
    class SymbolTable {
	    private:
		    vector<pair<string, pair<int, int>>> item;

	    public:
            int idNum = 0; // how many ids in this table

		    SymbolTable() {}
            void addItem(string name, int block, int id) {
                item.push_back(make_pair(name, make_pair(block, id)));
                idNum++;
            }
            string getIdName(int i) {
                return item.at(i).first;
            }
            pair<int, int> getIdIndex() {
                return item.back().second;
            }
            void setIdName(string name) {
                item.back().first = name;
            }
            void setIdIndex(int block, int num) {
                // in which block, and the order in that block
                item.back().second = make_pair(block, num);
            }

            int checkIdName(string str) {
                for (int i = 0; i < (int)item.size(); i++) {
                    if (str == getIdName(i)) {
                        return i + 1; // 在表中找到了，返回它的idNum = 索引值+1,因为idNum从1开始
                    }
                }
                // 如果在这张表中没找到，则返回-1
                return -1;
            }
    };
    int blockNum = 0; // how many blocks
    int popNum = 0;
    vector <SymbolTable> STstack;

    #endif
%}

OCTAL 0[0-7]*[1-7]
HEXADECIMAL 0[xX][0-9a-fA-F]+
DECIMAL [1-9][0-9]*|0
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
    #ifdef ONLY_FOR_LEX
        BEGIN BLOCKCOMMENT;
    #else
        return blockCommentBegin;
    #endif
}
<BLOCKCOMMENT>{blockCommentElement} {}
<BLOCKCOMMENT>{blockCommentEnd} {BEGIN INITIAL;}


{lineCommentBegin} {
    #ifdef ONLY_FOR_LEX
        BEGIN LINECOMMENT;
    #else
        return lineCommentBegin;
    #endif
}
<LINECOMMENT>{lineCommentElement} {}
<LINECOMMENT>{lineCommentEnd} {BEGIN INITIAL;}

"int" {
    /* 
    * Questions: 
    *   Q1: Why we need to return INT in further labs?
    *   Q2: What is "INT" actually?
    */
    #ifdef ONLY_FOR_LEX
        DEBUG_FOR_LAB4("INT\tint");
        offset += strlen(yytext);
    #else
        return INT;
    #endif
}
"void" {
    #ifdef ONLY_FOR_LEX
        DEBUG_FOR_LAB4("VOID\tvoid");
        offset += strlen(yytext);
    #else
        return VOID;
    #endif 
}
"while" {
    #ifdef ONLY_FOR_LEX
       DEBUG_FOR_LAB4("WHILE\twhile");
       offset += strlen(yytext);
    #else
        return WHILE;
    #endif
}
"if" {
    #ifdef ONLY_FOR_LEX
        DEBUG_FOR_LAB4("IF\tif");
        offset += strlen(yytext);
    #else
        return IF;
    #endif
}
"else" {
    #ifdef ONLY_FOR_LEX
        DEBUG_FOR_LAB4("ELSE\telse");
        offset += strlen(yytext);
    #else
        return ELSE;
    #endif
}
"return" {
    #ifdef ONLY_FOR_LEX
        DEBUG_FOR_LAB4("RETURN\treturn");
        offset += strlen(yytext);
    #else
        return RETURN;
    #endif
}

"=" {
    #ifdef ONLY_FOR_LEX
        DEBUG_FOR_LAB4("ASSIGN\t=");
        offset += strlen(yytext);
    #else
        return ASSIGN;
    #endif
}
"<" {
    #ifdef ONLY_FOR_LEX
        DEBUG_FOR_LAB4("LESS\t<");
        offset += strlen(yytext);
    #else
        return LESS;
    #endif
}
"<="
">"
">="
"==" {
    #ifdef ONLY_FOR_LEX
        DEBUG_FOR_LAB4("EQUAL\t==");
        offset += strlen(yytext);
    #else
        return EQUAL;
    #endif
}
"!="
"!"
"&&"
"||"
"+" {
    #ifdef ONLY_FOR_LEX
        DEBUG_FOR_LAB4("ADD\t+");
        offset += strlen(yytext);
    #else
        return ADD;
    #endif
}
"-" {
    #ifdef ONLY_FOR_LEX
        DEBUG_FOR_LAB4("SUB\t-");
        offset += strlen(yytext);
    #else
        return SUB;
    #endif
}
"*" {
    #ifdef ONLY_FOR_LEX
        DEBUG_FOR_LAB4("MUL\t*");
        offset += strlen(yytext);
    #else
        return MUL;
    #endif
}
"/" {
    #ifdef ONLY_FOR_LEX
        DEBUG_FOR_LAB4("DIV\t/");
        offset += strlen(yytext);
    #else
        return DIV;
    #endif
}
"%" {
    #ifdef ONLY_FOR_LEX
        DEBUG_FOR_LAB4("MOD\t\%");
        offset += strlen(yytext);
    #else
        return MOD;
    #endif
}
";" {
    #ifdef ONLY_FOR_LEX
        DEBUG_FOR_LAB4("SEMICOLON\t;");
        offset += strlen(yytext);
    #else
        return SEMICOLON;
    #endif
}
"(" {
    #ifdef ONLY_FOR_LEX
        DEBUG_FOR_LAB4("LPAREN\t(");
        offset += strlen(yytext);
    #else
        return LPAREN;
    #endif
}
")" {
    #ifdef ONLY_FOR_LEX
        DEBUG_FOR_LAB4("RPAREN\t)");
        offset += strlen(yytext);
    #else
    return RPAREN;
    #endif
}
"{" {
    #ifdef ONLY_FOR_LEX
        DEBUG_FOR_LAB4("LBRACE\t{");
        offset += strlen(yytext);
        blockNum++;
        SymbolTable st;
        STstack.push_back(st); // add a SymbolTable when meeting "{"
    #else
        return LBRACE;
    #endif
}
"}" {
    #ifdef ONLY_FOR_LEX
        DEBUG_FOR_LAB4("RBRACE\t}");
        offset += strlen(yytext);
        popNum++;
        STstack.pop_back(); // delete a SymbolTable when meeting "}"
    #else
        return RBRACE;
    #endif
}
"main" {
    #ifdef ONLY_FOR_LEX
        DEBUG_FOR_LAB4("MAIN\tmain");
        offset += strlen(yytext);
    #else
        return MAIN;
    #endif
}

{ID} {    
    #ifdef ONLY_FOR_LEX
        char* str = (char *)malloc(255*sizeof(char));
        sprintf(str, "ID\t%s", yytext);

        // 实现符号表功能：
        int block, idNum;
        string idPlace = "";
        if (STstack.empty()) {
            SymbolTable st;
            STstack.push_back(st); // 全局变量和全局函数的符号表
        }
        // 查找ID是否在任意一符号表中
        for (block = STstack.size() - 1; block >= 0; block--)
        {
            // 从最top的块（也即最top的符号表）开始找
            idNum = STstack.at(block).checkIdName(str); // 如果找到了<block + popNum, idNum>就是它在最近的符号表中的位置
            if (idNum != -1)
            {
                // 找到了，存储一下便于打印
                idPlace = "<" + to_string(block + popNum) + ", " + to_string(idNum) + ">";
                break;
            }
            else
            {
                continue;
            }
        }

	    // 如果遍历所有存在的符号表都没有找到，则它是一个当前符号表的新id，把它添加到当前符号表
	    STstack.back().addItem(str, blockNum, STstack.back().idNum + 1);
        if (idPlace == "") {
            idPlace = "<" + to_string(blockNum) + ", " + to_string(STstack.back().idNum) + ">";
        }
        
        DEBUG_FOR_LAB4(str, idPlace);
        offset += strlen(yytext);
        free(str);
    #else
        return ID;
    #endif
}

{OCTAL} {
    #ifdef ONLY_FOR_LEX
        char* str = (char *)malloc(11*sizeof(char));
        sprintf(str, "OCTAL\t%d", octalInChar_toDecimal(yytext));
        DEBUG_FOR_LAB4(str);
        offset += strlen(yytext);
        free(str);
    #else
        return OCTAL
    #endif
}
{HEXADECIMAL} {
    #ifdef ONLY_FOR_LEX
        char* str = (char *)malloc(11*sizeof(char));
        sprintf(str, "HEXADECIMAL\t%d", HexadecimalInChar_toDecimal(yytext));
        DEBUG_FOR_LAB4(str);
        offset += strlen(yytext);
        free(str);
    #else
        return HEXADECIMAL
    #endif
}
{DECIMAL} {
    #ifdef ONLY_FOR_LEX
        char* str = (char *)malloc(11*sizeof(char));
        sprintf(str, "DECIMAL\t%d", atoi(yytext));
        DEBUG_FOR_LAB4(str);
        offset += strlen(yytext);
        free(str);
    #else
        return DECIMAL
    #endif
}


{EOL} {
    offset = 0;
}
{WHITE} {
    offset += strlen(yytext);
}
%%

#ifdef ONLY_FOR_LEX
int main(int argc, char **argv){
    if(argc != 5){
        fprintf(stderr, "Argument Not Enough");
        exit(EXIT_FAILURE);
    }

    if(!(yyin = fopen(argv[1], "r"))){
        fprintf(stderr, "No such file or directory: %s", argv[1]);
        exit(EXIT_FAILURE);
    }

    if(!(yyout = fopen(argv[3], "w"))){
        fprintf(stderr, "No such file or directory: %s", argv[3]);
        exit(EXIT_FAILURE);
    }

    yylex();
    return 0;
}
#endif
