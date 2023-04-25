#include "stdafx.h"
#include <unordered_set>

#include "BasicBlock.h"
#include "IRType.h"
#include "IR.h"

using namespace gclang;

static int VALUE_COUNT = 0;

namespace gclang
{
	std::string op_ir_str[(int)gclang::EBinOpKind::eNone] = 
	{
		"add", // ePlus
		"sub", // eMinus
		"mul", // eMul
		"mod", // eMod
		"cmp_and", // eLogicalAnd
		"cmp_or", // eLogicalOr
		"cmp_not", // eLogicalNot
		"and", // eBitAnd
		"or", // eBitOr
		"xor", // eBitXor
		"not", // eBitNot
		"shl", // eBitLShift
		"shr", // eBitRShift
		"add", // eUnaryAdd
		"sub", // eUnarySub
		"", // eAssign
		"add", // ePlusAssign
		"sub", // eMinusAssign
		"mul", // eMulAssign
		"div", // eDivAssign
		"mod", // eModAssign
		"and", // eBitAndAssign
		"or", // eBitOrAssign
		"xor", //eBitXorAssign
		"shl", // eBitLShiftAssign
		"shr", // eBitRShiftAssign
		"cmp_eq", // eEqual
		"cmp_ne", // eNotEqual
		"cmp_gt", // eGT
		"cmp_lt", // eLT
		"cmp_ge", // eGE
		"cmp_le", // eLE
		"", // eCondExpr
		"" // eNone
	};
}


//= Value ======================================================
gclang::Value::Value()
{
	kind_ = EIRKind::eValue;
	type_ = nullptr;
}

void gclang::Value::SetKind(EIRKind kind)
{
	kind_ = kind;
}

EIRKind gclang::Value::GetKind()
{
	return kind_;
}

void gclang::Value::SetType(IRType* type)
{
	type_ = type;
}

IRType* gclang::Value::GetType()
{
	return type_;
}

void gclang::Value::SetName(std::string name)
{
	name_ = "%" + name;
}

std::string gclang::Value::GetName()
{
	return name_;
}

std::string gclang::Value::GetOrgName()
{
	return org_name_;
}

void gclang::Value::SetInt(int value)
{
	ivalue_ = value;
}

int gclang::Value::GetInt()
{
	return ivalue_;
}

void gclang::Value::SetFloat(float value)
{
	fvalue_ = value;
}

float gclang::Value::GetFloat()
{
	return fvalue_;
}

void gclang::Value::SetPtr(intptr_t ptr)
{
	ptrvalue_ = ptr;
}

intptr_t gclang::Value::GetPtr()
{
	return ptrvalue_;
}

bool gclang::Value::IsPtr()
{
	if (ptrvalue_ < 0)
		return false;
	return true;
}

void gclang::Value::AddUser(Value* user)
{
	users_.push_back(user);
}

std::list<Value*>& gclang::Value::GetUsers()
{
	return users_;
}


std::string gclang::Value::toString()
{
	std::ostringstream o;

	o << name_;

	return o.str();
}

//= Argument ======================================================

gclang::Argument::Argument(IRType* ty, std::string name)
{
	kind_ = EIRKind::eArgument;
	type_ = ty;
	id_ = VALUE_COUNT++;
	org_name_ = name;
	name_ = "%" + name + std::to_string(id_);
}

std::string gclang::Argument::toString()
{
	std::ostringstream o;
	
	o << type_->toString() << " " << name_;

	return o.str();
}
//= Constant ======================================================

gclang::Constant::Constant(IRType* ty)
{
	kind_ = EIRKind::eConstant;
	type_ = new IRType(IRType::EIRType::ePTR, ty); // Constant 는 무조건 ptr 타입으로 생성한다.
	//type_ = ty;
	id_ = VALUE_COUNT++;
	name_ = "@" + std::to_string(id_);
	org_name_ = name_;
}

