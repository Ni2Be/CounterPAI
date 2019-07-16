#include "LSTM_Eval.h"


#include <filesystem>
#include <fstream>
#include <vector>

#include "Sheet_Music.h"

Eval::LSTM_Eval::LSTM_Eval()
{


}



void Eval::LSTM_Eval::train()
{
	int in_size = 3, hidden_size = 10, out_size = 1;
	int batch_size = 10, sequence_size = 10;

	auto model = std::make_shared<Net>(Net(in_size, hidden_size, out_size));

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
			feature_vector.push_back(end_percentage);
			current_note++;
			if (current_note == target_c_scale_pitches.size() - 1)
				break;
		}

		//input (sequence, batch, features) 
		auto feature_tensor = torch::tensor(feature_vector, at::requires_grad(false).dtype(torch::kFloat32)).view({ static_cast<long>(cf.size()), 1, feature_count });
		features.push_back(feature_tensor);

		//targets -----------------------------------
		std::vector<float> target_vector;
		for (int i = 0; i < target_c_scale_pitches.size(); i++)
		{
			for (auto& one_hot : target_c_scale_pitches[i])
				target_vector.push_back(one_hot);
		}

		//output (sequence, batch, target_values) 
		auto taget_tensor = torch::tensor(target_vector, at::requires_grad(false).dtype(torch::kFloat32)).view({ static_cast<long>(cf.size()), 1, c_scale_note_count });
		targets.push_back(taget_tensor);
	}


	}
	catch
		(std::exception& e)
	{
		std::cerr << "\n" << e.what();
	}
}

void Eval::LSTM_Eval::train_cf()
{
	try {


	//std::cout.setstate(std::ios_base::failbit);
	std::cout << "\nTRAIN CF!";

	//load and convert trainings data
	//std::vector<Sheet_Music> sheets = load_sheets("data/trainings_data/train");
	std::vector<Sheet_Music> sheets = load_sheets("data/sheets");
	std::cout << "\sheets count " << sheets.size();
	std::vector<torch::Tensor> train_features;
	std::vector<torch::Tensor> train_targets;
	convert_to_cf_learn_data(sheets, train_features, train_targets);

	//load and convert test data
	//std::vector<Sheet_Music> test_sheets = load_sheets("data/trainings_data/test");
	std::vector<Sheet_Music> test_sheets = load_sheets("data/sheets");
	std::vector<torch::Tensor> test_features;
	std::vector<torch::Tensor> test_targets;
	convert_to_cf_learn_data(test_sheets, test_features, test_targets);



	//train net

	int in_size = 17 + 1, hidden_size = 64, out_size = 17;
	int batch_size = 1;

	auto model = std::make_shared<Net>(Net(in_size, hidden_size, out_size));
	model->set_learning_rate(0.9);


	//std::cout << std::endl << "Vor lernen: " << std::endl;
	//for (const auto& p : model->named_parameters())
	//{
	//	std::cout << p.key() << std::endl << p.value() << std::endl;
	//}

	//generate TEST cf
	//first input note
	std::vector<float> first_notes(in_size * 2, 0);
	//C
	first_notes[8] = 1.0f;
	first_notes[in_size - 1] = 0.0f;
	//D
	first_notes[in_size + 9] = 1.0f;
	first_notes[in_size * 2 - 1] = 0.1f;

	auto feature_tensor = torch::tensor(first_notes, at::requires_grad(false).dtype(torch::kFloat32)).view({ 2, 1, in_size });


	auto prediction = model->forward(feature_tensor);
	std::cout << std::endl << std::endl << "Prediction:\n" << prediction << std::endl;



	std::cout << std::endl << "loading model..." << std::endl;
	torch::load(model, "model_2_epoch_87000.pt");


	std::cout << std::endl << "learning..." << std::endl;

	//for (size_t epoch = 0; epoch < 1000000; epoch++)
	//{
	//	if (epoch % 1000 == 0)
	//	{
	//		float total_loss = 0.0f;
	//		for (int i = 0; i < test_features.size(); i++)
	//		{
	//			auto test_data_loss = model->test_prediction(test_features[i], test_targets[i]);
	//			total_loss += test_data_loss.item<float>() / (float)test_features.size();
	//		}

	//		std::cout << "epoch: " << epoch << ", Loss: " << std::sqrt(total_loss) << std::endl;
	//		torch::save(model, "model_2_epoch_" + std::to_string(epoch) + ".pt");
	//	}

	//	for(int i = 0; i < train_features.size(); i++)
	//	{
	//		model->learn_step(train_features[i], train_targets[i]);
	//	}

	//}


	//std::cout << std::endl << "Nach lernen: " << std::endl;
	//for (const auto& p : model->named_parameters())
	//{
	//	std::cout << p.key() << std::endl << p.value() << std::endl;
	//}





		auto predic = model->forward(feature_tensor);

		std::cout << std::endl << std::endl << "input_data :\n" << first_notes;
		std::cout << std::endl << std::endl << "Prediction normal:\n" << predic << std::endl;
		std::cout << std::endl << std::endl << "max:\n" << predic.max() << std::endl;
		std::cout << std::endl << std::endl << "argmax:\n" << predic.argmax() << std::endl;


	}
	catch
		(std::exception& e)
	{
		std::cerr << "\n" << e.what();
	}



	//std::cout.clear();
}


