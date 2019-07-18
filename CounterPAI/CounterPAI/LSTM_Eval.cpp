#include "LSTM_Eval.h"


#include <filesystem>
#include <fstream>
#include <vector>
#include <iomanip>

#include "Sheet_Music.h"

#include "Debug_Log.h"
#include "Windows_Folder_Dialog.h"

#include "Utility.h"

#include "rapid_csv.h"
#include <rapidjson/document.h>
#include <rapidjson/istreamwrapper.h>


//----------------------------- LSTM -----------------------------

Eval::LSTM::LSTM(int64_t input, int64_t hidden, int64_t output, int64_t hidden_layer_count, Opti opti, Loss_F loss_f)
	:
	lstm(torch::nn::LSTMOptions(input, hidden).layers(hidden_layer_count)),
	s_optimizer(opti),
	s_loss_func(loss_f)
{
	out = register_module("out", torch::nn::Linear(hidden, output));
	register_module("lstm", lstm);


	if(s_optimizer == Opti::ADAM)
		optimizer = std::make_shared<torch::optim::Adam>(torch::optim::Adam(this->parameters(), s_learning_rate));
	else if(s_optimizer == Opti::SGD)
		optimizer = std::make_shared<torch::optim::SGD>(torch::optim::SGD(this->parameters(), s_learning_rate));
}

void Eval::LSTM::set_learning_rate(double learning_rate)
{
	s_learning_rate = learning_rate;
	optimizer.reset();
	if (s_optimizer == Opti::ADAM)
		optimizer = std::make_shared<torch::optim::Adam>(torch::optim::Adam(this->parameters(), s_learning_rate));
	else if (s_optimizer == Opti::SGD)
		optimizer = std::make_shared<torch::optim::SGD>(torch::optim::SGD(this->parameters(), s_learning_rate));
}

torch::Tensor Eval::LSTM::forward(torch::Tensor x)
{
	//input (sequence, batch, features) 
	torch::nn::RNNOutput lstm_out = lstm->forward(x);
	//output (seq_len, batch, num_directions * hidden_size)
	x = lstm_out.output;
	x = out->forward(x).sigmoid();
	return x;
}

torch::Tensor Eval::LSTM::learn_step(torch::Tensor learn_data, torch::Tensor target_data, bool optimize)
{
	try
	{
		this->zero_grad();
		auto new_prediction = this->forward(learn_data);

		torch::Tensor loss;
		if(s_loss_func == Loss_F::MSL)
			loss = torch::mse_loss(new_prediction, target_data);
		else if(s_loss_func == Loss_F::BCEL)
			loss = torch::binary_cross_entropy(new_prediction, target_data);
		if (optimize)
		{
			loss.backward();
			optimizer->step();
		}
		return loss;
	}
	catch (std::exception& e)
	{
		std::cout << "\n\nEXEPTION:\n" << e.what();
		char ch;
		std::cin >> ch;
	}
}

torch::Tensor Eval::LSTM::test_prediction(torch::Tensor test_data, torch::Tensor target_data)
{
	return learn_step(test_data, target_data, false);
}



//----------------------------- LSTM EVAL -----------------------------

Eval::LSTM_Eval::LSTM_Eval()
{
}

