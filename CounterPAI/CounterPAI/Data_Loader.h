#pragma once
#include <torch/torch.h>
#include "Sheet_Music.h"

#include <tuple>

#include "Learn_Settings.h"
#include "Rule_Evaluation.h"

namespace Eval {
	enum class Data_Type
	{
		TRAIN,
		TEST,
		VALID
	};
	class Data_Loader
	{
	public:
		Data_Loader(Learn_Settings settings, bool is_training = true, Data_Type type = Data_Type::TRAIN);
		
		Learn_Settings settings;

		std::tuple<torch::Tensor, torch::Tensor> get_batch(std::vector<torch::Tensor>& features_vec, std::vector<torch::Tensor>& targets_vec);
		std::tuple<torch::Tensor, torch::Tensor> get_batch();



		std::vector<torch::Tensor> features_vec;
		std::vector<torch::Tensor> targets_vec;

		static std::vector<Sheet_Music> load_sheets(const std::string folder);

		static void print_2d_vector(const std::vector<std::vector<float>>& sheet_vec);
		static std::vector<std::vector<float>> convert_to_2d_vector(Sheet_Music& sheet);


		void remember_one_cf(std::vector<Sheet_Music>& sheets, std::vector<torch::Tensor>& features, std::vector<torch::Tensor>& targets);

		void evaluate_fux_rules_from_one_side_1(std::vector<Sheet_Music>& sheets, std::vector<torch::Tensor>& features, std::vector<torch::Tensor>& targets);

		void evaluate_fux_rules_from_two_sides_1(std::vector<Sheet_Music>& sheets, std::vector<torch::Tensor>& features, std::vector<torch::Tensor>& targets);
		void evaluate_fux_rules_from_two_sides_lnn_1(std::vector<Sheet_Music>& sheets, std::vector<torch::Tensor>& features, std::vector<torch::Tensor>& targets);

		void evaluate_fux_rules_from_two_sides_rule_targets(std::vector<Sheet_Music>& sheets, std::vector<torch::Tensor>& features, std::vector<torch::Tensor>& targets, Eval::Fux_Rule target_rule);
		void evaluate_fux_rules_back_n_forth_rule_targets(std::vector<Sheet_Music>& sheets, std::vector<torch::Tensor>& features, std::vector<torch::Tensor>& targets, Eval::Fux_Rule target_rule);
		
	};



	class Dataset : public torch::data::Dataset<Dataset>
	{
	public:
		Data_Loader loader;
		Data_Type type;

		Dataset(Learn_Settings settings, Data_Type type, bool is_training = true)
			:
			type(type),
			loader(settings, is_training, type)
		{

		};

		Dataset(Data_Loader& loader)
			:
			loader(loader)
		{
		};
		// Override the get method to load custom data.
		torch::data::Example<> get(size_t index) override 
		{
			return { loader.features_vec[index].clone(), loader.targets_vec[index].clone() };
		};

		// Override the size method to infer the size of the data set.
		torch::optional<size_t> size() const override 
		{
			return loader.features_vec.size();
		};
	};
}
