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



	class CustomDataset : public torch::data::Dataset<CustomDataset>
	{
	public:
		Data_Loader loader;

		explicit CustomDataset(Learn_Settings settings, bool is_training = true)
			:
			loader(settings, is_training)
		{
		};

		// Override the get method to load custom data.
		torch::data::Example<> get(size_t index) override 
		{
			return { loader.train_features_vec[index].clone(), loader.train_targets_vec[index].clone() };
		};

		// Override the size method to infer the size of the data set.
		torch::optional<size_t> size() const override 
		{
			return loader.train_features_vec.size();
		};
	};
}
