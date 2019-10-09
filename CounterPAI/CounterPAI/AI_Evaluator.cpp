#pragma once
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

#include "Rule_Evaluator.h"
#include "Rule_Evaluation.h"

#include "Net.h"
#include "LSTM.h"
#include "LNN.h"

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


		Rule_Evaluator evaluator;
		evaluator.evaluate_notes(sheet);

		std::vector<Sheet_Music> sheet_vec = { sheet };
		Data_Loader loader(settings, false);
		if (model_name == "R2_from_two_sides_rule_targets")
			loader.evaluate_fux_rules_from_two_sides_rule_targets(sheet_vec, features, targets_dummy, Eval::Fux_Rule::R2);
		else if (model_name == "R2_back_n_forth_rule_targets")
			loader.evaluate_fux_rules_back_n_forth_rule_targets(sheet_vec, features, targets_dummy, Eval::Fux_Rule::R2);

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

		std::string model_save_folder = "data/trainings_results/models/" + settings.test_name;
		std::experimental::filesystem::create_directory(model_save_folder);
		
		//Eval::Data_Loader data_loader(settings);


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
		else if (settings.nn_type == NN_Type::LNN)
		{
			model.reset(new LNN(settings.in_size, settings.hidden_size, settings.out_size, settings.hidden_layer_count, settings.optimizer, settings.loss_func, settings.dropout, device));
			model->set_learning_rate(settings.learning_rate);
		}
		std::cout << std::endl << "learning..." << std::endl;

		for (size_t epoch = 0; epoch < settings.epochs; epoch++)
		{
			std::cout << ".";
			if (epoch % 1 == 0)
			{

				float train_data_loss = 0;
				for (auto& batch : *train_data_loader)
					train_data_loss += model->test_prediction(batch.data.permute({ 2, 1, 0, 3 })[0], batch.target).item<float>() / train_data_set.loader.features_vec.size();

				float test_data_loss = 0;
				for (auto& batch : *test_data_loader)
					test_data_loss += model->test_prediction(batch.data.permute({ 2, 1, 0, 3 })[0], batch.target).item<float>() / test_data_set.loader.features_vec.size();

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
				model->learn_step(batch.data.permute({ 2, 1, 0, 3 })[0], batch.target);
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

void log_validation_results(const std::string file_name, const std::string model_name, int real_0_guess_0, int real_1_guess_0,int real_0_guess_1,int real_1_guess_1)
{

	std::string file_location = file_name + ".csv";

	std::pair<std::string, std::string> seperator = { "sep=", ";" };
	if (!std::experimental::filesystem::exists(file_location))
	{
		std::ofstream csv_file(file_location, std::fstream::app);
		if (!csv_file)
			std::cerr << "\ncould not open debug log: " << file_location;
		csv_file << "TP" << seperator.second << "TN" << seperator.second << "FN" << seperator.second << "FP" << seperator.second << "\n";
	}
	std::ofstream csv_file(file_location, std::fstream::app);
	if (!csv_file)
		std::cerr << "\ncould not open debug log: " << file_location;

	csv_file << real_1_guess_1 << seperator.second << real_1_guess_0 << seperator.second << real_0_guess_1 << seperator.second << real_0_guess_0 << seperator.second << model_name << "\n";
}


void Eval::AI_Evaluator::validate_net(const std::string& model_path, const std::string& settings_path, const std::string& save_path)
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
			model.reset(new LSTM(settings.in_size, settings.hidden_size, settings.out_size, settings.hidden_layer_count, settings.optimizer, settings.loss_func, settings.dropout, settings.is_bidirectional, device));
			model->set_learning_rate(settings.learning_rate);
		}
		else if (settings.nn_type == NN_Type::LNN)
		{
			model.reset(new LNN(settings.in_size, settings.hidden_size, settings.out_size, settings.hidden_layer_count, settings.optimizer, settings.loss_func, settings.dropout, device));
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