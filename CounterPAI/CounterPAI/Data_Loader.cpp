#pragma once
#include "Data_Loader.h"

#include <filesystem>
#include <sstream>
#include <algorithm>
#include <random>

#include "Rule_Evaluation.h"
#include "Utility.h"

//TODO
#include <windows.h>

bool memory_check(int counter)
{
	bool should_continue = true;
	MEMORYSTATUSEX status;
	status.dwLength = sizeof(status);
	GlobalMemoryStatusEx(&status);
	std::cout << "\r" << counter << " elements, memory " << status.ullAvailPhys << "      ";
	if (status.ullAvailPhys < 0.1 * status.ullTotalPhys)
	{
		std::cerr << "\nnot enought memory\n";
		std::cout << "continue? y/n ";
		char con;
		std::cin >> con;
		should_continue = (con == 'n'? false : true);
	}
	return should_continue;
}

Eval::Data_Loader::Data_Loader(Learn_Settings settings, bool is_training, Data_Type type)
	:
	settings(settings)
{
	if (!is_training)
		return;

	switch (type)
	{
	case Data_Type::TRAIN:
		std::cerr << "\ninit train data\n"; break;
	case Data_Type::TEST:
		std::cerr << "\ninit test data\n"; break;
	case Data_Type::VALID:
		std::cerr << "\ninit validation data\n"; break;
	}

	//load and convert trainings data
	std::vector<Sheet_Music> sheets;
	std::cout << "\nloading sheets\n";

	if (type == Data_Type::TRAIN)
		sheets = load_sheets(settings.train_data_folder);
	else if (type == Data_Type::TEST)
		sheets = load_sheets(settings.test_data_folder);
	else if (type == Data_Type::VALID)
		sheets = load_sheets(settings.valid_data_folder);
	
	std::cout << "\nsheets count " << sheets.size();

	std::cout << "\nconverting\n";

	if (settings.data_converter == "remember_one_cf")
	{
		remember_one_cf(sheets, features_vec, targets_vec);
	}
	else if (settings.data_converter == "evaluate_fux_rules_from_one_side_1")
	{
		evaluate_fux_rules_from_one_side_1(sheets, features_vec, targets_vec);
	}
	else if (settings.data_converter == "evaluate_fux_rules_from_two_sides_1")
	{
		evaluate_fux_rules_from_two_sides_1(sheets, features_vec, targets_vec);
	}
	else if (settings.data_converter == "evaluate_fux_rules_from_two_sides_lnn_1")
	{
		evaluate_fux_rules_from_two_sides_lnn_1(sheets, features_vec, targets_vec);
	}
	else if (settings.data_converter == "evaluate_fux_rules_from_two_sides_rule_targets")
	{
		Fux_Rule rule = Utility::from_str<Fux_Rule>(settings.data_converter_info);
		evaluate_fux_rules_from_two_sides_rule_targets(sheets, features_vec, targets_vec, rule);
	}
	else if (settings.data_converter == "evaluate_fux_rules_back_n_forth_rule_targets")
	{
		Fux_Rule rule = Utility::from_str<Fux_Rule>(settings.data_converter_info);
		evaluate_fux_rules_back_n_forth_rule_targets(sheets, features_vec, targets_vec, rule);
	}
}


