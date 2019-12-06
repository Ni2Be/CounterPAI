#pragma once
#include "AI_Evaluator.h"

#include <filesystem>
#include <fstream>
#include <vector>
#include <iomanip>
#include <cassert>
#include <string>

#include <algorithm>
#include <random>

#include "Sheet_Music.h"

#include "Debug_Log.h"
#include "Folder_Dialog.h"

#include "Utility.h"

#include "rapid_csv.h"

#include "Data_Loader.h"

#include "Rule_Evaluator.h"
#include "Rule_Evaluation.h"

#include "Net.h"
#include "LSTM.h"
#include "NN.h"

Eval::AI_Evaluator::AI_Evaluator()
	:
	device(torch::kCPU)
{
	if (torch::cuda::is_available()) 
	{
		std::cout << "\nCUDA is available! Training on GPU.\n" << std::endl;
		device = torch::kCUDA;
	}
	else
	{
		std::cout << "\nno CUDA support! Training on CPU.\n" << std::endl;
		device = torch::kCPU;
	}
}


int get_sixteenth_length(std::list<Music_Note> voice);

void Eval::AI_Evaluator::evaluate_notes(Sheet_Music& sheet)
{		
	try
	{
		if (get_sixteenth_length(sheet.get_cf()) != get_sixteenth_length(sheet.get_cp()))
		{
			std::cerr << "\nCounter Point and Cantus Firmus must be the same length";
			return;
		}
		else if (sheet.get_cf().size() == 0)
		{
			std::cerr << "\nThe voices are empty";
			return;
		}

		////LOAD
		std::string model_save_folder = Folder_Dialog::get_exe_path() + "/data/trained/";
		//std::string model_name = "R2_from_two_sides_rule_targets";
		std::string model_name = "R2_back_n_forth_rule_targets";
		std::cout << "Evaluating with: " + model_save_folder + model_name + ".pt";
		Learn_Settings settings(model_save_folder + model_name + ".json");
		settings.batch_size = 1;
		std::cout << settings;
		std::shared_ptr<Eval::Net> model;
		if (settings.nn_type == NN_Type::LSTM)
		{
			model.reset(new LSTM(settings.in_size, settings.hidden_size, settings.out_size, settings.hidden_layer_count, settings.optimizer, settings.loss_func, settings.dropout, settings.is_bidirectional, device));
			model->set_learning_rate(settings.learning_rate);
		}
		else if (settings.nn_type == NN_Type::DENSE_NN)
		{
			model.reset(new NN(settings.in_size, settings.hidden_size, settings.out_size, settings.hidden_layer_count, settings.optimizer, settings.loss_func, settings.dropout, device));
			model->set_learning_rate(settings.learning_rate);
		}
		torch::load(model, model_save_folder + model_name + ".pt");
		model->to(device);
		model->eval();


		std::vector<torch::Tensor> features;
		std::vector<torch::Tensor> targets_dummy;


		Rule_Evaluator evaluator;
		evaluator.evaluate_notes(sheet);

		std::vector<Sheet_Music> sheet_vec = { sheet };
		Data_Loader loader(settings, false);
		if (model_name == "R2_from_two_sides_rule_targets")
			loader.evaluate_fux_rules_from_two_sides_rule_targets(sheet_vec, features, targets_dummy, settings.data_converter_info);
		else if (model_name == "R2_back_n_forth_rule_targets")
			loader.evaluate_fux_rules_back_n_forth_rule_targets(sheet_vec, features, targets_dummy, settings.data_converter_info);

		std::cout << "result: ";
		auto itr = sheet.get_cp().begin();
		for (auto& f : features)
		{
			auto t = model->forward(f.clone());
			Rule_Evaluation temp;
			if (t[-1].item<float>() > 0.5f)
				temp.broken_rules.push_back(Fux_Rule::R2);
			itr->add_note_info("AI_EVAL", Utility::to_str(temp));
			itr++;
		}
		std::cout << std::endl;

	} catch(std::exception& e)
	{
		std::cerr << e.what();
	}
	return;
}



void log_results(const std::string file_name, const std::vector<std::string>& times, const std::vector<int>& epochs, const std::vector<float>& test_losses, const std::vector<float>& train_losses)
{

	//std::replace(s.begin(), s.end(), ' ', '-');

	std::string file_location = Folder_Dialog::get_exe_path() + "/data/trainings_results/" + file_name + ".csv";

	std::pair< std::string, std::string> seperator = { "sep=", "," };

	std::string time_clmn = "time";
	std::string epoch_clmn = "epoch";
	std::string test_loss_clmn = "test_loss";
	std::string train_loss_clmn = "train_loss";


	std::ofstream csv_file(file_location);
	if (!csv_file)
		std::cerr << "\ncould not open debug log: " << file_location;

	csv_file << seperator.first << seperator.second << "\n";
	csv_file << time_clmn << seperator.second << epoch_clmn << seperator.second << train_loss_clmn << seperator.second << test_loss_clmn << "\n";


	if (times.size() != epochs.size() || epochs.size() != test_losses.size() || epochs.size() != train_losses.size())
	{
		std::cerr << "\nlog_results sizes do not match: times: " << times.size() << ", epochs: " << epochs.size() << ", losses: train " << train_losses.size() << ", test " << test_losses.size() ;
	}

	std::vector<std::string>::const_iterator time = times.begin();
	std::vector<int>::const_iterator epoch = epochs.begin();
	std::vector<float>::const_iterator test_loss = test_losses.begin();
	std::vector<float>::const_iterator train_loss = train_losses.begin();
	for (; time != times.end(); time++, epoch++, test_loss++, train_loss++)
	{
		csv_file << *time << seperator.second << *epoch << seperator.second << *train_loss << seperator.second << *test_loss << "\n";
	}
}

