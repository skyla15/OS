// *****************
// * 헤더파일 정의 *
// *****************
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ***************
// * 매크로 정의 *
// ***************
#define SIZE_CLINE 1000		// 소스파일 한줄의 크기 (주석포함)
#define SIZE_STRING 10		// 10칸씩 레이블, 오퍼레이션, 오퍼랜드, 오퍼랜드2


#define SIZE_SYMTABLE 100	// 심볼테이블 크기
#define SIZE_OPTABLE 25		// 오퍼레이션 테이블 크기


// ******************
// * 전역 변수 정의 *
// ******************
FILE *srcFile;	// 소스파일 파일포인터
FILE *objFile;	// 오브젝트파일 파일포인터

// 심볼 테이블에 구성될 행과 열을 나타내는 구조체
struct symbolNode {
	char label[SIZE_STRING];	// 레이블 이름
	int address;			// 해당 레이블의 주소
};
struct symbolNode symbolTable[SIZE_SYMTABLE];	// 심볼 테이블

// 명령어와 해당 코드를 나타내는 구조체
struct operationNode {
	char operation[SIZE_STRING];	// 명령어 이름
	int code;			// 해당 명령어의 코드
};
// 오퍼레이션 테이블
struct operationNode operationTable[SIZE_OPTABLE] = {
	{"AND", 0x0}, {"ADD", 0x1}, {"LDA", 0x2},
	{"STA", 0x3}, {"BUN", 0x4}, {"BSA", 0x5},
	{"ISZ", 0x6},
	{"CLA", 0x7800}, {"CLE", 0x7400}, {"CMA", 0x7200},
	{"CME", 0x7100}, {"CIR", 0x7080}, {"CIL", 0x7040},
	{"INC", 0x7020}, {"SPA", 0x7010}, {"SNA", 0x7008},
	{"SZA", 0x7004}, {"SZE", 0x7002}, {"HLT", 0x7001},
	{"INP", 0xF800}, {"OUT", 0xF400}, {"SKI", 0xF200},
	{"SKO", 0xF100}, {"ION", 0xF080}, {"IOF", 0xF040}
};


// *************
// * 함수 정의 *
// *************
void pass1();	// 어셈블러 패스1
void pass2();	// 어셈블러 패스2

// 소스코드의 한줄을 3개로 나누는 함수 (레이블, 오퍼레이션, 오퍼랜드)
void seperate_cline(char *_cline, char *_label, char *_operation, char *_operand, char *_operand2);
// 문자열에 공백, 콤마, 엔터를 제거하는 함수
void remove_space(char *_s, int _max);
// 심볼테이블에 새로운 label을 입력하는 함수
void symbolTable_insert(char *_label, int _address);
// 심볼테이블에서 해당하는 심볼의 주소를 반환하는 함수
int symbolTable_search(char *_label);
// 오퍼레이션 테이블에서 해당 명령어에 대한 코드 반환하는 함수
uint16_t operationTable_search(char *_operation);
// MRI Instruction 을 만드는 함수
uint16_t make_MRI_instruction(char *_operation, char *_operand, char *_operand2);

