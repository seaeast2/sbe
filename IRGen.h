#pragma once

#include "Visitor.h"
#include "IRType.h"

namespace gclang {
	class Scope;
	class Sema;

	class Type;
	class PrimitiveType;
	class ArrayType;
	class FunctionType;
	class ClassType;

	class Value;
	class GlobalValue;
	class Function;
	class Class;
	class BasicBlock;

	class ConstantTable;


	// AST �� Tree ������ IR ǥ������ ��ȯ
	class IRGen : public Visitor
	{
	protected:
		Sema* sema_;

		// � ������
		Scope* cur_scope_; // symbol table
		BasicBlock* cur_bb_;

		Function* cur_func_;
		int var_cnt_{ 0 };
		int label_cnt_{ 0 };

		// IR Ÿ�Ե�
		IRType* i32_;
		IRType* f32_;

		std::unordered_set<IRType*, std::hash<IRType*>, IRTypeEqual> ir_types_;

		// Values
		ConstantTable* const_table_;
		std::vector<GlobalValue*> gvals_;
		std::vector<Function*> funcs_;
		std::vector<Class*> classs_;

		// VariableNode �� ó�� �� ��. lvalue, rvalue ���θ� �����ϱ� ���� ����
		// TODO : �� ��� ���� �� ���� ����� �ִ���, ����� ����.
		bool is_lvalue_mode_{false};

		// ���� intrinsic �Լ��� ó���ϰ� �ִ��� ���� ó��.
		// TODO : �� ��� ���� �� ���� ����� �ִ���, ����� ����.
		bool is_intrinsic_{false};

	public:
		using Visitor::Visit;

		IRGen(Sema* sema);
		~IRGen();

		void Generate();

		ConstantTable* GetConstTable();
		std::vector<GlobalValue*>& GetGlobalValues();
		std::vector<Function*>& GetFuncs();
		std::vector<Class*>& GetClasses();

		// Decls
		//virtual std::any Visit(Decl* node) override;
		virtual std::any Visit(VarDecl* node) override;
		virtual std::any Visit(FunctionDecl* node) override;
		virtual std::any Visit(ClassDecl* node) override;
		//virtual std::any Visit(DeclListNode* node) override;

		// Exprs
		virtual std::any Visit(ExprNode* node) override;
		//virtual std::any Visit(TypeNode* node) override;
		//virtual std::any Visit(AbstractAssignNode* node) override;
		virtual std::any Visit(AssignNode* node) override;
		//virtual std::any Visit(OpAssignNode* node) override;
		//virtual std::any Visit(AddressNode* node) override;
		virtual std::any Visit(CastNode* node) override;
		virtual std::any Visit(BinaryOpNode* node) override;
		//virtual std::any Visit(LogicalOpNode* node) override;
		//virtual std::any Visit(CondExprNode* node) override;
		virtual std::any Visit(FuncCallNode* node) override;
		//virtual std::any Visit(LHSNode* node) override;
		//virtual std::any Visit(ArrayRefNode* node) override;
		//virtual std::any Visit(DereferenceNode* node) override;
		virtual std::any Visit(VariableNode* node) override;
		//virtual std::any Visit(MemberRefNode* node) override;
		//virtual std::any Visit(LiteralNode* node) override;
		virtual std::any Visit(IntegerLiteralNode* node) override;
		virtual std::any Visit(FloatLiteralNode* node) override;
		virtual std::any Visit(StringLiteralNode* node) override;
		virtual std::any Visit(BoolLiteralNode* node) override;
		virtual std::any Visit(NullLiteralNode* node) override;
		//virtual std::any Visit(EnumConstantNode* node) override;
		//virtual std::any Visit(UnaryOpNode* node) override;
		//virtual std::any Visit(UnaryArithmeticOpNode* node) override;
		//virtual std::any Visit(PrefixOpNode* node) override;
		//virtual std::any Visit(SuffixOpNode* node) override;
		//virtual std::any Visit(NewOpNode* node) override;
		//virtual std::any Visit(DeleteOpNode* node) override;
		virtual std::any Visit(ExprListNode* node) override;


		// StmtNode
		virtual std::any Visit(StmtNode* node) override;
		virtual std::any Visit(BlockNode* node) override;
		//virtual std::any Visit(SwitchNode* node) override;
		virtual std::any Visit(CaseNode* node) override;
		//virtual std::any Visit(BreakNode* node) override;
		//virtual std::any Visit(ContinueNode* node) override;
		virtual std::any Visit(ExprStmtNode* node) override;
		//virtual std::any Visit(ForNode* node) override;
		//virtual std::any Visit(GotoNode* node) override;
		virtual std::any Visit(IfNode* node) override;
		//virtual std::any Visit(LabelNode* node) override;
		virtual std::any Visit(ReturnNode* node) override;
		//virtual std::any Visit(WhileNode* node) override;
		//virtual std::any Visit(DoWhileNode* node) override;
		//virtual std::any Visit(StmtListNode* node) override;

	private:
		// AST Type => IR Type ��ȯ
		void GenerateIRTypes();
		IRType* ConvertType(Type* ty);
		IRType* ConvertType(PrimitiveType* pty);
		IRArrayType* ConvertType(ArrayType* arrty);
		IRFunctionType* ConvertType(FunctionType* fnty);
		IRStructType* ConvertType(ClassType* cty);

		// void name() {} ������ �� �Լ� ����
		Function* NewVoidFunction(std::string name);
		Function* FindFunction(std::string name);
		IRType* FindIRType(IRType* ty);

		// �ӽ� Variable 1�� ����
		//Variable* NewVariable(std::string name);
		std::string NewLabel(const std::string& name = "");
		BasicBlock* NewBasicBlock(const std::string& name = "");

		Value* GetResult(std::any& res, std::string error_msg);
		Value* GetResultNullable(std::any& res, std::string msg);

		// Value �� Constant int, float�� �� Constant �� loading �ϴ� �ڵ带 �߰���.
		// Const�� const �޸� ��ġ�� ����Ǿ� �ֱ� ����.
		Value* LoadConst(Value* v);
		// ���� �Լ��� ù��° BB �� Alloca �� ����
		void InsertAlloca(Value* alloca);
	};
}