gclang::Constant::Constant(IRType* ty, int ivalue)
{
	kind_ = EIRKind::eConstant;
	type_ = new IRType(IRType::EIRType::ePTR, ty); // Constant 는 무조건 ptr 타입으로 생성한다.
	//type_ = ty;
	id_ = VALUE_COUNT++;
	name_ = "@" + std::to_string(id_);
	org_name_ = name_;

	ivalue_ = ivalue;
}

gclang::Constant::Constant(IRType* ty, float fvalue)
{
	kind_ = EIRKind::eConstant;
	type_ = new IRType(IRType::EIRType::ePTR, ty); // Constant 는 무조건 ptr 타입으로 생성한다.
	//type_ = ty;
	id_ = VALUE_COUNT++;
	name_ = "@" + std::to_string(id_);
	org_name_ = name_;

	fvalue_ = fvalue;
}

//gclang::Constant::Constant(IRType* ty, intptr_t pvalue)
//{
//	kind_ = EIRKind::eConstant;
//	type_ = new IRType(IRType::EIRType::ePTR, ty); // Constant 는 무조건 ptr 타입으로 생성한다.
//	//type_ = ty;
//	id_ = VALUE_COUNT++;
//	name_ = "@" + std::to_string(id_);
//	org_name_ = name_;
//
//	ptrvalue_ = pvalue;
//}

gclang::Constant::Constant(IRType* ty, std::string svalue)
{
	kind_ = EIRKind::eConstant;
	type_ = new IRType(IRType::EIRType::ePTR, ty); // Constant 는 무조건 ptr 타입으로 생성한다.
	//type_ = ty;
	id_ = VALUE_COUNT++;
	name_ = "@" + std::to_string(id_);
	org_name_ = name_;

	strvalue_ = svalue;
}

void gclang::Constant::SetStr(std::string str)
{
	strvalue_ = str;
}

std::string gclang::Constant::GetStr()
{
	return strvalue_;
}

std::string gclang::Constant::toString()
{
	std::ostringstream o;
	
	IRType* org_ty = type_->GetPtrType();

	//o << type_->toString() << " ";
	if (org_ty->GetEType() == IRType::EIRType::eI32)
		o << std::to_string(ivalue_);
	else if (org_ty->GetEType() == IRType::EIRType::eF32)
		o << std::to_string(fvalue_);
	else if (org_ty->IsPtrType())
	{
		if (org_ty->GetETypeKind() == IRType::EIRTypeKind::eIRArrayType)
		{
			IRArrayType* irarrty = static_cast<IRArrayType*>(org_ty);
			if (irarrty->IsString())
				o << strvalue_;
		}
		else 
			o << std::to_string(ptrvalue_);
	}
	return o.str();
}
//= GlobalValue ======================================================
gclang::GlobalValue::GlobalValue(IRType* type, std::string name)
{
	kind_ = EIRKind::eGlobalValue;

	type_ = new IRType(IRType::EIRType::ePTR, type); // GlobalValue 는 무조건 ptr 타입으로 생성한다.
	
	org_name_ = name;
	name_ = "@" + name;
}

void gclang::GlobalValue::AddInit(Constant* v)
{
	inits_.push_back(v);
}

std::vector<Constant*>& gclang::GlobalValue::GetInits()
{
	return inits_;
}

std::string gclang::GlobalValue::toString()
{
	std::ostringstream o;

	if (!type_)
		assert(0 && "invalid global value type");

	// GlobalValue의 type_ 은 기본적으로 포인터 이므로, pointer 의 원래 타입으로 작업을 한다.
	IRType* org_ty = type_->GetPtrType();

	if (org_ty->GetEType() == IRType::EIRType::eI32)
		o << name_ << " = " << type_->toString() << " " << ivalue_;
	else if (org_ty->GetEType() == IRType::EIRType::eF32)
		o << name_ << " = " << type_->toString() << " " << fvalue_;
	else if (org_ty->IsPtrType())
	{
		if(org_ty->GetETypeKind() == IRType::EIRTypeKind::eIRType ||
			org_ty->GetETypeKind() == IRType::EIRTypeKind::eIRFunctionType)
			o << name_ << " = " << type_->toString() << " " << ptrvalue_;
		else if (org_ty->GetETypeKind() == IRType::EIRTypeKind::eIRArrayType)
		{ 
			// TODO : IRArrayType을 좀더 제대로 출력 하도록 변경
			o << name_ << " = " << type_->toString();
			o << " [ ";
			VecToCommaStr(inits_);
			o << " ]";
		}
		else if (org_ty->GetETypeKind() == IRType::EIRTypeKind::eIRStructType)
		{ // TODO : IRStructType을 좀더 제대로 출력 하도록 변경
			o << name_ << " = " << type_->toString();
			o << " { ";
			VecToCommaStr(inits_);
			o << " }";
		}
	}

	return o.str();
}