// ************
// * 메인함수 *
// ************
int main(int argc, char *argv[]) {

	// 메인 함수에서 사용할 지역 변수를 정의한다.
	char *_srcFileName;	// 소스파일 이름이 있는 곳을 지정할 포인터
	char *_objFileName;	// 오브젝트파일 이름이 있는 곳을 지정할 포인터


	// 사용자가 소스파일과 오브젝트파일 이름을 입력했는지 확인한다.
	if(argc != 3) {

		printf("ERROR : %s 소스파일이름 오브젝트파일이름 식으로 입력하세요.\n", argv[0]);
		exit(1);
	}// END - if(argc != 3)
	

	// 파일 이름이 있는 곳을 지정한다.
	_srcFileName = argv[1];	// argv[1]은 소스파일 이름이 있는 곳이다.
	_objFileName = argv[2];	// argv[2]는 오브젝트파일 이름이 있는 곳이다.

	// 사용자가 입력한 소스파일과 오브젝트파일 이름을 이용해 파일을 열어준다.
	srcFile = fopen(_srcFileName, "r");	// 소스파일은 읽기 전용으로 파일을 연다.
	if(srcFile == NULL) {
		
		printf("ERROR : %s 소스파일 열기를 실패하였습니다.\n", _srcFileName);
		exit(1);
	}// END - if(srcFILE == NULL)

	objFile = fopen(_objFileName, "w");	// 오브젝트파일은 쓰기 전용으로 파일을 연다.
	if(objFile == NULL) {

		printf("ERROR : %s 오브젝트파일 열기를 실패하였습니다.\n", _objFileName);
		exit(1);
	}// END - if(objFILE == NULL)

	// 어셈블러 패스1과 패스2 과정을 실행한다.
	pass1();
	pass2();

	// 열었던 파일을 닫아준다.
	fclose(srcFile);
	fclose(objFile);

	return 0;
}// END - int main(int argc, char *argv[])


// ***************
// * 함수 구현부 *
// ***************

// 어셈블러 패스1 과정 함수
void pass1() {

	int _lc = 0; // Location Counter 로케이션 카운터
	
	// 소스코드가 10칸, 10칸, 10칸 나눠서
	// 레이블, 오퍼레이션, 오퍼랜드로 구별하는 소스코드의 예다.
	// 한줄에 30칸이며 각각 10칸씩 나누어 쪼갠다.
	char _cline[SIZE_CLINE];
	char _label[SIZE_STRING];
	char _operation[SIZE_STRING];
	char _operand[SIZE_STRING];
	char _operand2[SIZE_STRING];

	// 파일 포인터 위치를 0으로 초기화한다.
	rewind(srcFile);

	// 소스코드의 한줄을 읽어온다.
	while(fgets(_cline, SIZE_CLINE, srcFile) > 0) {

		// 소스코드 한줄을 3개로 나눈다.
		seperate_cline(_cline, _label, _operation, _operand, _operand2);	
		
		// 레이블이 있는지 확인한다.
		if(_label[0] == '\0') {

			// 레이블이 존재하지 않는다면
			if(strcmp(_operation, "ORG") == 0) {

				// 오퍼레이션이 ORG 일때,
				_lc = strtol(_operand, NULL, 16);
				continue;
			}// END - if(strcmp(_operation, "ORG") == 0)		
			
			else {

				// 오퍼레이션이  ORG가 아닐때,
				if(strcmp(_operation, "END") == 0) {

					// 오퍼레이션이 END일때 pass1이 종료된다.
					break;
				}// END - if(strcmp(_operation, "END") == 0)
			
				else {
					// 오퍼레이션이 END가 아닐때 아무일도 하지않는다.
				}// END - elseif(strcmp(_operation, "END") == 0)
			}// END - else(strcmp(_operation, "ORG") == 0) 
		} // END - if(_label[0] == '\0')

		else {

			// 레이블이 존재한다면 심볼테이블에 추가해준다.
			symbolTable_insert(_label, _lc);
		} // END - else (_label[0] == '\0')

		// lc 증가시킨다.
		_lc += 0x1;
	}// END - while(fgets(_cline, SIZE_CLINE, scrfile) > 0)
}// END - void pass1()