void Eval::LSTM_Eval::train()
{
	int in_size = 3, hidden_size = 10, out_size = 1;
	int batch_size = 10, sequence_size = 10;

	auto model = std::make_shared<LSTM>(LSTM(in_size, hidden_size, out_size, 1, LSTM::Opti::ADAM, LSTM::Loss_F::BCEL));

	model->set_learning_rate(0.04);

	//input (sequence, batch, features) 
	auto learn_data = torch::randn({ sequence_size, batch_size, in_size });
	auto test_data = torch::randn({ sequence_size, batch_size, in_size });

	std::vector<float> vec(batch_size * out_size * sequence_size, 0.33);
	assert(vec.size() == batch_size * out_size);
	auto target_data = torch::tensor(vec, at::requires_grad(false).dtype(torch::kFloat32)).view({ sequence_size, batch_size, out_size });

	std::cout << "learn_data :\n" << learn_data << std::endl << std::endl;
	std::cout << "target_data :\n" << target_data << std::endl << std::endl;


	std::cout << std::endl << "Vor lernen: " << std::endl;
	for (const auto& p : model->named_parameters())
	{
		std::cout << p.key() << std::endl << p.value() << std::endl;
	}

	auto prediction = model->forward(learn_data);
	std::cout << std::endl << std::endl << "Prediction:\n" << prediction << std::endl;

	std::cout << std::endl << "learning..." << std::endl;
	for (size_t epoch = 0; epoch < 1000; epoch++)
	{
		auto loss = model->learn_step(learn_data, target_data);

		if (epoch % 100 == 0)
		{
			auto test_data_loss = model->test_prediction(test_data, target_data);

			std::cout << "epoch: " << epoch << ", Loss: " << test_data_loss.item<float>() << std::endl;
			torch::save(model, "model.pt");
		}
	}


	std::cout << std::endl << "Nach lernen: " << std::endl;
	for (const auto& p : model->named_parameters())
	{
		std::cout << p.key() << std::endl << p.value() << std::endl;
	}


	prediction = model->forward(test_data);
	std::cout << std::endl << std::endl << "input_data :\n" << learn_data;
	std::cout << std::endl << std::endl << "target_data normal:\n" << *target_data[0][0][0].data<float>();
	std::cout << std::endl << std::endl << "Prediction normal:\n" << prediction << std::endl;
}




std::vector<Sheet_Music> load_sheets(const std::string folder)
{
	std::vector<Sheet_Music> sheets;
	for (auto& entry : std::experimental::filesystem::directory_iterator(folder))
	{
		std::ifstream ifs;
		ifs.open(entry.path().string());
		if (!ifs)
		{
			std::cerr << "\ncould not open " << entry.path().string();
			continue;
		}
		Sheet_Music temp_sheet;
		ifs >> temp_sheet;
		sheets.push_back(temp_sheet);
	}
	return sheets;
}


void convert_to_cf_learn_data(std::vector<Sheet_Music>& sheets, std::vector<torch::Tensor>& features, std::vector<torch::Tensor>& targets)
{
	try {

	const int c_scale_note_count = 17;
	int feature_count = c_scale_note_count + 1;
	for (auto& sheet : sheets)
	{
		auto cf = sheet.get_cf();

		Note_Pitch lowest_pitch;
		if (sheet.bass_is_cf)
			lowest_pitch = Note_Pitch::C2;
		else
			lowest_pitch = Note_Pitch::A3;


		float end_percentage = 0.0f;
		int cf_note_count = cf.size();
		int current_note = 0;

		//targets are the next note
		std::vector<std::vector<float>> target_c_scale_pitches(cf.size(), std::vector<float>(c_scale_note_count, 0.0f));


		for (auto& note : cf)
		{
			//targets
			target_c_scale_pitches[current_note][Music_Note::get_ACscale_distance(lowest_pitch, note.m_pitch)] = 1.0f;
			current_note++;
		}

		//features -----------------------------------
		current_note = 0;
		//first input is empty
		std::vector<float> feature_vector = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };
		for (auto& notes : target_c_scale_pitches)
		{
			//features
			feature_vector.insert(feature_vector.end(), notes.begin(), notes.end());

			//+2: the note before the last note should always return a 1.0f
			end_percentage = (float)(current_note + 2) / (float)cf_note_count;
			
			//TODO test result it is possible to train the LSTM without the endpercentage
			//feature_vector.push_back(0.0f);
			feature_vector.push_back(end_percentage);
			current_note++;
			if (current_note == target_c_scale_pitches.size() - 1)
				break;
		}

		//input (sequence, batch, features) 
		auto feature_tensor = torch::tensor(feature_vector, at::requires_grad(false).dtype(torch::kFloat32)).view({ static_cast<long>(cf.size()), 1, feature_count }).set_requires_grad(false);
		features.push_back(feature_tensor);

		//targets -----------------------------------
		std::vector<float> target_vector;
		for (int i = 0; i < target_c_scale_pitches.size(); i++)
		{
			for (auto& one_hot : target_c_scale_pitches[i])
				target_vector.push_back(one_hot);
		}

		//output (sequence, batch, target_values) 
		auto taget_tensor = torch::tensor(target_vector, at::requires_grad(false).dtype(torch::kFloat32)).view({ static_cast<long>(cf.size()), 1, c_scale_note_count }).set_requires_grad(false);
		targets.push_back(taget_tensor);
	}


	}
	catch
		(std::exception& e)
	{
		std::cerr << "\n" << e.what();
	}
}


