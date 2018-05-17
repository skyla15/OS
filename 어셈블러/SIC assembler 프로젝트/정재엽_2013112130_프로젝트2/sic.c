#include <stdio.h>
#include <stdlib.h>
#include <string.h> //문자열처리
#include <malloc.h> //동적할당

/***************************** DECLERATE VARIABLE ****************************/
typedef struct OperationCodeTable
{
	char Mnemonic[8];
	char Format;
	unsigned short int  ManchineCode;
}SIC_OPTAB;

typedef struct SymbolTable
{
	char Label[10];
	int Address;
}SIC_SYMTAB;

typedef struct IntermediateRecord { //중간파일구
	unsigned short int LineIndex; // 소스코드의 행을 표시
	unsigned short int Loc; // 해당 명령어의 메모리상 위치
	unsigned long int ObjectCode; // pass2 이후에 생성되는 목적코드
	char LabelField[32]; // 소스코드에 있는 라벨
	char OperatorField[32]; // Mnemonic (LDA, STA등의 표시) 
	char OperandField[32]; // 소스코드의 피연산자 
}IntermediateRec;


int Counter;
int LOCCTR[100]; //중간파일의 최대 길이
int LocctrCounter = 0; //LOCCTR의 index변수
int ProgramLen;
int Index;
int j;
int ManchineCode;
int SymtabCounter = 0; //symtab 개수와 포인트를 위함
int start_address; // 시작주소 저장
int program_length; // 프로그램의 길이 저장 = lastaddress - startddress
int ArrayIndex = 0; // 중간파일을 각각 가르키기 위한 index변수

unsigned short int FoundOnSymtab_flag = 0; // symtab에 없을 경우 0반환
unsigned short int FoundOnOptab_flag = 0; //optab에 없을 경우 0반환

char Buffer[256]; //버퍼사이즈 256
char Label[32]; //라벨의 최대 길이 32
char Mnemonic[32]; //명령어의 최대 길이 32
char Operand[32]; //피연산자의 최대길이 32

SIC_SYMTAB SYMTAB[20];
// 심볼 테이블 구조체 배열
IntermediateRec* IMRArray[100]; // 중간파일 변수 


static SIC_OPTAB OPTAB[] =
{
	{ "ADD",  '3',  0x18 },
	{ "AND",  '3',  0x40 },
	{ "COMP",  '3',  0x28 },
	{ "DIV",  '3',  0x24 },
	{ "J",  '3',  0x3C },
	{ "JEQ",  '3',  0x30 },
	{ "JGT",  '3',  0x34 },
	{ "JLT",  '3',  0x38 },
	{ "JSUB",  '3',  0x48 },
	{ "LDA",  '3',  0x00 },
	{ "LDCH",  '3',  0x50 },
	{ "LDL",  '3',  0x08 },
	{ "LDX",  '3',  0x04 },
	{ "MUL",  '3',  0x20 },
	{ "OR",  '3',  0x44 },
	{ "RD",  '3',  0xD8 },
	{ "RSUB",  '3',  0x4C },
	{ "STA",   '3',  0x0C },
	{ "STCH",  '3',  0x54 },
	{ "STL",   '3',  0x14 },
	{ "STSW",  '3',  0xE8 },
	{ "STX",  '3',  0x10 },
	{ "SUB",  '3',  0x1C },
	{ "TD",   '3',  0xE0 },
	{ "TIX",  '3',  0x2C },
	{ "WD",   '3',  0xDC },
	/* 3형식 LDB, LDS, LDT, LDX, STB, STS, STT, LPS 추가 */
	{ "LDX",  '3',  0x04 },
};
/* FORMAT 2 : ADDR, CLEAR, COMPR, DIVR, TIXR, SUBR, RMO, MULR, SVC, SSK, SHIFTL, SHITR
   FORMAT 3 : LDB, LDS, LDT, LDX, LPS, STB, STS, STT, LPX
   ONLY SIC Operations are included
*/




/****************************** DFINATE FUNCTION *****************************/

/************************ FUNCTIONS USED IN PASS1 BELOW***************************/

