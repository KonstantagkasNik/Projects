//Nikos Konstantagkas 1874
//Filipos Ntaskas 1309

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

typedef struct quad{
	int label;
	char ops[4][MAXWORD]; 
	struct quad *next;	
}myquad_t;

myquad_t *QHEAD=NULL;
myquad_t *QTAIL=NULL;

typedef struct list{
	int number;
	struct list *next;
}list_t;

int quadnumber=1; //mou deixnei ton arithmo ths epomenhs tetradas pou tha paraxthei
void genquad(char *op1, char *op2, char *op3, char *op4);
int nextquad();
void quadprint();
void newtemp(char w[MAXWORD]);
list_t *makeemptylist();
list_t *makelist(int x);
list_t *merge(list_t *l1, list_t *l2);
void backpatch(list_t *t, int z);

#define VARIABLE 100
#define FUNCTION 200
#define PARAMETER 300
#define CV	301
#define REF	302
#define COP	303

typedef struct argument{
	int type;
	struct argument *next;
}argument_t;

typedef struct entity{
	char name[MAXWORD];
	int offset;
	int type; 
	int param;
	int depth;
	argument_t *firstarg;
	argument_t *lastarg;
	struct entity *next;
}entity_t;

typedef struct scope{
	char name[MAXWORD];
	int depth;
	int frame;
	entity_t *firstentity;
	entity_t *lastentity;
	struct scope *parent;
}scope_t;

scope_t *TOPSCOPE=NULL;
scope_t *BOTTOMSCOPE=NULL;
void remove_scope();
void print_symbol(FILE *fp);
void insert_argument(entity_t *ent, int type);
entity_t *insert_entity(char name[MAXWORD], int type);
void insert_scope(char name[MAXWORD]);
entity_t *search_entity(char v[MAXWORD]);

FILE *fptable;
FILE *fpfinal;
FILE *fpicode;

char prog[MAXWORD];

void isdeclared(char x[MAXWORD])
{
	if( search_entity(x)==NULL){
		printf("Undeclared variable: %s at Line: %d\n",nextword,line);
		exit(1);
	}
}

void gnlvcode(char v[MAXWORD]);
void loadvr(char v[MAXWORD], int r);
void storerv(int r, char v[MAXWORD]);
void converttofinal();
void program();
void block(char  *);
void declarations();
void varlist();
void subprograms();
void func();
void funcbody(char *);
void sequence();
void formalpars(entity_t *ent);
void formalparlist(entity_t *ent);
void formalparitem(entity_t *ent);
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
	char *outfiles[5] =  {"1309_1874_test1.int","1309_1874_test2.int","1309_1874_test3.int","1309_1874_test4.int","1309_1874_test5.int"};
	char *tablefiles[5] =  {"1309_1874_test1.symbol","1309_1874_test2.symbol","1309_1874_test3.symbol","1309_1874_test4.symbol","1309_1874_test5.symbol"};
	char *finalfiles[5] =  {"1309_1874_test1.final","1309_1874_test2.final","1309_1874_test3.final","1309_1874_test4.final","1309_1874_test5.final"};
	int i;
	
	for(i=0; i<5; i++){
		quadnumber=1;
		QHEAD=QTAIL=NULL;
		fp = fopen(files[i],"r");
		if(fp==NULL){
			printf("Error opening file\n");
			exit(2);
		}
		
		fptable = fopen(tablefiles[i],"w");
		if(fptable==NULL){
			printf("Error opening file\n");
			exit(3);
		}
		
		fpfinal = fopen(finalfiles[i],"w");
		if(fpfinal==NULL){
			printf("Error opening file\n");
			exit(3);
		}
		fpicode = fopen(outfiles[i], "w");
		if(fpicode==NULL){
			printf("Error opening file\n");
			exit(3);
		}
		code = lex(nextword);
		fprintf(fpfinal,"movi R[0], 600\n");
		fprintf(fpfinal,"jmp L0\n");
		program();
		fclose(fptable);
		fclose(fpfinal);
		fclose(fpicode);
	}
	return 0;
}

