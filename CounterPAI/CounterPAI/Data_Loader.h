#pragma once
#include <torch/torch.h>
#include "Sheet_Music.h"

#include <tuple>

#include "Learn_Settings.h"

namespace Eval {
	class Data_Loader
	{
	public:
		Data_Loader(Learn_Settings settings, bool is_training = true);
		
		Learn_Settings settings;

		std::tuple<torch::Tensor, torch::Tensor> get_batch(std::vector<torch::Tensor>& features_vec, std::vector<torch::Tensor>& targets_vec);
		std::tuple<torch::Tensor, torch::Tensor> get_train_batch();
		std::tuple<torch::Tensor, torch::Tensor> get_test_batch();


		std::vector<torch::Tensor> train_features_vec;
		std::vector<torch::Tensor> train_targets_vec;

		std::vector<torch::Tensor> test_features_vec;
		std::vector<torch::Tensor> test_targets_vec;

		std::vector<Sheet_Music> load_sheets(const std::string folder);
		std::vector<std::vector<float>> convert_to_2d_vector(Sheet_Music& sheet);


		void remember_one_cf(std::vector<Sheet_Music>& sheets, std::vector<torch::Tensor>& features, std::vector<torch::Tensor>& targets);
		void evaluate_fux_rules_from_one_side_1(std::vector<Sheet_Music>& sheets, std::vector<torch::Tensor>& features, std::vector<torch::Tensor>& targets);
		void evaluate_fux_rules_from_two_sides_1(std::vector<Sheet_Music>& sheets, std::vector<torch::Tensor>& features, std::vector<torch::Tensor>& targets);
		void evaluate_fux_rules_from_two_sides_lnn_1(std::vector<Sheet_Music>& sheets, std::vector<torch::Tensor>& features, std::vector<torch::Tensor>& targets);
	};
}