// 어셈블러 패스2 과정 함수
void pass2() {

	int _lc = 0; // Location Counter 로케이션 카운터

	// 소스코드가 10칸, 10칸, 10칸 나눠서
	// 레이블, 오퍼레이션, 오퍼랜드로 구별하는 소스코드의 예다.
	// 한줄에 30칸이며 각각 10칸씩 나누어 쪼갠다.
	char _cline[SIZE_CLINE];
	char _label[SIZE_STRING];
	char _operation[SIZE_STRING];
	char _operand[SIZE_STRING];
	char _operand2[SIZE_STRING];

	// 파일 포인터 위치를 0으로 초기화한다.
	rewind(srcFile);
	rewind(objFile);
        // 소스코드의 한줄을 읽어온다.
        while(fgets(_cline, SIZE_CLINE, srcFile) > 0) {

                // 소스코드 한줄을 3개로 나눈다.
                seperate_cline(_cline, _label, _operation, _operand, _operand2);

		// Pseudo Instruction 인지 확인한다. (ORG, END, HEX, DEC)
		if(strcmp(_operation, "ORG") == 0 || strcmp(_operation, "END") == 0
			|| strcmp(_operation, "HEX") == 0 || strcmp(_operation, "DEC") == 0) {
		
			// Pseudo Instruction 일때,
	
			if(strcmp(_operation, "ORG") == 0) {

				// ORG 라면 _lc를 해당 어드레스로 초기화한다.
				_lc = strtol(_operand, NULL, 16);
				continue;	
			}// END - if(strcmp(_operation, "ORG") == 0)
			if(strcmp(_operation, "END") == 0) {

				// END 라면 끝낸다.
				break;
			}// END - if(strcmp(_operation, "END") == 0)
			if(strcmp(_operation, "HEX") == 0) {

				// 오퍼랜드를 16진수로 인식하여 출력한다.
				fprintf(objFile, "%X\t%04X\n", _lc, (uint16_t)strtol(_operand, NULL, 16));
			}// END - strcmp(_operation, "HEX") == 0
			if(strcmp(_operation, "DEC") == 0) {

				// 오퍼랜드를 10진수로 인식하여 출력한다.
				fprintf(objFile, "%X\t%04X\n", _lc, (uint16_t)strtol(_operand, NULL, 10));
			}// END - strcmp(_operation, "HEX") == 0
		}// END - if(strcmp(_operation, "ORG") == 0 || strcmp(_operation, "END") == 0)

		else {

			// Pseudo Instruction 이 아닐때,
			
			// MRI 명령어 인지 아닌지 확인한다.
			if(strcmp(_operation, "AND") == 0 || strcmp(_operation, "ADD") == 0
				|| strcmp(_operation, "LDA") == 0 || strcmp(_operation, "STA") == 0
				|| strcmp(_operation, "BUN") == 0 || strcmp(_operation, "BSA") == 0
				|| strcmp(_operation, "ISZ") == 0) {

				// 계산한 MRI Instruction을 출력한다.
				fprintf(objFile, "%X\t%04X\n", _lc,
					make_MRI_instruction(_operation, _operand, _operand2));
			} // END - if MRI operation

			else {
				
				// 해당 오퍼랜드의 값을 찾아 출력한다.
				fprintf(objFile, "%X\t%04X\n", _lc, operationTable_search(_operation));
			}// END - else - not MRI operation

		}// END - else - not Pseudo Instruction
		_lc += 1;
	}// END - while(fgets(_cline, SIZE_CLINE, srcFile) > 0)	
}// END - void pass2()


// 소스코드의 한줄을 3개로 나누는 함수 (레이블, 오퍼레이션, 오퍼랜드)
void seperate_cline(char *_cline, char *_label, char *_operation, char *_operand, char *_operand2) {

	// 분리전 각 배열을 깨끗하게 청소해준다.
	memset(_label, 0, SIZE_STRING);
	memset(_operation, 0, SIZE_STRING);
	memset(_operand, 0, SIZE_STRING);
	memset(_operand2, 0, SIZE_STRING);

	// 소스코드 한줄을 10칸씩 나누어 분리한다.
	strncpy(_label, _cline, SIZE_STRING);
	strncpy(_operation, _cline+SIZE_STRING, SIZE_STRING);
	strncpy(_operand, _cline+(SIZE_STRING*2), SIZE_STRING);
	strncpy(_operand2, _cline+(SIZE_STRING*3), SIZE_STRING);

	// 문자열에 ' ', ',', '\n' 을 제거해준다.
	remove_space(_label, SIZE_STRING);
	remove_space(_operation, SIZE_STRING);
	remove_space(_operand, SIZE_STRING);
	remove_space(_operand2, SIZE_STRING);
}// END - void seperate_cline(char *_cline, char *_label, char *_operation, char *_operand2)


