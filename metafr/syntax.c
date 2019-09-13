#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "defines.h"

#define MAXWORD		31

int lex(char word[MAXWORD]);
int symbol(char c);

int transition[9][20] = {
	{0,1,2,3,4,5,6,EOFTK, EQUALTK, MULTITK, PLUSTK,MINUSTK, LAGTK,RAGTK,LPARTK,RPARTK,LBRACKETTK,RBRACKETTK,QUESTTK,COMMATK},
	{S1,1,1,S1,S1,S1,S1,S1,S1,S1,S1,S1,S1,S1,S1,S1,S1,S1,S1,S1},
	{NUMTK,NUMTK,2,NUMTK,NUMTK,NUMTK,NUMTK,NUMTK,NUMTK,NUMTK,NUMTK,NUMTK,NUMTK,NUMTK,NUMTK,NUMTK,NUMTK,NUMTK,NUMTK,NUMTK},
	{LESSTK,LESSTK,LESSTK,LESSTK,NOTEQTK,LESSTK,LESSTK,LESSTK,LESSEQTK,LESSTK,LESSTK,LESSTK,LESSTK,LESSTK,LESSTK,LESSTK,LESSTK,LESSTK,LESSTK,LESSTK},
	{MORETK,MORETK,MORETK,MORETK,MORETK,MORETK,MORETK,MORETK,MOREEQTK,MORETK,MORETK,MORETK,MORETK,MORETK,MORETK,MORETK,MORETK,MORETK,MORETK,MORETK},
	{ER,ER,ER,ER,ER,ER,ER,ER,ASSIGNTK,ER,ER,ER,ER,ER,ER,ER,ER,ER,ER,ER},
	{DIVTK,DIVTK,DIVTK,DIVTK,DIVTK,DIVTK,DIVTK,DIVTK,DIVTK,7,DIVTK,DIVTK,DIVTK,DIVTK,DIVTK,DIVTK,DIVTK,DIVTK,DIVTK,DIVTK},
	{7,7,7,7,7,7,7,ER2,7,8,7,7,7,7,7,7,7,7,7,7},
	{7,7,7,7,7,7,0,ER2,7,8,7,7,7,7,7,7,7,7,7,7}
};

FILE *fp=NULL;
int code,seq;
char nextword[MAXWORD];
int line=1;


void program();
void block();
void declarations();
void varlist();
void subprograms();
void func();
void funcbody();
void sequence();
void formalpars();
void formalparlist();
void formalparitem();
void assignment_stat();
void IF_stat();
void WHILE_stat();
void DOWHILE_stat();
void EXIT_stat();
void INCASE_stat();
void FORECASE_stat();
void CALL_stat();
void RETURN_stat();
void PRINT_stat();
void expression();
void brack_or_stat();
void brackets_seq();
void else_part();
void brack_or_stat();
void term();
void add_oper();
void optional_sigh();
void factor();
void statement();
void condition();
void idtail();
void actualpars();
void actualparlist();
void actualparitem();
void boolterm();
void boolfactor();
void relational_oper();

int main(int argc, char *argv[])
{
	
	if(argc!=2){
		printf("Error executing\n");
		exit(1);
	}
	
	fp = fopen(argv[1],"r");
	if(fp==NULL){
		printf("Error opening file\n");
		exit(2);
	}
	
	code = lex(nextword); //h prwth lektiki monada
	program();
	printf("COMPILATION FINISHED SUCCESSFULLY!\n");
	return 0;
}

