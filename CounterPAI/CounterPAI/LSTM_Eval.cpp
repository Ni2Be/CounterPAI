#include "LSTM_Eval.h"


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
//
//void Eval::LSTM_Eval::train()
//{
//	int in_size = 3, hidden_size = 10, out_size = 1;
//	int batch_size = 10, sequence_size = 10;
//
//	auto model = std::make_shared<LSTM>(LSTM(in_size, hidden_size, out_size, 1, LSTM::Opti::ADAM, LSTM::Loss_F::BCEL));
//
//	model->set_learning_rate(0.04);
//
//	//input (sequence, batch, features) 
//	auto learn_data = torch::randn({ sequence_size, batch_size, in_size });
//	auto test_data = torch::randn({ sequence_size, batch_size, in_size });
//
//	std::vector<float> vec(batch_size * out_size * sequence_size, 0.33);
//	assert(vec.size() == batch_size * out_size);
//	auto target_data = torch::tensor(vec, at::requires_grad(false).dtype(torch::kFloat32)).view({ sequence_size, batch_size, out_size });
//
//	std::cout << "learn_data :\n" << learn_data << std::endl << std::endl;
//	std::cout << "target_data :\n" << target_data << std::endl << std::endl;
//
//
//	std::cout << std::endl << "Vor lernen: " << std::endl;
//	for (const auto& p : model->named_parameters())
//	{
//		std::cout << p.key() << std::endl << p.value() << std::endl;
//	}
//
//	auto prediction = model->forward(learn_data);
//	std::cout << std::endl << std::endl << "Prediction:\n" << prediction << std::endl;
//
//	std::cout << std::endl << "learning..." << std::endl;
//	for (size_t epoch = 0; epoch < 1000; epoch++)
//	{
//		auto loss = model->learn_step(learn_data, target_data);
//
//		if (epoch % 100 == 0)
//		{
//			auto test_data_loss = model->test_prediction(test_data, target_data);
//
//			std::cout << "epoch: " << epoch << ", Loss: " << test_data_loss.item<float>() << std::endl;
//			torch::save(model, "model.pt");
//		}
//	}
//
//
//	std::cout << std::endl << "Nach lernen: " << std::endl;
//	for (const auto& p : model->named_parameters())
//	{
//		std::cout << p.key() << std::endl << p.value() << std::endl;
//	}
//
//
//	prediction = model->forward(test_data);
//	std::cout << std::endl << std::endl << "input_data :\n" << learn_data;
//	std::cout << std::endl << std::endl << "target_data normal:\n" << *target_data[0][0][0].data<float>();
//	std::cout << std::endl << std::endl << "Prediction normal:\n" << prediction << std::endl;
//}


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

	const int percentage_to_end = 1;
	const int c_scale_note_count = 17;
	int feature_count = c_scale_note_count + percentage_to_end;
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


void log_results(const std::string file_name, const std::vector<std::string>& times, const std::vector<int>& epochs, const std::vector<float>& test_losses, const std::vector<float>& train_losses)
{

	//std::replace(s.begin(), s.end(), ' ', '-');

	std::string file_location = Folder_Dialog::get_exe_path() + "/data/trainings_results/" + file_name + ".csv";

	std::pair< std::string, std::string> seperator = { "sep=", ","};

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
		std::cerr << "\nlog_results sizes do not match: times: " << times.size() << ", epochs: " << epochs.size() << ", losses: test " << test_losses.size() << ", train " << train_losses.size() ;
	}

	std::vector<std::string>::const_iterator time = times.begin();
	std::vector<int>::const_iterator epoch = epochs.begin();
	std::vector<float>::const_iterator test_loss = test_losses.begin();
	std::vector<float>::const_iterator train_loss = train_losses.begin();
	for (;time != times.end(); time++, epoch++, test_loss++, train_loss++)
	{
		csv_file << *time << seperator.second << *epoch << seperator.second << *test_loss << seperator.second << *train_loss <<"\n";
	}
}



void generate_cf()
{

}


