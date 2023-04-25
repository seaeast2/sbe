#pragma once

#include "enums.h"

namespace gclang
{
	/*
	class Variable;
	class IRGen;
	// IR 을 tiny vm code 형식으로 변환
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

		// integer 형태의 값을 연속으로 쓰기
		void WriteGlobalDataInts(std::string label, EIRType type, int arg_count, ...);
		// 문자열 리터럴 쓰기
		void WriteGlobalDataString(std::string label, EIRType type, std::string data);

	};*/
}
