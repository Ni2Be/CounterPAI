#pragma once
#include "Test_Runner.h"
#include "AI_Evaluator.h"
#include "NN.h"
#include "Utility.h"

Eval::Test_Runner::Test_Runner()
{
}

void Eval::Test_Runner::run_test(const std::string& test_file)
{
	Learn_Settings settings(test_file);

	std::cout << "\nInit test: " << Utility::get_time_stamp() << "\n";

	std::cout << settings;

	AI_Evaluator evaluator;

	evaluator.train_net(settings);
	
	std::cout << "\nTest done: " << Utility::get_time_stamp() << "\n";
}