//EVALUATE FUX RULES
std::vector<std::vector<float>> convert_to_2d_vector(Sheet_Music& sheet)
{
	std::vector<std::vector<float>> sheet_vec;


	/*
	converts sheet music to a 2d vector:
	cp: C4 -> B4 (two halfs not sharp, not flat, not tied)
	probability
	new notes
	cf: C3 (one whole)
	new notes
	cf is bass: true

	(all notes whould be represented, here only 5 per voice)
	C4         |11110000| ...
	B4         |00001111| ...
	A4         |00000000| ...
	G3         |00000000| ...
	F3         |00000000| ...
	sharp      |00000000| ...
	flat       |00000000| ...
	tied       |00000000| ...
	new note   |10001000| ...
	F3         |00000000| ...
	E3         |00000000| ...
	D3         |00000000| ...
	C3         |11111111| ...
	B3         |00000000| ...
	new note   |10000000| ...
	cf is bass |11111111| ... 
	probability|11110000| ... (floats)
	*/

	//slice size
	//cp
	const int f_cp_c_scale_note_count = 17;
	const int f_cp_is_sharp = 1;
	const int f_cp_is_flat = 1;
	const int f_cp_is_tied = 1;
	const int f_cp_is_new_note = 1;
	//cf
	const int f_cf_c_scale_note_count = 17;
	const int f_cf_is_new_note = 1;
	//all
	const int f_cf_is_bass = 1;
	const int f_cp_probability = 1;
	int slice_size = f_cp_c_scale_note_count + f_cp_is_sharp + f_cp_is_flat + f_cp_is_tied  + f_cp_is_new_note + f_cf_c_scale_note_count + f_cf_is_new_note + f_cf_is_bass + f_cp_probability;


	//convert
	auto cf = sheet.get_cf();
	auto cp = sheet.get_cp();

	Note_Pitch lowest_pitch_cf;
	Note_Pitch lowest_pitch_cp;
	Voice cf_voice;
	Voice cp_voice;
	bool bass_is_cf = false;
	if (sheet.bass_is_cf)
	{
		cf_voice = Voice::Bass;
		cp_voice = Voice::Soprano;
		lowest_pitch_cf = Note_Pitch::C2;
		lowest_pitch_cp = Note_Pitch::A3;
		bass_is_cf = true;
	}
	else
	{
		cf_voice = Voice::Soprano;
		cp_voice = Voice::Bass;
		lowest_pitch_cf = Note_Pitch::A3;
		lowest_pitch_cp = Note_Pitch::C2;
	}
	int cf_note_count = cf.size();
	int eights_count = cf_note_count * 8;


	//features -----------------------------------
	int current_cf_note = 0;
	float probability = 0.0f;
	//the distance is needed as sixteenth for sheet get_note()
	for (int sixteenths = 0; sixteenths < eights_count * 2; sixteenths += 2)
	{
		std::vector<float> cp_slice;
		bool cp_was_at_note = false;
		Music_Note cp_note = sheet.get_note(cp_voice, sixteenths, cp_was_at_note);
		//f_cp_c_scale_note_count = 17;
		std::vector<float> cp_c_scale_pitches(f_cp_c_scale_note_count, 0.0f);

		cp_c_scale_pitches[16 - Music_Note::get_ACscale_distance(lowest_pitch_cp, cp_note.get_basic_note())] = 1.0f;


		//f_cp_is_sharp = 1;
		bool cp_is_sharp = cp_note.m_is_sharp;
		//f_cp_is_flat = 1;
		bool cp_is_flat = cp_note.m_is_flat;
		//f_cp_is_tied = 1;
		bool cp_is_tied = cp_note.m_is_tied;
		//f_cp_is_new_note = 1;
		bool cp_is_new_note = false;
		if (cp_was_at_note)
		{
			probability = cp_note.m_probability;
			cp_is_new_note = true;
		}
		bool cf_was_at_note = false;
		Music_Note cf_note = sheet.get_note(cf_voice, sixteenths, cf_was_at_note);
		//f_cf_c_scale_note_count = 17;
		std::vector<float> cf_c_scale_pitches(f_cf_c_scale_note_count, 0.0f);
		cf_c_scale_pitches[16 - Music_Note::get_ACscale_distance(lowest_pitch_cf, cf_note.get_basic_note())] = 1.0f;
		//f_cf_is_new_note = 1;
		bool cf_is_new_note = false;
		if (cf_was_at_note)
		{
			current_cf_note++;
			cf_is_new_note = true;
		}

		//f_cf_is_bass = 1;
		bool cf_is_bass = bass_is_cf;
		cp_slice.insert(cp_slice.end(), cp_c_scale_pitches.begin(), cp_c_scale_pitches.end());
		cp_slice.push_back((float)cp_is_sharp);
		cp_slice.push_back((float)cp_is_flat);
		cp_slice.push_back((float)cp_is_tied);
		cp_slice.push_back((float)cp_is_new_note);
		cp_slice.insert(cp_slice.end(), cf_c_scale_pitches.begin(), cf_c_scale_pitches.end());
		cp_slice.push_back((float)cf_is_new_note);
		cp_slice.push_back((float)cf_is_bass);
		cp_slice.push_back((float)probability);
		cp_slice.shrink_to_fit();
		sheet_vec.push_back(cp_slice);
	}


	return sheet_vec;
}

