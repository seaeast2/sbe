#pragma once


namespace gclang
{
	class Constant;
	class ConstantTable
	{
		std::unordered_map<int, Constant*> int_;
		std::unordered_map<float, Constant*> float_;
		std::unordered_map<intptr_t, Constant*> ptr_;
		std::unordered_map<std::string , Constant* > str_;

	public:
		ConstantTable();
		~ConstantTable();

		EResult AddInt(Constant* c);
		EResult AddFloat(Constant* c);
		EResult AddPtr(Constant* c);
		EResult AddStr(Constant* c);

		Constant* FindIntConst(int i);
		Constant* FindFloatConst(float f);
		Constant* FindPtrConst(intptr_t p);
		Constant* FindStrConst(std::string s);

		void Print();
	};
}