void gclang::GlobalValue::IterateArrayTy(IRType* arrty, int index)
{
	IRType* cur_arrty = arrty;
	while (cur_arrty->GetETypeKind() != IRType::EIRTypeKind::eIRArrayType)
	{

		cur_arrty = cur_arrty->GetPtrType();
	}
}

// === Function =========================================
gclang::Function::Function(IRType* type, std::string name)
{
	kind_ = EIRKind::eFunction;
	type_ = type;
	org_name_ = name;
	name_ = "@" + name;
}

void gclang::Function::AddParam(Argument* param)
{
	params_.push_back(param);
}

std::vector<Argument*>& gclang::Function::GetParam()
{
	return params_;
}

void gclang::Function::AddBB(BasicBlock* bb)
{
	bbs_.push_back(bb);
}

std::vector<BasicBlock*>& gclang::Function::GetBBs()
{
	return bbs_;
}

BasicBlock* gclang::Function::GetEntryBB()
{
	return bbs_[0];
}

void gclang::Function::SetIntrinsic(bool intrinsic)
{
	intrinsic_ = intrinsic;
}

bool gclang::Function::IsIntrinsic()
{
	return intrinsic_;
}

std::string gclang::Function::toString()
{
	std::ostringstream o;
	
	if (type_->GetETypeKind() != IRType::EIRTypeKind::eIRFunctionType)
		assert(0 && "invalid funtion type");

	IRFunctionType* fty = static_cast<IRFunctionType*>(type_);

	// return type 출력
	if (fty->GetReturnTy() == nullptr)
		o << "void"; 
	else
		o << fty->GetReturnTy()->toString(); // return type 출력

	// 함수명 출력
	o << " " << name_;

	// 함수 파라메터 출력
	o << "(" << VecToCommaStr(params_) << ")";

	if (intrinsic_)
	{
		o << " intrinsic" << endl;
		return o.str();
	}
	
	// intrinsic 함수가 아닐 경우
	o << endl;
	o << "{" << endl;

	INDENT_PUSH();
	for (auto bb : bbs_)
	{
		o << bb->toString() << endl;

	}
	INDENT_POP();

	o << "}" << endl;

	return o.str();
}


//= IRInst ======================================================
gclang::Inst::Inst()
{
	kind_ = EIRKind::eInst;
}

void gclang::Inst::SetID(int id)
{
	id_ = id;
}
int gclang::Inst::GetID()
{
	return id_;
}
void gclang::Inst::SetBB(BasicBlock* bb)
{
	bb_ = bb;
}
BasicBlock* gclang::Inst::GetBB()
{
	return bb_;
}
std::string gclang::Inst::toString()
{
	return "[Inst]";
}

// = Branch ================
gclang::Branch::Branch(Value* cond, BasicBlock* then_label, BasicBlock* else_label)
{
	kind_ = EIRKind::eBranch;
	type_ = nullptr;
	id_ = -1;

	cond_ = cond;
	if (cond_)
		cond_->AddUser(this);

	then_label_ = then_label;
	else_label_ = else_label;
}

void gclang::Branch::SetCond(Value* cond)
{
	cond_ = cond;
	if (cond_)
		cond_->AddUser(this);
}

Value* gclang::Branch::GetCond()
{
	return cond_;
}

void gclang::Branch::SetThenLabel(BasicBlock* then_label)
{
	then_label_ = then_label;
}

