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

	// Global Value 출력
	auto& gvals = irgen->GetGlobalValues();
	for (auto gval : gvals)
	{
		cout << gval->toString() << endl;
	}

	// 함수 출력
	auto& funcs = irgen->GetFuncs();
	for (auto func : funcs)
	{
		cout << func->toString() << endl;
	}

	// class 출력
	auto& classs = irgen->GetClasses();
	for (auto clss : classs)
	{
		cout << clss->toString() << endl;
	}
}