void convert_to_cp_rule_learn_data_1(std::vector<Sheet_Music>& sheets, std::vector<torch::Tensor>& features, std::vector<torch::Tensor>& targets)
{
	try {
		/*
		all notes get convertet to eight note slices
		 12345678
		|--------|
		|-----y--|
		|x-------|
		 ^
		 each slice as one input 
		if x is played on 1 (f_cp_is_new_note input == false) it would also be played at 2 3 4 5 (f_cp_is_new_note input == false)
		the target probability is allways the same as for x at 1
		*/

		//features
		//cp
		const int f_cp_c_scale_note_count = 17;
		const int f_cp_is_sharp = 1;
		const int f_cp_is_flat = 1;
		const int f_cp_is_tied = 1;
		const int f_cp_is_new_note = 1;
		//cf
		const int f_cf_c_scale_note_count = 17;
		const int f_cf_is_new_note = 1;
		//all
		const int f_percentage_to_end = 1;
		const int f_cf_is_bass = 1;
		int feature_count = f_cp_c_scale_note_count + f_cp_is_sharp + f_cp_is_flat + f_cp_is_tied + f_cp_is_new_note + f_cf_c_scale_note_count + f_cf_is_new_note + f_percentage_to_end + f_cf_is_bass;

		//targets
		const int t_note_probability = 1;
		int target_count = t_note_probability;


		for (auto& sheet : sheets)
		{
			auto cf = sheet.get_cf();
			auto cp = sheet.get_cp();

			Note_Pitch lowest_pitch_cf;
			Note_Pitch lowest_pitch_cp;
			Voice cf_voice;
			Voice cp_voice;
			bool is_bass_cf = false;
			if (sheet.bass_is_cf)
			{
				cf_voice = Voice::Bass;
				cp_voice = Voice::Soprano;
				lowest_pitch_cf = Note_Pitch::C2;
				lowest_pitch_cp = Note_Pitch::A3;
				is_bass_cf = true;
			}
			else
			{
				cf_voice = Voice::Soprano;
				cp_voice = Voice::Bass;
				lowest_pitch_cf = Note_Pitch::A3;
				lowest_pitch_cp = Note_Pitch::C2;
			}
			float end_percentage = 0.0f;
			int cf_note_count = cf.size();
			int eights_count = cf_note_count * 8;


			//targets are the probabilitys of the cp notes
			std::vector<float> target_probabilitys;
			for (auto& note : cp)
			{
				//targets (for a whole note fill in 8, half 4 etc.)
				for (int i = 0; i < (8 / static_cast<int>(note.m_value)); i++)
					target_probabilitys.push_back(note.m_probability);
			}
			assert(target_probabilitys.size() == eights_count);

			//features -----------------------------------
			int current_cf_note = 0;
			std::vector<float> feature_vector;
			//the distance is needed as sixteenth for sheet get_note()
			for (int sixteenths = 0; sixteenths < eights_count * 2; sixteenths += 2)
			{
				bool cp_was_at_note = false;
				Music_Note cp_note = sheet.get_note(cp_voice, sixteenths, cp_was_at_note);
				//f_cp_c_scale_note_count = 17;
				std::vector<float> cp_c_scale_pitches(f_cp_c_scale_note_count, 0.0f);

				cp_c_scale_pitches[Music_Note::get_ACscale_distance(lowest_pitch_cp, cp_note.get_basic_note())] = 1.0f;
				//f_cp_is_sharp = 1;
				bool cp_is_sharp = cp_note.m_is_sharp;
				//f_cp_is_flat = 1;
				bool cp_is_flat = cp_note.m_is_flat;
				//f_cp_is_tied = 1;
				bool cp_is_tied = cp_note.m_is_tied;
				//f_cp_is_new_note = 1;
				bool cp_is_new_note = false;
				if (cp_was_at_note)
					cp_is_new_note = true;

				bool cf_was_at_note = false;
				Music_Note cf_note = sheet.get_note(cf_voice, sixteenths, cf_was_at_note);
				//f_cf_c_scale_note_count = 17;
				std::vector<float> cf_c_scale_pitches(f_cf_c_scale_note_count, 0.0f);
				cf_c_scale_pitches[Music_Note::get_ACscale_distance(lowest_pitch_cf, cf_note.get_basic_note())] = 1.0f;
				//f_cf_is_new_note = 1;
				bool cf_is_new_note = false;
				if (cf_was_at_note)
				{
					current_cf_note++;
					cf_is_new_note = true;
				}
				//f_percentage_to_end = 1;
				float percentage_to_end = (float)(sixteenths) / (float)(eights_count * 2.0f);
				//f_cf_is_bass = 1;
				bool cf_is_bass = is_bass_cf;

				feature_vector.insert(feature_vector.end(), cp_c_scale_pitches.begin(), cp_c_scale_pitches.end());
				feature_vector.push_back((float)cp_is_sharp);
				feature_vector.push_back((float)cp_is_flat);
				feature_vector.push_back((float)cp_is_tied);
				feature_vector.push_back((float)cp_is_new_note);
				feature_vector.insert(feature_vector.end(), cf_c_scale_pitches.begin(), cf_c_scale_pitches.end());
				feature_vector.push_back((float)cf_is_new_note);
				feature_vector.push_back((float)percentage_to_end);
				feature_vector.push_back((float)cf_is_bass);
			}




			//input (sequence, batch, features) 
			auto feature_tensor = torch::tensor(feature_vector, at::requires_grad(false).dtype(torch::kFloat32)).view({ eights_count, 1, feature_count });
			features.push_back(feature_tensor);

			//targets -----------------------------------
			std::vector<float> target_vector = target_probabilitys;
			//for (int i = 0; i < target_probabilitys.size(); i++)
			//{
			//	for (auto& one_hot : target_probabilitys[i])
			//		target_vector.push_back(one_hot);
			//}

			//output (sequence, batch, target_values) 
			auto taget_tensor = torch::tensor(target_vector, at::requires_grad(false).dtype(torch::kFloat32)).view({ eights_count, 1, target_count });
			targets.push_back(taget_tensor);
		}
	}
	catch
		(std::exception& e)
	{
		std::cerr << "\n" << e.what();
	}
}