//label 체크
char* ReadLabel() {
	j = 0;//zeroing
	while (Buffer[Index] != ' ' && Buffer[Index] != '\t' && Buffer[Index] != '\n')
		Label[j++] = Buffer[Index++];
	Label[j] = '\0';
    return(Label);
}

//빈칸, tap 이 아닌 곳까지 index 이동 후 계속 입력
void SkipSpace() {
	while (Buffer[Index] == ' ' || Buffer[Index] == '\t')
		Index++;
}

//opeerator check
char* ReadOprator() {
	j = 0;//zeroing
	while (Buffer[Index] != ' ' && Buffer[Index] != '\t' && Buffer[Index] != '\n')
		Mnemonic[j++] = Buffer[Index++];
	//Mnemonic에 읽어온 줄의 opcode를 입력
	Mnemonic[j] = '\0';
	return(Mnemonic);
}

char* ReadOperand() {
	j = 0;//zeroing
	while (Buffer[Index] != ' ' && Buffer[Index] != '\t' && Buffer[Index] != '\n')
		// 입력된 줄의 현재 index가 space / tap / \n 이 아닐 경우
		Operand[j++] = Buffer[Index++];
	//operand에 현재 줄의 operand 읽어온다.
	Operand[j] = '\0';
	return(Operand);
}

//OPCODE가 END를 가르킬 때 LABEL에 값이 들어있는 경우 호출하여 LABEL의 값을 SYMTAB에 저장
void RecordSymtab(char* label) {
	strcpy(SYMTAB[SymtabCounter].Label, label);
	SYMTAB[SymtabCounter].Address = LOCCTR[LocctrCounter - 1];
	//SYMBOL TABLE의 address에 현재의 LABEL
	SymtabCounter++;
}

int SearchSymtab(char* label) {
	FoundOnSymtab_flag = 0;
	for (int k = 0; k <= SymtabCounter; k++) {
	//SymbolTable을 확인후
	if (!strcmp(SYMTAB[k].Label, label)) {
		//SYMTAB 구조체의 label요소와 label을 비교하여 같은 값이 있는 지 확인
		FoundOnSymtab_flag = 1;
        return (FoundOnSymtab_flag);
		break;
	}
}
return (FoundOnSymtab_flag);
//SYMTAB에 없다면 0반환
}

int SearchOptab(char * Mnemonic) {
	int size = sizeof(OPTAB) / sizeof(SIC_OPTAB);
	//OPTAB에 저장된 요소들 3*n을 SIC_OPTAB 요소 3으로 나눈 것 : 3n / 3 = n, 총 정의된 opcode의 갯수
	FoundOnOptab_flag = 0;
	//FoundOnOptab_flag 초기화
	for (int i = 0; i<size; i++) {
		//찾은 opcode를 검사
		if (!strcmp(Mnemonic, OPTAB[i].Mnemonic)) {
			//OPTAB(정의해둔 명령어들에 현재라인에 있는 opcode가 있다면
			Counter = i;
			//Counter 증가, 이번에 들어온 line에서의 opcode가 있는 배열 i를 가르키게 한다.
			FoundOnOptab_flag = 1;
			//찾았다면 1 반환
			break;
		}
	}
	return (FoundOnOptab_flag);
	//못찾았으면 0, 있다면 1 반환
}

int StrToDec(char* c) {
	// 입력된 스트링을 10진수로 변경, 문자 - '0' (아스키 48은 빼면 숫자값과 동일)
	int dec_num = 0;
	char temp[10];
	strcpy(temp, c);

	int len = strlen(c);
	for (int k = len - 1, l = 1; k >= 0; k--)
	{
		dec_num = dec_num + (int)(temp[k] - '0')*l;
		l = l * 10;
	}
	return (dec_num);
}