void Eval::LSTM_Eval::train_remember_one_cf()
{
	try {


		//std::cout.setstate(std::ios_base::failbit);
		std::cout << "\nTRAIN CF!";

		//load and convert trainings data
		//std::vector<Sheet_Music> sheets = load_sheets("data/trainings_data/train");

		std::ifstream ifs;
		std::string file = "data/trainings_data/remember_one_cf/cf.sheet";
		ifs.open(file);
		if (!ifs)
		{
			std::cerr << "\ncould not open " << file;
			return;
		}
		Sheet_Music temp_sheet;
		ifs >> temp_sheet;
		std::vector<Sheet_Music> sheets;
		sheets.push_back(temp_sheet);
		std::cout << "\sheets count " << sheets.size();
		std::vector<torch::Tensor> train_features;
		std::vector<torch::Tensor> train_targets;
		convert_to_cf_learn_data(sheets, train_features, train_targets);


		//train net

		int in_size = 17 + 1, hidden_size = 24, out_size = 17;
		int batch_size = 1;

		auto model = std::make_shared<Net>(Net(in_size, hidden_size, out_size, 2));
		model->set_learning_rate(0.9);

		std::cout << "\n\ntrain Features:\n" << train_features[0];
		std::cout << "\n\ntrain Targets:\n" <<  train_targets[0];


		auto prediction = model->forward(train_features[0]);
		std::cout << std::endl << std::endl << "Prediction:\n" << prediction << std::endl;



		std::cout << std::endl << "loading model..." << std::endl;
		torch::load(model, "model_remember_one_cf_hl2_hls24.pt");

		torch::Tensor feature_tensor = torch::zeros({ (int)sheets[0].get_cf().size(), batch_size, 18 });
		torch::Tensor target_tensor = torch::zeros({ (int)sheets[0].get_cf().size(), batch_size, 17 });

		for (int b = 0; b < batch_size; b++)
			for (int s = 0; s < sheets[0].get_cf().size(); s++)
				for (int i = 0; i < in_size; i++)
					feature_tensor[s][b][i] = train_features[0][s][0][i];
		for (int b = 0; b < batch_size; b++)
			for (int s = 0; s < sheets[0].get_cf().size(); s++)
				for (int i = 0; i < out_size; i++)
					target_tensor[s][b][i] = train_targets[0][s][0][i];

		std::cout << std::endl << "learning..." << std::endl;

		for (size_t epoch = 0; epoch < 1000000; epoch++)
		{
			if (epoch % 100 == 0)
			{
				float total_loss = 0.0f;
				for (int i = 0; i < train_features.size(); i++)
				{
					auto test_data_loss = model->test_prediction(feature_tensor, target_tensor);
					total_loss += test_data_loss.item<float>() / (float)train_features.size();
				}

				std::cout << "epoch: " << epoch << ", Loss: " << std::sqrt(total_loss) << std::endl;
				torch::save(model, "model_remember_one_cf_hl2_hls24.pt");
			}

			for(int i = 0; i < train_features.size(); i++)
			{
				model->learn_step(feature_tensor, target_tensor).cuda();
			}

		}
		auto predic = model->forward(train_features[0]);

		std::cout << std::endl << std::endl << "input_data :\n" << train_features[0];
		std::cout << std::endl << std::endl << "Prediction normal:\n" << predic << std::endl;
		std::cout << std::endl << std::endl << "max:\n" << predic.max() << std::endl;
		std::cout << std::endl << std::endl << "argmax:\n" << predic.argmax() << std::endl;


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