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

	// seekg�� �̿��� ���� ũ�� ����
	ifs.seekg(0, ifs.end);
	int length = (int)ifs.tellg();
	ifs.seekg(0, ifs.beg);


	Code = new int8_t[length];
	// read data as a block:
	ifs.read((char*)Code, length);

	// file format Ȯ��
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

	// 1 byte �� �����ϸ鼭 �Էµ� ���ڿ��� �ִ��� ��
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
		�����ڵ�
		0	PUSH main
		8	CALL 0
		16	HALT
	*/
	Ip = STACK_STEP * 2;
	Ip = FinStr = int(GetOperand() * STACK_STEP); // Operand �� Ip Offset �� �ִٰ� ������ �ڵ�
	Stack = new int8_t[STACK_STEP * 100000];
}

long long GCVM::GetOperand()
{
	long long i = 0; // 8byte ũ�⸦ ������ �Ѵ�.

#ifdef BIG_ENDIAN

	// |0|1|2|3|4|5|6|7|8|....|n| : Instruction region
	//  ^ Ip �� ��ġ
	if (Code[Ip + 7] < 0)
		i = -1; // 0xffffffffffffffff

	// ��ȣ Ȯ�� and Reverse. java �� big endien �� reverse �Ͽ� little endien ���� ����
	// C++ ������ ���� �̷��� �� �ʿ䰡 ����.
	// |0 |0 |7|6|5|4|3|2| : i == 0 �� ���
	// |ff|ff|7|6|5|4|3|2| : i == -1 �� ���
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
	//  ^ Ip �� ��ġ
	if (Code[Ip + 1] < 0)
		i = -1; // 0xffffffffffffffff

	// ��ȣ Ȯ�� and Reverse. java �� big endien �� reverse �Ͽ� little endien ���� ����
	// C++ ������ ���� �̷��� �� �ʿ䰡 ����.
	// |0 |0 |7|6|5|4|3|2| : i == 0 �� ���
	// |ff|ff|7|6|5|4|3|2| : i == -1 �� ���
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
	Ip += 8; // 8byte �����Ͽ� ���� ��ɾ� ����
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
		Fetch(); // ���� inline ���
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
			Ip = (int)Operand; // Operand ��ġ�� ��� �̵�
			break;
		}

		case int(Opcode::BIF):
		{
			if (Pop() != 0)
				Ip = (int)Operand; // Operand ��ġ�� ��� �̵�
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
		return GetCode(operand); // Operand ���� Code Offset ���� ��

	case static_cast<int>(OpcodeRegion::R_IMM):
		return operand; // Operand ���� ��ð� �� ��

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
	// Stack �� �ִ� Argument �� �������� ���ڿ� ���
	long long Addr = GetStack(Sp - STACK_STEP * 2);
	int8_t b = Code[(int)Addr++];
	while (b != 0)
	{
		std::cout << b;
		b = Code[(int)Addr++];
	}
	std::cout << endl;
}