int StrToHex(char* c)
// 스트링을 16진수로.
{
	int hex_num = 0;
	char temp[10];
	strcpy(temp, c);

	int len = strlen(temp);
	for (int k = len - 1, l = 1; k >= 0; k--)
	{
		if (temp[k] >= '0' && temp[k] <= '9')
			hex_num = hex_num + (int)(temp[k] - '0')*l;
		else if (temp[k] >= 'A' && temp[k] <= 'F')
			hex_num = hex_num + (int)(temp[k] - 'A' + 10)*l;
		else if (temp[k] >= 'a' && temp[k] >= 'f')
			hex_num = hex_num + (int)(temp[k] - 'a' + 10)*l;
		else;
		l = l * 16;
	}
	return (hex_num);
}
int ComputeLen(char* c) {
	unsigned int b;
	char len[32];

	strcpy(len, c);
	if (len[0] == 'C' || len[0] == 'c' && len[1] == '\'') {
		// \' 작은따옴표, < C' > , 즉 BYTE로 캐릭터를 받을 경우 
		for (b = 2; b <= strlen(len); b++) {
			if (len[b] == '\'') {
				b -= 2;
				//따옴표 2개를 loc에서 빼기위함.
				//문자만 길이에 반영.
				break;
			}
		}
	}
	if (len[0] == 'X' || len[0] == 'x' && len[1] == '\'')
		b = 1;
	//16진수 수를 받을 경우 1바이트(8비트)만 증가시킴 
	return (b);
}
/********************************** FUNCTIONS USED IN PASS 1 ABOVE *************************************/



/********************************** FUNCTIONS USED IN PASS 2 BELOW*************************************/
/*
list작성
기본적으로 IMRArray에 저장한 값들을 읽어오는 과정
objectcode작성
*/
// 리스트 생성
void CreateProgramList() {
	int loop;
	FILE *fptr_list;

	fptr_list = fopen("sic.list", "w"); //파일 작성 

	if (fptr_list == NULL) // 파일 생성 오류 
	{
		printf("ERROE: Unable to open the sic.list.\n");
		exit(1);
	}

	fprintf(fptr_list, "%-4s\t%-10s%-10s%-10s\t%s\n", "LOC", "LABEL", "OPERATOR", "OPERAND", "OBJECT CODE");
	for (loop = 0; loop<ArrayIndex; loop++) // 각 루프마다 
	{
		if (!strcmp(IMRArray[loop]->OperatorField, "START") || !strcmp(IMRArray[loop]->OperatorField, "RESW") || !strcmp(IMRArray[loop]->OperatorField, "RESB") || !strcmp(IMRArray[loop]->OperatorField, "END"))
			//OPTAB에 있는 OPCODE가 아닌경우
			fprintf(fptr_list, "%04x\t%-10s%-10s%-10s\n", IMRArray[loop]->Loc, IMRArray[loop]->LabelField, IMRArray[loop]->OperatorField, IMRArray[loop]->OperandField);
		else
			//OPTAB에 있는 OPCODE인 경우 
			fprintf(fptr_list, "%04x\t%-10s%-10s%-10s\t%06x\n", IMRArray[loop]->Loc, IMRArray[loop]->LabelField, IMRArray[loop]->OperatorField, IMRArray[loop]->OperandField, IMRArray[loop]->ObjectCode);
	}
	fclose(fptr_list);
}

