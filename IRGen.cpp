#include "stdafx.h"

#include "ASTContext.h"
#include "Sema.h"

#include "Scope.h"
#include "BasicBlock.h"
#include "IR.h"
#include "ValueEvaluator.h"
#include "ConstantTable.h"
#include "IRGen.h"


using namespace gclang;

#define LINKBB(PRE, SUCC) PRE->AddSucc(SUCC); SUCC->AddPred(PRE)

IRGen::IRGen(Sema* sema)
{
#ifdef _DEBUG_PRINT_PREFIX
	DEBUG_SETPREFIX("(IRGen)");
#endif
	sema_ = sema;

	cur_scope_ = sema_->top_level_scope_;
	cur_bb_ = nullptr;

	i32_ = new IRType(IRType::EIRType::eI32);
	f32_ = new IRType(IRType::EIRType::eF32);

	const_table_ = new ConstantTable();
}

IRGen::~IRGen()
{
	delete i32_;
	delete f32_;

	delete const_table_;
}

void IRGen::Generate()
{
#ifdef _DEBUG_PRINT_IRGEN
	DEBUG_PRINT("IRGen::Generate()");
#endif

	GenerateIRTypes();

	// ���� ���� �ʱ�ȭ �Լ� �߰�
	cur_func_ = NewVoidFunction("global_var_initializer");
	cur_bb_ = cur_func_->GetBBs()[0];

	auto& gvars = sema_->context_->GetVarDecls();
	auto& gfunc = sema_->context_->GetFunctionDecls();
	auto& clsss = sema_->context_->GetClassDecls();

	// ���������� ó��
	for (auto gvar : gvars)
	{
		gvar->Accept(this);
	}

	for (auto func : gfunc)
	{
		func->Accept(this); // �Լ� �������� �ذ�
	}

	for (auto clss : clsss)
	{
		clss->Accept(this);
	}
}

ConstantTable* gclang::IRGen::GetConstTable()
{
	return const_table_;
}

std::vector<GlobalValue*>& gclang::IRGen::GetGlobalValues()
{
	return gvals_;
}

std::vector<Function*>& gclang::IRGen::GetFuncs()
{
	return funcs_;
}

std::vector<Class*>& gclang::IRGen::GetClasses()
{
	return classs_;
}



