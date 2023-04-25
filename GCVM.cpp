#include "Common.h"

#include "enums.h"
#include "utils.h"

#include "GCVM.h"


using namespace gclang;

GCVM::GCVM() : Code(nullptr), Stack(nullptr)
{
}

GCVM::~GCVM()
{
	if (Code)
		delete[] Code;

	if (Stack)
		delete[] Stack;
}

bool GCVM::LoadBytecode(string filename)
{
	std::ifstream ifs(filename, std::ifstream::binary);
	if (!ifs)
		return false;

	// seekg를 이용한 파일 크기 추출
	ifs.seekg(0, ifs.end);
	int length = (int)ifs.tellg();
	ifs.seekg(0, ifs.beg);


	Code = new int8_t[length];
	// read data as a block:
	ifs.read((char*)Code, length);

	// file format 확인
	int i = Find(string("Jzero!!").c_str(), (const char*)Code);
	if (i < 0)
		return false;

	// 
	//CodeBuf.insert(CodeBuf.end(), Code, Code + length);
	ifs.close();
	return true;
}

int GCVM::Find(const char* needle, const char* haystack)
{
	int i = 0;
	int needle_len = sizeof(needle) / sizeof(char);
	int haystack_len = sizeof(haystack) / sizeof(int8_t);

	// 1 byte 씩 전진하면서 입력된 문자열이 있는지 비교
	for (; i < haystack_len - needle_len + 1; ++i)
	{
		bool found = true;
		for (int j = 0; j < needle_len; ++j)
		{
			if (haystack[i + j] != needle[j])
			{
				found = false;
				break;
			}
		}
		if (found)
			return i;
	}

	return -1;
}

void GCVM::Init(string filename)
{
	Ip = Sp = 0;
	if (!LoadBytecode(filename)) {
		std::cout << "Can not open program." << endl;
		return;
	}

	/*
		시작코드
		0	PUSH main
		8	CALL 0
		16	HALT
	*/
	Ip = STACK_STEP * 2;
	Ip = FinStr = int(GetOperand() * STACK_STEP); // Operand 에 Ip Offset 이 있다고 가정한 코드
	Stack = new int8_t[STACK_STEP * 100000];
}

long long GCVM::GetOperand()
{
	long long i = 0; // 8byte 크기를 가져야 한다.

#ifdef BIG_ENDIAN

	// |0|1|2|3|4|5|6|7|8|....|n| : Instruction region
	//  ^ Ip 의 위치
	if (Code[Ip + 7] < 0)
		i = -1; // 0xffffffffffffffff

	// 부호 확장 and Reverse. java 의 big endien 을 reverse 하여 little endien 으로 변경
	// C++ 에서는 굳이 이렇게 할 필요가 없다.
	// |0 |0 |7|6|5|4|3|2| : i == 0 인 경우
	// |ff|ff|7|6|5|4|3|2| : i == -1 인 경우
	//for (int j = 7; j > 1; j--)
	//	i = (i << 8) | Code[Ip + j];
	i <<= 48;
	i = (long long)Code[Ip + 7] << 40 |
		(long long)Code[Ip + 6] << 32 |
		(long long)Code[Ip + 5] << 24 |
		(long long)Code[Ip + 4] << 16 |
		(long long)Code[Ip + 3] << 8 |
		(long long)Code[Ip + 2];
#else
	// |0|1|2|3|4|5|6|7|8|....|n| : Instruction region
	//  ^ Ip 의 위치
	if (Code[Ip + 1] < 0)
		i = -1; // 0xffffffffffffffff

	// 부호 확장 and Reverse. java 의 big endien 을 reverse 하여 little endien 으로 변경
	// C++ 에서는 굳이 이렇게 할 필요가 없다.
	// |0 |0 |7|6|5|4|3|2| : i == 0 인 경우
	// |ff|ff|7|6|5|4|3|2| : i == -1 인 경우
	i <<= 48;
	i = Code[Ip + 2] << 40 |
		Code[Ip + 3] << 32 |
		Code[Ip + 4] << 24 |
		Code[Ip + 5] << 16 |
		Code[Ip + 6] << 8 |
		Code[Ip + 7];
#endif

	return i;
}

