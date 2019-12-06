#pragma once
#include "Trainings_Data_Gen.h"
#include "Test_Runner.h"
#include "AI_Evaluator.h"

namespace Eval
{
	class AI_Config_CLI
	{
	public:
		AI_Config_CLI() {};

		void run_dialog();
		void generate_trainings_sheets_dialog();
		void analyse_sheets_dialog();
		void validate_net_dialog();
		void validate_one_net();
		void batch_validation();

		void run_tests();
		void generate_trainings_sheets();
		void analyse_sheets();


		int train_sheets_count = 10000;
		int test_sheets_count = 2000;
		int valid_sheets_count = 2000;

		Eval::Trainings_Data_Gen::Settings train_settings = Eval::Trainings_Data_Gen::Settings::Stochastic;
		Eval::Trainings_Data_Gen::Settings test_settings = Eval::Trainings_Data_Gen::Settings::Post_Sep;
		Eval::Trainings_Data_Gen::Settings valid_settings = Eval::Trainings_Data_Gen::Settings::Post_Sep;

		std::string train_data_folder = "data/trainings_data/train";
		std::string test_data_folder = "data/trainings_data/test";
		std::string valid_data_folder = "data/trainings_data/valid";
		std::string selected_data_folder = "data/trainings_data/selected";
		Eval::Trainings_Data_Gen generator;


		Eval::Test_Runner test_runner;
		Eval::AI_Evaluator ai_evaluator;
	};
}