//diavazei thn epomenh lektiki monada
//epistrefei to kwdiko ths
//kai sto word apothikeuei thn lektikh monada ws string
int lex(char word[MAXWORD])
{
	int state;
	char c;
	int col;
	int i;
	int num;
	
	state=0;
	i=0;
	//oi katastasei pou prepei na synexisw arithmountai apo 0 ews 8
	//oi telikes einai panw apo to 100
	while(state<10){
		c = getc(fp);
		col = symbol(c);
		state = transition[state][col];
		if(c=='\n'){
			line++;
		}
		
		if(state==0 || state==7 || state==8){
			i=0;
		}
		else{
			word[i++] = c; //apothikeuw ton xaraktira pou diavasa
		}
		
		if(state==S1 || state == NUMTK || state== LESSTK || state==MORETK || state == DIVTK){
			ungetc(c,fp);
			if(c=='\n'){
				line--;
			}
			i--;
		}
		
	}
	word[i]  = '\0';
	
	
	if(state==S1){
		if(strcmp(word,"and")==0){
			return ANDTK;
		}
		if(strcmp(word,"exit")==0){
			return EXITTK;
		}if(strcmp(word,"if")==0){
			return IFTK;
		}
		if(strcmp(word,"program")==0){
			return PROGRAMTK;
		}
		if(strcmp(word,"when")==0){
			return WHENTK;
		}
		if(strcmp(word,"declare")==0){
			return DECLARETK;
		}
		if(strcmp(word,"procedure")==0){
			return PROCEDURETK;
		}
		if(strcmp(word,"in")==0){
			return INTK;
		}
		if(strcmp(word,"or")==0){
			return ORTK;
		}
		if(strcmp(word,"call")==0){
			return CALLTK;
		}
		if(strcmp(word,"do")==0){
			return DOTK;
		}
		if(strcmp(word,"function")==0){
			return FUNCTIONTK;
		}
		if(strcmp(word,"inout")==0){
			return INOUTTK;
		}
		if(strcmp(word,"return")==0){
			return RETURNTK;
		}
		if(strcmp(word,"else")==0){
			return ELSETK;
		}
		if(strcmp(word,"print")==0){
			return PRINTTK;
		}
		if(strcmp(word,"not")==0){
			return NOTTK;
		}
		if(strcmp(word,"while")==0){
			return WHILETK;
		}
		if(strcmp(word,"enddeclare")==0){
			return ENDDECLARETK;
		}
		if(strcmp(word,"incase")==0){
			return INCASETK;
		}
		if(strcmp(word,"forcase")==0){
			return FORCASETK;
		}
		if(strcmp(word,"copy")==0){
			return COPYTK;
		}
		return IDTK;
	}
	else if(state==NUMTK){
		num = atoi(word);
		if(num>32768){
			printf("Warning: Big integer at Line: %d\n",line);
		}
	}
	else if(state==ER){
		printf("Error found : and not = at Line: %d\n",line);
		exit(3);
	}
	else if(state==ER2){
		printf("Error found EOF inside comments at Line: %d\n",line);
		exit(4);
	}
	
	return state;
}

//epistrefei se poia sthlh antistoixei o xarakthras c
int symbol(char c)
{
	if(c==' ' || c=='\n' || c=='\t') return 0;
	if(isalpha(c)) return 1;
	if(isdigit(c)) return 2;
	if(c=='<') return 3;
	if(c=='>') return 4;
	if(c==':') return 5;
	if(c=='/') return 6;
	if(c==EOF) return 7;
	if(c=='=') return 8;
	if(c=='*') return 9;
	if(c=='+') return 10;
	if(c=='-') return 11;
	if(c=='{') return 12;
	if(c=='}') return 13;
	if(c=='(') return 14;
	if(c==')') return 15;
	if(c=='[') return 16;
	if(c==']') return 17;
	if(c==';') return 18;
	if(c==',') return 19;
	
	printf("Unknown character: '%c' %d\n",c,c);
	exit(1);
}


void errfunc(char token[MAXWORD])
{
	printf("Error expected: %s, found %s at Line: %d\n",token,nextword,line);
	exit(1);
}

void program()
{
	if(code==PROGRAMTK){
		code = lex(nextword);
		if(code == IDTK){
			code = lex(nextword);
			block();
		}
		else{
			errfunc("ID");
		}
	}
	else{
		errfunc("program");
	}
}

void block()
{
	if(code==LAGTK){
		code = lex(nextword);
		declarations();
		subprograms();
		sequence();
		if(code==RAGTK){
			code = lex(nextword);
		}
		else{
			errfunc("}");
		}
	}
	else{
		errfunc("{");
	}
}

void declarations()
{
	
	if(code==DECLARETK){
		code = lex(nextword);
		varlist();
		if(code==ENDDECLARETK){
			code = lex(nextword);
		}
		else{
			errfunc("enddeclare");
		}
	}
}

void varlist()
{
	if(code==IDTK){
		code = lex(nextword);
		while(code == COMMATK){
			code = lex(nextword);
			if(code == IDTK){
				code = lex(nextword);
			}
			else{
				errfunc("ID");
			}
		}
	}
}