// 문자열에 공백, 콤마, 엔터를 제거하는 함수
void remove_space(char *_s, int _max) {

	int _i;
	for(_i = 0; _i < _max; _i++)
		if(_s[_i] == ' ' || _s[_i] == ',' || _s[_i] == '\n')
			_s[_i] = '\0';
}// END - void remove_space(char *_s, int _max)


// 심볼테이블에 새로운 label을 입력하는 함수
void symbolTable_insert(char *_label, int _address) {

	int _i;

	for(_i = 0; _i < SIZE_SYMTABLE; _i++) {

		// 심볼테이블 빈칸을 찾아서 입력한다.
		if(symbolTable[_i].label[0] == '\0') {

			strncpy(symbolTable[_i].label, _label, SIZE_STRING);
			symbolTable[_i].address = _address;
			break;
		}// END - if(symbolTable[_i].label[0] == '\0')
	}// END - for(_i = 0; _i < SIZE_SYMTABLE; _i++)
}// END - void symbolTable_insert(char *_label, int _address)


// 심볼테이블에서 해당하는 심볼의 주소를 반환하는 함수
int symbolTable_search(char *_label) {

	int _address;
	int _i;

        for(_i = 0; _i < SIZE_SYMTABLE; _i++) {

		//printf("%d : %s %X\n", _i, symbolTable[_i].label, symbolTable[_i].address);
                // 심볼테이블에서 해당하는 심볼과 맞는 심블을 찾고
		// 해당 주소를 반환한다.
                if(strcmp(_label, symbolTable[_i].label) == 0) {
			_address = symbolTable[_i].address;
			break;
                }// END - if(symbolTable[_i].label[0] == '\0')
        }// END - for(_i = 0; _i < SIZE_SYMTABLE; _i++)

	return _address;
}// END - int symbolTable_search(char *_label)

// 오퍼레이션 테이블에서 해당 명령어에 대한 코드 반환하는 함수
uint16_t operationTable_search(char *_operation) {

	uint16_t _code;

	int _i;

	for(_i = 0; _i < SIZE_OPTABLE; _i++) {

		// 오퍼레이션 테이블에서 찾고자하는 오퍼레이션에 맞는
		// 코드를 반환한다.
		if(strcmp(_operation, operationTable[_i].operation) == 0) {
			_code = operationTable[_i].code;
			break;
		}// END - if(strcmp(_operation, operationTable[_i].operation) == 0)
	}// END - for(_i = 0; _i < SIZE_OPTABLE; _i++)

	return _code;
}// END - uint16_t operationTable_search(char *operation)

// MRI Instruction 을 만드는 함수
uint16_t make_MRI_instruction(char *_operation, char *_operand, char *_operand2) {

	uint16_t _MRI_instruction = 0x00;
	uint16_t _2to4 = 0x00;	// 2~4 비트
	uint16_t _5to15 = 0x00;	// 5~15 비트
	uint16_t _1 = 0x00;	// 첫번째 비트

	// 오퍼레이션에 해당하는 코드를 가지고와서
	// 왼쪽으로 12번 시프트연산한다.
	_2to4 = operationTable_search(_operation);
	_2to4 = _2to4 << 12;

	// 오퍼랜드에 해당하는 주솟값을 가져온다.
	_5to15 = symbolTable_search(_operand);

	// 간접주소 vs 직접주소 지정 방식
	// 오퍼랜드2가 I인지 I가 아닌지 판별하여 1 또는 0비트로 설정한다.
	// 왼쪽으로 15번 시프트이동하여 최상위 비트로 설정한다.
	if(strcmp(_operand2, "I") == 0)
		_1 = 0x01;
	else
		_1 = 0x00;
	_1 = _1 << 15;
	
	// 비트마다 구한 값을 OR 계산하여 합쳐준다.
	_MRI_instruction = _1 | _2to4 | _5to15;

	return _MRI_instruction;
};