void CreateObjectCode() {
	int first_address;
	int last_address;
	int temp_address;
	int temp_objectcode[30];
	int first_index;
	int last_index;
	int x, xx;
	int loop;

	char temp_operator[12][10];
	char temp_operand[12][10];

	FILE *fptr_obj;
	fptr_obj = fopen("sic.obj", "w");
	if (fptr_obj == NULL) // 파일 생성 오류
	{
		printf("ERROE: Unable to open the sic.obj.\n");
		exit(1);
	}

	printf("Creating Object Code...\n\n");

	loop = 0;
	if (!strcmp(IMRArray[loop]->OperatorField, "START")) //HEADER 입력
	{
		printf("H%-6s%06x%06x\n", IMRArray[loop]->LabelField, start_address, program_length);
		fprintf(fptr_obj, "H^%-6s^%06x^%06x\n", IMRArray[loop]->LabelField, start_address, program_length);
		loop++;
	}

	while (1)
	{

		first_address = IMRArray[loop]->Loc; //한줄의 시작주소 
		last_address = IMRArray[loop]->Loc + 29;
		//한줄의 마지막주소
		first_index = loop;
        //인덱스 초기화

							//출력할 수 있는 길이를 계산 
		for (x = 0, temp_address = first_address; temp_address <= last_address; loop++)
			//반복문
		{
			if (!strcmp(IMRArray[loop]->OperatorField, "END"))
				//만약에 OPCODE가 "END"일 경우 
				break; // 리스트작성을 종료한다.
			else if (strcmp(IMRArray[loop]->OperatorField, "RESB") && strcmp(IMRArray[loop]->OperatorField, "RESW"))
				// OPCODE가 RESB이거나 RESW일 경우(변수입력일 경우) 
			{
				temp_objectcode[x] = IMRArray[loop]->ObjectCode; //Object코드를 tem_objectcode에 넘겨주고.
				strcpy(temp_operator[x], IMRArray[loop]->OperatorField); //Operatorfiled의 값을 operator에 저장.
				strcpy(temp_operand[x], IMRArray[loop]->OperandField); //operandfiled의 값을 temp_operand에 저장 
				last_index = loop + 1; //index의 값을 변수의 값이 다 들어올때까지 늘려준다.
				x++; // 명령어 갯수 1개 증가(1바이트씩)
			}
			else
			temp_address = IMRArray[loop + 1]->Loc; //다음번 변수가 last_address를 넘어가는 지 확인하기 위해 저장 
		}

		printf("T%06x%02x", first_address, (IMRArray[last_index]->Loc - IMRArray[first_index]->Loc));
		//object코드에 첫번째 주소와 그 줄에 쓰여진 objectcode의 길이를 출력
		fprintf(fptr_obj, "T^%06x^%02x", first_address, (IMRArray[last_index]->Loc - IMRArray[first_index]->Loc));
		//object코드에 첫번째 주소와 그 줄에 쓰여진 objectcode의 길이를 출력

		for (xx = 0; xx<x; xx++)
			//한줄에 들어갈 수 있는 최대의 목적코드를 출력,입력.
		{
			if ((strcmp(temp_operator[xx], "BYTE") == 0) && (temp_operand[xx][0] == 'X' || temp_operand[xx][0] == 'x')) {
				printf("%02x", temp_objectcode[xx]);
				//16진수로 표현된 1바이트의 경우 1바이트 형식에 맞추어 출력
				fprintf(fptr_obj, "^%02x", temp_objectcode[xx]);
			}
			else {
				printf("%06x", temp_objectcode[xx]);
				fprintf(fptr_obj, "^%06x", temp_objectcode[xx]);
			}
		}

		printf("\n");
		fprintf(fptr_obj, "\n");

		if (!strcmp(IMRArray[loop]->OperatorField, "END"))
			break;
		//end일경우 입력 종료 
	}

	printf("E%06x\n\n", start_address);
	fprintf(fptr_obj, "E^%06x\n\n", start_address);
	// 시작주소를 입력하고 종료 
	fclose(fptr_obj);
}
/********************************** FUNCTIONS USED IN PASS 2 ABOVE *************************************/





