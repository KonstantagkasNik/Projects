
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "defines.h"

#define MAXWORD 	31

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

///////////////////////////////////
//           ENDIAMESOS          //
///////////////////////////////////
typedef struct quad{
	int label;
	char ops[4][MAXWORD]; //ta 4 stoixeia tis tetradas
	struct quad *next;	///deikths sthn epomenh tetrada
}myquad_t;


//deikths sthn arxi kai to telos tis listas
myquad_t *QHEAD=NULL;
myquad_t *QTAIL=NULL;

typedef struct list{
	int number;
	struct list *next;
}list_t;

int quadnumber=1;


void genquad(char *op1, char *op2, char *op3, char *op4);
int nextquad();
void quadprint();
void newtemp(char w[MAXWORD]);
list_t *makeemptylist();
list_t *makelist(int x);
list_t *merge(list_t *l1, list_t *l2);
void backpatch(list_t *t, int z);



void program();
void block(char  *);
void declarations();
void varlist();
void subprograms();
void func();
void funcbody(char *);
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

//epistrefoun apo ena place to kathena
void expression(char place[MAXWORD]);
void term(char place[MAXWORD]);
void factor(char place[MAXWORD]);

void brack_or_stat();
void brackets_seq();
void else_part();
void brack_or_stat();
void add_oper();
void optional_sigh();
void statement();
void condition(list_t **ltrue, list_t **lfalse);
void idtail();
void actualpars();
void actualparlist();
void actualparitem();
void boolterm(list_t **ltrue, list_t **lfalse);
void boolfactor(list_t **ltrue, list_t **lfalse);
void relational_oper(char oper[MAXWORD]);

int main(int argc, char *argv[])
{
	char *files[5] = {"test1.st","test2.st","test3.st","test4.st","test5.st"};
	char *outfiles[5] =  {"1309_am2_test1.int","1309_am2_test2.int","1309_am2_test3.int","1309_am2_test4.int","1309_am2_test5.int"};
	FILE *fout;
	int i;
	
	for(i=0; i<5; i++){
		quadnumber=1;
		fp = fopen(files[i],"r");
		if(fp==NULL){
			printf("Error opening file\n");
			exit(2);
		}
		
		code = lex(nextword); //h prwth lektiki monada
		program();
		//printf("COMPILATION FINISHED SUCCESSFULLY!\n");
		fout = fopen(outfiles[i], "w");
		quadprint(fout);
		fclose(fout);
	}
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
	char id[MAXWORD];
	
	if(code==PROGRAMTK){
		code = lex(nextword);
		if(code == IDTK){
			strcpy(id,nextword);
			code = lex(nextword);
			block(id);
		}
		else{
			errfunc("ID");
		}
	}
	else{
		errfunc("program");
	}
}

