#include "stdafx.h"

#include "IRType.h"

using namespace gclang;


//= IRType ======================================================

gclang::IRType::IRType(EIRType etype)
{
	etype_ = etype;
	if (etype_ == EIRType::eI32)
		name_ = "i32";
	else if (etype_ == EIRType::eF32)
		name_ = "f32";
	else if (etype == EIRType::ePTR)
		name_ = "ptr";
}

gclang::IRType::IRType(EIRType etype, IRType* ptr_type)
{
	etype_ = EIRType::ePTR;
	ptr_type_ = ptr_type;
	
	name_ = ptr_type_->toString();
}

void gclang::IRType::SetETypeKind(EIRTypeKind kind)
{
	kind_ = kind;
}

gclang::IRType::EIRTypeKind gclang::IRType::GetETypeKind()
{
	return kind_;
}

void gclang::IRType::SetEType(EIRType etype)
{
	etype_ = etype;
}

gclang::IRType::EIRType gclang::IRType::GetEType()
{
	return etype_;
}

void gclang::IRType::SetName(std::string name)
{
	name_ = name;
}

std::string gclang::IRType::GetName()
{
	return name_;
}

std::string gclang::IRType::GetName() const
{
	return name_;
}

void gclang::IRType::SetPtrType(IRType* ptr_type)
{
	ptr_type_ = ptr_type;
}

IRType* gclang::IRType::GetPtrType()
{
	return ptr_type_;
}

bool gclang::IRType::IsPtrType()
{
	return etype_ == IRType::EIRType::ePTR;
}

std::string gclang::IRType::toString()
{
	std::ostringstream o;
	if (IsPtrType() && ptr_type_->GetETypeKind() == EIRTypeKind::eIRType)
		o << "ptr " << ptr_type_->toString();
	else
		o << GetName();

	return o.str();
}

// IRArrayType ========================
gclang::IRArrayType::IRArrayType(IRType* type, int count)
{
	kind_ = EIRTypeKind::eIRArrayType;
	etype_ = EIRType::ePTR;
	ptr_type_ = type;
	count_ = count;
	is_string_ = false;

	name_ = "[ " + std::to_string(count_) + " x " + ptr_type_->toString() + " ]";
}

void gclang::IRArrayType::SetCount(int count)
{
	count_ = count;
}

int gclang::IRArrayType::GetCount()
{
	return count_;
}

void gclang::IRArrayType::SetIsString(bool is_string)
{
	is_string_ = is_string;
}

bool gclang::IRArrayType::IsString()
{
	return is_string_;
}

std::string gclang::IRArrayType::PrintInitializer()
{
	// TODO : Initializer 출력.
	return std::string();
}

std::string gclang::IRArrayType::toString()
{
	// IRArrayType 의 toString 구현
	std::ostringstream o;
	
	o << "ptr [ " << count_<< " x " << ptr_type_->toString() << " ]";
	
	// TODO : array initializer 출력

	return o.str();
}

// IRFunctionType ========================
gclang::IRFunctionType::IRFunctionType(IRType* retty)
{
	kind_ = EIRTypeKind::eIRFunctionType;
	etype_ = EIRType::ePTR;
	
	ptr_type_ = this;
	retty_ = retty;

	UpdateName(); // 이름 설정하기
}

void gclang::IRFunctionType::SetReturnTy(IRType* retty)
{
	retty_ = retty;
}

IRType* gclang::IRFunctionType::GetReturnTy()
{
	return retty_;
}

void gclang::IRFunctionType::AddParam(IRType* param)
{
	paramtys_.push_back(param);
}

std::vector<IRType*>& gclang::IRFunctionType::GetParams()
{
	return paramtys_;
}

void gclang::IRFunctionType::UpdateName()
{
	// IRFunctionType::name_ 업데이트 하기
	// ex) 'i32 (i32, f32, ptr, i32)' : 함수 타입의 이름 형식
	if (retty_)
		name_ = retty_->GetName();
	else
		name_ = "void";
	name_ += "(";
	name_ += VecToCommaStr(paramtys_);
	name_ += ")";
}

std::string gclang::IRFunctionType::toString()
{
	std::ostringstream o;
	o << name_;
	return o.str();
}

// IRFunctionType ========================
gclang::IRStructType::IRStructType(std::string name)
{
	kind_ = EIRTypeKind::eIRStructType;
	etype_ = EIRType::ePTR;
	ptr_type_ = this;

	org_name_ = name;
	name_ = "%struct." + name;
}

std::string gclang::IRStructType::GetOrgName()
{
	return org_name_;
}


void gclang::IRStructType::AddFieldTy(IRType* field_type)
{
	field_types_.push_back(field_type);
}

std::vector<IRType*>& gclang::IRStructType::GetFieldTys()
{
	return field_types_;
}

std::string gclang::IRStructType::toStringFull()
{
	// toStringFull() : %struct.name = type { %struct.base, i32 } 형태로 출력
	std::ostringstream o;

	o << name_ << " = type {";
		
	o << VecToCommaStr(field_types_);

	o << " }";

	return o.str();
}

std::string gclang::IRStructType::toString()
{
	// 이름만 출력
	// toString() : %struct.name
	std::ostringstream o;
	o << "ptr " << name_;
	return o.str();
}