void GCVM::Fetch()
{
	CurOpcode = Code[Ip];
	CurOperandRegion = Code[Ip + 1];
	if (CurOperandRegion != 0)
	{
		Operand = GetOperand();
	}
	Ip += 8; // 8byte 전진하여 다음 명령어 지정
}

void GCVM::Stop(string s)
{
	std::cout << s << endl;
	exit(1);
}



void GCVM::Interpret()
{
	for (;;)
	{
		Fetch(); // 추후 inline 고려
		switch (CurOpcode)
		{
		case (int)Opcode::HALT:
			Stop("Execution complete.");
			break;

		case int(Opcode::NOOP):
			break;

		case int(Opcode::ADD):
		{
			long long val1 = Pop();
			long long val2 = Pop();
			Push(val1 + val2);
			break;
		}

		case int(Opcode::PUSH):
		{
			long long val = Dereference(CurOperandRegion, Operand);
			Push(val);
			break;
		}

		case int(Opcode::POP):
		{
			long long val = Pop();
			Assign(Operand, val);
			break;
		}

		case int(Opcode::GOTO):
		{
			Ip = (int)Operand; // Operand 위치로 제어를 이동
			break;
		}

		case int(Opcode::BIF):
		{
			if (Pop() != 0)
				Ip = (int)Operand; // Operand 위치로 제어를 이동
			break;
		}

		case int(Opcode::CALL):
		{
			long long func;
			func = GetStack(Sp - STACK_STEP * 2 - (int)(STACK_STEP * Operand));
			if (func >= 0)
			{
				Push(Ip);
				Push(Bp);
				Bp = Sp;
				Ip = (int)func;
			}
			else if (func == -1)
			{
				DoPrint();
			}
			else
			{
				Stop(format_string("no CALL defined for  : %d", func));
			}
			break;
		}

		case int(Opcode::RETURN):
		{
			Sp = Bp;
			Bp = (int)Pop();
			Ip = (int)Pop();
			break;
		}

		case int(Opcode::LT):
		{
			Stop("LT not implemented yet.");
			break;
		}

		case int(Opcode::LE):
		{
			Stop("LE not implemented yet.");
			break;
		}

		default:
		{
			Stop(format_string("Illegal opcode %d", CurOpcode));
			break;
		}

		}
	}
}

long long GCVM::Dereference(int operandRegion, long long operand)
{
	switch (operandRegion)
	{
	case static_cast<int>(OpcodeRegion::R_ABS):
		return GetCode(operand); // Operand 값이 Code Offset 값일 때

	case static_cast<int>(OpcodeRegion::R_IMM):
		return operand; // Operand 값이 즉시값 일 때

	case static_cast<int>(OpcodeRegion::R_STACK):
		return GetStack(Bp + (int)operand);

	default:
	{
		Stop(format_string("Dereference failed. operand region : %d", operandRegion));
		break;
	}
	}
	return 0;
}

void GCVM::Assign(long long ad, long long val)
{
	switch (CurOperandRegion)
	{
	case static_cast<int>(OpcodeRegion::R_ABS):
		break;

	case static_cast<int>(OpcodeRegion::R_IMM):
		break;

	case static_cast<int>(OpcodeRegion::R_STACK):
		break;

	default:
		break;
	}
}

void GCVM::DoPrint()
{
	// Stack 에 있는 Argument 를 바탕으로 문자열 출력
	long long Addr = GetStack(Sp - STACK_STEP * 2);
	int8_t b = Code[(int)Addr++];
	while (b != 0)
	{
		std::cout << b;
		b = Code[(int)Addr++];
	}
	std::cout << endl;
}

