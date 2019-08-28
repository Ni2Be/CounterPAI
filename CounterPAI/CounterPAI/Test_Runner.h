#pragma once

#include <string>

namespace Eval
{
	class Test_Runner
	{
	public:
		Test_Runner();

		void run_test(const std::string& parameters_file);
	};
}