void convert_to_cp_rule_learn_data_2(std::vector<Sheet_Music>& sheets, std::vector<torch::Tensor>& features, std::vector<torch::Tensor>& targets)
{
	try {
		/*
		all notes get convertet to eight note slices
		 12345678
		|--------|--------|--------|
		|--------|-xxxxx--|--------|
		|--------|--------|--------|
		  |-----------------|
		   -------> <-------
		      one sequence:
			  input is one slice from the left side + one from the right side
			  input for the note looked at is 2 times the note looked at slice

			  targets are 1.0 for each but the note looked at 
			  (could be interpreted as "allways assume the other notes are right")
			  target for the note looked at is the probability of that note

		if x is played on 1 (f_cp_is_new_note input == false) it would also be played at 2 3 4 5 (f_cp_is_new_note input == false)
		the target probability is allways the same as for x at 1
		*/

		//features
		//cp 
		//left
		const int f_cp_c_scale_note_count_l = 17;
		const int f_cp_is_sharp_l = 1;
		const int f_cp_is_flat_l = 1;
		const int f_cp_is_tied_l = 1;
		const int f_cp_is_new_note_l = 1;
		//right
		const int f_cp_c_scale_note_count_r = 17;
		const int f_cp_is_sharp_r = 1;
		const int f_cp_is_flat_r = 1;
		const int f_cp_is_tied_r = 1;
		const int f_cp_is_new_note_r = 1;
		//cf
		//left
		const int f_cf_c_scale_note_count_l = 17;
		const int f_cf_is_new_note_l = 1;
		//right
		const int f_cf_c_scale_note_count_r = 17;
		const int f_cf_is_new_note_r = 1;
		//all
		//const int f_is_looked_at_note = 1;
		const int f_cf_is_bass = 1;
		int feature_count = 
			  f_cp_c_scale_note_count_l + f_cp_is_sharp_l + f_cp_is_flat_l + f_cp_is_tied_l + f_cp_is_new_note_l
			+ f_cp_c_scale_note_count_l + f_cp_is_sharp_l + f_cp_is_flat_l + f_cp_is_tied_l + f_cp_is_new_note_l 
			+ f_cf_c_scale_note_count_l + f_cf_is_new_note_l
			+ f_cf_c_scale_note_count_r + f_cf_is_new_note_r
			+ f_cf_is_bass;

		//targets
		const int t_note_probability = 1;
		int target_count = t_note_probability;

		for (auto& sheet : sheets)
		{
			auto sheet_vec = convert_to_2d_vector(sheet);
			//insert eight 0 vectors to front and back 
			std::vector<float> zeros(sheet_vec.front().size(), 0.0f);
			for (int i = 0; i < 8; i++)
				sheet_vec.insert(sheet_vec.begin(), zeros);
			for (int i = 0; i < 8; i++)
				sheet_vec.insert(sheet_vec.end(), zeros);

			//for each new note create a sequence
			for (int i = 0; i < sheet_vec.size(); i++)
			{
				/*
				int feature_count = 
				  f_cp_c_scale_note_count_l + f_cp_is_sharp_l + f_cp_is_flat_l + f_cp_is_tied_l + f_cp_is_new_note_l
				+ f_cp_c_scale_note_count_l + f_cp_is_sharp_l + f_cp_is_flat_l + f_cp_is_tied_l + f_cp_is_new_note_l 
				+ f_cf_c_scale_note_count_l + f_cf_is_new_note_l
				+ f_cf_c_scale_note_count_r + f_cf_is_new_note_r
				+ f_cf_is_bass;
			    */

				//at pos 20 the information is saved if it is a new note
				if (sheet_vec[i][20] == 1.0f)
				{
				//convert to sequence
					std::vector<std::vector<float>> sequence;
					std::vector<float> sequence_targets;
					for (int distance = 8; distance >= 0; distance--)
					{
						std::vector<float> slice;
						//from the left
						sheet_vec[i - distance];
						slice.insert(slice.begin(), sheet_vec[i - distance].begin(), sheet_vec[i - distance].end() - 1 - 1);//-1 without cf_is_bass, -1 without the probability
						//from the right
						slice.insert(slice.begin(), sheet_vec[i + distance].begin(), sheet_vec[i + distance].end() - 1);//with cf_is_bass, -1 == without the probability
						sequence.push_back(slice);
						if (distance != 0)
							sequence_targets.push_back(1.0f);
						else
							sequence_targets.push_back(sheet_vec[i].back()); // the probability
					}

				//convert to tensors
					std::vector<float> sequence_faltened;
					for (int i = 0; i < sequence.size(); i++)
					{
						for (int h = 0; h < sequence[i].size(); h++)
						{
							sequence_faltened.push_back(sequence[i][h]);
						}
					}
					//input (sequence, batch, features) 
					auto feature_tensor = torch::tensor(sequence_faltened, at::requires_grad(false).dtype(torch::kFloat32)).view({ (int)sequence.size(), 1, feature_count });
					features.push_back(feature_tensor);

					//targets -----------------------------------
					std::vector<float> target_vector = sequence_targets;
					//output (sequence, batch, target_values) 
					auto taget_tensor = torch::tensor(target_vector, at::requires_grad(false).dtype(torch::kFloat32)).view({ (int)target_vector.size(), 1, target_count });

					targets.push_back(taget_tensor);
				}
			}
		}
	}
	catch (std::exception& e)
	{
		std::cerr << "\n" << e.what();
	}
}


