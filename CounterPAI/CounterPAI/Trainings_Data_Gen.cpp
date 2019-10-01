#pragma once
#include "Trainings_Data_Gen.h"
#include <list>
#include <fstream>
#include <filesystem>
#include <vector>
#include <iomanip>

#include "Note_Evaluation.h"

Eval::Trainings_Data_Gen::Trainings_Data_Gen()
	:
	m_gen(m_rd())
{
}

void save_sheet(Sheet_Music& sheet, const std::string folder)
{
	std::ofstream ofs;
	ofs.open(folder.c_str());
	if (!ofs)
	{
		std::cerr << "could not open " << folder << "\n";
		return;
	}
	ofs << sheet;
}

void Eval::Trainings_Data_Gen::generate_data(int count, const std::string& target_folder, Settings settings, const std::string& sorce_folder)
{
	std::cout << "\nGenerating " + std::to_string(count) + " sheets\n";
	std::cout << "Target: " + target_folder + "\n";
	if (settings == Settings::Mutate)
		std::cout << "Sorce: " + sorce_folder + "\n";
	std::vector<Sheet_Music> sheets;
	for (auto& entry : std::experimental::filesystem::directory_iterator(sorce_folder))
	{
		if (!is_directory(entry.path()))
		{
			std::ifstream ifs(entry.path().c_str());
			if (!ifs)
			{
				std::cerr << "could not open " << entry.path() << "\n";
				continue;
			}
			Sheet_Music temp;
			ifs >> temp;
			if (temp.get_cf().size() > 0)
				sheets.push_back(temp);
		}
	}

	std::cout << "\n";
	for (int i = 0; i < count; i++)
	{
		if(i % 100 == 0)
			std::cout << "\r" + std::to_string(i);
		Sheet_Music sheet;
		if (settings == Settings::Random)
			sheet = generate_random_sheet();
		else if (settings == Settings::Mutate)
		{
			sheet = generate_mutated_sheet(sheets);
		}
		m_evaluator.evaluate_notes(sheet);
		save_sheet(sheet, target_folder + "/sheet_" + std::to_string(i) + ".sheet");
	}
	std::cout << "\r" + std::to_string(count) + " generated";
}


Sheet_Music Eval::Trainings_Data_Gen::generate_mutated_sheet(const std::vector<Sheet_Music>& sheets)
{
	std::uniform_int_distribution<> dist(0, sheets.size() - 1);
	Sheet_Music sheet = sheets[dist(m_gen)];

	int mutate_cp_count = 2;
	int mutate_cf_count = 2;

	for (int i = 0; i < mutate_cp_count; i++)
	{
		std::uniform_int_distribution<> note_val_dist(0, 16);
		std::uniform_int_distribution<> mutate_note_dist(0, sheet.get_cp().size() - 1);
		int mutate_index = mutate_note_dist(m_gen);

		int counter = 0;
		for (auto& note : sheet.get_cp())
		{
			if (counter++ == mutate_index)
			{
				note.m_pitch = note.get_ACscale_pitch(note_val_dist(m_gen));
				note.m_is_flat = false;
				note.m_is_sharp = false;
				break;
			}
		}
	}
	for (int i = 0; i < mutate_cf_count; i++)
	{
		std::uniform_int_distribution<> note_val_dist(0, 16);
		std::uniform_int_distribution<> mutate_note_dist(0, sheet.get_cf().size() - 1);
		int mutate_index = mutate_note_dist(m_gen);

		int counter = 0;
		for (auto& note : sheet.get_cf())
		{
			if (counter++ == mutate_index)
			{
				note.m_pitch = note.get_ACscale_pitch(note_val_dist(m_gen));
				note.m_is_flat = false;
				note.m_is_sharp = false;
				break;
			}
		}
	}
	return sheet;
}

Sheet_Music Eval::Trainings_Data_Gen::generate_random_sheet()
{
	Sheet_Music sheet;
	std::uniform_int_distribution<> dist(0, 1);
	sheet.bass_is_cf = dist(m_gen);

	if (sheet.bass_is_cf)
	{
		generate_rule_cf(sheet.m_bass, Note_Pitch::C2, Voice::Bass);
		generate_random_cp(sheet, Note_Pitch::A3, Voice::Soprano);
	}
	else
	{
		generate_rule_cf(sheet.m_soprano, Note_Pitch::A3, Voice::Soprano);
		generate_random_cp(sheet, Note_Pitch::C2, Voice::Bass);
	}
	return sheet;
}