void save_prediction_difference(const std::string file_name, const std::vector<double>& prediction, const std::vector<double>& actual_value)
{
	std::string file_location = Folder_Dialog::get_exe_path() + "/data/trainings_results/" + file_name + ".csv";

	std::pair< std::string, std::string> seperator = { "sep=", ";" };

	std::string prediction_clmn = "prediction";
	std::string actual_value_clmn = "actual_value";

	std::ofstream csv_file(file_location);
	if (!csv_file)
		std::cerr << "\ncould not open debug log: " << file_location;

	csv_file << seperator.first << seperator.second << "\n";
	csv_file << prediction_clmn << seperator.second << actual_value_clmn << "\n";


	if (prediction.size() != actual_value.size())
	{
		std::cerr << "\nsave_prediction_difference sizes do not match: prediction: " << prediction_clmn.size() << ", actual_value: " << actual_value.size();
	}

	std::vector<double>::const_iterator pre = prediction.begin();
	std::vector<double>::const_iterator val = actual_value.begin();
	for (; pre != prediction.end(); pre++, val++)
	{
		csv_file << *pre << seperator.second << *val << "\n";
	}
}

void generate_cf()
{

}

std::string get_test_name(Eval::Learn_Settings settings)
{
	std::string test_case_name = settings.test_name
		+ "_hlc" + std::to_string(settings.hidden_layer_count)
		+ "_hls" + std::to_string(settings.hidden_size)
		+ "_opti_" + Utility::to_str(settings.optimizer)
		+ "_lossf_" + Utility::to_str(settings.loss_func)
		+ "_lr_" + std::to_string(settings.learning_rate);
	std::replace(test_case_name.begin(), test_case_name.end(), '.', '_');
	return test_case_name;
}

void Eval::AI_Evaluator::train_net(Learn_Settings settings)
{
	//std::cout.setstate(std::ios_base::failbit);
	try {
		std::cout << "\nTRAIN CF!";
		if (settings.data_converter == "equality_test")
		{
			test_on_equals(settings);
			return;
		}


		std::string model_save_folder = "data/trainings_results/models/" + settings.test_name;
		std::experimental::filesystem::create_directory(model_save_folder);
		
		// generate data set
		Dataset train_data_set(settings, Data_Type::TRAIN);
		Dataset test_data_set(settings, Data_Type::TEST);


		auto map_train_data_set = train_data_set.map(torch::data::transforms::Stack<>());
		auto map_test_data_set = test_data_set.map(torch::data::transforms::Stack<>());

		// generate data loader.
		auto train_data_loader = torch::data::make_data_loader<torch::data::samplers::SequentialSampler>(
			std::move(map_train_data_set),
			settings.batch_size);
		auto test_data_loader = torch::data::make_data_loader<torch::data::samplers::SequentialSampler>(
			std::move(map_test_data_set),
			settings.batch_size);

		//log progress
		std::vector<std::string> times;
		std::vector<int> epochs;
		std::vector<float> train_losses;
		std::vector<float> test_losses;

		std::string test_case_name = get_test_name(settings);


		std::shared_ptr<Eval::Net> model;
		if (settings.nn_type == NN_Type::LSTM)
		{
			model.reset(new LSTM(settings.in_size, settings.hidden_size, settings.out_size, settings.hidden_layer_count, settings.optimizer, settings.loss_func, settings.dropout, settings.is_bidirectional, device));
			model->set_learning_rate(settings.learning_rate);
		}
		else if (settings.nn_type == NN_Type::DENSE_NN)
		{
			model.reset(new NN(settings.in_size, settings.hidden_size, settings.out_size, settings.hidden_layer_count, settings.optimizer, settings.loss_func, settings.dropout, device));
			model->set_learning_rate(settings.learning_rate);
		}
		std::cout << "\nModel:\n";
		std::cout << model;


		std::cout << std::endl << "learning..." << std::endl;

		for (size_t epoch = 0; epoch < settings.epochs; epoch++)
		{
			std::cout << ".";
			if (epoch % 10 == 0)
			{
				float train_data_loss = 0;
				for (auto& batch : *train_data_loader)
				{
					if (settings.nn_type == NN_Type::LSTM)
						train_data_loss += model->test_prediction(batch.data.permute({ 2, 1, 0, 3 })[0], batch.target).item<float>() / train_data_set.loader.features_vec.size();
					else if (settings.nn_type == NN_Type::DENSE_NN)
						train_data_loss += model->test_prediction(batch.data, batch.target).item<float>() / train_data_set.loader.features_vec.size();
				}
				float test_data_loss = 0;

				for (auto& batch : *test_data_loader)
				{
					if (settings.nn_type == NN_Type::LSTM)
						test_data_loss += model->test_prediction(batch.data.permute({ 2, 1, 0, 3 })[0], batch.target).item<float>() / test_data_set.loader.features_vec.size();
					else if (settings.nn_type == NN_Type::DENSE_NN)
						test_data_loss += model->test_prediction(batch.data, batch.target).item<float>() / test_data_set.loader.features_vec.size();
				}
				//TODO should be set in Net
				//if ((epoch > 0)
				//	&& (epoch % 10 == 0))
				//{
				//	settings.learning_rate *= 0.1;
				//	std::cout << "new learning rate: " << settings.learning_rate;
				//	model->set_learning_rate(settings.learning_rate);
				//}
				//
				std::stringstream ss;
				ss << std::setw(3) << std::setfill('0') << epoch;
				std::string epoch_str = ss.str();

				std::cout << "\nepoch: " << epoch_str << ", Loss: train: " << 1000 * train_data_loss << ", test: " << 1000 * test_data_loss  << std::endl;

				torch::save(model, model_save_folder + "/" + epoch_str + "_epoch_" + test_case_name + ".pt");
				times.push_back(Utility::get_time_stamp());
				epochs.push_back(epoch);
				train_losses.push_back(train_data_loss);
				test_losses.push_back(test_data_loss);
				
				log_results(test_case_name, times, epochs, test_losses, train_losses);
				if (train_data_loss == 0.0f
					&& settings.loss_func == Loss_F::BCEL)
					break;

			}
			for (auto& batch : *train_data_loader)
			{
				if (settings.nn_type == NN_Type::LSTM)
					model->learn_step(batch.data.permute({ 2, 1, 0, 3 })[0], batch.target);
				else if (settings.nn_type == NN_Type::DENSE_NN)
					model->learn_step(batch.data, batch.target);
			}
		}
		//SAVE
		torch::save(model, model_save_folder + "/" + test_case_name + ".pt");
		torch::save(*model->get_optimizer(), model_save_folder + "/" + test_case_name + "opti" + ".pt");

		//log progress
		log_results(test_case_name, times, epochs, test_losses, train_losses);
	}
	catch
		(std::exception& e)
	{
		std::cerr << "\n" << e.what();
	}
	//std::cout.clear();
}



