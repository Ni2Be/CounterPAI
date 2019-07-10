#pragma once
#include <string>
#include "Sheet_Music.h"
#include "Rule_Eval.h"
#include <random>

class Trainings_Data_Gen
{
public:
	Trainings_Data_Gen();
	void generate_data(int count, const std::string folder);
	Sheet_Music generate_sheet();
	void generate_cf(std::list<Music_Note>& voice_line, Note_Pitch min, Voice voice);
	void generate_cp(Sheet_Music& sheet, Note_Pitch min, Voice voice);


	int m_sheet_bars = 13;
	Eval::Rule_Eval m_evaluator;

	std::random_device m_rd;
	std::mt19937 m_gen;
};