void Eval::Trainings_Data_Gen::generate_rule_cf(std::list<Music_Note>& voice_line, Note_Pitch min, Voice voice)
{
	//gen first note
	std::uniform_int_distribution<> all_pitches_dist(0, 16);
	voice_line.push_back(
		Music_Note(Music_Note::get_ACscale_pitch(min, all_pitches_dist(m_gen)), Note_Value::Whole, voice)
	);


	//gen mid notes
	std::uniform_int_distribution<> pitch_big_jump_dist(0, 7);
	std::uniform_int_distribution<> pitch_small_jump_dist(1, 2);
	std::uniform_int_distribution<> pitch_jump_prob_dist(0, 100);
	std::uniform_int_distribution<> up_down_dist(0, 1);
	for (int i = 1; i < m_sheet_bars; i++)
	{
		int distance = Music_Note::get_ACscale_distance(min, voice_line.back().m_pitch);

		//
		int jump_height = 0;
		if (pitch_jump_prob_dist(m_gen) < 80)
			jump_height = pitch_small_jump_dist(m_gen);
		else
			jump_height = pitch_big_jump_dist(m_gen);

		//last bar should only be approached by a step
		if (i == m_sheet_bars - 1)
			jump_height = 1;

		if(up_down_dist(m_gen) == 0)
			distance = (distance + jump_height) % 16;
		else
		{
			if (distance - jump_height < 0)
				distance = (16 - std::abs(distance - jump_height)) % 16;
			else
				distance = (distance - jump_height) % 16;
		}

		voice_line.push_back(
			Music_Note(Music_Note::get_ACscale_pitch(min, distance), Note_Value::Whole, voice)
		);
	}


	//gen last note / set front to unison fith or octave
	std::uniform_int_distribution<> unifioc_dist(0, 2);
	int unifioc = unifioc_dist(m_gen);

	Music_Note note = voice_line.back();
	voice_line.pop_front();
	//std::cout << "\nuznifi: " << unifioc;
	if (unifioc == 0)
		voice_line.push_front(note);
	else if (unifioc == 1)
	{
		int distance = Music_Note::get_ACscale_distance(min, note.m_pitch);
		//std::cout << "\ndi: " << distance;
		if (distance + 4 > 16)
			distance = (distance + 7) % 17;
		else
			distance = (distance + 4);
		//std::cout << "\ndi: " << distance;
		voice_line.push_front(
			Music_Note(Music_Note::get_ACscale_pitch(min, distance), Note_Value::Whole, voice)
		);
	}
	else if (unifioc == 2)
	{
		int distance = Music_Note::get_ACscale_distance(min, note.m_pitch);
		//std::cout << "\ndi: " << distance;
		if (distance + 7 > 16)
			distance = (distance + 10) % 17;
		else
			distance = (distance + 7);
		//std::cout << "\ndi: " << distance;
		voice_line.push_front(
			Music_Note(Music_Note::get_ACscale_pitch(min, distance), Note_Value::Whole, voice)
		);
	}
}