std::tuple<torch::Tensor, torch::Tensor> Eval::Data_Loader::get_batch(std::vector<torch::Tensor>& features_vec, std::vector<torch::Tensor>& targets_vec)
{	
	if (features_vec.size() <= 0 || targets_vec.size() <= 0)
	{
		std::cerr << "\nno features, LSTM will be corrupted";
		return std::tuple<torch::Tensor, torch::Tensor>(torch::randn({1,1,settings.in_size }), torch::randn({ 1,1,settings.out_size }));
	}


	//shuffle up the test data
	std::vector<int> random_order(features_vec.size());
	std::iota(random_order.begin(), random_order.end(), 0);
	std::shuffle(random_order.begin(), random_order.end(), std::mt19937{ std::random_device{}() });

	torch::Tensor train_feature_tensor;
	torch::Tensor train_target_tensor;


	//put batches in one tensor for features and targets
	if (settings.nn_type == NN_Type::LSTM)
	{
		if (settings.in_size != features_vec[0][0][0].numel())
			std::cerr << "\nnet in size is not equal to input feature size, net will be currupted!\n" 
			<< "features: " << features_vec[0].numel() << " input: " << settings.in_size << "\n";


		train_feature_tensor = torch::zeros({ features_vec[0].size(0), settings.batch_size, settings.in_size });
		train_target_tensor = torch::zeros({ targets_vec[0].size(0), settings.batch_size, settings.out_size });

		int random_order_size = random_order.size();
		for (int b = 0, b2 = 0; b < settings.batch_size; b++, b2++)
			for (int s = 0; s < features_vec[0].size(0); s++)
				for (int i = 0; i < settings.in_size; i++)
				{
					if (b % random_order.size() == 0)//if batchsize is bigger than testdata
						b2 = 0;
					train_feature_tensor[s][b][i] = features_vec[random_order[b2]][s][0][i];
				}
		for (int b = 0, b2 = 0; b < settings.batch_size; b++, b2++)
			for (int s = 0; s < targets_vec[0].size(0); s++)
				for (int i = 0; i < settings.out_size; i++)
				{
					if (b % random_order.size() == 0)//if batchsize is bigger than testdata
						b2 = 0;
					train_target_tensor[s][b][i] = targets_vec[random_order[b2]][s][0][i];
				}
	}
	else if (settings.nn_type == NN_Type::LNN)
	{
		if (settings.in_size != features_vec[0].numel())
			std::cerr << "\nnet in size is not equal to input feature size, net will be currupted!\n"
			<< "features: " << features_vec[0].numel() << " input: " << settings.in_size << "\n";


		train_feature_tensor = torch::zeros({ settings.batch_size, settings.in_size });
		train_target_tensor = torch::zeros({ settings.batch_size, settings.out_size });

		int random_order_size = random_order.size();
		for (int b = 0, b2 = 0; b < settings.batch_size; b++, b2++)
				for (int i = 0; i < settings.in_size; i++)
				{
					if (b % random_order.size() == 0)//if batchsize is bigger than testdata
						b2 = 0;
					train_feature_tensor[b][i] = features_vec[random_order[b2]][0][i];
				}
		for (int b = 0, b2 = 0; b < settings.batch_size; b++, b2++)
				for (int i = 0; i < settings.out_size; i++)
				{
					if (b % random_order.size() == 0)//if batchsize is bigger than testdata
						b2 = 0;
					train_target_tensor[b][i] = targets_vec[random_order[b2]][0][i];
				}
	}
	return std::tuple<torch::Tensor, torch::Tensor>(train_feature_tensor, train_target_tensor);
}

std::tuple<torch::Tensor, torch::Tensor> Eval::Data_Loader::get_batch()
{
	return get_batch(features_vec, targets_vec);
}


