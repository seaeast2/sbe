#pragma once

#include "enums.h"

namespace gclang
{
	/*
	class Variable;
	class IRGen;
	// IR �� tiny vm code �������� ��ȯ
	class VMCodeGen
	{
	protected:
		IRGen* irgen_ = nullptr;

		int indent_ = 0;
		std::string indent_str_ = "    ";

		std::ofstream os_;

	public:
		VMCodeGen();
		~VMCodeGen() = default;

		void Generate(IRGen *irgen);

	private:
		void GenerateDataSection(std::vector<Variable*>& vars);

		void GenerateCodeSection();

		void PushIndent() { indent_++; }
		void PopIndent() { indent_--; }
		void PrintIndent();

		// integer ������ ���� �������� ����
		void WriteGlobalDataInts(std::string label, EIRType type, int arg_count, ...);
		// ���ڿ� ���ͷ� ����
		void WriteGlobalDataString(std::string label, EIRType type, std::string data);

	};*/
}