BasicBlock* gclang::Branch::GetThenLabel()
{
	return then_label_;
}

void gclang::Branch::SetElseLabel(BasicBlock* else_label)
{
	else_label_ = else_label;
}

BasicBlock* gclang::Branch::GetElseLabel()
{
	return else_label_;
}

std::string gclang::Branch::toString()
{
	std::ostringstream o;
	if (cond_)
		o << "br " << cond_->GetType()->toString() << " " << cond_->GetName() << ", ";
	else
		o << "br ";

	o << then_label_->PrintLabel();

	if (else_label_)
		o << ", " << else_label_->PrintLabel();
	return o.str();
}

// = Return ================
gclang::Return::Return(Value* retval)
{
	kind_ = EIRKind::eReturn;
	type_ = nullptr;
	id_ = -1;

	retval_ = retval;
	if (retval_)
		retval_->AddUser(this);
}

void gclang::Return::SetRetVal(Value* retval)
{
	retval_ = retval;
	if (retval_)
		retval_->AddUser(this);
}

Value* gclang::Return::GetRetVal()
{
	return retval_;
}

std::string gclang::Return::toString()
{
	std::ostringstream o;
	if (retval_)
		o << "return " << retval_->GetType()->toString() << " " << retval_->GetName();
	else
		o << "return";
	return o.str();
}

// = Switch ================
gclang::Switch::Switch()
{
	kind_ = EIRKind::eSwitch;
}

gclang::Case::Case()
{
	kind_ = EIRKind::eCase;
}
// == Cast ====================
gclang::Cast::Cast(IRType* castty, Value* src)
{
	kind_ = EIRKind::eCast;
	type_ = castty;
	id_ = VALUE_COUNT++;
	name_ = "%" + std::to_string(id_);
	
	src_ = src;
	if (src_)
		src_->AddUser(this);
}

void gclang::Cast::SetSrc(Value* src)
{
	src_ = src;
	if (src_)
		src_->AddUser(this);
}

Value* gclang::Cast::GetSrc()
{
	return src_;
}

std::string gclang::Cast::toString()
{
	// ex) %3 = cast f32, %2
	std::ostringstream o;

	o << name_ << " = " << "cast " << type_->toString() << ", " << src_->GetName();

	return o.str();
}


// == BinOp ====================
gclang::BinOp::BinOp(IRType* retty, EBinOpKind op, Value* lhs, Value* rhs)
{
	kind_ = EIRKind::eBinOp;
	type_ = retty;
	op_ = op;
	id_ = VALUE_COUNT++;
	name_ = "%" + std::to_string(id_);

	lhs_ = lhs;
	if (lhs_)
		lhs_->AddUser(this);

	rhs_ = rhs;
	if (rhs_)
		rhs_->AddUser(this);
}

void gclang::BinOp::SetOp(EBinOpKind op)
{
	op_ = op;
}

EBinOpKind gclang::BinOp::GetOp()
{
	return op_;
}

void gclang::BinOp::SetLHS(Value* lhs)
{
	lhs_ = lhs;
	if (lhs_)
		lhs_->AddUser(this);
}

Value* gclang::BinOp::GetLHS()
{
	return lhs_;
}

void gclang::BinOp::SetRHS(Value* rhs)
{
	rhs_ = rhs;
	if (rhs_)
		rhs_->AddUser(this);
}

Value* gclang::BinOp::GetRHS()
{
	return rhs_;
}


std::string gclang::BinOp::toString()
{
	std::ostringstream o;

	// '%3 = add i32 %2, %1' 형태로 출력해야 함. 뒷쪽의 타입은 리턴타입
	o << name_ << " = " << op_ir_str[(int)op_]; // 명령어 출력
	o << " " << type_->toString() << " " << lhs_->GetName() << ", " << rhs_->GetName();

	return o.str();
}
// == Call ====================


gclang::Call::Call(IRType* retty, std::string func_name)
{
	kind_ = EIRKind::eCall;
	type_ = retty;
	id_ = VALUE_COUNT++;
	org_name_ = func_name;
	func_name_ = "@" + org_name_;
	name_ = "%" + std::to_string(id_);
}

