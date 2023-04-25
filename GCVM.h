#pragma once
#define BIG_ENDIAN
#define STACK_STEP 8

namespace gclang
{
	enum class Opcode : int
	{
		HALT = 0,
		NOOP,
		ADD,
		SUB,
		MUL,
		DIV,
		MOD,
		NEG,
		PUSH,
		POP,
		CALL,
		RETURN,
		GOTO,
		BIF,
		LT,
		LE,
		GT,
		GE,
		EQ,
		NEQ,
		LOCAL,
		LOAD,
		STORE
	};

	enum class OpcodeKind : int
	{
		LABEL = 101,
		STRING,
		CODE,
		PROC,
		GLOBAL,
		END
	};

	enum class OpcodeRegion : int
	{
		R_NONE = 0,
		R_ABS,
		R_IMM,
		R_STACK,
		R_HEAP
	};

	class GCVM
	{
	private:
		// Jzero Instruction 구조. 
		// |<---------- 64bit ---------->|
		// |opcode|operand region|operand|
		// | 8bit |    8bit      | 48bit |

		int8_t* Code, // Instruction region
			* Stack; // Stack region
	//vector<int8_t> CodeBuf, StackBuf;
		int Ip, // Instruction pointer
			Sp, // Stack pointer
			Bp, // Base pointer
			Hp, // Heap pointer?
			CurOpcode, // Opcode
			CurOperandRegion, // Operand region
			FinStr;

		long long Operand;

	public:
		GCVM();
		~GCVM();

		bool LoadBytecode(std::string filename);
		// bytecode 내에서 특정 문자열 찾기
		int Find(const char* needle, const char* haystack);

		void Init(std::string filename);

		// 48bit Operand 부분 가져오기
		inline long long GetOperand();
		// 다음 명령어로 이동
		void Fetch();

		void Stop(std::string s);

		void Interpret();

		inline long long Pop()
		{
			Sp -= STACK_STEP; // 먼저 Sp 를 이동 시키고 작업
#ifdef BIG_ENDIAN
			long long ret =
				(long long)Stack[Sp + 7] << 56 |
				(long long)Stack[Sp + 6] << 48 |
				(long long)Stack[Sp + 5] << 40 |
				(long long)Stack[Sp + 4] << 32 |
				(long long)Stack[Sp + 3] << 24 |
				(long long)Stack[Sp + 2] << 16 |
				(long long)Stack[Sp + 1] << 8 |
				(long long)Stack[Sp];
#else
			long long ret =
				(long long)Stack[Sp] << 56 |
				(long long)Stack[Sp + 1] << 48 |
				(long long)Stack[Sp + 2] << 40 |
				(long long)Stack[Sp + 3] << 32 |
				(long long)Stack[Sp + 4] << 24 |
				(long long)Stack[Sp + 5] << 16 |
				(long long)Stack[Sp + 6] << 8 |
				(long long)Stack[Sp + 7];
#endif



			return ret;
		};

		inline void Push(long long val)
		{
#ifdef BIG_ENDIAN
			Stack[Sp + 7] = (val & 0xff00000000000000) >> 56;
			Stack[Sp + 6] = (val & 0x00ff000000000000) >> 48;
			Stack[Sp + 5] = (val & 0x0000ff0000000000) >> 40;
			Stack[Sp + 4] = (val & 0x000000ff00000000) >> 32;
			Stack[Sp + 3] = (val & 0x00000000ff000000) >> 24;
			Stack[Sp + 2] = (val & 0x0000000000ff0000) >> 16;
			Stack[Sp + 1] = (val & 0x000000000000ff00) >> 8;
			Stack[Sp] = (val & 0x00000000000000ff);
#else
			Stack[Sp] = (val & 0xff00000000000000) >> 56;
			Stack[Sp + 1] = (val & 0x00ff000000000000) >> 48;
			Stack[Sp + 2] = (val & 0x0000ff0000000000) >> 40;
			Stack[Sp + 3] = (val & 0x000000ff00000000) >> 32;
			Stack[Sp + 4] = (val & 0x00000000ff000000) >> 24;
			Stack[Sp + 5] = (val & 0x0000000000ff0000) >> 16;
			Stack[Sp + 6] = (val & 0x000000000000ff00) >> 8;
			Stack[Sp + 7] = (val & 0x00000000000000ff);
#endif
			Sp += STACK_STEP;
		};

		inline long long Dereference(int operandRegion, long long operand);

		// 미구현. 특정 메모리 위치에 value 저장
		inline void Assign(long long ad, long long val);

		// 해당 위치의 Instruction 을 얻어온다.
		inline long long GetCode(int pos)
		{
#ifdef BIG_ENDIAN
			return
				(long long)Code[pos + 7] << 56 |
				(long long)Code[pos + 6] << 48 |
				(long long)Code[pos + 5] << 40 |
				(long long)Code[pos + 4] << 32 |
				(long long)Code[pos + 3] << 24 |
				(long long)Code[pos + 2] << 16 |
				(long long)Code[pos + 1] << 8 |
				(long long)Code[pos];
#else
			return
				(long long)Code[pos] << 56 |
				(long long)Code[pos + 1] << 48 |
				(long long)Code[pos + 2] << 40 |
				(long long)Code[pos + 3] << 32 |
				(long long)Code[pos + 4] << 24 |
				(long long)Code[pos + 5] << 16 |
				(long long)Code[pos + 6] << 8 |
				(long long)Code[pos + 7];
#endif
		};

		inline long long GetStack(int pos)
		{
#ifdef BIG_ENDIAN
			return
				(long long)Stack[pos + 7] << 56 |
				(long long)Stack[pos + 6] << 48 |
				(long long)Stack[pos + 5] << 40 |
				(long long)Stack[pos + 4] << 32 |
				(long long)Stack[pos + 3] << 24 |
				(long long)Stack[pos + 2] << 16 |
				(long long)Stack[pos + 1] << 8 |
				(long long)Stack[pos];
#else
			return
				(long long)Stack[pos] << 56 |
				(long long)Stack[pos + 1] << 48 |
				(long long)Stack[pos + 2] << 40 |
				(long long)Stack[pos + 3] << 32 |
				(long long)Stack[pos + 4] << 24 |
				(long long)Stack[pos + 5] << 16 |
				(long long)Stack[pos + 6] << 8 |
				(long long)Stack[pos + 7];
#endif
		};

		void DoPrint();

	};
}


