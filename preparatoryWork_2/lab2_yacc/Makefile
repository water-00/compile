all: calc InfixToPostfix

calc.tab.c calc.tab.h:	calc.y
	bison -t -v -d calc.y

InfixToPostfix.tab.c InfixToPostfix.tab.h: InfixToPostfix.y
	bison -t -v -d InfixToPostfix.y

calc: calc.tab.c calc.tab.h
	g++ -Wall -o calc calc.tab.c

InfixToPostfix: InfixToPostfix.tab.c InfixToPostfix.tab.h
	gcc -o InfixToPostfix InfixToPostfix.tab.c

clean:
	rm calc calc.tab.c calc.tab.h calc.output InfixToPostfix InfixToPostfix.tab.c InfixToPostfix.tab.h InfixToPostfix.output