std::any gclang::IRGen::Visit(VarDecl* node)
{
#ifdef _DEBUG_PRINT_IRGEN
	DEBUG_PRINT("IRGen::Visit(VarDecl*)");
#endif
	// TODO : VarDecl ó��
		// ���� ���Դٴ� ���� ���� Ȥ�� ���� ���� �����̴�.
		// ���������� �������� �޸� ������ �ʱ�ȭ ���� �Բ� �����Ѵ�.
		// ���������� �Լ� ���۽ÿ� alloca �� �޸𸮸� �Ҵ��ϰ�, assign ���� �ʱ�ȭ�� �Ѵ�.
	std::any result;
	ValueEvaluator ve;

	Type* ty = node->GetTypeNode()->GetType();
	IRType* irty = ConvertType(ty);
	if (ty->IsBoolTy() || ty->IsIntTy() || ty->IsFloatTy() || ty->IsStringTy())
	{
		if (cur_scope_->IsGlobal())
		{// ���� ���� ������ ��� ó��
			GlobalValue* gv = new GlobalValue(irty, node->GetName());

			bool eval_failed = false;
			if (ty->IsBoolTy() || ty->IsIntTy())
			{ // �������� ��� ó��
				int eval_ivalue = 0;
				gv->SetInt(eval_ivalue);
				if (ve.EvaluateInt(node->GetInitializer(), eval_ivalue) == EResult::eSuccess)
					gv->SetInt(eval_ivalue);
				else
					eval_failed = true;
			}
			else if (ty->IsFloatTy())
			{ // �ε��Ҽ������� ��� ó��
				float eval_fvalue = 0.0f;
				gv->SetFloat(eval_fvalue);
				if (ve.EvaluateFloat(node->GetInitializer(), eval_fvalue) == EResult::eSuccess)
					gv->SetInt(eval_fvalue);
				else
					eval_failed = true;
			}
			else if (ty->IsStringTy())
			{
				eval_failed = true;
			}
			
			if (eval_failed)
			{
				// ���� ������ Initializer �� expr �� ������ �� ó��
				result = Visit(node->GetInitializer());
				Value* src = GetResult(result, "invalid global variable initailizer");
				if (ty->IsStringTy())
				{	// �ӽ� string Ÿ���� ���� type���� ����
					gv->SetType(src->GetType());
				}

				// store i32 src, ptr @c
				Store* irstore = new Store( src, gv);
				cur_bb_->AddInst(irstore);
			}
			
			
			cur_scope_->AddValue(node->GetName(), gv);// symbol table �� ����
			gvals_.push_back(gv);
		}
		else
		{ // �������� ������ ��� ó��
			if (node->IsParam())
			{
				// ���������� �Լ��� �Ķ������ ���, Argument �� 
				// ���� ����� ���� Visit(FunctionDecl) �� �ѱ�.
				/*if (ty->IsStringTy() || ty->IsClassTy())
					irty = new IRType(IRType::EIRType::ePTR, irty);*/

				Argument* arg = new Argument(irty, node->GetName());
				
				if (!is_intrinsic_)
				{ // intrinsic �Լ��� �ƴҶ��� ����
					Alloca* iralloca = new Alloca(irty, node->GetName());
					InsertAlloca(iralloca);
					cur_scope_->AddValue(node->GetName(), iralloca); // symbol table �� ����

					Store* irstore = new Store(arg, iralloca);
					cur_bb_->AddInst(irstore);
				}

				result = static_cast<Value*>(arg);
			}
			else
			{// ��������  ������ ��� ó��
				// Alloca �� �޸� �Ҵ�
				Alloca* iralloca = new Alloca(irty, node->GetName());
				InsertAlloca(iralloca);
				cur_scope_->AddValue(node->GetName(), iralloca); // symbol table �� ����
				
				if (node->GetInitializer())
				{// �ʱ�ȭ ���� �ִ� ���
					result = Visit(node->GetInitializer());
					Value* src = GetResult(result, "invalid global variable initailizer");
					if (ty->IsStringTy())
					{	// �ӽ� string Ÿ���� ���� type���� ����
						iralloca->SetType(src->GetType());
					}

					// store i32 src, ptr @c
					Store* irstore = new Store(src, iralloca);
					cur_bb_->AddInst(irstore);
				}
			}
		}
	}
	else if (ty->IsArrayTy())
	{
		// TODO : �迭 ���� ó��
	}
	else if (ty->IsClassTy())
	{
		// TODO : class ���� ó��
	}

	return result;
}

std::any gclang::IRGen::Visit(FunctionDecl* node)
{
#ifdef _DEBUG_PRINT_IRGEN
	DEBUG_PRINT("IRGen::Visit(FunctionDecl*)");
#endif
	std::any result;

	// ���� Scope ����
	cur_scope_ = node->GetScope();

	// 1. IRFunctionType ����.
	IRType* irfty = ConvertType(node->GetTypeNode()->GetType());
	if (irfty->GetETypeKind() != IRType::EIRTypeKind::eIRFunctionType)
		assert(0 && "invalid function type");
	
	// 2. Function ��ü ����
	Function* irfn = new Function(irfty, node->GetName());
	irfn->SetIntrinsic(node->IsIntrinsic());
	funcs_.push_back(irfn); // �Լ� ��Ͽ� �߰�
	cur_func_ = irfn;// ���� BB, Function ����
	is_intrinsic_ = cur_func_->IsIntrinsic();

	if (!is_intrinsic_) // intrinsic �Լ��� ���� BB �� �߰����� �ʴ´�.
		cur_bb_ = NewBasicBlock("entry");
		
	// 3. Emit code
	// Parameter ó��
	auto& params = node->GetParams();
	for (auto param : params)
	{
		if (param)
		{
			result = Visit(param);
			Value* arg = GetResult(result, "invalid argument");
			irfn->AddParam(static_cast<Argument*>(arg));
		}
	}
	if (node->GetBody())
	{
		result = Visit(node->GetBody());
	}

	return std::any();
}

std::any gclang::IRGen::Visit(ClassDecl* node)
{
	cur_scope_ = node->GetScope();
	return std::any();
}

std::any gclang::IRGen::Visit(ExprNode* node)
{
	return node->Accept(this);
}

