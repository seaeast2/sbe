#pragma once

namespace gclang
{
	class BasicBlock;
	class IRType;

	// ============================================
	class Value
	{
	protected:
		EIRKind kind_;
		IRType* type_;
		std::string name_;
		std::string org_name_;
		int id_{ -1 }; // instruction id. for compute lifetime interval in cfg.

		// 4byte 크기의 즉치값은 Value 에 기본적으로 내장
		int ivalue_;
		float fvalue_;
		intptr_t ptrvalue_{-1};

		std::list<Value*> users_;

	public:
		Value();
		virtual ~Value() = default;

		void SetKind(EIRKind kind);
		EIRKind GetKind();

		void SetType(IRType* type);
		IRType* GetType();

		void SetName(std::string name);
		virtual std::string GetName();
		std::string GetOrgName();

		void SetInt(int value);
		int GetInt();

		void SetFloat(float value);
		float GetFloat();

		void SetPtr(intptr_t ptr);
		intptr_t GetPtr();
		// 현재 Value 가 Pointer 타입인지 여부
		bool IsPtr();

		void AddUser(Value* user);
		std::list<Value*>& GetUsers();


		virtual std::string toString();
	};

	// Values =========================================
	// Function 의 파라메터로 들어올 Instruction 을 생성할 place holder
	class Argument : public Value
	{
	public:
		Argument() = delete;
		Argument(IRType* ty, std::string name);
		virtual ~Argument() = default;

		virtual std::string toString() override;
	};

	// 상수로 저장될 모든 것들
	class Constant : public Value
	{
		std::string strvalue_;
	public:
		Constant() = default;
		Constant(IRType* ty);
		Constant(IRType* ty, int ivalue);
		Constant(IRType* ty, float fvalue);
		//Constant(IRType* ty, intptr_t pvalue);
		Constant(IRType* ty, std::string svalue);
		virtual ~Constant() = default;

		void SetStr(std::string str);
		std::string GetStr();

		virtual std::string toString() override;
	};

	// 전역 변수들
	// global 변수는 별도의 영역에 생성되는 메모리에 저장된다.
	// 따라서 type_ 은 포인터 변수로 선언한다.
	class GlobalValue : public Constant
	{
	protected:
		// TODO : array, struct type 일 경우 초기화 값 입력.
		std::vector<Constant*> inits_;

	public:
		GlobalValue() = delete;
		GlobalValue(IRType* type, std::string name);
		virtual ~GlobalValue() = default;

		void AddInit(Constant* v);
		std::vector<Constant*>& GetInits();

		virtual std::string toString() override;

	private:
		void IterateArrayTy(IRType* arrty, int index);
	};

	// FunctionDecl 의 Evaluation 결과로 생성된 함수 객체.
	class Function : public Constant
	{
	private:
		std::vector<Argument*> params_;
		std::vector<BasicBlock*> bbs_;

		bool intrinsic_;
	public:
		Function() = delete;
		Function(IRType* type, std::string name);
		virtual ~Function() = default;

		void AddParam(Argument* param);
		std::vector<Argument*>& GetParam();

		void AddBB(BasicBlock* bb);
		std::vector<BasicBlock*>& GetBBs();
		BasicBlock* GetEntryBB();

		void SetIntrinsic(bool intrinsic);
		bool IsIntrinsic();

		virtual std::string toString() override;
	};

	// Instructions ===================================
	class Inst : public Value
	{
	protected:
		
		BasicBlock* bb_{ nullptr };

	public:

		Inst();
		virtual ~Inst() = default;

		void SetID(int id);
		int GetID();

		void SetBB(BasicBlock* bb);
		BasicBlock* GetBB();

		virtual std::string toString() override;
	};

	// 점프
	class Branch : public Inst {
	private:
		Value* cond_{nullptr};
		BasicBlock* then_label_{nullptr};
		BasicBlock* else_label_{nullptr};

	public:
		Branch() = delete;
		Branch(Value* cond, BasicBlock* then_label, BasicBlock* else_label);
		virtual ~Branch() = default;

		void SetCond(Value* cond);
		Value* GetCond();

		void SetThenLabel(BasicBlock* then_label);
		BasicBlock* GetThenLabel();

		void SetElseLabel(BasicBlock* else_label);
		BasicBlock* GetElseLabel();

		virtual std::string toString() override;
	};

	class Return : public Inst {
	private:
		Value* retval_;

	public:
		Return() = delete;
		Return(Value* retval);
		virtual ~Return() = default;