void Eval::AI_Evaluator::test_on_equals(Learn_Settings settings)
{
	//std::cout.setstate(std::ios_base::failbit);
	try {
		std::cout << "\nTest on equals!";

		bool use_string_cmp = false;
		bool use_704bit_cmp = true;

		if (use_string_cmp)
		{
			/*
			encodes a tensor like with strings
			(0000100
			 0011011
			 1111000)
			to
			(4,0),(1,1),(4,0),(2,1),(1,0),(6,1),(3,0)
			*/
			auto encode = [](const std::vector<at::Tensor>& features)
			{
				std::cout << "\nencoding...\n";
				std::vector<std::string> feature_vec;
				float percent_done = 0.0f;
				double avg_size = 0.0;
				double avg_pair_size = 0.0;
				for (const auto& tensor : features)
				{
					percent_done += (1.0f / features.size());
					if ((int)percent_done % 2 == 0)
						std::cout << "\r" << percent_done << " / 1.0 done                  ";

					std::vector<int> elem_vec(tensor.data<float>(), tensor.data<float>() + tensor.numel());
					int value = elem_vec[0];
					int counter = 1;
					std::string one_tensor_code;
					for (const auto& element : elem_vec)
					{
						if (value == element)
							counter++;
						else
						{
							one_tensor_code += std::to_string(counter) + ',';
							one_tensor_code += std::to_string(value) + ';';
							value = element;
							counter = 1;
							avg_pair_size += 1;
						}
					}
					avg_size += one_tensor_code.size();
					one_tensor_code.shrink_to_fit();
					feature_vec.push_back(one_tensor_code);
				}
				std::cout << "\r" << 1.0f << " / 1.0 done                  ";
				std::cout << "\navg string size: " << avg_size / features.size() << "\n";
				std::cout << "\navg pair size: " << avg_pair_size / features.size() << "\n";
				return std::move(feature_vec);
			};


			std::vector<std::string> train_codes;
			std::vector<std::string> test_codes;
			std::vector<std::string> valid_codes;
			// convert data sets
			{
				Dataset train_data_set(settings, Data_Type::TRAIN);
				train_codes = encode(train_data_set.loader.features_vec);
			}
			{
				Dataset test_data_set(settings, Data_Type::TEST);
				test_codes = encode(test_data_set.loader.features_vec);
			}
			{
				Dataset valid_data_set(settings, Data_Type::VALID);
				valid_codes = encode(valid_data_set.loader.features_vec);
			}
			/*
				compares two encoded tensors like:
				T1: (4,0),(1,1),(4,0),(2,1),(1,0),(6,1),(3,0)
				T2: (4,0),(1,1),(2,0),(6,1),(1,0),(6,1),(1,0)
				first by size of the pairs: in this case 7 == 7
				and then by values: in this case in pair 3 the
				function would return false 4 != 2;
			*/
			auto codes_equal = [](std::string lhs, std::string rhs)
			{
				if (lhs.size() != rhs.size())
					return false;
				if (std::strcmp(lhs.c_str(), rhs.c_str()) != 0)
					return false;

				return true;
			};
			//log progress
			int test_equals = 0;
			int valid_equals = 0;
			float percent_done = 0.0f;

			std::cout << "\ncomparing...\n";
			for (const auto& train_tensor : train_codes)
			{
				percent_done += (1.0f / train_codes.size());
				if ((int)percent_done % 2 == 0)
					std::cout << "\r" << percent_done << " / 1.0 done               ";
				for (const auto& test_tensor : test_codes)
				{
					if (codes_equal(train_tensor, test_tensor))
						test_equals++;
				}
				for (const auto& valid_tensor : valid_codes)
				{
					if (codes_equal(train_tensor, valid_tensor))
						valid_equals++;
				}
			}

			std::cout << "\r" << 1.0f << " / 1.0 done     ";

			//log result
			std::cout << "\ntest equals: " << test_equals
				<< "\nvalid equals: " << valid_equals << "\n";
		}
		else if (use_704bit_cmp)
		{
			auto encode = [](const std::vector<at::Tensor>& features, const std::vector<at::Tensor>& targets)
			{
				std::cout << "\nencoding...\n";
				std::vector<std::pair<std::vector<uint64_t>, std::vector<int>>> feature_vec;
				float percent_done = 0.0f;
				int index_counter = 0;
				for (auto tensor : features)
				{
					percent_done += (1.0f / features.size());
					if ((int)percent_done % 2 == 0)
						std::cout << "\r" << percent_done << " / 1.0 done                  ";
					std::vector<int> elem_vec(tensor.data<float>(), tensor.data<float>() + tensor.numel());

					int value = elem_vec[0];
					std::vector<uint64_t> one_tensor_code;
					long long code_snippet = 0;
					for (int i = 0, counter = 0; i < elem_vec.size(); i++, counter++)
					{
						if (counter >= 64)
						{
							counter = 0;
							one_tensor_code.push_back(code_snippet);
							code_snippet = 0;
						}
						if (elem_vec[i] == 0)
							code_snippet = code_snippet << 1;
						else {
							code_snippet = code_snippet << 1;
							code_snippet += 1;
						}
					}
					one_tensor_code.push_back(code_snippet);
					one_tensor_code.shrink_to_fit();
					
					auto target_tensor = targets[index_counter++];
					std::vector<int> target_elem_vec(target_tensor.data<float>(), target_tensor.data<float>() + target_tensor.numel());
					for (auto i : target_elem_vec)
						one_tensor_code.push_back(i);
					one_tensor_code.shrink_to_fit();

					feature_vec.push_back({ one_tensor_code, target_elem_vec });
				}
				std::cout << "\r" << 1.0f << " / 1.0 done                  ";
				return std::move(feature_vec);
			};


			std::vector<std::pair<std::vector<uint64_t>, std::vector<int>>> train_codes;
			std::vector<std::pair<std::vector<uint64_t>, std::vector<int>>> test_codes;
			std::vector<std::pair<std::vector<uint64_t>, std::vector<int>>> valid_codes;
			// convert data sets
			{
				Dataset train_data_set(settings, Data_Type::TRAIN);
				train_codes = encode(train_data_set.loader.features_vec, train_data_set.loader.targets_vec);
			}
			{
				Dataset test_data_set(settings, Data_Type::TEST);
				test_codes = encode(test_data_set.loader.features_vec, test_data_set.loader.targets_vec);
			}
			{
				Dataset valid_data_set(settings, Data_Type::VALID);
				valid_codes = encode(valid_data_set.loader.features_vec, valid_data_set.loader.targets_vec);
			}

			//sort vectors
			std::cout << "\nsorting...\n";
			auto comp256bit = [](const auto& lhs, const auto& rhs) {
				if (lhs.first.size() != rhs.first.size())
					std::cout << "\nError: encoding not equal\n";
				for (int i = 0; i < lhs.first.size(); i++)
				{
					if (lhs.first[i] == rhs.first[i])
						continue;
					return lhs.first[i] > rhs.first[i];
				}
				return false;
			};
			auto equ256bit = [](const auto& lhs, const auto& rhs) {
				if (lhs.first.size() != rhs.first.size())
					return false;
				for (int i = 0; i < lhs.first.size(); i++)
				{
					if (lhs.first[i] != rhs.first[i])
						return false;
				}
				return true;
			};

			auto sort_bit_codes = [&comp256bit](std::vector<std::pair<std::vector<uint64_t>, std::vector<int>>>& code) {
				std::sort(code.begin(), code.end(), comp256bit);
			};
			sort_bit_codes(train_codes);
			sort_bit_codes(test_codes);
			sort_bit_codes(valid_codes);

			//compare
			//two sets with all
			std::cout << "\ncomparing...\n";

			std::vector<std::pair<std::vector<uint64_t>, std::vector<int>>> train_test_equals(train_codes.size() + test_codes.size());
			auto itr_train_test = set_intersection(train_codes.begin(),
				train_codes.end(),
				test_codes.begin(),
				test_codes.end(),
				train_test_equals.begin(),
				comp256bit);
			train_test_equals.erase(std::unique(train_test_equals.begin(), train_test_equals.end(), equ256bit), train_test_equals.end());
			std::vector<std::pair<std::vector<uint64_t>, std::vector<int>>> train_valid_equals(train_codes.size() + valid_codes.size());
			auto itr_train_valid = set_intersection(train_codes.begin(),
				train_codes.end(),
				valid_codes.begin(),
				valid_codes.end(),
				train_valid_equals.begin(),
				comp256bit);
			train_valid_equals.erase(std::unique(train_valid_equals.begin(), train_valid_equals.end(), equ256bit), train_valid_equals.end());
			/*
			at this point
			train_test_equals and train_valid_equals
			contain all elements that were equal, but the elements are unique
			like 
			A: { 0, 3, 3, 4, 5 }
			B: { 2, 3, 3, 8, 9 }
			would become:
			C: { 3 }

			but we want all elements counted as often as they are duplicats:
			for A and B it should output that 4 elements are equal
			the first 3 of A with both 3s of B and the second 3 of A with both 3s of B

			so the next step is to count the elements
			*/
			std::cout << "\ncounting...\n";
			int unique_test_equals = 0, unique_valid_equals = 0;
			int test_equals_in_AB = 0, valid_equals_in_AB = 0;
			int test_equals_in_A = 0, valid_equals_in_A = 0;
			int test_equals_in_B = 0, valid_equals_in_B = 0;
			std::vector<int> unique_test_rules(9);
			std::vector<int> test_rules_in_AB(9);
			std::vector<int> test_rules_in_A(9);
			std::vector<int> test_rules_in_B(9);
			for (auto itr = train_test_equals.begin(); itr != train_test_equals.end(); itr++)
			{
				unique_test_equals++;
				int train_data_counter = 0;
				for (int i = 0; i < train_codes.size(); i++)
					if (equ256bit(*itr, train_codes[i]))
						train_data_counter++;

				int test_data_counter = 0;
				for (int i = 0; i < test_codes.size(); i++)
					if (equ256bit(*itr, test_codes[i]))
						test_data_counter++;
				test_equals_in_AB += train_data_counter * test_data_counter;
				test_equals_in_A += train_data_counter;
				test_equals_in_B += test_data_counter;
				for (int i = 0; i < itr->second.size(); i++)
				{
					unique_test_rules[i] += itr->second[i];
					test_rules_in_AB[i] += itr->second[i] * train_data_counter * test_data_counter;
					test_rules_in_A[i] += itr->second[i] * train_data_counter;
					test_rules_in_B[i] += itr->second[i] * test_data_counter;
				}
			}
			std::vector<int> unique_valid_rules(9);
			std::vector<int> valid_rules_in_AB(9);
			std::vector<int> valid_rules_in_A(9);
			std::vector<int> valid_rules_in_B(9);
			for (auto itr = train_valid_equals.begin(); itr != train_valid_equals.end(); itr++)
			{
				unique_valid_equals++;
				int train_data_counter = 0;
				for (int i = 0; i < train_codes.size(); i++)
					if (equ256bit(*itr, train_codes[i]))
						train_data_counter++;

				int valid_data_counter = 0;
				for (int i = 0; i < valid_codes.size(); i++)
					if (equ256bit(*itr, valid_codes[i]))
						valid_data_counter++;

				valid_equals_in_AB += train_data_counter * valid_data_counter;
				valid_equals_in_A += train_data_counter;
				valid_equals_in_B += valid_data_counter;

				for (int i = 0; i < itr->second.size(); i++)
				{
					unique_valid_rules[i] += itr->second[i];
					valid_rules_in_AB[i] += itr->second[i] * train_data_counter * valid_data_counter;
					valid_rules_in_A[i] += itr->second[i] * train_data_counter;
					valid_rules_in_B[i] += itr->second[i] * valid_data_counter;
				}
			}
			std::cout << "\ntest_equals:\n"
				<< "unique            : " << unique_test_equals << "\n"
				<< "in train * in test: " << test_equals_in_AB << "\n"
				<< "in train          : " << test_equals_in_A << "\n"
				<< "in test           : " << test_equals_in_B << "\n";

			std::cout << "\nunique:\n";
			for (int i = 0; i < unique_test_rules.size(); i++)
				std::cout << "R" << i + 1 << ": " << unique_test_rules[i] << ", ";
			std::cout << "\nin train * in test:\n";
			for (int i = 0; i < test_rules_in_AB.size(); i++)
				std::cout << "R" << i + 1 << ": " << test_rules_in_AB[i] << ", ";
			std::cout << "\nin train:\n";
			for (int i = 0; i < test_rules_in_A.size(); i++)
				std::cout << "R" << i + 1 << ": " << test_rules_in_A[i] << ", ";
			std::cout << "\nin test:\n";
			for (int i = 0; i < test_rules_in_B.size(); i++)
				std::cout << "R" << i + 1 << ": " << test_rules_in_B[i] << ", ";

			std::cout << "\n\nvalid_equals:\n"
				<< "unique             : " << unique_valid_equals << "\n"
				<< "in train * in valid: " << valid_equals_in_AB << "\n"
				<< "in train           : " << valid_equals_in_A << "\n"
				<< "in valid           : " << valid_equals_in_B << "\n";

			std::cout << "\nunique:\n";
			for (int i = 0; i < unique_valid_rules.size(); i++)
				std::cout << "R" << i + 1 << ": " << unique_valid_rules[i] << ", ";
			std::cout << "\nin train * in valid:\n";
			for (int i = 0; i < valid_rules_in_AB.size(); i++)
				std::cout << "R" << i + 1 << ": " << valid_rules_in_AB[i] << ", ";
			std::cout << "\nin train:\n";
			for (int i = 0; i < valid_rules_in_A.size(); i++)
				std::cout << "R" << i + 1 << ": " << valid_rules_in_A[i] << ", ";
			std::cout << "\nin valid:\n";
			for (int i = 0; i < valid_rules_in_B.size(); i++)
				std::cout << "R" << i + 1 << ": " << valid_rules_in_B[i] << ", ";
			std::cout << "\n\n";
		}
		else
		{
		auto encode = [](const std::vector<at::Tensor>& features, const std::vector<at::Tensor>& targets)
		{
			std::cout << "\nencoding...\n";
			std::vector<std::pair<std::vector<uint64_t>, std::vector<int>>> feature_vec;
			float percent_done = 0.0f;
			int index_counter = 0;
			for (auto tensor : features)
			{
				percent_done += (1.0f / features.size());
				if ((int)percent_done % 2 == 0)
					std::cout << "\r" << percent_done << " / 1.0 done                  ";
				std::vector<int> elem_vec(tensor.data<float>(), tensor.data<float>() + tensor.numel());

				int value = elem_vec[0];
				std::vector<uint64_t> one_tensor_code;
				long long code_snippet = 0;
				for (int i = 0, counter = 0; i < elem_vec.size(); i++, counter++)
				{
					if (counter >= 64)
					{
						counter = 0;
						one_tensor_code.push_back(code_snippet);
						code_snippet = 0;
					}
					if (elem_vec[i] == 0)
						code_snippet = code_snippet << 1;
					else {
						code_snippet = code_snippet << 1;
						code_snippet += 1;
					}
				}
				one_tensor_code.push_back(code_snippet);
				one_tensor_code.shrink_to_fit();

				auto target_tensor = targets[index_counter++];
				std::vector<int> target_elem_vec(target_tensor.data<float>(), target_tensor.data<float>() + target_tensor.numel());

				feature_vec.push_back({ one_tensor_code, target_elem_vec });
			}
			std::cout << "\r" << 1.0f << " / 1.0 done                  ";
			return std::move(feature_vec);
		};

		std::vector<std::pair<std::vector<uint64_t>, std::vector<int>>> train_codes;
		std::vector<std::pair<std::vector<uint64_t>, std::vector<int>>> test_codes;
		std::vector<std::pair<std::vector<uint64_t>, std::vector<int>>> valid_codes;
		// convert data sets
		{
			Dataset train_data_set(settings, Data_Type::TRAIN);
			train_codes = encode(train_data_set.loader.features_vec, train_data_set.loader.targets_vec);
		}
		{
			Dataset test_data_set(settings, Data_Type::TEST);
			test_codes = encode(test_data_set.loader.features_vec, test_data_set.loader.targets_vec);
		}
		{
			Dataset valid_data_set(settings, Data_Type::VALID);
			valid_codes = encode(valid_data_set.loader.features_vec, valid_data_set.loader.targets_vec);
		}

		auto codes_equal = [](std::pair<std::vector<uint64_t>, std::vector<int>> lhs, std::pair<std::vector<uint64_t>, std::vector<int>> rhs)
		{
			for (int i = 0; i < lhs.first.size(); i++)
			{
				if (lhs.first[i] != rhs.first[i])
					return false;
			}
			return true;
		};

		//log progress
		int test_equals = 0;
		int valid_equals = 0;
		float percent_done = 0.0f;
		std::vector<int> test_data_rules(9);
		std::vector<int> valid_data_rules(9);

		std::cout << "\ncomparing...\n";
		for (const auto& train_tensor : train_codes)
		{
			percent_done += (1.0f / train_codes.size());
			if ((int)percent_done % 2 == 0)
			{
				std::cout << "\rtest equal: " <<  percent_done << " / 1.0 done               ";
			}
			for (const auto& test_tensor : test_codes)
			{
				if (codes_equal(train_tensor, test_tensor))
				{
					for (int i = 0; i < train_tensor.second.size(); i++)
					{
						test_data_rules[i] += train_tensor.second[i];
					}
					test_equals++;
				}
			}
			for (const auto& valid_tensor : valid_codes)
			{
				if (codes_equal(train_tensor, valid_tensor))
				{
					for (int i = 0; i < train_tensor.second.size(); i++)
					{
						valid_data_rules[i] += train_tensor.second[i];
					}
					valid_equals++;
				}
			}
		}

		std::cout << "\r" << 1.0f << " / 1.0 done                                                                     ";

		//log result		

		std::cout << "\ntest equals: " << test_equals << "\n";
		for (int i = 0; i < test_data_rules.size(); i++)
			std::cout << "R" << i + 1 << ": " << test_data_rules[i] << ", ";

		std::cout << "\nvalid equals: " << valid_equals << "\n";
		for (int i = 0; i < valid_data_rules.size(); i++)
			std::cout << "R" << i + 1 << ": " << valid_data_rules[i] << ", ";
		std::cout << "\n";
		}
	}
	catch
		(std::exception& e)
	{
		std::cerr << "\n" << e.what();
	}
	//std::cout.clear();
}

