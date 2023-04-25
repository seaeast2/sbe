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


	// AST 를 Tree 형식의 IR 표현으로 변환
	class IRGen : public Visitor
	{
	protected:
		Sema* sema_;

		// 운영 변수들
		Scope* cur_scope_; // symbol table
		BasicBlock* cur_bb_;

		Function* cur_func_;
		int var_cnt_{ 0 };
		int label_cnt_{ 0 };

		// IR 타입들
		IRType* i32_;
		IRType* f32_;

		std::unordered_set<IRType*, std::hash<IRType*>, IRTypeEqual> ir_types_;

		// Values
		ConstantTable* const_table_;
		std::vector<GlobalValue*> gvals_;
		std::vector<Function*> funcs_;
		std::vector<Class*> classs_;

		// VariableNode 를 처리 할 때. lvalue, rvalue 여부를 구분하기 위한 변수
		// TODO : 이 방법 말고 더 좋은 방법이 있는지, 고민해 보자.
		bool is_lvalue_mode_{false};

		// 현재 intrinsic 함수를 처리하고 있는지 여부 처리.
		// TODO : 이 방법 말고 더 좋은 방법이 있는지, 고민해 보자.
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
		// AST Type => IR Type 변환
		void GenerateIRTypes();
		IRType* ConvertType(Type* ty);
		IRType* ConvertType(PrimitiveType* pty);
		IRArrayType* ConvertType(ArrayType* arrty);
		IRFunctionType* ConvertType(FunctionType* fnty);
		IRStructType* ConvertType(ClassType* cty);

		// void name() {} 형식의 빈 함수 생성
		Function* NewVoidFunction(std::string name);
		Function* FindFunction(std::string name);
		IRType* FindIRType(IRType* ty);

		// 임시 Variable 1개 생성
		//Variable* NewVariable(std::string name);
		std::string NewLabel(const std::string& name = "");
		BasicBlock* NewBasicBlock(const std::string& name = "");

		Value* GetResult(std::any& res, std::string error_msg);
		Value* GetResultNullable(std::any& res, std::string msg);

		// Value 가 Constant int, float일 때 Constant 를 loading 하는 코드를 추가함.
		// Const는 const 메모리 위치에 저장되어 있기 때문.
		Value* LoadConst(Value* v);
		// 현재 함수의 첫번째 BB 에 Alloca 를 삽입
		void InsertAlloca(Value* alloca);
	};
}

