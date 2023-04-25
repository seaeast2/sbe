#include "stdafx.h"

#include "BasicBlock.h"
#include "IR.h"
#include "IRGen.h"
#include "ConstantTable.h"
#include "IRPrinter.h"

void gclang::IRPrinter::Print(IRGen* irgen)
{
	if (!irgen)
		return;

	ConstantTable* ctable = irgen->GetConstTable();
	ctable->Print();

	// Global Value ���
	auto& gvals = irgen->GetGlobalValues();
	for (auto gval : gvals)
	{
		cout << gval->toString() << endl;
	}

	// �Լ� ���
	auto& funcs = irgen->GetFuncs();
	for (auto func : funcs)
	{
		cout << func->toString() << endl;
	}

	// class ���
	auto& classs = irgen->GetClasses();
	for (auto clss : classs)
	{
		cout << clss->toString() << endl;
	}
}