std::any gclang::IRGen::Visit(AssignNode* node)
{
	// IRGen AssignNode ����
	std::any result;
	Value *lhs = nullptr, *rhs = nullptr;

	if (node->GetLHS())
	{
		is_lvalue_mode_ = true;
		result = Visit(node->GetLHS());
		lhs = GetResult(result, "invalid lvalue");
	}

	if (node->GetRHS())
	{
		is_lvalue_mode_ = false;
		result = Visit(node->GetRHS());
		rhs = GetResult(result, "invalid rvalue");
	}

	Store* store = new Store(rhs, lhs);
	cur_bb_->AddInst(store);

	result = (Value*)store;

	return result;
}

std::any gclang::IRGen::Visit(CastNode* node)
{
	std::any result;
	if (node->GetExpr())
	{
		// TODO : CastNode => IR Cast ��ȯ
		Type* castty = node->GetCastType()->GetType();

		IRType* ircastty = nullptr;
		if (castty->IsBoolTy() ||
			castty->IsIntTy())
		{ // I32 �� ��ȯ
			ircastty = new IRType(IRType::EIRType::eI32);
		}
		else if (castty->IsFloatTy())
		{ // F32 �� ��ȯ
			ircastty = new IRType(IRType::EIRType::eF32);
		}
		else if (castty->IsArrayTy() ||	castty->IsClassTy() || castty->IsStringTy())
		{// PTR �� ��ȯ
			ircastty = ConvertType(castty);
		}

		result = Visit(node->GetExpr());
		Value* src = GetResult(result, "invalid cast expr");

		Cast* ircast = new Cast(ircastty, src);
		cur_bb_->AddInst(ircast);

		result = (Value*)ircast;
	}
		

	return result;
}

std::any gclang::IRGen::Visit(BinaryOpNode* node)
{
	// Binary operation ó��
	std::any result;

	result = Visit(node->GetLHS());
	Value* lhs = GetResult(result, "error for binary operator");
	lhs = LoadConst(lhs);

	result = Visit(node->GetRHS());
	Value* rhs = GetResult(result, "error for binary operator");
	rhs = LoadConst(rhs);

	// TODO : logical operation �� ��� BB �� �ѷ� ����� ó���ϵ���.
	// ��ü���� ����� llvm �� bitcode ������� ���� �ϵ��� �Ѵ�.

	BinOp* irbinary = new BinOp(i32_, node->GetOp(), lhs, rhs);
	cur_bb_->AddInst(irbinary);

	result = (Value*)irbinary;

	return result;
}

std::any gclang::IRGen::Visit(FuncCallNode* node)
{
	// TODO : �Լ� ȣ�� IR ��ȯ.
	std::any result; 
	IRType* irretty = ConvertType(node->GetFuncDecl()->GetReturnTy()->GetType());
	Call* call = new Call(irretty, node->GetFuncDecl()->GetName());

	auto args = node->GetArgs()->GetExprs();
	for (auto arg : args)
	{
		result = Visit(arg);
		Value* varg = GetResult(result, "invalid function argument");
		varg = LoadConst(varg);
		call->AddArg(varg);
	}
	cur_bb_->AddInst(call);

	result = (Value*)call;

	return result;
}

std::any gclang::IRGen::Visit(VariableNode* node)
{
	// TODO : VariableNode �� lvalue, rvalue ����
	std::any result;

	// Symbol table ���� ������ �ּҸ� ��� �´�.
	Value* val = cur_scope_->FindValue(node->GetName());
	if (!val)
		assert(0 && "post declaration is not allowed.");
	result = val;
	
	if (!is_lvalue_mode_)
	{ // VariableNode �� rvalue�� ��쿡�� load �� �ּҿ��� ���� �о� �´�.
		Load* load_var = new Load(i32_, val);
		cur_bb_->AddInst(load_var);
		ir_types_.insert(load_var->GetType());

		result = (Value*)load_var;
	}
	return result;
}

std::any gclang::IRGen::Visit(IntegerLiteralNode* node)
{
	std::any result;
	
	Constant* irint = const_table_->FindIntConst(node->GetValue());
	if (!irint)
	{
		irint = new Constant(i32_, node->GetValue());
		const_table_->AddInt(irint);
	}
	result = static_cast<Value*>(irint);

	return result;
}

