#include "Test_Runner.h"
#include "AI_Evaluator.h"
#include "LNN.h"

Eval::Test_Runner::Test_Runner()
{
}

void Eval::Test_Runner::run_test(const std::string& test_file)
{
	Learn_Settings settings(test_file);

	std::cout << settings;

	AI_Evaluator evaluator;

	evaluator.train_net(settings);
	
}

