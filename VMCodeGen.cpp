#include "Common.h"

//#include "Entity.h"
#include "IRGen.h"
#include "VMCodeGen.h"


using namespace gclang;
/*
VMCodeGen::VMCodeGen()
{
}

void gclang::VMCodeGen::Generate(IRGen* irgen)
{
	irgen_ = irgen;

	os_.open("output.vm");

	

	GenerateDataSection(irgen_->GetGlobalDatas());

	//GenerateCodeSection()

	os_.close();
}

void VMCodeGen::GenerateDataSection(std::vector<Variable*>& vars)
{
	os_ << ".data:" << endl;

	PushIndent();
	for (auto const& var : vars)
	{
		WriteGlobalDataInts(var->GetName(), var->GetIRType(), 1, 0);
	}
	PopIndent();
}

void gclang::VMCodeGen::GenerateCodeSection()
{
}

void gclang::VMCodeGen::PrintIndent()
{
	int n = indent_;
	while (n > 0)
	{
		os_ << indent_str_;
		n--;
	}
}

void gclang::VMCodeGen::WriteGlobalDataInts(std::string label, EIRType type, int arg_count, ...)
{
	va_list ap;
	va_start(ap, arg_count);

	PrintIndent();

	// label 
	os_ << label << " ";

	switch (type)
	{
	case EIRType::eBYTE:
		os_ << "db";
		break;

	case EIRType::eWORD:
		os_ << "dw";
		break;

	case EIRType::eDWORD:
		os_ << "dd";
		break;

	case EIRType::eQWORD:
		os_ << "dq";
		break;

	default:
		os_ << "db";
		break;
	}

	os_ << " ";
	
	for (int i = 0; i < arg_count; i++)
	{
		int arg = va_arg(ap, int);
		
		os_ << "0x" << hex << arg;

		if (i + 1 < arg_count)
			os_ << ",";
	}
	va_end(ap);

	os_ << endl;
}

void gclang::VMCodeGen::WriteGlobalDataString(std::string label, EIRType type, std::string data)
{
}

*/
