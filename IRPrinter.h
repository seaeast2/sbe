#pragma once

namespace gclang
{
	class IRGen;

	class IRPrinter
	{
	public:
		IRPrinter() = default;
		~IRPrinter() = default;

		void Print(IRGen* irgen);
	};
}