		void SetRetVal(Value* retval);
		Value* GetRetVal();

		virtual std::string toString() override;
	};

	class Switch : public Inst {
	public:
		Switch();
		virtual ~Switch() = default;
	};

	class Case : public Inst {
	public:
		Case();
		virtual ~Case() = default;
	};

	// Cast : ir 의 type casting 를 수행
	// AST 와는 다르게 ir 상에서는 i32, f32, ptr 타입밖에 없기 때문에,
	// ir 타입간의 casting 만 지원한다.
	// cast castty, src
	// ex) %3 = cast f32, %2
	class Cast : public Inst {
	private:
		Value* src_{nullptr};

	public:
		Cast() = delete;
		Cast(IRType* castty, Value* src);
		virtual ~Cast() = default;

		void SetSrc(Value* src);
		Value* GetSrc();

		virtual std::string toString() override;
	};

	// BinOp 구현
	// add retty lhs, rhs
	// ex) %3 = add i32 %2, %1
	class BinOp : public Inst {
	private:
		EBinOpKind op_;

		Value* lhs_;
		Value* rhs_;

	public:
		BinOp() = delete;
		BinOp(IRType* retty, EBinOpKind op, Value* lhs, Value* rhs);
		virtual ~BinOp() = default;

		void SetOp(EBinOpKind op);
		EBinOpKind GetOp();

		void SetLHS(Value* lhs);
		Value* GetLHS();

		void SetRHS(Value* rhs);
		Value* GetRHS();

		virtual std::string toString() override;
	};

	// 함수 호출
	// call retty, func_name arg1, arg2
	// ex) %3 = call i32, @print i32 %2, i32 %1
	class Call : public Inst {
	private:
		std::string func_name_; // @print 처럼 '@' 이 붙은 이름
		std::vector<Value*> args_;		
	public:
		Call() = delete;
		Call(IRType* retty, std::string func_name);
		virtual ~Call() = default;

		void SetFuncName(std::string func_name);
		std::string GetFuncName();

		void AddArg(Value* arg);
		std::vector<Value*>& GetArgs();

		virtual std::string toString() override;
	};

	// == Memory Operation =====================

	// 스택에 메모리 할당
	// Alloca 는 무조건 함수의 최상단에 위치해야 한다. 그래야 제대로 stack 할당이 된다.
	// Return value : 포인터 형
	class Alloca : public Inst {
	
	public:
		Alloca() = delete;
		Alloca(IRType* type, std::string name);

		virtual ~Alloca() = default;

		virtual std::string toString() override;
	};

	// 메모리 주소에 값을 읽어옴.
	// 포인터값이나 일반 값을 반환
	// 형태 : result = load retty, src
	// ex) %1 = load i32, ptr %0
	class Load : public Inst {
	private:
		Value* ptr_; // 메모리 위치

	public:
		Load() = delete;
		// ty : return type, ptr : 포인터
		Load(IRType* ty, Value* ptr);
		virtual ~Load() = default;

		void SetPtr(Value* ptr);
		Value* GetPtr();

		virtual std::string toString() override;
	};

	// 레지스터에 있는 값을 메모리 주소위치에 기록
	// store 는 스스로 무엇인가를 저장하지 않는다.	
	// 형태 : store src, dest
	// ex) store i32 %0, ptr %2
	class Store : public Inst {
	private:
		Value* src_{nullptr};
		Value* dest_{nullptr};

	public:
		Store() = delete;
		Store(Value* src, Value* dest);
		virtual ~Store() = default;

		void SetSrc(Value* src);
		Value* GetSrc();

		void SetDest(Value* dest);
		Value* GetDest();

		virtual std::string toString() override;
	};

	// Class 나 Array 의 메모리 접근의 추상화
	class GetElementPtr : public Inst {

	public:
		GetElementPtr() = default;
		virtual ~GetElementPtr() = default;

		virtual std::string toString() override;
	};

	// SSA Operation =======================

	// phi 함수
	class Phi : public Inst {
	public:
		Phi();
		virtual ~Phi() = default;
	};


	// Objects ========================================
	
	// Class 객체는 IR 단계에서 실체가 존재 하는 객체가 아님.
	// 단지 Function 과 Variable 의 관계를 표시하는 meta data 임.
	class Class
	{
	// TODO : Class 객체에, Field, method 목록 추가
	public:
		Class() = default;
		virtual ~Class() = default;


		virtual std::string toString();
	};
}