std::any gclang::IRGen::Visit(FloatLiteralNode* node)
{
	std::any result;
	Constant* irfloat = const_table_->FindFloatConst(node->GetValue());
	if (!irfloat)
	{
		irfloat = new Constant(f32_, node->GetValue());
		const_table_->AddFloat(irfloat);
	}
	result = static_cast<Value*>(irfloat);
	return result;
}

std::any gclang::IRGen::Visit(StringLiteralNode* node)
{
	std::any result;
	// TODO : string litral�����ϱ�. string �� �迭�� ���� �� �� �ִ�.
	Constant* irstr = const_table_->FindStrConst(node->GetValue());
	if (!irstr)
	{
		IRType* irstrty = new IRArrayType(i32_, node->GetValue().size());
		IRType* found = FindIRType(irstrty);
		if (found)
		{
			delete irstrty;
			irstrty = found;
		}
		static_cast<IRArrayType*>(irstrty)->SetIsString(true);
		irstr = new Constant(irstrty, node->GetValue());
		const_table_->AddStr(irstr);
	}

	result = static_cast<Value*>(irstr);
	return result;
}

std::any gclang::IRGen::Visit(BoolLiteralNode* node)
{
	std::any result;
	Constant* irint = const_table_->FindIntConst((int)node->GetValue());
	if (!irint)
	{
		irint = new Constant(i32_, (int)node->GetValue());
		const_table_->AddInt(irint);
	}
	result = static_cast<Value*>(irint);
	return result;
}

std::any gclang::IRGen::Visit(NullLiteralNode* node)
{
	std::any result;
	Constant* irint = const_table_->FindIntConst(0);
	if (!irint)
	{
		irint = new Constant(i32_, 0);
		const_table_->AddInt(irint);
	}
	result = static_cast<Value*>(irint);
	return result;
}

std::any gclang::IRGen::Visit(ExprListNode* node)
{
	// TODO : ArrayInitializer ó��
	Visit(node);
	return std::any();
}

std::any gclang::IRGen::Visit(StmtNode* node)
{
	return node->Accept(this);
}

std::any gclang::IRGen::Visit(BlockNode* node)
{
	cur_scope_ = node->GetScope();
	Visitor::Visit(node);

	return std::any();
}

std::any gclang::IRGen::Visit(CaseNode* node)
{
	// TODO : IRGen::Visit(CaseNode* node) �����ϱ�
	cur_scope_ = node->GetScope();
	Visitor::Visit(node);
	
	return std::any();
}

std::any gclang::IRGen::Visit(ExprStmtNode* node)
{
	if (node->GetExpr())
		return Visit(node->GetExpr());
	
	return std::any();
}

std::any gclang::IRGen::Visit(IfNode* node)
{
	// TODO : IRGen::Visit(IfNode* node) �����ϱ�
	std::any result;

	// 1. condition bb ����
	BasicBlock* cond_bb = NewBasicBlock("ifcond");
	LINKBB(cur_bb_, cond_bb);
	cur_bb_ = cond_bb;
	result = Visit(node->GetCond());
	Value* cond = GetResult(result, "invalid if condition");

	// 2. then bb ����
	BasicBlock* then_bb = NewBasicBlock("ifthen");
	LINKBB(cond_bb, then_bb);
	cur_bb_ = then_bb;
	Visit(node->GetThenBody());
	BasicBlock* out_then_bb = cur_bb_;

	// 3. else bb ����
	BasicBlock* else_bb = NewBasicBlock("ifelse");
	LINKBB(cond_bb, else_bb);
	cur_bb_ = else_bb;
	Visit(node->GetElseBody());
	BasicBlock* out_else_bb = cur_bb_;

	// 4. exit bb ����
	BasicBlock* exit_bb = NewBasicBlock("ifexit");
	LINKBB(out_then_bb, exit_bb);
	LINKBB(out_else_bb, exit_bb);
	cur_bb_ = exit_bb;

	// 5. ���ǰ˻� bb�� Branch �߰�
	Branch* br = new Branch(cond, then_bb, else_bb);
	cond_bb->AddInst(br);

	// 6. then_bb �������� jump ó��
	Inst* last = then_bb->GetInsts().back();
	if (last && 
		(last->GetKind() == EIRKind::eBranch ||
		last->GetKind() == EIRKind::eReturn))
		return std::any();
	br = new Branch(nullptr, exit_bb, nullptr); // ������ branch
	then_bb->AddInst(br);
	
	return std::any();
}