void log_validation_results(const std::string file_name, const std::string model_name, int real_0_guess_0, int real_1_guess_0,int real_0_guess_1,int real_1_guess_1)
{
	std::string file_location = file_name + ".csv";

	std::pair<std::string, std::string> seperator = { "sep=", ";" };
	if (!std::experimental::filesystem::exists(file_location))
	{
		std::ofstream csv_file(file_location, std::fstream::app);
		if (!csv_file)
			std::cerr << "\ncould not open debug log: " << file_location;
		csv_file << "TN" << seperator.second << "FP" << seperator.second << "FN" << seperator.second << "TP" << seperator.second << "model_name" << seperator.second + "\n";
	}
	std::ofstream csv_file(file_location, std::fstream::app);
	if (!csv_file)
		std::cerr << "\ncould not open debug log: " << file_location;

	csv_file << real_0_guess_0 << seperator.second << real_0_guess_1 << seperator.second << real_1_guess_0 << seperator.second << real_1_guess_1 << seperator.second << model_name << "\n";
}


void Eval::AI_Evaluator::validate_net(const std::string& model_path, const std::string& settings_path, const std::string& save_path)
{
	try
	{ 
		////LOAD
		std::cout << "\n\nValidating\n";

		std::cout << "Evaluating with: " + model_path;
		Learn_Settings settings(settings_path);

		if (settings.data_converter_info == "ALL_RULES")
			return validate_all_rules_net(model_path, settings_path, save_path);

		if (settings.valid_data_folder == "null")
		{
			std::cerr << "\nSettings have no validation folder\n";
			return;
		}

		settings.batch_size = 1;
		std::cout << settings;
		std::shared_ptr<Eval::Net> model;
		if (settings.nn_type == NN_Type::LSTM)
		{
			model.reset(new LSTM(settings.in_size, settings.hidden_size, settings.out_size, settings.hidden_layer_count, settings.optimizer, settings.loss_func, settings.dropout, settings.is_bidirectional, device));
			model->set_learning_rate(settings.learning_rate);
		}
		else if (settings.nn_type == NN_Type::DENSE_NN)
		{
			model.reset(new NN(settings.in_size, settings.hidden_size, settings.out_size, settings.hidden_layer_count, settings.optimizer, settings.loss_func, settings.dropout, device));
			model->set_learning_rate(settings.learning_rate);
		}
		torch::load(model, model_path);
		model->to(device);
		model->eval();


		Data_Loader loader(settings, true, Data_Type::VALID);

		std::cout << "\nAi is working 0%";
		//real value / guess
		int real_0_guess_0 = 0;
		int real_1_guess_0 = 0;
		int real_0_guess_1 = 0;
		int real_1_guess_1 = 0;
		int counter = 0;
		for (int i = 0; i < loader.features_vec.size(); i++)
		{
			if (i % (loader.features_vec.size() / 10) == 0)
				std::cout << "\rAi is working " << counter++ * 10 << "%";
			auto t = model->forward(loader.features_vec[i].clone());
			if ((t[-1].item<float>() > 0.5f)
				&& (loader.targets_vec[i].item<float>() == 1.0))
			{
				real_1_guess_1++;
			}
			else if ((t[-1].item<float>() > 0.5f)
				&& (loader.targets_vec[i].item<float>() == 0.0))
			{
				real_0_guess_1++;
			}

			else if ((t[-1].item<float>() <= 0.5f)
				&& (loader.targets_vec[i].item<float>() == 1.0))
			{
				real_1_guess_0++;
			}

			else if ((t[-1].item<float>() <= 0.5f)
				&& (loader.targets_vec[i].item<float>() == 0.0))
			{
				real_0_guess_0++;
			}
		}
		std::cout << "\rAi is finished      ";

		if(save_path != "do_not_save")
			log_validation_results(save_path, model_path, real_0_guess_0, real_1_guess_0, real_0_guess_1, real_1_guess_1);

		std::cout << "\nvaldiation features: " << loader.features_vec.size()
			<< "\n\nResult:\n"
			<< "                                prediction"
			<< "\n                      rule broken  |   rule complied"
			<< "\n            broken   " << std::setw(11) << real_1_guess_1 << "   |   " << real_1_guess_0
			<< "\nactual value     --------------------------------------------"
			<< "\n          complied   " << std::setw(11) << real_0_guess_1 << "   |   " << real_0_guess_0
			<< "\n"
			<< "\nright " << real_0_guess_0 + real_1_guess_1 << ", wrong: " << real_0_guess_1 + real_1_guess_0 << "\n";

	}
	catch (std::exception& e)
	{
		std::cerr << e.what();
	}
	return;
}


