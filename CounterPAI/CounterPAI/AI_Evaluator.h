#pragma once
#include "Evaluator_Base.h"

#include <torch/torch.h>

#include "Learn_Settings.h"

#include <bitset>
#include <vector>
#include "Net.h"

namespace Eval
{

	class Dataset;
	class AI_Evaluator : public Evaluator_Base
	{
	public:
		AI_Evaluator();

		Sheet_Music testing();


		std::vector<std::pair<std::shared_ptr<Eval::Net>, Learn_Settings>> models;
		void load_nets();

		void train_net(Learn_Settings settings);
		void validate_net(const std::string& model_path, const std::string& settings_path, const std::string& save_path = "do_not_save");
		void validate_all_rules_net(const std::string& model_path, const std::string& settings_path, const std::string& save_path = "do_not_save");
		void test_on_equals(Learn_Settings settings);
		

		torch::Device device;


		void evaluate_notes(Sheet_Music& sheet);
	};
}