void subprograms()
{
	while(code == FUNCTIONTK || code == PROCEDURETK){
		func();
	}
}

void func()
{
	if(code == FUNCTIONTK){
		code = lex(nextword);
		if(code == IDTK){
			code = lex(nextword);
			funcbody();
		}
	}
	else if(code==PROCEDURETK){
		code = lex(nextword);
		if(code == IDTK){
			code = lex(nextword);
			funcbody();
		}
	}
	else {
		errfunc("function or procedure");
	}
}

void funcbody()
{
	formalpars();
	block();
}

void formalpars()
{
	if(code==LPARTK){
		code = lex(nextword);
		formalparlist();
		if(code==RPARTK){
			code = lex(nextword);
		}
		else{
			errfunc(")");
		}
	}
	
}

void formalparlist()
{
	formalparitem();
	while(code == COMMATK){
		code = lex(nextword);
		formalparitem();
	}
}

void formalparitem()
{
	if(code==INTK){
		code = lex(nextword);
		if(code == IDTK){
			code = lex(nextword);
		}
		else{
			errfunc("missing ID on in");
		}
	}
	else if(code == INOUTTK){
		code = lex(nextword);
		if(code == IDTK){
			code = lex(nextword);
		}
		else{
			errfunc("missing ID on inout");
		}
	}
	else if(code == COPYTK){
		code = lex(nextword);
		if(code == IDTK){
			code = lex(nextword);
		}
		else{
			errfunc("missing ID on copy");
		}
	}
	
}

void sequence(){
	
	statement();
	while(code == QUESTTK){
		code = lex(nextword);
		statement();
	}
}

void statement()
{
	if(code == IDTK){
		assignment_stat();
	}
	else if(code == IFTK){
		IF_stat();
	}
	else if(code == WHILETK){
		WHILE_stat();
	}
	else if(code == DOTK){
		DOWHILE_stat();
	}
	else if(code == EXITTK){
		EXIT_stat();
	}
	else if(code == INCASETK){
		INCASE_stat();
	}
	else if(code == FORCASETK){
		FORECASE_stat();
	}
	else if(code == CALLTK){
		CALL_stat();
		
	}
	else if(code == RETURNTK){
		RETURN_stat();
	}
	else if(code == PRINTTK){
		PRINT_stat();
	}
}

void assignment_stat(){
	
	if(code == IDTK){
		code = lex(nextword);
		if(code == ASSIGNTK)
		{
			code = lex(nextword);
			expression();
			
		}
	}
}

void expression()
{
	optional_sigh();
	term();
	while(code == PLUSTK || code == MINUSTK){
		add_oper();
		term();
	}
}

void add_oper()
{
	if(code == PLUSTK || code == MINUSTK){
		code = lex(nextword);
	}
	else{
		errfunc("+ or -");
	}
}

void optional_sigh(){
	if(code==PLUSTK || code == MINUSTK){
		add_oper();
	}
	
}


void term()
{
	
	factor();
	while(code==MULTITK || code == DIVTK){
		code=lex(nextword);
		factor();
	}
}


void factor(){
	if(code == NUMTK){
		code = lex(nextword);
	}else if(code == LPARTK){
		code=lex(nextword);
		expression();
		if(code==RPARTK){
			code = lex(nextword);
		}
		else{
			errfunc(")");
		}
	}else if(code==IDTK){
		code=lex(nextword);
		idtail();
	}
}

void idtail()
{
	if(code==LPARTK){
		actualpars();
	}
}

void actualpars()
{
	if(code==LPARTK){
		code = lex(nextword);
		actualparlist();
	}
}

void actualparlist()
{
	actualparitem();
	while(code == COMMATK){
		code = lex(nextword);
		actualparitem();
	}
}

void actualparitem()
{
	if(code==INTK){
		code = lex(nextword);
		expression();
	}
	else if(code == INOUTTK){
		code = lex(nextword);
		if(code == IDTK){
			code = lex(nextword);
		}
		else{
			errfunc("missing ID on inout");
		}
	}
	else if(code == COPYTK){
		code = lex(nextword);
		if(code == IDTK){
			code = lex(nextword);
		}
		else{
			errfunc("missing ID on copy");
		}
	}
	
}