/******************************* MAIN FUNCTION *******************************/
void main(void)
{
	FILE* fptr;

	char filename[10];
	char label[32];
	char opcode[32];
	char operand[32];

	int loc = 0;
	int line = 0;
	int loop;
	int is_empty_line;
	int is_comment;
	int loader_flag = 0;

	printf(" ******************************************************************************\n");
	printf(" * Program: SIC ASSEMBYER                                                     *\n");
	printf(" * HOW TO USE                                                                 *\n");
	printf(" *   - TYPE ASM FILE                                                          *\n");
	printf(" * RESULT                                                                     *\n");
	printf(" *   - LIST FILE                                                              *\n");
    printf(" *   - OBJ  FILE                                                              *\n");
	printf(" ******************************************************************************\n");


	printf("\nEnter the file name you want to assembly (sic.asm):");
	scanf("%s", filename);
	fptr = fopen(filename, "r");
	if (fptr == NULL)
	{
		printf("ERROE: Unable to open the %s file.\n", filename);
		exit(1);
	}

	/********************************** PASS 1 ***********************************/
	printf("Pass 1 Processing...\n");
	while (fgets(Buffer, 256, fptr) != NULL)
		//fgets(문자열 포인터, 입력할 최대문자수, 파일포인터)
		//파일에서 1줄씩 입력받는다.
	{
		is_empty_line = strlen(Buffer);
		//입력받은 파일 라인의 길이 확인
		Index = 0;
		j = 0;

		/* ************************************************** 라벨 읽어오는 과정 ********************************************************* */
		strcpy(label, ReadLabel());
		/*
         * ReadLabel, 전역변수 index와 j를 사용하여
		 * 들어온 문장 첫번째가 space, tap, LF가 아닐경우
		 * Label배열에  입력된 파일의 행을 입력한다.
         */
		if (Label[0] == '.')
			is_comment = 1;
		else
			is_comment = 0;
		//입력된 라인이 코멘트 라인일 경우 = 1 아닐 경우 0

		if (is_empty_line>1 && is_comment != 1)
			//읽어들인 행이 2개글자 이상이고 코맨트가 아닐경우 
		{
			Index = 0;
			//위의 strcpy(label, ReadLabel())에서 증가시켜진 index값 초기화
			j = 0;
			/* 현재의 IMRArray[ArrayIndex] # 요소 초기화   */

			IMRArray[ArrayIndex] = (IntermediateRec*)malloc(sizeof(IntermediateRec));/* [A] */
																					 //IntermediateRecord형 구조체 포인터 = IMRArray, ArrayIndex = 0
			IMRArray[ArrayIndex]->LineIndex = ArrayIndex;
			//IMRArray[0]->Lineindex = 0 , 초기화

			strcpy(label, ReadLabel());
			//ReadLabel로부터 입력받은 값을 label에 저장 
			//줄수에 따라 index증가
			strcpy(IMRArray[ArrayIndex]->LabelField, label);
			//IMRArray[0]->LabelField에 label저장.
			SkipSpace();
			//현재 index가 tap이거나 빈공간일 경우 index를 증가 시켜준다.

			if (line == 0)
				/* ************************************* 1번째 줄일 경우 OPCODE 읽어오는 과정 ******************************************* */
			{
				strcpy(opcode, ReadOprator());
				//Opcode를 읽어옵니다.
				strcpy(IMRArray[ArrayIndex]->OperatorField, opcode);/* [A] */
																	//읽어온 opcode를 IMRArray에 입력.
				if (!strcmp(opcode, "START"))
					//읽어온 opcode가 start일경우
				{
					SkipSpace();
					strcpy(operand, ReadOperand());
					strcpy(IMRArray[ArrayIndex]->OperandField, operand);/* [A] */
																		//operand를 읽어온 뒤
					LOCCTR[LocctrCounter] = StrToHex(operand);
					//operand(시작주소)를 hex로 바꾸고 operand 
					start_address = LOCCTR[LocctrCounter];
					//프로그램 시작주소를 start_address에 입력
				}
				else
					//만약 line 0의 opcode가 START가 아닌경우 경우 
				{
					LOCCTR[LocctrCounter] = 0;
					start_address = LOCCTR[LocctrCounter];
					// 프로그램의 시작주소 = 0
				}
			}
			else
				/* ************************************* 1번째 줄 아닐경우 OPCODE 읽어오는 과정 ******************************************* */
			{
				strcpy(opcode, ReadOprator());
				//opcode를 Mnemonic에 집어넣고 리턴을 받아 opcode에 집어넣는다.
				strcpy(IMRArray[ArrayIndex]->OperatorField, opcode);
				//현재 IMRArray[ArrayIndex]->OperatorField에 opcode를 집어넣는다.
				SkipSpace();
				//operand로 이동

				/* ************************************* 1번째 줄 아닐경우 OPERAND 읽어오는 과정 ******************************************* */

				strcpy(operand, ReadOperand());
				strcpy(IMRArray[ArrayIndex]->OperandField, operand);
				//operand입력

				/* OPCODE가 END를 가르킬 때 -> a) Label이 있는 경우     b) Label이 없는 경우 -> Symtab에 추가*/
				if (strcmp(opcode, "END"))
					//만약 파일의 끝일 경우 
				{
					if (label[0] != '\0')
						//label에 값이 들어있을 경우
					{
						if (SearchSymtab(label))
							//SYMTAB에 있는 label일 경우 
						{
							fclose(fptr);
							//파일을 종료시키고
							printf("ERROE: Duplicate Symbol\n");
							//오류메시지
							FoundOnSymtab_flag = 0;
							//initiatin FoundOnSymtab_flag, 초기화
							exit(1);
							// 종료
						}
						RecordSymtab(label);
						//SYMTAB에 없는 label일 경우 Symtab에 입력 후 SymtabCounter증가 
					}


					/* ************************************* opcode를 통한 loc 계산 ******************************/
					if (SearchOptab(opcode))
						//만약 읽어들인 줄에 있던 opcode가 Optab에 정의가 되어있던 것이라면 if문 실행
						LOCCTR[LocctrCounter] = loc + (int)(OPTAB[Counter].Format - '0');
					//OPTAB[Counter].Format = '3', char format, 따라서 '3'은 문자로 처리되고 이는 아스키코드에서 10진수로 51, '0'은 아스키코드 10진수로 48 
					//따라서 '0'을 빼주는 것은 정수 3으로 만들어준다. char 형은 정수형
					//counter는 SearchOptab에서 가져온다. OPTAB에 있는 몇번째 배열인지를 가져옴
					//loc 전역변수로 초기값 0
					else if (!strcmp(opcode, "WORD"))
						//만약 opcode가 'WORD'라면 
						LOCCTR[LocctrCounter] = loc + 3;
					//Loccounter 만 증가시켜준다, LocctrCounter도 전역변수로 초기값 0
					else if (!strcmp(opcode, "RESW"))
						LOCCTR[LocctrCounter] = loc + 3 * StrToDec(operand);
					//opcode == RESW라면 loc + 3*(operand에 들어온 값을 16진수로 바꾼 것)
					else if (!strcmp(opcode, "RESB"))
						LOCCTR[LocctrCounter] = loc + StrToDec(operand);
					//opcode == RESB라면 loc + (operand에 들어온 값을 16진수로 바꾼 것)
					else if (!strcmp(opcode, "BYTE"))
						LOCCTR[LocctrCounter] = loc + ComputeLen(operand);
					/* opcode가 BYTE일 때
					`			   1번째 : 문자를 받을 경우, 2번째 : 16진수를 받을 경우를 따져 현재 loc에 더한다.*/

					else {
						fclose(fptr); //opcode를 opsearch를 통해 optab을 확인하고 왔으나 없다
									  //오류 
						printf("ERROE: Invalid Operation Code\n");
						//오류
						exit(1);
						//프로그램 종료
					}
				}
			}
			loc = LOCCTR[LocctrCounter];
			// 전역변수 loc에 현재 저장된 로케이션 위치를 저장.

			/* NEGATIVE INDEX 사용법 나중에 또 참고 */
			IMRArray[ArrayIndex]->Loc = LOCCTR[LocctrCounter - 1];
			// 1: 만약 첫번째 라인을 읽어왔다면 IMRArray[ArrayIndex]->Loc = LOCCTR[0-1], 쓰레기값.
			// 2: 두번째 라인을 읽어왔다면 IMRArray[ArrayIndex]->Loc=LOCCTR[1-1], 첫번째 라인의 loc정보가 입력된다.
			//첫번째 줄을 읽으면 LOCCTR[0]에는 첫번째 줄의 pc의 값이 들어가게 되고 

			LocctrCounter++;
			//한줄 읽고나서 LocctrCounter 증가
			ArrayIndex++;
			// IMRArray[ArrayIndex] 한칸 증가 
		}

		FoundOnOptab_flag = 0;
		//FoundOnOptab 초기화
		line += 1;
		//파일의 몇번째 줄인지 증가
	}
	program_length = LOCCTR[LocctrCounter - 2] - LOCCTR[0];
	/*
	1: 만약 첫번쨰 줄을 읽었다면 LOCCTR[-2]-LOCCTR[0] // 쓰레기값
	2: 만약 두번째 줄을 읽었다면 LOCCTR[-1]-LOCCTR[0] // 쓰레기값
	3: 만약 세번째 줄을 읽었다면 LOCCTR[0]-LOCTCTR[0] // 0
	sic/xe머신의 첫번째 줄은 start로 주소지정(혹은 Dynamic Loading을 위한 default값 0으로 지정
	2번째 줄부터 opcode이용, 그러나 2개의 라인으로 할 수 있는 것은 없기때문에 negative index value를 사용하여도 문제는 없다.
	*/


	/*
	PASS1
	1 : LABEL 읽어오기
	a) IS LABEL A COMMENT => (NO, INSERT LABEL INTO IMRArray[ArrayIndex]->LabelField

	2 : OPCODE 읽어오기
	a) IS THE LINE THE FIRST LINE => (YES, INSERT OPCODE INTO IMRArray[ArrayIndex]->OperatorField )
	IS THE OPCODE 'START' => (NO, SET LOCCTR[LocctrCounter] = 0, start_address = LOCCTR[LocctrCounter] )
	b)							  =>> (NO, INSERT OPCE INT TO IMRArray[ArrayIndex]->OperatorField )

	3 : OPERAND 읽어오기
	a) SET IMRArray[ArrayIndex]->OperandField = operand
	b) IS THE OPCODE 'END' => DOES LABEL HAVE ANY VALUE? => (YES, ERROR MSG), (NO, INSERT LABEL INT Symtab by "RecordSymtab(label)"

	4 : LOC 계산 ( opcode를 이용 )
	a) IS OPCODE AMONG OPTAB => (YES, LOCCTR(LocctrCounter] = loc +int(OPTAB[Counter].Format-'0'), <= loc + 3
	b) IS OPCODE WORD => (YES, LOCCTR(LocctrCounter] = loc + 3
	c) IS OPCODE RESW => (YES, LOCCTR(LocctrCounter] = loc + 3*(sizeofoperand in hex)
	d) IS OPCODE BYTE => IS OPERAND CHARACTER? ( LOCCTR[LocctrCounter] = loc + number of characters )
	IS OPERAND HE? ( LOCCTR(LocctrCounter] = loc + 1 )

	5 : ERROR HANDLING : NO OPCODE FOUND IN OPTAB, EXTERMINATE, ASSERT ERROR MSG.

	6 : SET current loc with LOCCTR[LocctrCounter]

	7 : SIMPLE ARITHMETICS
	IMRArray[ArrayIndex]->Loc = LOCTTR[LocctrCounter-1]
	LocctrCounter++
	ArrayIndex++

	FoundOnOptab_flag = 0       (Initiate Global Variable )
	line +=1					(USING line variable in the PASS1 only to check if it is the first line)

	8 : Calculate program_length = LOCTTR[LocctrCounter-2]-LOCCTR[0]
	*/



	/********************************** PASS 2 ***********************************/
	/*****************************STILL IN MAIN FUNCTION**************************/
	printf("Pass 2 Processing...\n");

	unsigned long inst_fmt; //
	unsigned long inst_fmt_opcode; // 목적코드의 opcode
	unsigned long inst_fmt_index; // index addressing mode
	unsigned long inst_fmt_address; // 


	for (loop = 1; loop<ArrayIndex; loop++) {
		/*
		IMRArray 요소 : Label, OperatorField, OperandField
		loop = 1 ~ MAX ArrayIndex까지 각 요소 참조
		*/
		inst_fmt_opcode = 0;
		inst_fmt_index = 0;
		inst_fmt_address = 0;

		strcpy(opcode, IMRArray[loop]->OperatorField);
		// 현재 index까지 들어온 opcode들을 하나씩 대입하여 확인

		/********************************* OPCODE 목적코드 입력 *********************************/
		if (SearchOptab(opcode)) {
			//OPTAB에 있는 명령어라면 
			inst_fmt_opcode = OPTAB[Counter].ManchineCode;
			//현재 opcode의 목적코드 복사 
			inst_fmt_opcode <<= 16;
			/*
			unsigned long : 4BYTE => 0000 0000 0000 0000 0000 0000 0000 0000
			ex) 0x34가 들어올 경우
			0000 0000 0000 0000 0000 0011 0000 0100
			16bit shift하면
			0000 0011 0000 0100 0000 0000 0000 0000
			inst_fmt_opcode = inst_fmt_opcode << 16
			shift 16bits to the left
			*/

			IMRArray[loop]->ObjectCode = inst_fmt_opcode;
			// 목적코드에 입력 


			/********************OPCODE가 OPTAB에 있는 경우, OPERAND *********************************/
			strcpy(operand, IMRArray[loop]->OperandField);
			// operand 읽어옴

			if (operand[strlen(operand) - 2] == ',' && operand[strlen(operand) - 1] == 'X') {
				//operand의 마지막이 ,X일경우 => X indexed mode라면 
				inst_fmt_index = 0x008000;
				/*
				flag bit에 1 입력 => 0000 0000 0100 0000 0000 0000 0000
				ni xbpe
				*/
				operand[strlen(operand) - 2] = '\0';
				// ,X 삭제 
			}
			else
				inst_fmt_index = 0x000000;
			//X indexed mode가 아니라면 inst_fmt_index를 0x0으로 초기화

			for (int search_symtab = 0; search_symtab<SymtabCounter; search_symtab++) {
				if (!strcmp(operand, SYMTAB[search_symtab].Label)) //operand가 SYMTAB에 있다면
					inst_fmt_address = (long)SYMTAB[search_symtab].Address; //해당 SYMBOL의 위치를 저장
			}

			inst_fmt = inst_fmt_opcode + inst_fmt_index + inst_fmt_address; //목적코드 조립
			IMRArray[loop]->ObjectCode = inst_fmt;
			/* 목적코드 입력(현재 전부 3형식)
			0000 0000 0000 0000   0000 0000 0000 0000
			( O P C O D E ) + ni xbpe + (  a d d r e s s  )
			*/
		}
		else if (!strcmp(opcode, "WORD")) { //opcode가 WORD인 경우 
			strcpy(operand, IMRArray[loop]->OperandField); //operand를 가져오고
			IMRArray[loop]->ObjectCode = StrToDec(operand); //operand를 10진수 아스키코드로 바꾸어 저장.
		}
		else if (!strcmp(opcode, "BYTE")) { //opcode가 BYTE인 경우 
			strcpy(operand, IMRArray[loop]->OperandField); //operand를 가져오고
			IMRArray[loop]->ObjectCode = 0; // 1BYTE크기의 값을 입력.(loop가 돌면서 계속 증가할 수도 있음)

			if (operand[0] == 'C' || operand[0] == 'c' && operand[1] == '\'') { // opreand가 C' 로 시작할경우
				for (int x = 2; x <= (int)(strlen(operand) - 2); x++) { // 문자열부분 C'****' **** 부분을 
					IMRArray[loop]->ObjectCode = IMRArray[loop]->ObjectCode + (int)operand[x]; // 10진수 아스키코드로 변환하여
																							   // 목적코드와 더해준다.
					IMRArray[loop]->ObjectCode <<= 8;
				}
			}

			if (operand[0] == 'X' || operand[0] == 'x' && operand[1] == '\'') {
				/*
				operand의 값이 16진수일 경우
				X'F1'이 들어왔다고 가정하면
				operand의 값을 계산(StrToHex)하고
				문자와 마찬가지로 왼쪽으로 SHIFT 8BIT하여 목적코드에 입력.
				*/
				char *operand_ptr;
				operand_ptr = &operand[2];
				*(operand_ptr + 2) = '\0';
				for (int x = 2; x <= (int)(strlen(operand) - 2); x++) {
					IMRArray[loop]->ObjectCode = IMRArray[loop]->ObjectCode + StrToHex(operand_ptr);
					IMRArray[loop]->ObjectCode <<= 8;
				}
			}

			IMRArray[loop]->ObjectCode >>= 8; //각 반복문에 1바이트 좌측으로 밀었던 것을 다시 오른쪽으로 1바이트 이동
		}
		else
			/* do nothing */;
	}

	CreateProgramList(); //LIST 만들기
	CreateObjectCode(); //목적파일 생성 

	for (loop = 0; loop<ArrayIndex; loop++)
		free(IMRArray[loop]);
	//PASS 1에서 동적할당한 IMRArray 메모리 순차적으로 해제 

	printf("Compeleted Assembly\n");
	fclose(fptr);

	exit(0);
}