int lex(char word[MAXWORD])
{
	int state;
	char c;
	int col;
	int i;
	int num;
	
	state=0;
	i=0; //posous xaraktires exei diavasei
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
			word[i++] = c; 
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
			strcpy(prog,nextword); 
			strcpy(id,nextword);
			code = lex(nextword);
			insert_scope(id);
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
		print_symbol(fptable);
		converttofinal();
		remove_scope();
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
		insert_entity(nextword,VARIABLE);
		code = lex(nextword);
		while(code == COMMATK){
			code = lex(nextword);
			if(code == IDTK){
				insert_entity(nextword,VARIABLE);
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
	entity_t *ent;
	
	ent = insert_entity(id,FUNCTION);
	insert_scope(id);
	formalpars(ent);
	block(id);
}

void formalpars(entity_t *ent)
{
	if(code==LPARTK){
		code = lex(nextword);
		formalparlist(ent);
		if(code==RPARTK){
			code = lex(nextword);
		}
		else{
			errfunc(")");
		}
	}
	
}

void formalparlist(entity_t *ent)
{
	formalparitem(ent);
	while(code == COMMATK){
		code = lex(nextword);
		formalparitem(ent);
	}
}

void formalparitem(entity_t *ent)
{
	if(code==INTK){
		code = lex(nextword);
		if(code == IDTK){
			insert_entity(nextword,PARAMETER);
			insert_argument(ent,CV);
			code = lex(nextword);
		}
		else{
			errfunc("missing ID on in");
		}
	}
	else if(code == INOUTTK){
		code = lex(nextword);
		if(code == IDTK){
			insert_entity(nextword,PARAMETER);
			insert_argument(ent,REF);
			code = lex(nextword);
		}
		else{
			errfunc("missing ID on inout");
		}
	}
	else if(code == COPYTK){
		code = lex(nextword);
		if(code == IDTK){
			insert_entity(nextword,PARAMETER);
			insert_argument(ent,COP);
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
		isdeclared(nextword);
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
		isdeclared(nextword);
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

//h condition epistrefei 2 listes: 
//1h lista: ltrue: tetrades pou 3eroume pws h synthiki einai true
//diplos deikths giati prepei na ton tropopoihsw
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

void genquad(char *op1, char *op2, char *op3, char *op4)
{
	myquad_t *qt;
	
	qt = (myquad_t  *)malloc(sizeof(myquad_t ));
	qt->next = NULL;	
	if(QHEAD==NULL){
		QHEAD = qt;
	}
	else{
		QTAIL->next = qt;
	}
	QTAIL=qt;
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

void newtemp(char w[MAXWORD])
{
	//thn prwth fora pou tha trexei pairnei thn arxikopoihsh
	// stis epomenes tha synexisei apo ekei poy emeine
	static int count=1;
	
	sprintf(w,"T_%d",count);
	insert_entity(w,VARIABLE);
	count++;
	
}

list_t *makeemptylist()
{
	return NULL;
}

list_t *makelist(int x) //o arithmos(label) ths tetradas
{
	list_t *l = (list_t *)malloc(sizeof(list_t));
	
	l->next = NULL;
	l->number = x;
	return l;
}

list_t *merge(list_t *l1, list_t *l2) //xrhsimopoieitai stis condition,boolterm, boolfactor
{
	list_t *lt;
	if(l1==NULL){
		return l2;
	}
	
	lt = l1; 
	while(lt!=NULL){
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


void insert_scope(char name[MAXWORD])
{
	scope_t *newscope;
	
	newscope = (scope_t *)malloc(sizeof(scope_t));
	if(newscope ==NULL){
		printf("Error malloc\n");
		exit(1);
	}
	newscope->firstentity = NULL;
	newscope->lastentity = NULL;
	newscope->frame = 12;
	
	strcpy(newscope->name,name);
	if(BOTTOMSCOPE!=NULL){
		newscope->depth = BOTTOMSCOPE->depth+1;
		newscope->parent = BOTTOMSCOPE;
		BOTTOMSCOPE = newscope;
	}
	else{
		newscope->parent = NULL;
		BOTTOMSCOPE = newscope;
		TOPSCOPE = newscope;
		newscope->depth = 0;
	}
	
	
}

void remove_scope()
{
	BOTTOMSCOPE = BOTTOMSCOPE->parent;
}

entity_t *insert_entity(char name[MAXWORD], int type)
{
	entity_t *newentity;
	
	newentity = (entity_t *)malloc(sizeof(entity_t));
	
	strcpy(newentity->name,name);
	newentity->type = type;
	newentity->next = NULL;
	newentity->firstarg = NULL;
	newentity->lastarg = NULL;
	newentity->depth = BOTTOMSCOPE->depth; 
	if(type!=FUNCTION){
		newentity->offset = BOTTOMSCOPE->frame;
		BOTTOMSCOPE->frame += 4;
	}
	if(BOTTOMSCOPE->firstentity==NULL){ 
		BOTTOMSCOPE->firstentity = newentity;
		BOTTOMSCOPE->lastentity =newentity;
	}
	else{ 
		BOTTOMSCOPE->lastentity->next = newentity;
		BOTTOMSCOPE->lastentity = newentity;
	}
	
	return  newentity;
}


void insert_argument(entity_t *ent, int type)
{
	argument_t *arg;
	
	arg = (argument_t*)malloc(sizeof(argument_t));
	arg->type = type;
	arg->next = NULL;
	
	if(ent->firstarg==NULL){
		ent->firstarg = arg;
		ent->lastarg = arg;
	}
	else{
		ent->lastarg->next = arg;
		ent->lastarg = arg;
	}
}

void print_symbol(FILE *fp)
{
	scope_t *tmp;
	entity_t *ent;
	argument_t *arg;
	
	
	fprintf(fp,"===========================================================\n");
	tmp = BOTTOMSCOPE;
	while(tmp!=NULL){
		fprintf(fp,"%s(DEPTH: %d) (FRAMELENGTH: %d) \n",tmp->name,tmp->depth,tmp->frame);
		ent = tmp->firstentity;
		while(ent!=NULL){
			if(ent->type!=FUNCTION){
				fprintf(fp,"\t%s (%d)\n",ent->name,ent->offset);
			}
			else{
				fprintf(fp,"\t%s ",ent->name);
				arg = ent->firstarg;
				while(arg!=NULL){
					if(arg->type==CV){
						fprintf(fp," (CV) ");
					}
					else if(arg->type==REF){
						fprintf(fp," (REF) ");
					}
					else if(arg->type==COP){
						fprintf(fp," (COP) ");
					}
					arg = arg->next;
				}
				fprintf(fp,"\n");
			}
			
			ent = ent->next;
		}
		tmp = tmp->parent;
	}
	
	fprintf(fp,"===========================================================\n");
}


entity_t *search_entity(char v[MAXWORD])
{
	scope_t *tmp;
	entity_t *ent;
	
	tmp = BOTTOMSCOPE;
	while(tmp!=NULL){
		ent = tmp->firstentity;
		while(ent!=NULL){
			if(strcmp(ent->name,v)==0){
				return ent;
			}
			ent = ent->next;
		}
		tmp = tmp->parent;
	}
	return NULL;
}


void gnlvcode(char v[MAXWORD])
{
	entity_t *ent = search_entity(v);
	int epipeda;
	int i;
	
	fprintf(fpfinal,"movi R[255], M[R[0]+4]\n"); 
	epipeda = BOTTOMSCOPE->depth - ent->depth -1; //to -1 giati hdh anevika ena epipedo 
	for(i=0; i<epipeda; i++){
		fprintf(fpfinal,"movi R[255], M[R[255]+4]\n");	
	}
	fprintf(fpfinal,"movi R[254], %d\n",ent->offset);
	fprintf(fpfinal,"movi R[255], R[254], R[255]\n");
}

void loadvr(char v[MAXWORD], int r)
{
	entity_t *ent;
	
	ent = search_entity(v);
	
	if(isdigit(v[0])){
		fprintf(fpfinal,"movi R[%d], %s\n",r,v);
		return;
	}
	
	if(ent->depth==0){
		fprintf(fpfinal,"movi R[%d], M[%d+600]\n",r,ent->offset);
	}
	else if(ent->depth==BOTTOMSCOPE->depth){
		if(ent->type==VARIABLE){
			fprintf(fpfinal,"movi R[%d], M[%d+R[0]]\n",r,ent->offset);
		}
		else if(ent->type==PARAMETER && ent->param==CV){
			fprintf(fpfinal,"movi R[%d], M[%d+R[0]]\n",r,ent->offset);
		}
		else if(ent->type==PARAMETER && ent->param==REF){
			fprintf(fpfinal,"movi R[255], M[%d+R[0]]\n",ent->offset);
			fprintf(fpfinal,"movi R[%d], M[R[255]]\n",r);
		}
		else if(ent->type==PARAMETER && ent->param==COP){
			printf("Warning: COP not implemented\n");
		}
	}
	else if(ent->depth<BOTTOMSCOPE->depth){
		if(ent->type==VARIABLE){
			gnlvcode(v);
			fprintf(fpfinal,"movi R[%d], M[R[255]]\n",r);
		}
		else if(ent->type==PARAMETER && ent->param==CV){
			gnlvcode(v);
			fprintf(fpfinal,"movi R[%d], M[R[255]]\n",r);
		}
		else if(ent->type==PARAMETER && ent->param==REF){
			gnlvcode(v);
			fprintf(fpfinal,"movi R[255], M[R[255]]\n");
			fprintf(fpfinal,"movi R[%d], M[R[255]]\n",r);
		}
		else if(ent->type==PARAMETER && ent->param==COP){
			printf("Warning: COP not implemented\n");
		}
	}
}

void storerv(int r, char v[MAXWORD])
{
	entity_t *ent;
	
	ent = search_entity(v);
	
	if(ent->depth==0){
		fprintf(fpfinal,"movi M[%d+600],R[%d]\n",r,ent->offset);
	}
	else if(ent->depth==BOTTOMSCOPE->depth){
		if(ent->type==VARIABLE){
			fprintf(fpfinal,"movi M[%d+R[0]], R[%d]\n",r,ent->offset);
		}
		else if(ent->type==PARAMETER && ent->param==CV){
			fprintf(fpfinal,"movi M[%d+R[0]], R[%d]\n",r,ent->offset);
		}
		else if(ent->type==PARAMETER && ent->param==REF){
			fprintf(fpfinal,"movi R[255], M[%d+R[0]]\n",ent->offset);
			fprintf(fpfinal,"movi M[R[255]],R[%d]\n",r);
		}
		else if(ent->type==PARAMETER && ent->param==COP){
			printf("Warning: COP not implemented\n");
		}
	}
	else if(ent->depth<BOTTOMSCOPE->depth){
		if(ent->type==VARIABLE){
			gnlvcode(v);
			fprintf(fpfinal,"movi M[R[255]],R[%d]\n",r);
		}
		else if(ent->type==PARAMETER && ent->param==CV){
			gnlvcode(v);
			fprintf(fpfinal,"movi M[R[255]], R[%d]\n",r);
		}
		else if(ent->type==PARAMETER && ent->param==REF){
			gnlvcode(v);
			fprintf(fpfinal,"movi R[255], M[R[255]]\n");
			fprintf(fpfinal,"movi M[R[255]], R[%d]\n",r);
		}
		else if(ent->type==PARAMETER && ent->param==COP){
			printf("Warning: COP not implemented\n");
		}
	}
}

void converttofinal()
{
	quadprint(fpicode);
	myquad_t *qtmp;
	
	qtmp = QHEAD;
	while(qtmp!=NULL){
		fprintf(fpfinal,"L%d: ",qtmp->label);
		if(strcmp(qtmp->ops[0],"begin_block")==0){
			fprintf(fpfinal,"\n");
			if(strcmp(qtmp->ops[3],prog)==0){
				fprintf(fpfinal,"L0:\n");
			}
		}
		else if(strcmp(qtmp->ops[0],"jump")==0){
			fprintf(fpfinal,"jmp L%s\n",qtmp->ops[3]);
		}
		else if(strcmp(qtmp->ops[0],"+")==0){
			loadvr(qtmp->ops[1],1);
			loadvr(qtmp->ops[2],2);
			fprintf(fpfinal,"addi R[3],R[1],R[2]\n");
			storerv(3, qtmp->ops[3]);
		}
		else if(strcmp(qtmp->ops[0],"-")==0){
			loadvr(qtmp->ops[1],1);
			loadvr(qtmp->ops[2],2);
			fprintf(fpfinal,"subi R[3],R[1],R[2]\n");
			storerv(3, qtmp->ops[3]);
		}
		else if(strcmp(qtmp->ops[0],"*")==0){
			loadvr(qtmp->ops[1],1);
			loadvr(qtmp->ops[2],2);
			fprintf(fpfinal,"muli R[3],R[1],R[2]\n");
			storerv(3, qtmp->ops[3]);
		}
		else if(strcmp(qtmp->ops[0],"/")==0){
			loadvr(qtmp->ops[1],1);
			loadvr(qtmp->ops[2],2);
			fprintf(fpfinal,"divi R[3],R[1],R[2]\n");
			storerv(3, qtmp->ops[3]);
		}
		else if(strcmp(qtmp->ops[0],"<")==0){
			loadvr(qtmp->ops[1],1);
			loadvr(qtmp->ops[2],2);
			fprintf(fpfinal,"cmpi R[1],R[2]\n");
			fprintf(fpfinal,"ja L%s\n",qtmp->ops[3]);
		}
		else if(strcmp(qtmp->ops[0],">")==0){
			loadvr(qtmp->ops[1],1);
			loadvr(qtmp->ops[2],2);
			fprintf(fpfinal,"cmpi R[1],R[2]\n");
			fprintf(fpfinal,"jb L%s\n",qtmp->ops[3]);
		}
		else if(strcmp(qtmp->ops[0],"<=")==0){
			loadvr(qtmp->ops[1],1);
			loadvr(qtmp->ops[2],2);
			fprintf(fpfinal,"cmpi R[1],R[2]\n");
			fprintf(fpfinal,"jae L%s\n",qtmp->ops[3]);
		}
		else if(strcmp(qtmp->ops[0],">=")==0){
			loadvr(qtmp->ops[1],1);
			loadvr(qtmp->ops[2],2);
			fprintf(fpfinal,"cmpi R[1],R[2]\n");
			fprintf(fpfinal,"jbe L%s\n",qtmp->ops[3]);
		}
		else if(strcmp(qtmp->ops[0],"=")==0){
			loadvr(qtmp->ops[1],1);
			loadvr(qtmp->ops[2],2);
			fprintf(fpfinal,"cmpi R[1],R[2]\n");
			fprintf(fpfinal,"je L%s\n",qtmp->ops[3]);
		}
		else if(strcmp(qtmp->ops[0],"<>")==0){
			loadvr(qtmp->ops[1],1);
			loadvr(qtmp->ops[2],2);
			fprintf(fpfinal,"cmpi R[1],R[2]\n");
			fprintf(fpfinal,"jne L%s\n",qtmp->ops[3]);
		}
		else{
			fprintf(fpfinal,"\n");
		}
		qtmp =qtmp->next;
	}
	QHEAD=QTAIL=NULL;
}