void IF_stat(){
	if(code == IFTK){
		code = lex(nextword);
		if(code == LPARTK){
			code = lex(nextword);
			condition();
			if(code == RPARTK){
				code = lex(nextword);
				brack_or_stat();
				else_part();
			}
			else{
				errfunc(")");
			}
		}
		else{
			errfunc("(");
		}
	}
}

void condition(){
	boolterm();
	while(code == ORTK){
		code=lex(nextword);
		boolterm();
	}
}

void boolterm(){
	boolfactor();
	while(code == ANDTK){
		code=lex(nextword);
		boolfactor();
	}
}

void boolfactor()
{
	if(code==NOTTK){
		code=lex(nextword);
		if(code==LBRACKETTK){
			code=lex(nextword);
			condition();
			if(code==RBRACKETTK){
				code=lex(nextword);
			}else{
				errfunc("]");
			}
		}else{
			errfunc("[");
		}
	}else if(code==LBRACKETTK){
		code=lex(nextword);
		condition();
		if(code==RBRACKETTK){
			code=lex(nextword);
		}else{
			errfunc("]");
		}
	}else{
		expression();
		relational_oper();
		expression();
	}
	
}

void relational_oper()
{
	if(code==EQUALTK){
		code=lex(nextword);
	}else if(code==MORETK){
		code=lex(nextword);
	}else if(code==LESSTK){
		code=lex(nextword);
	}else if(code==MOREEQTK){
		code=lex(nextword);
	}else if(code==LESSEQTK){
		code=lex(nextword);
	}else if(code==NOTEQTK){
		code=lex(nextword);
	}
	else{
		errfunc("rel_oper");
	}
}

void brack_or_stat()
{
	if(code==LAGTK){
		brackets_seq();
	}
	else{
		statement();
	}
}

void brackets_seq()
{
	if(code == LAGTK){
		code=lex(nextword);
		sequence();
		if(code==RAGTK){
			code = lex(nextword);
		}
		else{
			errfunc("}");
			
		}
	}
	else{
		errfunc("{");
	}
}

void else_part(){
	
	if(code == ELSETK){
		code = lex(nextword);
		brack_or_stat();
	}
}

void WHILE_stat()
{
	if(code==WHILETK){
		code = lex(nextword);
		if(code == LPARTK){
			code = lex(nextword);
			condition();
			if(code == RPARTK){
				code = lex(nextword);
				brack_or_stat();
			}
			else{
				errfunc(")");
			}
		}
		else{
			errfunc("(");
		}
	}
	else{
		errfunc("while");
	}
}

void DOWHILE_stat()
{
	if(code ==DOTK){
		code=lex(nextword);
		brack_or_stat();
		if(code==WHILETK){
			code = lex(nextword);
			if(code == LPARTK){
				code=lex(nextword);
				condition();
				if(code == RPARTK){
					code=lex(nextword);
				}
				else{
					errfunc(")");
				}
			}
		}else{
			errfunc("while");
		}
	}else{
		errfunc("do");
	}
}

void EXIT_stat()
{
	if(code==EXITTK){
		code=lex(nextword);
	}else{
		errfunc("exit");
	}
}

void INCASE_stat()
{
	
	
}

void FORECASE_stat()
{
	
}

void CALL_stat()
{
	if (code == CALLTK){
		code = lex(nextword);
		if(code == IDTK){
			code = lex(nextword);
			if(code == LPARTK){
				code=lex(nextword);
				actualparlist();
				if(code == RPARTK){
					code=lex(nextword);
				}else{
					errfunc(")");
				}
			}else{
				errfunc("(");
			}
		}else{
			errfunc("missing ID");
		}
	}
}

void RETURN_stat()
{
	
	code = lex(nextword);
	if(code == LPARTK){
		code = lex(nextword);
		expression();
		if(code == RPARTK){
			code=lex(nextword);
		}
		else{
			errfunc(")");
		}
	}else{
		errfunc("(");
	}
}

void PRINT_stat()
{
	code = lex(nextword);
	if(code == LPARTK){
		code = lex(nextword);
		expression();
		if(code == RPARTK){
			code=lex(nextword);
		}
		else{
			errfunc(")");
		}
	}else{
		errfunc("(");
	}
}