void gclang::Call::SetFuncName(std::string func_name)
{
	org_name_ = func_name;
	func_name_ = "@" + org_name_;
}

std::string gclang::Call::GetFuncName()
{
	return func_name_;
}

void gclang::Call::AddArg(Value* arg)
{
	args_.push_back(arg);
}

std::vector<Value*>& gclang::Call::GetArgs()
{
	return args_;
}

std::string gclang::Call::toString()
{
	// TODO : Call toString() 구현.
	// ex) %3 = call i32, @print i32 %2, i32 %1
	std::ostringstream o;
	
	
	if (type_)
		o << name_ << " = call "<< type_->toString() << ", " << func_name_;
	else
		o << "call void, " << func_name_;

	std::string args_str;
	for (auto arg : args_)
	{
		args_str += " " + arg->GetType()->toString() + " " + arg->GetName() + ",";
	}
	if (!args_str.empty())
		args_str.pop_back();

	o << args_str;
	return o.str();
}

// == Alloca ====================
gclang::Alloca::Alloca(IRType* type, std::string name)
{
	kind_ = EIRKind::eAlloca;
	type_ = new IRType(IRType::EIRType::ePTR, type);
	id_ = VALUE_COUNT++;
	org_name_ = name;
	name_ = "%" + name + std::to_string(id_);
}

std::string gclang::Alloca::toString()
{
	std::ostringstream o;

	// '%0 = alloca i32' 형태로 출력해야 함. 뒷쪽의 타입은 리턴타입
	o << name_ << " = alloca " << type_->toString();

	return o.str();
}

// == Load ====================
gclang::Load::Load(IRType* ty, Value* ptr)
{
	kind_ = EIRKind::eLoad;
	type_ = ty;
	id_ = VALUE_COUNT++;
	name_ = "%" + std::to_string(id_);

	ptr_ = nullptr;
	if (ptr)
	{
		ptr->AddUser(this);
		ptr_ = ptr;
	}
}

void gclang::Load::SetPtr(Value* ptr)
{
	if (ptr)
	{
		ptr->AddUser(this);
		ptr_ = ptr;
	}
}

Value* gclang::Load::GetPtr()
{
	return ptr_;
}

std::string gclang::Load::toString()
{
	std::ostringstream o;
	// '%1 = load i32, ptr i32 %0' 형태로 출력해야 함.
	o << name_ << " = load " << type_->toString() << ", " << ptr_->GetType()->toString() << " " << ptr_->GetName();
	return o.str();
}

// == Store ====================
gclang::Store::Store(Value* src, Value* dest)
{
	kind_ = EIRKind::eStore;
	type_ = nullptr;
	id_ = -1;
	name_ = "";

	src_ = src;
	if (src_)
	{
		src_->AddUser(this);
	}

	dest_ = dest;

	if (dest_)
	{
		dest_->AddUser(this);
		if (!dest_->IsPtr())
			cout << "Store dest should be ptr type." << endl;
	}

}

void gclang::Store::SetSrc(Value* src)
{
	src_ = src;
	if (src_)
	{
		src_->AddUser(this);
	}
}

Value* gclang::Store::GetSrc()
{
	return src_;
}

void gclang::Store::SetDest(Value* dest)
{
	dest_ = dest;
	if (dest_)
	{
		dest_->AddUser(this);
		if (!dest_->IsPtr())
			cout << "Store dest should be ptr type." << endl;
	}
}

Value* gclang::Store::GetDest()
{
	return dest_;
}

std::string gclang::Store::toString()
{
	std::ostringstream o;
	// 'store i32 %0, ptr %2' 형태로 출력 해야 함.
	o << "store " << src_->GetType()->toString() << " " << src_->GetName() << ", " << dest_->GetType()->toString() << " " << dest_->GetName();
	return o.str();
}

// == GetElementPtr ====================
std::string gclang::GetElementPtr::toString()
{
	return std::string();
}

// == Phi ====================
gclang::Phi::Phi()
{
	kind_ = EIRKind::ePhi;
}