void log_all_rules_validation_results(const std::string file_name, const std::string model_name, std::vector<std::vector<int>>& confusion_mat)
{
	/*
	confusion_mat:
	   TP TN FP FN
	R1 o  o  o  o
	:  :  :  :  :
	R9 o  o  o  o
	*/
	const int TP = 0, TN = 1, FP = 2, FN = 3;

	std::string file_location = file_name + ".csv";

	std::pair<std::string, std::string> seperator = { "sep=", ";" };
	if (!std::experimental::filesystem::exists(file_location))
	{
		std::ofstream csv_file(file_location, std::fstream::app);
		if (!csv_file)
			std::cerr << "\ncould not open debug log: " << file_location;
		for(int i = 1; i <= 9; i++)
		{
			csv_file << "R" << i << seperator.second
				<< "TN" << seperator.second
				<< "FP" << seperator.second
				<< "FN" << seperator.second
				<< "TP" << seperator.second;
		}
		csv_file << "model_name" << seperator.second + "\n";
		
	}
	std::ofstream csv_file(file_location, std::fstream::app);
	if (!csv_file)
		std::cerr << "\ncould not open debug log: " << file_location;
	for (int i = 1; i <= 9; i++)
	{
		csv_file << "R" << i << seperator.second
			<< confusion_mat[i - 1][TN] << seperator.second
			<< confusion_mat[i - 1][FP] << seperator.second
			<< confusion_mat[i - 1][FN] << seperator.second
			<< confusion_mat[i - 1][TP] << seperator.second;
	}
	csv_file << model_name << "\n";

	for (int i = 1; i <= 9; i++)
	{
		std::cout
			<< "R" << i << " ("
			<< "TN: " << confusion_mat[i - 1][TN] << ", "
			<< "FP: " << confusion_mat[i - 1][FP] << ", "
			<< "FN: " << confusion_mat[i - 1][FN] << ", "
			<< "TP: " << confusion_mat[i - 1][TP] << "),  ";
	}
}