void block(char *id)
{
	if(code==LAGTK){
		code = lex(nextword);
		declarations();
		subprograms();
		genquad("begin_block","_","_",id);
		sequence();
		genquad("end_block","_","_",id);
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
	char id[MAXWORD];
	if(code == FUNCTIONTK){
		code = lex(nextword);
		if(code == IDTK){
			strcpy(id,nextword);
			code = lex(nextword);
			funcbody(id);
		}
	}
	else if(code==PROCEDURETK){
		code = lex(nextword);
		if(code == IDTK){
			strcpy(id,nextword);
			code = lex(nextword);
			funcbody(id);
		}
	}
	else {
		errfunc("function or procedure");
	}
}

void funcbody(char *id)
{
	formalpars();
	block(id);
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
	char place[MAXWORD];
	char id[MAXWORD];
	
	if(code == IDTK){
		strcpy(id,nextword);
		code = lex(nextword);
		if(code == ASSIGNTK)
		{
			code = lex(nextword);
			expression(place);
			genquad(":=",place, "_",id);
		}
		else{
			errfunc("missing := ");
		}
	}
}

void expression(char place[MAXWORD])
{
	char t1[MAXWORD],t2[MAXWORD];
	char w[MAXWORD];
	char oper[MAXWORD];
	
	optional_sigh();
	term(t1);
	while(code == PLUSTK || code == MINUSTK){
		add_oper(oper);
		term(t2);
		newtemp(w);
		genquad(oper,t1,t2,w);
		strcpy(t1,w);
	}
	//synoliko place
	strcpy(place,t1);
}

void term(char place[MAXWORD])
{
	char f1[MAXWORD],f2[MAXWORD];
	char w[MAXWORD];
	char oper[MAXWORD];
	
	factor(f1);
	while(code==MULTITK || code == DIVTK){
		strcpy(oper,nextword);
		code=lex(nextword);
		factor(f2);
		newtemp(w);
		genquad(oper,f1,f2,w);
		strcpy(f1,w);
	}
	strcpy(place,f1);
}


void factor(char place[MAXWORD]){
	char w[MAXWORD];
	if(code == NUMTK){
		strcpy(place,nextword);
		code = lex(nextword);
	}else if(code == LPARTK){
		code=lex(nextword);
		expression(place);
		if(code==RPARTK){
			code = lex(nextword);
		}
		else{
			errfunc(")");
		}
	}else if(code==IDTK){
		strcpy(place,nextword);
		code=lex(nextword);
		if(code==LPARTK){
			idtail();
			newtemp(w);
			genquad("par",w,"RET","_");
			genquad("call",place,"_","_");
			strcpy(place,w);
		}
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
		if(code==RPARTK){
			code = lex(nextword);
		}
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
	char place[MAXWORD];
	
	if(code==INTK){
		code = lex(nextword);
		expression(place);
		genquad("par",place,"CV","_");
	}
	else if(code == INOUTTK){
		code = lex(nextword);
		if(code == IDTK){
			genquad("par",nextword,"REF","_");
			code = lex(nextword);
		}
		else{
			errfunc("missing ID on inout");
		}
	}
	else if(code == COPYTK){
		code = lex(nextword);
		if(code == IDTK){
			genquad("par",nextword,"CP","_");
			code = lex(nextword);
		}
		else{
			errfunc("missing ID on copy");
		}
	}
	
}

void IF_stat(){
	list_t *ltrue;
	list_t *lfalse;
	list_t *l1;
	
	if(code == IFTK){
		code = lex(nextword);
		if(code == LPARTK){
			code = lex(nextword);
			condition(&ltrue,&lfalse);
			backpatch(ltrue,nextquad());
			if(code == RPARTK){
				code = lex(nextword);
				brack_or_stat();
				l1 = makelist(nextquad());
				genquad("jump","_","_","_");
				backpatch(lfalse,nextquad());
				else_part();
				backpatch(l1,nextquad());
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

void condition( list_t **ltrue, list_t **lfalse){
	list_t *btrue1;
	list_t *btrue2;
	list_t *bfalse1;
	list_t *bfalse2;
	int q;
	
	boolterm(&btrue1,&bfalse1);
	*ltrue = btrue1;
	*lfalse = bfalse1;
	while(code == ORTK){
		code=lex(nextword);
		q= nextquad(); 
		boolterm(&btrue2,&bfalse2);
		
		backpatch(*lfalse,q);
		*ltrue = merge(*ltrue, btrue2);
		*lfalse = bfalse2;
	}
}

void boolterm(list_t **ltrue, list_t **lfalse){
	list_t *btrue1;
	list_t *btrue2;
	list_t *bfalse1;
	list_t *bfalse2;
	int q;
	
	boolfactor(&btrue1,&bfalse1);
	*ltrue = btrue1;
	*lfalse = bfalse1;
	while(code == ANDTK){
		code=lex(nextword);
		q= nextquad(); 
		boolfactor(&btrue2,&bfalse2);
		
		backpatch(*ltrue,q);
		*lfalse = merge(*lfalse,bfalse2);
		*ltrue = btrue2;
	}
}

void boolfactor(list_t **ltrue, list_t **lfalse)
{
	
	char place1[MAXWORD];
	char place2[MAXWORD];
	char oper[MAXWORD];
	
	if(code==NOTTK){
		code=lex(nextword);
		if(code==LBRACKETTK){
			code=lex(nextword);
			condition(lfalse, ltrue);
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
		condition(ltrue,lfalse);
		if(code==RBRACKETTK){
			code=lex(nextword);
		}else{
			errfunc("]");
		}
	}else{
		expression(place1);
		relational_oper(oper);
		expression(place2);
		*ltrue = makelist(nextquad());
		genquad(oper,place1,place2,"_");
		*lfalse = makelist(nextquad());
		genquad("jump","_","_","_");
	}
	
}

void relational_oper(char oper[MAXWORD])
{
	strcpy(oper,nextword);
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
	list_t *ltrue;
	list_t *lfalse;
	int q;
	char tmp[10];
	
	if(code==WHILETK){
		code = lex(nextword);
		if(code == LPARTK){
			code = lex(nextword);
			q = nextquad();
			sprintf(tmp,"%d",q);
			condition(&ltrue,&lfalse);
			backpatch(ltrue,nextquad());
			if(code == RPARTK){
				code = lex(nextword);
				brack_or_stat();
				genquad("jump","_","_",tmp);
				backpatch(lfalse,nextquad());
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
	list_t *ltrue;
	list_t *lfalse;
	int q;
	
	if(code ==DOTK){
		code=lex(nextword);
		q = nextquad();
		brack_or_stat();
		if(code==WHILETK){
			code = lex(nextword);
			if(code == LPARTK){
				code=lex(nextword);
				condition(&ltrue, &lfalse);
				backpatch(lfalse,nextquad());
				backpatch(ltrue,q);
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
	char id[MAXWORD];
	if (code == CALLTK){
		code = lex(nextword);
		if(code == IDTK){
			strcpy(id,nextword);
			code = lex(nextword);
			if(code == LPARTK){
				code=lex(nextword);
				actualparlist();
				genquad("call",id,"_","_");
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
	char place[MAXWORD];

	code = lex(nextword);
	if(code == LPARTK){
		code = lex(nextword);
		expression(place);
		genquad("ret","_","_",place);
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
	char place[MAXWORD];
	
	code = lex(nextword);
	if(code == LPARTK){
		code = lex(nextword);
		expression(place);
		genquad("out","_","_",place);
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



void add_oper(char oper[MAXWORD])
{
	if(code == PLUSTK || code == MINUSTK){
		strcpy(oper,nextword);
		code = lex(nextword);
	}
	else{
		errfunc("+ or -");
	}
}

void optional_sigh(){
	char oper[MAXWORD];
	
	if(code==PLUSTK || code == MINUSTK){
		add_oper(oper);
	}
	
}





///////////////////////////////////
//           ENDIAMESOS          //
///////////////////////////////////
void genquad(char *op1, char *op2, char *op3, char *op4)
{
	myquad_t *qt;
	
	qt = (myquad_t  *)malloc(sizeof(myquad_t ));
	qt->next = NULL;	//tha mpeiteleutaia
	if(QHEAD==NULL){
		QHEAD = qt;
	}
	else{
		//to teleutaio deixnei sto neo
		QTAIL->next = qt;
	}
	//teleutaio to neo
	QTAIL=qt;
	
	
	//gemisma pediwn
	strcpy(qt->ops[0],op1);
	strcpy(qt->ops[1],op2);
	strcpy(qt->ops[2],op3);
	strcpy(qt->ops[3],op4);
	qt->label = nextquad();
	quadnumber++;
	
}

int nextquad()
{
	return quadnumber;
}



void quadprint(FILE *fout)
{
	int i;
	myquad_t  *q;
	
	q = QHEAD;
	while(q!=NULL){
		fprintf(fout,"%d: ",q->label);
		for(i=0; i<4; i++){
			fprintf(fout," %s ",q->ops[i]);
		}
		fprintf(fout,"\n");
		q = q->next;
	}
	
}

//apothikeuei sto w to onoma ths neas proswrinhs metavlitis
void newtemp(char w[MAXWORD])
{
	static int count=1;
	
	sprintf(w,"T_%d",count);
	
	count++;
	
}

//kenh lista
list_t *makeemptylist()
{
	return NULL;
}

//ftiaxnei ena komvo me timi to x
list_t *makelist(int x)
{
	list_t *l = (list_t *)malloc(sizeof(list_t));
	
	l->next = NULL;
	l->number = x;
	return l;
}

//sygxnwneuei thn l1 kai thn l2 kai epistrefei to apotelesma
list_t *merge(list_t *l1, list_t *l2)
{
	list_t *lt;
	if(l1==NULL){
		return l2;
	}
	
	lt = l1; //h arxh ths prwths
	//prepei na ftasw sto telos ths 1h kai na valw na deixnei sthn arxh ths allhs (l2)
	while(lt!=NULL){
		//o teleutaios komvos
		if(lt->next==NULL){
			lt->next = l2;
			return l1;
		}
		lt = lt->next;
	}
	
	return NULL;
}

void backpatch(list_t *t, int z)
{
	myquad_t  *q;
	char buf[MAXWORD];
	
	sprintf(buf,"%d",z);
	
	while(t!=NULL){
		//psa3e gia thn tetrada me arithmo t->number
		q = QHEAD;
		while(q!=NULL){
			if(q->label==t->number){
				break;
			}
			q=q->next;
		}
		if(q==NULL){
			printf("Backpatch error: quad %d not found\n",t->number);
			exit(1);
		}
		else{
			strcpy(q->ops[3],buf);
		}
		t = t->next;
	}
}