void Eval::Trainings_Data_Gen::generate_random_cp(Sheet_Music& sheet, Note_Pitch min, Voice voice)
{
	int sixteenths = (m_sheet_bars - 2) * 16; // before last bar will be filled / last bar will be a whole note
	Note_Value val;

	std::uniform_int_distribution<> note_value_dist(0, 9);
	std::uniform_int_distribution<> note_value_under_whole_dist(1, 2);

	//use less eights
	Note_Value note_values[10] = { Note_Value::Whole, Note_Value::Halfe, Note_Value::Quarter, 
		Note_Value::Whole, Note_Value::Halfe, Note_Value::Quarter,
		Note_Value::Whole, Note_Value::Halfe, Note_Value::Quarter,
		Note_Value::Eighth };

	std::uniform_int_distribution<> pitch_dist(0, 16);
	std::uniform_int_distribution<> flat_sharp_dist(0, 40);
	std::uniform_int_distribution<> tied_dist(0, 10);

	Note_Pitch pitch;

	auto add_note = [&sheet, &val, &pitch, &min, &voice, &pitch_dist, &flat_sharp_dist, &tied_dist](std::mt19937& gen) {
		pitch = Music_Note::get_ACscale_pitch(min, pitch_dist(gen));

		
		int flat_sharp = flat_sharp_dist(gen);
		if (pitch < min)
			flat_sharp = 0;
		if (pitch > Music_Note::get_ACscale_pitch(min, 16))
			flat_sharp = 0;

		bool is_tied = !tied_dist(gen);
		if (flat_sharp == 39)
			sheet.add_note(Music_Note(static_cast<Note_Pitch>((int)pitch + 1), val, voice, is_tied, true, false));
		else if (flat_sharp == 40)
			sheet.add_note(Music_Note(static_cast<Note_Pitch>((int)pitch - 1), val, voice, is_tied, false, true));
		else
			sheet.add_note(Music_Note(pitch, val, voice, is_tied));
	};

	do
	{
		if(sixteenths == -4)
			val = note_values[note_value_under_whole_dist(m_gen)];
		else
			val = note_values[note_value_dist(m_gen)];

		add_note(m_gen);

		sixteenths -= 16 / static_cast<int>(val);

		if (val == Note_Value::Eighth)//eights are allways a pair
		{
			add_note(m_gen);

			sixteenths -= 16 / static_cast<int>(val);
		}
	} while (sixteenths >= -4 && sixteenths != 0);

	int sixteenth_left = 16 + sixteenths;
	//std::cout << "\nsixteenth: " << sixteenths;
	//std::cout << "\nsixteenth_left: " << sixteenth_left;
	if (sixteenth_left != 0)
	{
		val = static_cast<Note_Value>(16 / sixteenth_left);

		add_note(m_gen);

	}
	val = Note_Value::Whole;
	add_note(m_gen);
}


void Eval::Trainings_Data_Gen::analyse_sheets(const std::string& folder)
{
	Sheet_Statistic overall_stat;
	int counter = 0;
	std::cout << "\nanalysing sheets\n0";
	for (auto& entry : std::experimental::filesystem::directory_iterator(folder))
	{
		if (!is_directory(entry.path()))
		{
			std::ifstream ifs;
			ifs.open(entry.path().c_str());
			if (!ifs)
			{
				std::cerr << "could not open " << entry.path() << "\n";
				break;
			}
			Sheet_Music sheet;
			ifs >> sheet;




			Sheet_Statistic stat(sheet);
			overall_stat += stat;
			if (counter++ % 1000 == 0)
				std::cout << "\r" << counter - 1;
		}
	}
	std::cout << "\rdone                  ";
	std::cout << "\nStatistics " + folder + " :\n"
		<< overall_stat;
}

Eval::Sheet_Statistic::Sheet_Statistic(const Sheet_Music& sheet, const std::string& sheet_path)
	:
	sheet(&sheet),
	sheet_data_path(sheet_path)
{
	total_cf_notes = sheet.get_cf().size();
	for (const auto& n : sheet.get_cf())
	{
		cf_note_values[n.m_value]++;
		cf_note_pitches[n.m_pitch]++;
	}

	total_cp_notes = sheet.get_cp().size();
	for (const auto& n : sheet.get_cp())
	{
		cp_note_values[n.m_value]++;
		cp_note_pitches[n.m_pitch]++;
		if(n.m_is_tied)
			tied_notes++;
		if(n.m_is_flat)
			flat_notes++;
		if(n.m_is_sharp)
			sharp_notes++;

		Note_Evaluation note_evaluation(n.get_note_info(Note_Evaluation::C_INDEX_NAME));

		motions[note_evaluation.m_motion]++;
		intervals[note_evaluation.m_interval]++;
		jump_intervals[note_evaluation.m_jump_interval]++;
		directions[note_evaluation.m_direction]++;
		bar_postions[note_evaluation.m_position]++;
		beat_postions[note_evaluation.m_beat_pos]++;


		Rule_Evaluation rule_evaluation(n.get_note_info(Rule_Evaluation::C_INDEX_NAME));
		for(auto r : rule_evaluation.broken_rules)
			broken_rules[r]++;
	}
}