void Eval::AI_Evaluator::validate_all_rules_net(const std::string& model_path, const std::string& settings_path, const std::string& save_path)
{
	try
	{
		////LOAD
		std::cout << "\n\nValidating\n";

		std::cout << "Evaluating with: " + model_path;
		Learn_Settings settings(settings_path);

		if (settings.valid_data_folder == "null")
		{
			std::cerr << "\nSettings have no validation folder\n";
			return;
		}

		settings.batch_size = 1;
		std::cout << settings;
		std::shared_ptr<Eval::Net> model;
		if (settings.nn_type == NN_Type::LSTM)
		{
			model.reset(new LSTM(
				settings.in_size, 
				settings.hidden_size, 
				settings.out_size, 
				settings.hidden_layer_count, 
				settings.optimizer, 
				settings.loss_func, 
				settings.dropout, 
				settings.is_bidirectional, 
				device));
			model->set_learning_rate(settings.learning_rate);
		}
		else if (settings.nn_type == NN_Type::DENSE_NN)
		{
			model.reset(new NN(
				settings.in_size, 
				settings.hidden_size, 
				settings.out_size, 
				settings.hidden_layer_count, 
				settings.optimizer, 
				settings.loss_func, 
				settings.dropout, 
				device));
			model->set_learning_rate(settings.learning_rate);
		}
		torch::load(model, model_path);
		model->to(device);
		model->eval();


		Data_Loader loader(settings, true, Data_Type::VALID);

		std::cout << "\nAi is working 0%";

		/* 
		Matrix: 
		   TP TN FP FN
		R1 o  o  o  o
		:  :  :  :  :
		R9 o  o  o  o 
		*/
		const int TP = 0, TN = 1, FP = 2, FN = 3;
		std::vector<std::vector<int>> confusion_mat(9, std::vector<int>(4));

		int counter = 0;
		 
		for (int i = 0; i < loader.features_vec.size(); i++)
		{
			if (i % (loader.features_vec.size() / 10) == 0)
				std::cout << "\rAi is working " << counter++ * 10 << "%";

			auto target_tensor = model->forward(loader.features_vec[i].clone());
			//last sequence element should be a 9 element tensor
			auto targets = target_tensor[-1];
			std::vector<float> prediction;
			for (int i = 0; i < targets.numel(); i++)
				prediction.push_back(targets[i].item<float>());
			at::Tensor real_vals;
			if (settings.nn_type == NN_Type::LSTM)
				real_vals = loader.targets_vec[i][0][0];
			else if (settings.nn_type == NN_Type::DENSE_NN)
				real_vals = loader.targets_vec[i][0];

			std::vector<float> real_values;
			for (int i = 0; i < real_vals.numel(); i++)
				real_values.push_back(real_vals[i].item<float>());
			//std::vector<float> real_values((float*)real_vals.data_ptr(), (float*)real_vals.data_ptr() + real_vals.numel());


			for (int rule_index = 0; rule_index < prediction.size(); rule_index++)
			{
				if ((prediction[rule_index] > 0.5f)
					&& (real_values[rule_index] == 1.0))
				{
					confusion_mat[rule_index][TP]++;
				}
				else if ((prediction[rule_index] > 0.5f)
					&& (real_values[rule_index] == 0.0))
				{
					confusion_mat[rule_index][FP]++;
				}
				else if ((prediction[rule_index] <= 0.5f)
					&& (real_values[rule_index] == 0.0))
				{
					confusion_mat[rule_index][TN]++;
				}
				else if ((prediction[rule_index] <= 0.5f)
					&& (real_values[rule_index] == 1.0))
				{
					confusion_mat[rule_index][FN]++;
				}
			}

		}
		std::cout << "\rAi is finished      ";

		if (save_path != "do_not_save")
			log_all_rules_validation_results(save_path, model_path, confusion_mat);
	}
	catch (std::exception& e)
	{
		std::cerr << e.what();
	}
	return;
}




//----------------------------- Testing ---------------------


Sheet_Music Eval::AI_Evaluator::testing()
{

	//std::vector<Sheet_Music> sheets = load_sheets("data/sheets/");

	//std::cout << sheets.back();

	//auto vec = convert_to_2d_vector(sheets.back());


	////load and convert trainings data
	//std::cout << "\nsheets count " << sheets.size();
	//std::vector<torch::Tensor> train_features;
	//std::vector<torch::Tensor> train_targets;

	//convert_to_cp_rule_learn_data_2(sheets, train_features, train_targets);

	//std::cout << train_features;

	//return sheets.back();
}