std::vector<Sheet_Music> Eval::Data_Loader::load_sheets(const std::string folder)
{
	std::vector<Sheet_Music> sheets;
	std::cout << "\nloading sheets\n";
	int ram_counter = 0;
	for (auto& entry : std::experimental::filesystem::directory_iterator(folder))
	{
		if (ram_counter++ % 100 == 0)
			if (memory_check(ram_counter) == false)
				break;

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
	memory_check(ram_counter);
	return std::move(sheets);
}

void Eval::Data_Loader::remember_one_cf(std::vector<Sheet_Music>& sheets, std::vector<torch::Tensor>& features, std::vector<torch::Tensor>& targets)
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


//EVALUATE FUX RULES
void print_2d_tensor_formatted(const torch::Tensor& tensor, int breaker)
{
	//std::cout << tensor;
	std::cout << "dim: " << tensor.dim() << "\n";
	
	for (int c = 0; c < tensor.size(2); c++)
	{
		if (c == breaker)
		{
			std::cout << "       ";
			for (int r = 0; r < tensor.size(0); r++)
				std::cout << "-";
			std::cout << "\n";
		}
		std::cout << std::setfill(' ') << std::setw(5) << c << ": ";
		for (int r = 0; r < tensor.size(0); r++)
		{
			std::cout << tensor[r][0][c].item<float>();
		}
		std::cout << "\n";
	}

}

void Eval::Data_Loader::print_2d_vector(const std::vector<std::vector<float>>& sheet_vec)
{
	if (sheet_vec.size() == 0)
	{
		std::cout << "\nsheet vector is empty\n";
		return;
	}
	for (int r = 0; r < sheet_vec[0].size(); r++)
	{
		for (int c = 0, eights = 0; c < sheet_vec.size(); c++, eights++)
		{
			if (eights % 8 == 0)
			{
				std::cout << "|";
				eights = 0;
			}
			std::cout << sheet_vec[c][r];
		}
		std::cout << std::endl;
	}
}

std::vector<std::vector<float>> Eval::Data_Loader::convert_to_2d_vector(Sheet_Music& sheet)
{
	std::vector<std::vector<float>> sheet_vec;
	if (sheet.get_cf().size() == 0)
		return sheet_vec;


	/*
	converts sheet music to a 2d vector:
	cp: C4 -> B4 (two halfs not sharp, not flat, not tied)
	probability
	new notes
	cf: C3 (one whole)
	new notes
	cf is bass: true

	(all notes whould be represented, here only 5 per voice)
	C4         |11110000|0000 ...
	B4         |00001111|1111 ...
	A4         |00000000|0000 ...
	G3         |00000000|0000 ...
	F3         |00000000|0000 ...
	sharp      |00000000|0000 ...
	flat       |00000000|0000 ...
	tied       |00000000|1111 ...
	new note   |10001000|1000 ...
	F3         |00000000|0000 ...
	E3         |00000000|0000 ...
	D3         |00000000|1111 ...
	C3         |11111111|0000 ...
	B3         |00000000|0000 ...
	new note   |10000000|1000 ...
	cf is bass |11111111|1111 ...
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
	int slice_size = f_cp_c_scale_note_count + f_cp_is_sharp + f_cp_is_flat + f_cp_is_tied + f_cp_is_new_note + f_cf_c_scale_note_count + f_cf_is_new_note + f_cf_is_bass + f_cp_probability;


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
		//std::cout << "\ncp: "<< cp_note.get_basic_note();
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
			Rule_Evaluation temp;
			if (cp_note.get_note_info(Rule_Evaluation::C_INDEX_NAME) != "null")
				temp = Rule_Evaluation(cp_note.get_note_info(Rule_Evaluation::C_INDEX_NAME));
			probability = temp.m_probability;
			cp_is_new_note = true;
		}
		bool cf_was_at_note = false;
		Music_Note cf_note = sheet.get_note(cf_voice, sixteenths, cf_was_at_note);
		//f_cf_c_scale_note_count = 17;
		std::vector<float> cf_c_scale_pitches(f_cf_c_scale_note_count, 0.0f);
		//std::cout << "\ncf: " << cf_note.get_basic_note();
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

void Eval::Data_Loader::evaluate_fux_rules_from_one_side_1(std::vector<Sheet_Music>& sheets, std::vector<torch::Tensor>& features, std::vector<torch::Tensor>& targets)
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


		int ram_counter = 0;
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
				{
					Rule_Evaluation temp(note.get_note_info(Rule_Evaluation::C_INDEX_NAME));
					target_probabilitys.push_back(temp.m_probability);
				}
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
					if (ram_counter++ % 100 == 0)
						if (memory_check(ram_counter) == false)
							break;
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

void Eval::Data_Loader::evaluate_fux_rules_from_two_sides_1(std::vector<Sheet_Music>& sheets, std::vector<torch::Tensor>& features, std::vector<torch::Tensor>& targets)
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
			+ f_cp_c_scale_note_count_r + f_cp_is_sharp_r + f_cp_is_flat_r + f_cp_is_tied_r + f_cp_is_new_note_r
			+ f_cf_c_scale_note_count_l + f_cf_is_new_note_l
			+ f_cf_c_scale_note_count_r + f_cf_is_new_note_r
			+ f_cf_is_bass;

		//targets
		const int t_note_probability = 1;
		int target_count = t_note_probability;

		int ram_counter = 0;
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
				if (ram_counter++ % 100 == 0)
					if (memory_check(ram_counter) == false)
						break;
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

						//old version
						//if (distance != 0)
						//	sequence_targets.push_back(1.0f);
						//else
						//	sequence_targets.push_back(sheet_vec[i].back()); // the probability
					}
					//new version
					sequence_targets.push_back(sheet_vec[i].back()); // the probability

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

void Eval::Data_Loader::evaluate_fux_rules_from_two_sides_lnn_1(std::vector<Sheet_Music>& sheets, std::vector<torch::Tensor>& features, std::vector<torch::Tensor>& targets)
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

			  target for the note looked at (last in sequence) is the probability of that note

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
			+ f_cp_c_scale_note_count_r + f_cp_is_sharp_r + f_cp_is_flat_r + f_cp_is_tied_r + f_cp_is_new_note_r
			+ f_cf_c_scale_note_count_l + f_cf_is_new_note_l
			+ f_cf_c_scale_note_count_r + f_cf_is_new_note_r
			+ f_cf_is_bass;

		//targets
		const int t_note_probability = 1;
		int target_count = t_note_probability;

		int ram_counter = 0;
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
					if (ram_counter++ % 100 == 0)
						if (memory_check(ram_counter) == false)
							break;
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
					}
					sequence_targets.push_back(sheet_vec[i].back()); // the probability

					//convert to tensors
					std::vector<float> sequence_faltened;
					for (int i = 0; i < sequence.size(); i++)
					{
						for (int h = 0; h < sequence[i].size(); h++)
						{
							sequence_faltened.push_back(sequence[i][h]);
						}
					}

					//input (batch, features) 
					auto feature_tensor = torch::tensor(sequence_faltened, at::requires_grad(false).dtype(torch::kFloat32)).view({ 1, (int)sequence.size() * feature_count });
					features.push_back(feature_tensor);

					//targets -----------------------------------
					std::vector<float> target_vector = sequence_targets;
					//output (batch, target_values) 
					auto taget_tensor = torch::tensor(target_vector, at::requires_grad(false).dtype(torch::kFloat32)).view({ 1, (int)target_vector.size() * target_count });

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



void Eval::Data_Loader::evaluate_fux_rules_from_two_sides_rule_targets(std::vector<Sheet_Music>& sheets, std::vector<torch::Tensor>& features, std::vector<torch::Tensor>& targets, Eval::Fux_Rule target_rule)
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

			  target for the note looked at (last in sequence) is 1.0 if the rule was broken an 0.0 else

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
			+ f_cp_c_scale_note_count_r + f_cp_is_sharp_r + f_cp_is_flat_r + f_cp_is_tied_r + f_cp_is_new_note_r
			+ f_cf_c_scale_note_count_l + f_cf_is_new_note_l
			+ f_cf_c_scale_note_count_r + f_cf_is_new_note_r
			+ f_cf_is_bass;

		//targets
		const int t_note_probability = 1;
		int target_count = t_note_probability;

		int ram_counter = 0;

		int feature_true_counter = 0;
		int feature_false_counter = 0;

		for (auto& sheet : sheets)
		{
			auto sheet_vec = convert_to_2d_vector(sheet);

			//insert eight 0 vectors to front and back 
			std::vector<float> zeros(sheet_vec.front().size(), 0.0f);
			for (int i = 0; i < 8; i++)
				sheet_vec.insert(sheet_vec.begin(), zeros);
			for (int i = 0; i < 8; i++)
				sheet_vec.insert(sheet_vec.end(), zeros);

			//std::cout << "\nsheet:\n";
			//print_2d_vector(sheet_vec);

			auto note_itr = sheet.get_cp().begin();
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
					if (ram_counter++ % 100 == 0)
						if (memory_check(ram_counter) == false)
							break;
					//convert to sequence
					std::vector<std::vector<float>> sequence;
					std::vector<float> sequence_targets;
					for (int distance = 8; distance >= 0; distance--)
					{
						std::vector<float> slice;
						//from the left
						slice.insert(slice.begin(), sheet_vec[i - distance].begin(), sheet_vec[i - distance].end() - 1 - 1);//-1 without cf_is_bass, -1 without the probability
						//from the right
						slice.insert(slice.begin(), sheet_vec[i + distance].begin(), sheet_vec[i + distance].end() - 1);//with cf_is_bass, -1 == without the probability
						sequence.push_back(slice);
					}

					//std::cout << "\n-------";
					//std::cout << "\n" << *note_itr;
					Rule_Evaluation evaluation(note_itr->get_note_info(Rule_Evaluation::C_INDEX_NAME));
					//std::cout << "\n" << evaluation;					
					//char ch;
					//std::cin >> ch;
					note_itr++;


					if (evaluation.was_rule_broken(target_rule))
					{
						sequence_targets.push_back(1.0f);
						feature_true_counter++;
					}
					else
					{
						sequence_targets.push_back(0.0f);
						feature_false_counter++;
					}

					//convert to tensors
					std::vector<float> sequence_faltened;

					for (int s = 0; s < sequence.size(); s++)
					{
						for (int h = 0; h < sequence[s].size(); h++)
						{
							sequence_faltened.push_back(sequence[s][h]);
						}
					}
					//input (sequence, batch, features) 
					auto feature_tensor = torch::tensor(sequence_faltened, at::requires_grad(false).dtype(torch::kFloat32)).view({ (int)sequence.size(), 1, feature_count });
					features.push_back(feature_tensor);


					//std::cout << "\ntensor:\n";
					//print_2d_tensor_formatted(feature_tensor, 42);
					//std::cin >> ch;

					//targets -----------------------------------
					std::vector<float> target_vector = sequence_targets;
					//output (sequence, batch, target_values) 
					auto taget_tensor = torch::tensor(target_vector, at::requires_grad(false).dtype(torch::kFloat32)).view({ (int)target_vector.size(), 1, target_count });

					targets.push_back(taget_tensor);

				}
			}
		}
		memory_check(ram_counter);
		std::cout << "\n" << feature_true_counter << " features 1.0\n" << feature_false_counter << " features 0.0\n";
	}
	catch (std::exception& e)
	{
		std::cerr << "\n" << e.what();
	}
}


	void Eval::Data_Loader::evaluate_fux_rules_back_n_forth_rule_targets(std::vector<Sheet_Music>& sheets, std::vector<torch::Tensor>& features, std::vector<torch::Tensor>& targets, Eval::Fux_Rule target_rule)
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

				  target for the note looked at (last in sequence) is 1.0 if the rule was broken an 0.0 else

			if x is played on 1 (f_cp_is_new_note input == false) it would also be played at 2 3 4 5 (f_cp_is_new_note input == false)
			the target probability is allways the same as for x at 1
			*/

			//features
			//cp 
			const int f_cp_c_scale_note_count_l = 17;
			const int f_cp_is_sharp_l = 1;
			const int f_cp_is_flat_l = 1;
			const int f_cp_is_tied_l = 1;
			const int f_cp_is_new_note_l = 1;
			//cf
			const int f_cf_c_scale_note_count_l = 17;
			const int f_cf_is_new_note_l = 1;
			//all
			//const int f_is_looked_at_note = 1;
			const int f_cf_is_bass = 1;

			int feature_count =
				f_cp_c_scale_note_count_l + f_cp_is_sharp_l + f_cp_is_flat_l + f_cp_is_tied_l + f_cp_is_new_note_l
				+ f_cf_c_scale_note_count_l + f_cf_is_new_note_l
				+ f_cf_is_bass;

			//targets
			const int t_note_probability = 1;
			int target_count = t_note_probability;

			int ram_counter = 0;

			int feature_true_counter = 0;
			int feature_false_counter = 0;

			for (auto& sheet : sheets)
			{
				auto sheet_vec = convert_to_2d_vector(sheet);

				//insert eight 0 vectors to front and back 
				std::vector<float> zeros(sheet_vec.front().size(), 0.0f);
				for (int i = 0; i < 8; i++)
					sheet_vec.insert(sheet_vec.begin(), zeros);
				for (int i = 0; i < 8; i++)
					sheet_vec.insert(sheet_vec.end(), zeros);

				//std::cout << "\nsheet:\n";
				//print_2d_vector(sheet_vec);

				auto note_itr = sheet.get_cp().begin();
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
						if (ram_counter++ % 100 == 0)
							if (memory_check(ram_counter) == false)
								break;
						//convert to sequence
						std::vector<std::vector<float>> sequence;
						std::vector<float> sequence_targets;
						for (int distance = 8; distance >= -8; distance--)
						{
							std::vector<float> slice;
							//from the left
							slice.insert(slice.begin(), sheet_vec[i - distance].begin(), sheet_vec[i - distance].end()- 1);//-1 without the probability
							sequence.push_back(slice);
						}

						//std::cout << "\n-------";
						//std::cout << "\n" << *note_itr;
						Rule_Evaluation evaluation(note_itr->get_note_info(Rule_Evaluation::C_INDEX_NAME));
						//std::cout << "\n" << evaluation;					
						//char ch;
						//std::cin >> ch;
						note_itr++;


						if (evaluation.was_rule_broken(target_rule))
						{
							sequence_targets.push_back(1.0f);
							feature_true_counter++;
						}
						else
						{
							sequence_targets.push_back(0.0f);
							feature_false_counter++;
						}

						//convert to tensors
						std::vector<float> sequence_faltened;

						for (int s = 0; s < sequence.size(); s++)
						{
							for (int h = 0; h < sequence[s].size(); h++)
							{
								sequence_faltened.push_back(sequence[s][h]);
							}
						}
						//input (sequence, batch, features) 
						auto feature_tensor = torch::tensor(sequence_faltened, at::requires_grad(false).dtype(torch::kFloat32)).view({ (int)sequence.size(), 1, feature_count });
						features.push_back(feature_tensor);


						//std::cout << "\ntensor:\n";
						//print_2d_tensor_formatted(feature_tensor, 42);
						////char ch;
						//std::cin >> ch;

						//targets -----------------------------------
						std::vector<float> target_vector = sequence_targets;
						//output (sequence, batch, target_values) 
						auto taget_tensor = torch::tensor(target_vector, at::requires_grad(false).dtype(torch::kFloat32)).view({ (int)target_vector.size(), 1, target_count });

						targets.push_back(taget_tensor);

					}
				}

			}
			memory_check(ram_counter);
			std::cout << "\n" << feature_true_counter << " features 1.0\n" << feature_false_counter << " features 0.0\n";
		}
		catch (std::exception& e)
		{
			std::cerr << "\n" << e.what();
		}
}