void log_results(const std::string file_name, const std::vector<std::string>& times, const std::vector<int>& epochs, const std::vector<float>& losses)
{

	//std::replace(s.begin(), s.end(), ' ', '-');

	std::string file_location = Windows_File_Loader::get_exe_path() + "/data/trainings_results/" + file_name + ".csv";

	std::pair< std::string, std::string> seperator = { "sep=", ","};

	std::string time_clmn = "time";
	std::string epoch_clmn = "epoch";
	std::string loss_clmn = "loss";
	

	std::ofstream csv_file(file_location);
	if (!csv_file)
		std::cerr << "\ncould not open debug log: " << file_location;

	csv_file << seperator.first << seperator.second << "\n";
	csv_file << time_clmn << seperator.second << epoch_clmn << seperator.second << loss_clmn << "\n";


	if (times.size() != epochs.size() || epochs.size() != losses.size())
	{
		std::cerr << "\nlog_results sizes do not match: times: " << times.size() << ", epochs: " << epochs.size() << ", losses: " << losses.size();
	}

	std::vector<std::string>::const_iterator time = times.begin();
	std::vector<int>::const_iterator epoch = epochs.begin();
	std::vector<float>::const_iterator loss = losses.begin();
	for (;time != times.end(); time++, epoch++, loss++)
	{
		csv_file << *time << seperator.second << *epoch << seperator.second << *loss <<"\n";
	}
}


void Eval::LSTM_Eval::train_remember_one_cf(LSTM_Settings settings)
{
	//std::cout.setstate(std::ios_base::failbit);
	try {
		std::cout << "\nTRAIN CF!";

		//load and convert trainings data
		std::vector<Sheet_Music> sheets = load_sheets(settings.train_data_folder);
		std::cout << "\nsheets count " << sheets.size();
		std::vector<torch::Tensor> train_features;
		std::vector<torch::Tensor> train_targets;
		convert_to_cf_learn_data(sheets, train_features, train_targets);

		//load and convert test data
		std::vector<Sheet_Music> test_sheets = load_sheets(settings.test_data_folder);
		std::vector<torch::Tensor> test_features;
		std::vector<torch::Tensor> test_targets;
		convert_to_cf_learn_data(test_sheets, test_features, test_targets);




		//convert sheet tensor vectors to batch tensors
		torch::Tensor feature_tensor = torch::zeros({ (int)sheets[0].get_cf().size(), settings.batch_size, 18 });
		torch::Tensor target_tensor = torch::zeros({ (int)sheets[0].get_cf().size(), settings.batch_size, 17 });

		for (int b = 0; b < settings.batch_size; b++)
			for (int s = 0; s < sheets[0].get_cf().size(); s++)
				for (int i = 0; i < settings.in_size; i++)
					feature_tensor[s][b][i] = train_features[0][s][0][i];
		for (int b = 0; b < settings.batch_size; b++)
			for (int s = 0; s < sheets[0].get_cf().size(); s++)
				for (int i = 0; i < settings.out_size; i++)
					target_tensor[s][b][i] = train_targets[0][s][0][i];

		torch::Tensor test_feature_tensor = torch::zeros({ (int)sheets[0].get_cf().size(), settings.batch_size, 18 });
		torch::Tensor test_target_tensor = torch::zeros({ (int)sheets[0].get_cf().size(), settings.batch_size, 17 });

		for (int b = 0; b < settings.batch_size; b++)
			for (int s = 0; s < sheets[0].get_cf().size(); s++)
				for (int i = 0; i < settings.in_size; i++)
					test_feature_tensor[s][b][i] = test_features[0][s][0][i];
		for (int b = 0; b < settings.batch_size; b++)
			for (int s = 0; s < sheets[0].get_cf().size(); s++)
				for (int i = 0; i < settings.out_size; i++)
					test_target_tensor[s][b][i] = test_targets[0][s][0][i];


		//log progress
		std::vector<std::string> times;
		std::vector<int> epochs;
		std::vector<float> losses;

		std::string test_case_name = settings.test_name
			+ "_hlc" + std::to_string(settings.hidden_layer_count)
			+ "_hls" + std::to_string(settings.hidden_size)
			+ "_opti_" + Utility::to_str(settings.optimizer)
			+ "_lossf_" + Utility::to_str(settings.loss_func)
			+ "_lr_" + std::to_string(settings.learning_rate);

		//LSTM
		auto model = std::make_shared<LSTM>(LSTM(settings.in_size, settings.hidden_size, settings.out_size, settings.hidden_layer_count, settings.optimizer, settings.loss_func));
		model->set_learning_rate(settings.learning_rate);

		std::cout << std::endl << "learning..." << std::endl;
		for (size_t epoch = 0; epoch < settings.epochs; epoch++)
		{
			if (epoch % 10 == 0)
			{
				float total_loss = 0.0f;
				for (int i = 0; i < train_features.size(); i++)
				{
					auto test_data_loss = model->test_prediction(test_feature_tensor, test_target_tensor);
					total_loss += test_data_loss.item<float>() / (float)train_features.size();
				}

				std::cout << "epoch: " << epoch << ", Loss: " << total_loss << std::endl;
				torch::save(*model->optimizer, test_case_name + "opti" + ".pt");
				torch::save(model, test_case_name + ".pt");
				times.push_back(Utility::get_time_stamp());
				epochs.push_back(epoch);
				losses.push_back(total_loss);
			}

			for(int i = 0; i < train_features.size(); i++)
			{
				model->learn_step(feature_tensor, target_tensor);
			}
		}
		//SAVE
		torch::save(model, test_case_name + ".pt");
		torch::save(*model->optimizer, test_case_name + "opti" + ".pt");

		//LOAD
		torch::load(model, test_case_name + ".pt");
		torch::load(*model->optimizer, test_case_name + "opti" + ".pt");
		model->eval();
		auto predic = model->forward(test_features[0]);
		
		std::cout << std::endl << std::endl << "Prediction:\n"  << predic << std::endl;

		//log progress
		log_results(test_case_name, times, epochs, losses);
	}
	catch
		(std::exception& e)
	{
		std::cerr << "\n" << e.what();
	}
	//std::cout.clear();
}

