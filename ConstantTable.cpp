#include "stdafx.h"

#include "IRType.h"
#include "IR.h"
#include "ConstantTable.h"

using namespace gclang;

gclang::ConstantTable::ConstantTable()
{
}

gclang::ConstantTable::~ConstantTable()
{
}

EResult gclang::ConstantTable::AddInt(Constant* c)
{
	if (!c)
		assert(0 && "invalid object.");

	auto it = int_.find(c->GetInt());
	if (it != int_.end())
	{// 중복이 있을 경우
		delete c;
		return EResult::eFail;
	}

	int_.insert({ c->GetInt(), c });
	return EResult::eSuccess;
}

EResult gclang::ConstantTable::AddFloat(Constant* c)
{
	if (!c)
		assert(0 && "invalid object.");

	auto it = float_.find(c->GetFloat());
	if (it != float_.end())
	{// 중복이 있을 경우
		delete c;
		return EResult::eFail;
	}

	float_.insert({ c->GetFloat(), c });
	return EResult::eSuccess;
}

EResult gclang::ConstantTable::AddPtr(Constant* c)
{
	if (!c)
		assert(0 && "invalid object.");

	auto it = ptr_.find(c->GetPtr());
	if (it != ptr_.end())
	{// 중복이 있을 경우
		delete c;
		return EResult::eFail;
	}

	ptr_.insert({ c->GetPtr(), c });
	return EResult::eSuccess;
}

EResult gclang::ConstantTable::AddStr(Constant* c)
{
	if (!c)
		assert(0 && "invalid object.");

	auto it = str_.find(c->GetStr());
	if (it != str_.end())
	{// 중복이 있을 경우
		delete c;
		return EResult::eFail;
	}

	str_.insert({ c->GetStr(), c });
	return EResult::eSuccess;
}

Constant* gclang::ConstantTable::FindIntConst(int i)
{
	auto found = int_.find(i);
	if (found != int_.end())
		return found->second;
	return nullptr;
}

Constant* gclang::ConstantTable::FindFloatConst(float f)
{
	auto found = float_.find(f);
	if (found != float_.end())
		return found->second;
	return nullptr;
}

Constant* gclang::ConstantTable::FindPtrConst(intptr_t p)
{
	auto found = ptr_.find(p);
	if (found != ptr_.end())
		return found->second;
	return nullptr;
}

Constant* gclang::ConstantTable::FindStrConst(std::string s)
{
	auto found = str_.find(s);
	if (found != str_.end())
		return found->second;
	return nullptr;
}

void gclang::ConstantTable::Print()
{
	for (auto iitr : int_)
	{
		cout << iitr.second->GetName() << " = " << iitr.second->GetType()->toString() << " " << iitr.second->toString() << endl;
	}

	for (auto fitr : float_)
	{
		cout << fitr.second->GetName() << " = " << fitr.second->GetType()->toString() << " " << fitr.second->toString() << endl;
	}

	for (auto pitr : ptr_)
	{
		cout << pitr.second->GetName() << " = " << pitr.second->GetType()->toString() << " " << pitr.second->toString() << endl;
	}

	for (auto sitr : str_)
	{
		cout << sitr.second->GetName() << " = " << sitr.second->GetType()->toString() << " " << sitr.second->toString() << endl;
	}

	cout << endl;
}