Sheet_Music Eval::LSTM_Eval::testing()
{

	std::vector<Sheet_Music> sheets = load_sheets("data/sheets/");

	std::cout << sheets.back();

	auto vec = convert_to_2d_vector(sheets.back());


	//load and convert trainings data
	std::cout << "\nsheets count " << sheets.size();
	std::vector<torch::Tensor> train_features;
	std::vector<torch::Tensor> train_targets;

	convert_to_cp_rule_learn_data_2(sheets, train_features, train_targets);

	std::cout << train_features;

	return sheets.back();
}


void Eval::LSTM_Eval::train_net(LSTM_Settings settings)
{
	//std::cout.setstate(std::ios_base::failbit);
	try {
		std::cout << "\nTRAIN CF!";
		std::string model_save_folder = "data/trainings_results/models/";


		//load and convert trainings data
		std::vector<Sheet_Music> sheets = load_sheets(settings.train_data_folder);
		std::cout << "\nsheets count " << sheets.size();
		std::vector<torch::Tensor> train_features;
		std::vector<torch::Tensor> train_targets;

		//load and convert test data
		std::vector<Sheet_Music> test_sheets = load_sheets(settings.test_data_folder);
		std::vector<torch::Tensor> test_features;
		std::vector<torch::Tensor> test_targets;

		if (settings.data_converter == "convert_to_cf_learn_data")
		{
			convert_to_cf_learn_data(sheets, train_features, train_targets);
			convert_to_cf_learn_data(test_sheets, test_features, test_targets);
		}
		else if (settings.data_converter == "convert_to_cp_rule_learn_data_1")
		{
			convert_to_cp_rule_learn_data_1(sheets, train_features, train_targets);
			convert_to_cp_rule_learn_data_1(test_sheets, test_features, test_targets);
		}
		else if ("convert_to_cp_rule_learn_data_2")
		{
			convert_to_cp_rule_learn_data_2(sheets, train_features, train_targets);
			convert_to_cp_rule_learn_data_2(test_sheets, test_features, test_targets);
		}

		if (train_features.size() < test_features.size())
		{
			std::cerr << "\ntrainings data must be at least the same size as test data\n";
			return;
		}

		//could put batches in one tensor
		//torch::Tensor test_feature_tensor = torch::zeros({ (int)sheets[0].get_cf().size(), settings.batch_size, settings.in_size });
		//torch::Tensor test_target_tensor = torch::zeros({ (int)sheets[0].get_cf().size(), settings.batch_size, settings.out_size });

		//for (int b = 0; b < settings.batch_size; b++)
		//	for (int s = 0; s < sheets[0].get_cf().size(); s++)
		//		for (int i = 0; i < settings.in_size; i++)
		//			test_feature_tensor[s][b][i] = test_features[0][s][0][i];
		//for (int b = 0; b < settings.batch_size; b++)
		//	for (int s = 0; s < sheets[0].get_cf().size(); s++)
		//		for (int i = 0; i < settings.out_size; i++)
		//			test_target_tensor[s][b][i] = test_targets[0][s][0][i];


		//log progress
		std::vector<std::string> times;
		std::vector<int> epochs;
		std::vector<float> train_losses;
		std::vector<float> test_losses;

		std::string test_case_name = settings.test_name
			+ "_hlc" + std::to_string(settings.hidden_layer_count)
			+ "_hls" + std::to_string(settings.hidden_size)
			+ "_opti_" + Utility::to_str(settings.optimizer)
			+ "_lossf_" + Utility::to_str(settings.loss_func)
			+ "_lr_" + std::to_string(settings.learning_rate);
		std::replace(test_case_name.begin(), test_case_name.end(), '.', ',');

		//LSTM
		auto model = std::make_shared<LSTM>(LSTM(settings.in_size, settings.hidden_size, settings.out_size, settings.hidden_layer_count, settings.optimizer, settings.loss_func));
		model->set_learning_rate(settings.learning_rate);

		std::cout << std::endl << "learning..." << std::endl;
		
		for (size_t epoch = 0; epoch < settings.epochs; epoch++)
		{
			std::cout << ".";
			if (epoch % 10 == 0)
			{
				float train_total_loss = 0.0f;
				float test_total_loss = 0.0f;
				//shuffle up the test data
				std::vector<int> random_order(test_features.size());
				std::iota(random_order.begin(), random_order.end(), 0);
				std::shuffle(random_order.begin(), random_order.end(), std::mt19937{ std::random_device{}() });

				for (int i = 0; i < test_features.size(); i++)
				{
					auto test_data_loss = model->test_prediction(test_features[random_order[i]], test_targets[random_order[i]]);
					test_total_loss += test_data_loss.item<float>() / (float)test_features.size();
					auto train_data_loss = model->test_prediction(train_features[random_order[i]], train_targets[random_order[i]]);
					train_total_loss += train_data_loss.item<float>() / (float)test_features.size();
				}

				std::cout << "\nepoch: " << epoch << ", Loss: train: " << train_total_loss << ", test: " << test_total_loss << std::endl;
				torch::save(*model->optimizer, model_save_folder + test_case_name + "_epoch_" + std::to_string(epoch) + "_opti" + ".pt");
				torch::save(model, model_save_folder + test_case_name + ".pt");
				times.push_back(Utility::get_time_stamp());
				epochs.push_back(epoch);
				test_losses.push_back(test_total_loss);
				train_losses.push_back(train_total_loss);
			}

			//shuffle up the trainings data
			std::vector<int> random_order(train_features.size());
			std::iota(random_order.begin(), random_order.end(), 0);
			std::shuffle(random_order.begin(), random_order.end(), std::mt19937{ std::random_device{}() });
			for (int i = 0; i < train_features.size(); i++)
			{
				model->learn_step(train_features[random_order[i]], train_targets[random_order[i]]);
			}
		}
		//SAVE
		torch::save(model, model_save_folder + test_case_name + ".pt");
		torch::save(*model->optimizer, model_save_folder + test_case_name + "opti" + ".pt");

		//LOAD
		torch::load(model, model_save_folder + test_case_name + ".pt");
		torch::load(*model->optimizer, model_save_folder + test_case_name + "opti" + ".pt");
		model->eval();
		if (test_features.size())
		{
			auto predic = model->forward(test_features[0]);
			std::cout << std::endl << std::endl << "Prediction:\n" << predic << std::endl;

		}

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
	data_converter = parameter["data_converter"].GetString();
}



Eval::LSTM_Test_Runner::LSTM_Test_Runner()
{

}

void Eval::LSTM_Test_Runner::run_test(const std::string& test_file)
{
	LSTM_Settings settings(test_file);
	
	std::cout << settings;

	evaluator.train_net(settings);
}



std::ostream& Eval::operator<<(std::ostream& os, LSTM_Settings& test)
{
	os << "\nTest name: " << test.test_name 
		<< "\nin_size: " << test.in_size << ", hidden_size: " << test.hidden_size << ", out_size: " << test.out_size 
		<< "\nhidden_layer_count: " << test.hidden_layer_count << ", batch_size: " << test.batch_size << ", epochs: " << test.epochs
		<< "\noptimizer: " << test.optimizer << ", loss_func: " << test.loss_func 
		<< "\nlearning_rate: " << test.learning_rate 
		<< "\ntrain_data_folder: " << test.train_data_folder 
		<< "\ntest_data_folder: " << test.test_data_folder
		<< "\ndata_converter: " << test.data_converter
		<< "\n";
	return os;
}