void generate_cf()
{

}

//----------------------------- LSTM TEST RUNNER -----------------------------

Eval::LSTM_Settings::LSTM_Settings(const std::string& parameters_file)
{
	read_parameters(parameters_file);
}

void Eval::LSTM_Settings::read_parameters(const std::string& parameters_file_name)
{
	rapidjson::Document parameter;
	std::ifstream ifs(parameters_file_name);
	if (!ifs)
	{
		std::cerr << "\ncould not open: " + parameters_file_name + "\n";
		return;
	}
	rapidjson::IStreamWrapper isw(ifs);
	parameter.ParseStream(isw);

	test_name = parameter["name"].GetString();

	in_size = parameter["in_size"].GetInt();
	hidden_size = parameter["hidden_size"].GetInt();
	out_size = parameter["out_size"].GetInt();

	hidden_layer_count = parameter["hidden_layer_count"].GetInt();
	batch_size = parameter["batch_size"].GetInt();
	epochs = parameter["epochs"].GetInt();
	optimizer = opti_from_string(parameter["optimizer"].GetString());
	loss_func = loss_f_from_string(parameter["loss_func"].GetString());
	learning_rate = parameter["learning_rate"].GetFloat();
	train_data_folder = parameter["train_data_folder"].GetString();
	test_data_folder = parameter["test_data_folder"].GetString();
}



Eval::LSTM_Test_Runner::LSTM_Test_Runner()
{

}

void Eval::LSTM_Test_Runner::run_test(const std::string& test_file)
{
	LSTM_Settings settings(test_file);
	
	std::cout << settings;

	evaluator.train_remember_one_cf(settings);
}



std::ostream& Eval::operator<<(std::ostream& os, LSTM_Settings& test)
{
	os << "\nTest name: " << test.test_name 
		<< "\nin_size: " << test.in_size << ", hidden_size: " << test.hidden_size << ", out_size: " << test.out_size 
		<< "\nhidden_layer_count: " << test.hidden_layer_count << ", batch_size: " << test.batch_size << ", epochs: " << test.epochs
		<< "\noptimizer: " << test.optimizer << ", loss_func: " << test.loss_func 
		<< "\nlearning_rate: " << test.learning_rate 
		<< "\ntrain_data_folder: " << test.train_data_folder 
		<< "\ntest_data_folder: " << test.test_data_folder << "\n";
	return os;
}