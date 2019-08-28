//#include "LSTM_Eval.h"
//
//
//#include <filesystem>
//#include <fstream>
//#include <vector>
//#include <iomanip>
//#include <cassert>
//
//
//#include <algorithm>
//#include <random>
//
//#include "Sheet_Music.h"
//
//#include "Debug_Log.h"
//#include "Folder_Dialog.h"
//
//#include "Utility.h"
//
//#include "rapid_csv.h"
//
//#include "LSTM_Data_Loader.h"
//
//
//Eval::LSTM_Eval::LSTM_Eval()
//	:
//	device(torch::kCPU)
//{
//	if (torch::cuda::is_available()) {
//		std::cout << "CUDA is available! Training on GPU." << std::endl;
//		device = torch::kCUDA;
//	}
//	//NO CUDA SUPPORT
//	std::cout << "TESTING! NO CUDA SUPPORT." << std::endl;
//	device = torch::kCPU;
//}
//
//void log_results(const std::string file_name, const std::vector<std::string>& times, const std::vector<int>& epochs, const std::vector<float>& test_losses, const std::vector<float>& train_losses)
//{
//
//	//std::replace(s.begin(), s.end(), ' ', '-');
//
//	std::string file_location = Folder_Dialog::get_exe_path() + "/data/trainings_results/" + file_name + ".csv";
//
//	std::pair< std::string, std::string> seperator = { "sep=", ","};
//
//	std::string time_clmn = "time";
//	std::string epoch_clmn = "epoch";
//	std::string test_loss_clmn = "test_loss";
//	std::string train_loss_clmn = "train_loss";
//	
//
//	std::ofstream csv_file(file_location);
//	if (!csv_file)
//		std::cerr << "\ncould not open debug log: " << file_location;
//
//	csv_file << seperator.first << seperator.second << "\n";
//	csv_file << time_clmn << seperator.second << epoch_clmn << seperator.second << test_loss_clmn << seperator.second << train_loss_clmn << "\n";
//
//
//	if (times.size() != epochs.size() || epochs.size() != test_losses.size() || epochs.size() != train_losses.size())
//	{
//		std::cerr << "\nlog_results sizes do not match: times: " << times.size() << ", epochs: " << epochs.size() << ", losses: test " << test_losses.size() << ", train " << train_losses.size() ;
//	}
//
//	std::vector<std::string>::const_iterator time = times.begin();
//	std::vector<int>::const_iterator epoch = epochs.begin();
//	std::vector<float>::const_iterator test_loss = test_losses.begin();
//	std::vector<float>::const_iterator train_loss = train_losses.begin();
//	for (;time != times.end(); time++, epoch++, test_loss++, train_loss++)
//	{
//		csv_file << *time << seperator.second << *epoch << seperator.second << *test_loss << seperator.second << *train_loss <<"\n";
//	}
//}
//
//void generate_cf()
//{
//
//}
//
//void Eval::LSTM_Eval::train_net(Learn_Settings settings)
//{
//	//std::cout.setstate(std::ios_base::failbit);
//	try {
//		std::cout << "\nTRAIN CF!";
//		std::string model_save_folder = "data/trainings_results/models/";
//
//		LSTM_Data_Loader data_loader(settings);
//
//		//log progress
//		std::vector<std::string> times;
//		std::vector<int> epochs;
//		std::vector<float> train_losses;
//		std::vector<float> test_losses;
//
//		std::string test_case_name = settings.test_name
//			+ "_hlc" + std::to_string(settings.hidden_layer_count)
//			+ "_hls" + std::to_string(settings.hidden_size)
//			+ "_opti_" + Utility::to_str(settings.optimizer)
//			+ "_lossf_" + Utility::to_str(settings.loss_func)
//			+ "_lr_" + std::to_string(settings.learning_rate);
//		std::replace(test_case_name.begin(), test_case_name.end(), '.', ',');
//
//		//LSTM
//		auto model = std::make_shared<LSTM>(LSTM(settings.in_size, settings.hidden_size, settings.out_size, settings.hidden_layer_count, settings.optimizer, settings.loss_func, settings.dropout, device));
//		model->set_learning_rate(settings.learning_rate);
//
//		std::cout << std::endl << "learning..." << std::endl;
//		
//		for (size_t epoch = 0; epoch < settings.epochs; epoch++)
//		{
//			std::cout << ".";
//			if (epoch % 10 == 0)
//			{
//				auto train_batch = data_loader.get_train_batch();
//				torch::Tensor train_feature_tensor = std::get<0>(train_batch);
//				torch::Tensor train_target_tensor = std::get<1>(train_batch);
//				auto test_batch = data_loader.get_test_batch();
//				torch::Tensor test_feature_tensor = std::get<0>(test_batch);
//				torch::Tensor test_target_tensor = std::get<1>(test_batch);
//
//
//				auto train_data_loss = model->test_prediction(train_feature_tensor, train_target_tensor).item<float>();
//				auto test_data_loss = model->test_prediction(test_feature_tensor, test_target_tensor).item<float>();
//				
//
//				std::cout << "\nepoch: " << epoch << ", Loss: train: " << train_data_loss << ", test: " << test_data_loss << std::endl;
//				torch::save(*model->optimizer, model_save_folder + test_case_name + "_epoch_" + std::to_string(epoch) + "_opti" + ".pt");
//				torch::save(model, model_save_folder + test_case_name + ".pt");
//				times.push_back(Utility::get_time_stamp());
//				epochs.push_back(epoch);
//				train_losses.push_back(train_data_loss);
//				test_losses.push_back(test_data_loss);
//			}
//
//			auto train_batch = data_loader.get_train_batch();
//			torch::Tensor train_feature_tensor = std::get<0>(train_batch);
//			torch::Tensor train_target_tensor = std::get<1>(train_batch);
//
//			model->learn_step(train_feature_tensor, train_target_tensor);
//		}
//		//SAVE
//		torch::save(model, model_save_folder + test_case_name + ".pt");
//		torch::save(*model->optimizer, model_save_folder + test_case_name + "opti" + ".pt");
//
//		////LOAD
//		//torch::load(model, model_save_folder + test_case_name + ".pt");
//		//torch::load(*model->optimizer, model_save_folder + test_case_name + "opti" + ".pt");
//		//model->eval();
//
//		//log progress
//		log_results(test_case_name, times, epochs, test_losses, train_losses);
//	}
//	catch
//		(std::exception& e)
//	{
//		std::cerr << "\n" << e.what();
//	}
//	//std::cout.clear();
//}
//
//
//
//
////----------------------------- LSTM EVAL Testing ---------------------
//
//
//Sheet_Music Eval::LSTM_Eval::testing()
//{
//
//	//std::vector<Sheet_Music> sheets = load_sheets("data/sheets/");
//
//	//std::cout << sheets.back();
//
//	//auto vec = convert_to_2d_vector(sheets.back());
//
//
//	////load and convert trainings data
//	//std::cout << "\nsheets count " << sheets.size();
//	//std::vector<torch::Tensor> train_features;
//	//std::vector<torch::Tensor> train_targets;
//
//	//convert_to_cp_rule_learn_data_2(sheets, train_features, train_targets);
//
//	//std::cout << train_features;
//
//	//return sheets.back();
//}