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

		// 4byte ũ���� ��ġ���� Value �� �⺻������ ����
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
		// ���� Value �� Pointer Ÿ������ ����
		bool IsPtr();

		void AddUser(Value* user);
		std::list<Value*>& GetUsers();


		virtual std::string toString();
	};

	// Values =========================================
	// Function �� �Ķ���ͷ� ���� Instruction �� ������ place holder
	class Argument : public Value
	{
	public:
		Argument() = delete;
		Argument(IRType* ty, std::string name);
		virtual ~Argument() = default;

		virtual std::string toString() override;
	};

	// ����� ����� ��� �͵�
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

	// ���� ������
	// global ������ ������ ������ �����Ǵ� �޸𸮿� ����ȴ�.
	// ���� type_ �� ������ ������ �����Ѵ�.
	class GlobalValue : public Constant
	{
	protected:
		// TODO : array, struct type �� ��� �ʱ�ȭ �� �Է�.
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

	// FunctionDecl �� Evaluation ����� ������ �Լ� ��ü.
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

	// ����
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

	// Cast : ir �� type casting �� ����
	// AST �ʹ� �ٸ��� ir �󿡼��� i32, f32, ptr Ÿ�Թۿ� ���� ������,
	// ir Ÿ�԰��� casting �� �����Ѵ�.
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

	// BinOp ����
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

	// �Լ� ȣ��
	// call retty, func_name arg1, arg2
	// ex) %3 = call i32, @print i32 %2, i32 %1
	class Call : public Inst {
	private:
		std::string func_name_; // @print ó�� '@' �� ���� �̸�
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

	// ���ÿ� �޸� �Ҵ�
	// Alloca �� ������ �Լ��� �ֻ�ܿ� ��ġ�ؾ� �Ѵ�. �׷��� ����� stack �Ҵ��� �ȴ�.
	// Return value : ������ ��
	class Alloca : public Inst {
	
	public:
		Alloca() = delete;
		Alloca(IRType* type, std::string name);

		virtual ~Alloca() = default;

		virtual std::string toString() override;
	};

	// �޸� �ּҿ� ���� �о��.
	// �����Ͱ��̳� �Ϲ� ���� ��ȯ
	// ���� : result = load retty, src
	// ex) %1 = load i32, ptr %0
	class Load : public Inst {
	private:
		Value* ptr_; // �޸� ��ġ

	public:
		Load() = delete;
		// ty : return type, ptr : ������
		Load(IRType* ty, Value* ptr);
		virtual ~Load() = default;

		void SetPtr(Value* ptr);
		Value* GetPtr();

		virtual std::string toString() override;
	};

	// �������Ϳ� �ִ� ���� �޸� �ּ���ġ�� ���
	// store �� ������ �����ΰ��� �������� �ʴ´�.	
	// ���� : store src, dest
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

	// Class �� Array �� �޸� ������ �߻�ȭ
	class GetElementPtr : public Inst {

	public:
		GetElementPtr() = default;
		virtual ~GetElementPtr() = default;

		virtual std::string toString() override;
	};

	// SSA Operation =======================

	// phi �Լ�
	class Phi : public Inst {
	public:
		Phi();
		virtual ~Phi() = default;
	};


	// Objects ========================================
	
	// Class ��ü�� IR �ܰ迡�� ��ü�� ���� �ϴ� ��ü�� �ƴ�.
	// ���� Function �� Variable �� ���踦 ǥ���ϴ� meta data ��.
	class Class
	{
	// TODO : Class ��ü��, Field, method ��� �߰�
	public:
		Class() = default;
		virtual ~Class() = default;


		virtual std::string toString();
	};
}