std::ostream& Eval::operator<<(std::ostream& os, const Sheet_Statistic& stat)
{
	os << std::setfill(' ');

	os << "\ncf:\nValues:\n";
	for (auto i : stat.cf_note_values)
		os << std::setw(11) << i.first << ":"  << i.second << "\n";
	os << std::setw(11) << "Total: " << stat.total_cf_notes;

	os << "\nPitches:\n";
	for (auto i : stat.cf_note_pitches)
		os << std::setw(11) << i.first << ": " << i.second << "\n";

	os << "\ncp:\nValues:\n";
	for (auto i : stat.cp_note_values)
		os << std::setw(11) << i.first << ": " << i.second << "\n";
	os << std::setw(11) << "Total: " << stat.total_cp_notes;
	os << "\nPitches:\n";
	for (auto i : stat.cp_note_pitches)
		os << std::setw(11) << i.first << ": " << i.second << "\n";
	os << "\nTieds Notes: " << stat.tied_notes;
	os << "\nFlat Notes:  " << stat.flat_notes;
	os << "\nSharp Notes: " << stat.sharp_notes;


	os << "\n\nEvaluation:\n";
	os << "Motions:\n";
	for (auto i : stat.motions)
		os << std::setw(11) << i.first << ": " << i.second << "\n";
	os << "\nIntervals:\n";
	for (auto i : stat.intervals)
		os << std::setw(11) << i.first << ": " << i.second << "\n";
	os << "\nJump Intervals:\n";
	for (auto i : stat.jump_intervals)
		os << std::setw(11) << i.first << ": " << i.second << "\n";
	os << "\nDirections:\n";
	for (auto i : stat.directions)
		os << std::setw(11) << i.first << ": " << i.second << "\n";
	os << "\nBar Positions:\n";
	for (auto i : stat.bar_postions)
		os << std::setw(25) << i.first << ": " << i.second << "\n";
	os << "\nBeat Positions:\n";
	for (auto i : stat.beat_postions)
		os << std::setw(11) << i.first << ": " << i.second << "\n";

	os << "\nRules Broken:\n";
	for (auto i : stat.broken_rules)
		os << std::setw(11) << i.first << ": " << i.second << "\n";

	os << "\n";
	return os;
}


Eval::Sheet_Statistic Eval::Sheet_Statistic::operator+(const Sheet_Statistic &b) const
{
	Sheet_Statistic stat = b;
	stat += *this;
	return stat;
}

Eval::Sheet_Statistic Eval::Sheet_Statistic::operator+=(const Sheet_Statistic &b)
{
	total_cf_notes += b.total_cf_notes;
	total_cp_notes += b.total_cp_notes;

	for (auto i : b.cf_note_values)
		cf_note_values[i.first] += i.second;
	for (auto i : b.cf_note_pitches)
		cf_note_pitches[i.first] += i.second;

	for (auto i : b.cp_note_values)
		cp_note_values[i.first] += i.second;
	for (auto i : b.cp_note_pitches)
		cp_note_pitches[i.first] += i.second;

	tied_notes += b.tied_notes;
	flat_notes += b.flat_notes;
	sharp_notes += b.sharp_notes;

	for (auto i : b.motions)
		motions[i.first] += i.second;
	for (auto i : b.intervals)
		intervals[i.first] += i.second;
	for (auto i : b.jump_intervals)
		jump_intervals[i.first] += i.second;
	for (auto i : b.directions)
		directions[i.first] += i.second;
	for (auto i : b.bar_postions)
		bar_postions[i.first] += i.second;
	for (auto i : b.beat_postions)
		beat_postions[i.first] += i.second;

	for (auto i : b.broken_rules)
		broken_rules[i.first] += i.second;

	return *this;
}



std::ostream& Eval::operator<<(std::ostream& os, const Eval::Trainings_Data_Gen::Settings& settings)
{
	static const std::array<std::string, 2> settings_vec
	{
		"Random",
		"Mutate"
	};
	os << settings_vec[static_cast<int>(settings)];
	return os;
}
std::istream& Eval::operator>>(std::istream& is, Eval::Trainings_Data_Gen::Settings& rule)
{
	std::string s;
	is >> s;
	if (s == "Random")
		rule = Eval::Trainings_Data_Gen::Settings::Random;
	else if (s == "Mutate")
		rule = Eval::Trainings_Data_Gen::Settings::Mutate;
	return is;
}