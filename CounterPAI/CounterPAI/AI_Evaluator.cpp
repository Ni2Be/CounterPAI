#include "AI_Evaluator.h"

#include <filesystem>
#include <fstream>
#include <vector>
#include <iomanip>
#include <cassert>


#include <algorithm>
#include <random>

#include "Sheet_Music.h"

#include "Debug_Log.h"
#include "Folder_Dialog.h"

#include "Utility.h"

#include "rapid_csv.h"

#include "Data_Loader.h"

#include "Net.h"
#include "LSTM.h"
#include "LNN.h"

Eval::AI_Evaluator::AI_Evaluator()
	:
	device(torch::kCPU)
{
	if (torch::cuda::is_available()) {
		std::cout << "CUDA is available! Training on GPU." << std::endl;
		device = torch::kCUDA;
	}
	//NO CUDA SUPPORT
	std::cout << "TESTING! NO CUDA SUPPORT." << std::endl;
	device = torch::kCPU;
}


void Eval::AI_Evaluator::evaluate_notes(Sheet_Music& sheet)
{		
	try
	{
		////LOAD
		std::string model_save_folder = Folder_Dialog::get_exe_path() + "/data/trained/";
		std::string model_name = "evaluate_fux_rules_from_two_sides_6";
		std::cout << "Evaluating with: " + model_save_folder + model_name + ".pt";
		Learn_Settings settings(model_save_folder + model_name + ".json");
		settings.batch_size = 1;
		std::cout << settings;
		std::shared_ptr<Eval::Net> model;
		if (settings.nn_type == NN_Type::LSTM)
		{
			model.reset(new LSTM(settings.in_size, settings.hidden_size, settings.out_size, settings.hidden_layer_count, settings.optimizer, settings.loss_func, settings.dropout, device));
			model->set_learning_rate(settings.learning_rate);
		}
		else if (settings.nn_type == NN_Type::LNN)
		{
			model.reset(new LNN(settings.in_size, settings.hidden_size, settings.out_size, settings.hidden_layer_count, settings.optimizer, settings.loss_func, settings.dropout, device));
			model->set_learning_rate(settings.learning_rate);
		}
		torch::load(model, model_save_folder + model_name + ".pt");
		model->to(device);
		model->eval();


		std::vector<torch::Tensor> features;
		std::vector<torch::Tensor> targets_dummy;

		std::vector<Sheet_Music> sheet_vec = { sheet };
		if (settings.nn_type == NN_Type::LSTM)
		{
			Data_Loader loader(settings, false);
			loader.evaluate_fux_rules_from_two_sides_1(sheet_vec, features, targets_dummy);
		}

		std::cout << "result: ";
		auto itr = sheet.get_cp().begin();
		for (auto& f : features)
		{
			auto t = model->forward(f);
			itr->m_probability = t[8].item<float>();
			itr++;
			std::cout << t[8].item<float>() << ", ";
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
	csv_file << time_clmn << seperator.second << epoch_clmn << seperator.second << test_loss_clmn << seperator.second << train_loss_clmn << "\n";


	if (times.size() != epochs.size() || epochs.size() != test_losses.size() || epochs.size() != train_losses.size())
	{
		std::cerr << "\nlog_results sizes do not match: times: " << times.size() << ", epochs: " << epochs.size() << ", losses: test " << test_losses.size() << ", train " << train_losses.size();
	}

	std::vector<std::string>::const_iterator time = times.begin();
	std::vector<int>::const_iterator epoch = epochs.begin();
	std::vector<float>::const_iterator test_loss = test_losses.begin();
	std::vector<float>::const_iterator train_loss = train_losses.begin();
	for (; time != times.end(); time++, epoch++, test_loss++, train_loss++)
	{
		csv_file << *time << seperator.second << *epoch << seperator.second << *test_loss << seperator.second << *train_loss << "\n";
	}
}

void save_prediction_difference(const std::string file_name, const std::vector<double>& prediction, const std::vector<double>& actual_value)
{
	std::string file_location = Folder_Dialog::get_exe_path() + "/data/trainings_results/" + file_name + ".csv";

	std::pair< std::string, std::string> seperator = { "sep=", "," };

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
	std::replace(test_case_name.begin(), test_case_name.end(), '.', ',');
	return test_case_name;
}

void Eval::AI_Evaluator::train_net(Learn_Settings settings)
{
	//std::cout.setstate(std::ios_base::failbit);
	try {
		std::cout << "\nTRAIN CF!";
		std::string model_save_folder = "data/trainings_results/models/";

		Eval::Data_Loader data_loader(settings);

		//log progress
		std::vector<std::string> times;
		std::vector<int> epochs;
		std::vector<float> train_losses;
		std::vector<float> test_losses;

		std::string test_case_name = get_test_name(settings);


		std::shared_ptr<Eval::Net> model;
		if (settings.nn_type == NN_Type::LSTM)
		{
			model.reset(new LSTM(settings.in_size, settings.hidden_size, settings.out_size, settings.hidden_layer_count, settings.optimizer, settings.loss_func, settings.dropout, device));
			model->set_learning_rate(settings.learning_rate);
		}
		else if (settings.nn_type == NN_Type::LNN)
		{
			model.reset(new LNN(settings.in_size, settings.hidden_size, settings.out_size, settings.hidden_layer_count, settings.optimizer, settings.loss_func, settings.dropout, device));
			model->set_learning_rate(settings.learning_rate);
		}
		std::cout << std::endl << "learning..." << std::endl;

		for (size_t epoch = 0; epoch < settings.epochs; epoch++)
		{
			std::cout << ".";
			if (epoch % 10 == 0)
			{
				auto train_batch = data_loader.get_train_batch();
				torch::Tensor train_feature_tensor = std::get<0>(train_batch);
				torch::Tensor train_target_tensor = std::get<1>(train_batch);
				auto test_batch = data_loader.get_test_batch();
				torch::Tensor test_feature_tensor = std::get<0>(test_batch);
				torch::Tensor test_target_tensor = std::get<1>(test_batch);


				auto train_data_loss = model->test_prediction(train_feature_tensor, train_target_tensor).item<float>();
				auto test_data_loss = model->test_prediction(test_feature_tensor, test_target_tensor).item<float>();

				if (epoch == 1000)
				{
					std::cout << "new learning rate: " << 0.1 * settings.learning_rate;
					model->set_learning_rate(0.1 * settings.learning_rate);
				}
				std::cout << "\nepoch: " << epoch << ", Loss: train: " << train_data_loss << ", test: " << test_data_loss << std::endl;
				torch::save(*model->get_optimizer(), model_save_folder + test_case_name + "_epoch_" + std::to_string(epoch) + "_opti" + ".pt");
				torch::save(model, model_save_folder + test_case_name + ".pt");
				times.push_back(Utility::get_time_stamp());
				epochs.push_back(epoch);
				train_losses.push_back(train_data_loss);
				test_losses.push_back(test_data_loss);
			}

			auto train_batch = data_loader.get_train_batch();
			torch::Tensor train_feature_tensor = std::get<0>(train_batch);
			torch::Tensor train_target_tensor = std::get<1>(train_batch);

			model->learn_step(train_feature_tensor, train_target_tensor);
		}
		//SAVE
		torch::save(model, model_save_folder + test_case_name + ".pt");
		torch::save(*model->get_optimizer(), model_save_folder + test_case_name + "opti" + ".pt");

		////LOAD
		//torch::load(model, model_save_folder + test_case_name + ".pt");
		//torch::load(*model->optimizer, model_save_folder + test_case_name + "opti" + ".pt");
		//model->eval();

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


void Eval::AI_Evaluator::test_net(Learn_Settings settings)
{
	try
	{
		const int test_count = 5000;
		////LOAD

		std::string model_save_folder = Folder_Dialog::get_exe_path() + "/data/trainings_results/models/";
		std::string test_case_name = get_test_name(settings);
		std::string model_name = model_save_folder + test_case_name + ".pt";

		std::cout << "Test Net: " + model_name;
		settings.batch_size = 1;
		std::cout << "\n" << settings;
		std::shared_ptr<Eval::Net> model;
		if (settings.nn_type == NN_Type::LSTM)
		{
			model.reset(new LSTM(settings.in_size, settings.hidden_size, settings.out_size, settings.hidden_layer_count, settings.optimizer, settings.loss_func, settings.dropout, device));
			model->set_learning_rate(settings.learning_rate);
		}
		else if (settings.nn_type == NN_Type::LNN)
		{
			model.reset(new LNN(settings.in_size, settings.hidden_size, settings.out_size, settings.hidden_layer_count, settings.optimizer, settings.loss_func, settings.dropout, device));
			model->set_learning_rate(settings.learning_rate);
		}
		torch::load(model, model_name);
		model->to(device);
		model->eval();

		Eval::Data_Loader data_loader(settings);

		std::vector<double> predictions;
		std::vector<double> values;
		for (int i = 0; i < test_count; i++)
		{
			auto test_batch = data_loader.get_test_batch();
			torch::Tensor test_feature_tensor = std::get<0>(test_batch);
			torch::Tensor test_target_tensor = std::get<1>(test_batch);

			auto t = model->forward(test_feature_tensor);
			predictions.push_back(t[8].item<float>());
			values.push_back(test_target_tensor[8].item<float>());
		}
		save_prediction_difference(test_case_name + "_predictions", predictions, values);

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