std::any gclang::IRGen::Visit(ReturnNode* node)
{
	std::any result; 
	result = Visit(node->GetExpr());
	Value* retval = GetResultNullable(result, "missing return value.");
	Return* retinst = new Return(retval);
	cur_bb_->AddInst(retinst);

	return std::any();
}

//Variable* gclang::IRGen::NewVariable(std::string name)
//{
//	// symbol table
//	//cur_scope_
//	std::string temp_name = "t" + std::to_string(var_cnt_++);
//	Variable* v = name.empty() ? new Variable(temp_name) : new Variable(name);
//	cur_scope_->AddVar(v);
//	
//	return v;
//}

void gclang::IRGen::GenerateIRTypes()
{
	// TODO : AST Type => IR Type ��ȯ
}

IRType* gclang::IRGen::ConvertType(Type* ty)
{
	switch (ty->GetKind())
	{
	case ETypeKind::ePrimitiveType:
		return ConvertType(static_cast<PrimitiveType*>(ty));

	case ETypeKind::eClassType:
		return ConvertType(static_cast<ClassType*>(ty));

	case ETypeKind::eFunctionType:
		return ConvertType(static_cast<FunctionType*>(ty));

	case ETypeKind::eArrayType:
		return ConvertType(static_cast<ArrayType*>(ty));
	}

	assert(0 && "invalid type");
	return nullptr;
}

IRType* gclang::IRGen::ConvertType(PrimitiveType* ty)
{
	if (ty->IsBoolTy() || ty->IsIntTy())
		return i32_;

	if (ty->IsFloatTy())
		return f32_;

	if (ty->IsStringTy())
	{
		// TODO : ���߿� StringType �� ����� �ٽ� ���� ���
		// string type �� IRType ��ȯ.
		// ���� AST �� StringType �� ���� ������ ���⼭�� 0ũ���� �ӽ� �迭�� �����ϰ�,
		// VarDecl ���� initializer ���� ũ�⸦ Ȯ���Ͽ� IRArrayType �� ũ�⸦ Ȯ���Ѵ�.
		IRType* irstrty = new IRArrayType(i32_, 0); // �ӽð� 
		IRType* found = FindIRType(irstrty);
		if (found)
		{
			delete irstrty;
			irstrty = found;
		}
		static_cast<IRArrayType*>(irstrty)->SetIsString(true);
		return irstrty;
	}

	return nullptr;
}

IRArrayType* gclang::IRGen::ConvertType(ArrayType* arrty)
{
	IRType* irty = ConvertType(arrty->GetBaseType());
	IRArrayType* irarrty = new IRArrayType(irty, arrty->GetLength());

	IRType* found = FindIRType(irarrty);
	if (found)
	{ // �ߺ��� ���
		delete irarrty;
		return static_cast<IRArrayType*>(found);
	}
	ir_types_.insert(irarrty);

	return irarrty;
}

IRFunctionType* gclang::IRGen::ConvertType(FunctionType* fnty)
{
	// FunctionType => IRFunctionType ��ȯ
	IRType* irretty = ConvertType(fnty->GetReturnTy());
	IRFunctionType* irfuncty = new IRFunctionType(irretty);
	auto params = fnty->GetParams();
	for (auto param : params)
		irfuncty->AddParam(ConvertType(param));
	irfuncty->UpdateName();


	IRType* found = FindIRType(irfuncty);
	if (found)
	{ // �ߺ��� ���
		delete irfuncty;
		return static_cast<IRFunctionType*>(found);
	}
	ir_types_.insert(irfuncty);

	return irfuncty;
}

IRStructType* gclang::IRGen::ConvertType(ClassType* cty)
{
	IRStructType* irstty = new IRStructType(cty->GetName());
	auto fields = cty->GetFields();
	for (auto field : fields)
		irstty->AddFieldTy(ConvertType(field));

	IRType* found = FindIRType(irstty);
	if (found)
	{ // �ߺ��� ���
		delete irstty;
		return static_cast<IRStructType*>(found);
	}
	ir_types_.insert(irstty);

	return irstty;
}

