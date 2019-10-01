#pragma once
#include <string>
#include "Sheet_Music.h"
#include "Rule_Evaluator.h"

#include <vector>
#include <random>

#include <iostream>

#include <map>

namespace Eval
{
	class Trainings_Data_Gen
	{
	public:
		enum class Settings
		{
			Random,
			Mutate
		};

		Trainings_Data_Gen();
		void generate_data(int count, const std::string& folder, Settings settings, const std::string& sorce_folder = "data/sheets");
		Sheet_Music generate_random_sheet();
		Sheet_Music generate_mutated_sheet(const std::vector<Sheet_Music>& sheets);
		void generate_rule_cf(std::list<Music_Note>& voice_line, Note_Pitch min, Voice voice);
		void generate_random_cp(Sheet_Music& sheet, Note_Pitch min, Voice voice);


		void analyse_sheets(const std::string& folder);

		int m_sheet_bars = 13;
		Eval::Rule_Evaluator m_evaluator;

		std::random_device m_rd;
		std::mt19937 m_gen;



	};


	class Sheet_Statistic;
	std::ostream& operator<<(std::ostream& os, const Sheet_Statistic& stat);
	class Sheet_Statistic
	{
	public:
		Sheet_Statistic() {};
		Sheet_Statistic(const Sheet_Music& sheet, const std::string& sheet_path = "");

		const Sheet_Music* sheet;
		std::string sheet_data_path;

		int total_cf_notes;
		std::map<Note_Value, int> cf_note_values;
		std::map<Note_Pitch, int> cf_note_pitches;

		int total_cp_notes;
 		std::map<Note_Value, int> cp_note_values;
		std::map<Note_Pitch, int> cp_note_pitches;
		int tied_notes;
		int flat_notes;
		int sharp_notes;

		std::map<Motion, int> motions;
		std::map<Interval, int> intervals;
		std::map<Interval, int> jump_intervals;
		std::map<Direction, int> directions;
		std::map<Bar_Position, int> bar_postions;
		std::map<Beat_Position, int> beat_postions;

		std::map<Fux_Rule, int> broken_rules;

		Sheet_Statistic operator+(const Sheet_Statistic &b) const;
		Sheet_Statistic operator+=(const Sheet_Statistic &b);
		friend std::ostream& operator<<(std::ostream& os, const Sheet_Statistic& stat);
	};
	std::ostream& operator<<(std::ostream& os, const Eval::Trainings_Data_Gen::Settings& settings);
	std::istream& operator>>(std::istream& is, Eval::Trainings_Data_Gen::Settings& rule);
}