Function* gclang::IRGen::NewVoidFunction(std::string name)
{
	// �Լ� �ߺ� üũ
	Function* irfn = FindFunction(name);
	if (irfn)
		return irfn;
	
	// �Լ� Ÿ�� ����
	IRFunctionType* irfty = new IRFunctionType(nullptr); // void () �Լ� Ÿ�� ����
	IRType* irty = FindIRType(irfty); // �ߺ��˻�
	if (irty)
	{
		delete irfty;
		irfty = static_cast<IRFunctionType*>(irty);
	}
	ir_types_.insert(static_cast<IRType*>(irfty)); // �ű� �Լ� Ÿ���� ����
	// �Լ� �ű� ����
	irfn = new Function(irfty, name); // �Լ� ����
	irfn->AddBB(new BasicBlock(NewLabel("entry"))); // ���� BB �߰�
	funcs_.push_back(irfn);

	return irfn;
}

Function* gclang::IRGen::FindFunction(std::string name)
{
	// �Լ� �ߺ� üũ
	auto it_func = std::find_if(funcs_.begin(), funcs_.end(), [&](Function* func)
		{
			return func->GetOrgName() == name;
		});
	if (it_func != funcs_.end())
	{
		return *it_func;
	}

	return nullptr;
}

IRType* gclang::IRGen::FindIRType(IRType* ty)
{
	auto it = ir_types_.find(ty);// �ߺ��˻�
	if (it != ir_types_.end())
		return *it;
	return nullptr;
}

std::string gclang::IRGen::NewLabel(const std::string& name)
{
	if (name.empty())
		return "L" + std::to_string(label_cnt_++);
	else
		return "L" + name + std::to_string(label_cnt_++);
}

BasicBlock* gclang::IRGen::NewBasicBlock(const std::string& name)
{
	BasicBlock* bb = name.empty() ? new BasicBlock(NewLabel()) : new BasicBlock(NewLabel(name));
	if (cur_func_ != nullptr)
	{
		cur_func_->AddBB(bb);
	}
	return bb;
}

Value* gclang::IRGen::GetResult(std::any& res, std::string error_msg)
{
	if (!res.has_value())
		assert(0 && error_msg.c_str());

	Value* val = std::any_cast<Value*>(res);
	return val;
}

Value* gclang::IRGen::GetResultNullable(std::any& res, std::string msg)
{
	if (!res.has_value())
	{
		cout << msg;
		return nullptr;
	}

	Value* val = std::any_cast<Value*>(res);
	return val;
}

Value* gclang::IRGen::LoadConst(Value* v)
{
	if (v->GetKind() == EIRKind::eConstant || v->GetKind() == EIRKind::eGlobalValue)
	{
		// ����� ��� �������̹Ƿ�, �������� ���� Ÿ���� �⺻Ÿ���̸�, 
		// Load ��ɾ �߰��Ͽ� �������ͷ� ���� �ε��Ѵ�.
		// �������� ����Ÿ���� String, Array, Class Ÿ���̸�, �����͸� �״�� ����Ѵ�.
		// ���� Load ��ɾ �߰��� �ʿ䰡 ����.
		IRType* ptrty = v->GetType()->GetPtrType();
		if (ptrty->GetETypeKind() == IRType::EIRTypeKind::eIRType)
		{
			Load* load = nullptr;
			if (ptrty->GetEType() == IRType::EIRType::eI32)
				load = new Load(i32_, v);
			else if (ptrty->GetEType() == IRType::EIRType::eF32)
				load = new Load(f32_, v);

			cur_bb_->AddInst(load);
			return load;
		}
	}

	return v;
}

/*
* Alloca ��ɾ�� machine stack�� �Ҵ��ϴ� ��ɾ� �̴�.
* ���� �Լ��� Stack Frame�� ������ �� �ش� �Լ����� ������ ���� ��� ������ ���ؼ� �̸� stack �� �Ҵ��ؾ� �Ѵ�.
* �� ����� ���ؼ� Alloca �� ���� �Լ��� ù��° BB ���� �����ؾ� �Ѵ�. 
*/
void gclang::IRGen::InsertAlloca(Value* alloca)
{
	cur_func_->GetEntryBB()->AddInst(static_cast<Inst*>(alloca));
}

