#include "Evaluator_Base.h"
#include <string>
#include <iomanip>

#include "Utility.h"


Eval::Note_Evaluation::Note_Evaluation()
	:
	m_motion(Motion::NoMo), 
	m_direction(Direction::No_Dir), 
	m_jump_interval(Interval::No_Interval), 
	m_interval(Interval::No_Interval), 
	m_interval_tied(Interval::No_Interval), 
	m_position(Bar_Position::No_Bar), 
	m_beat_pos(Beat_Position::No_Beat),
	m_probability(1.0f), 
	m_sixteenth_position(-1)
{}

int get_sixteenth_length(std::list<Music_Note> voice);

Eval::Evaluator_Base::Evaluator_Base()
{
}

void Eval::Evaluator_Base::evaluate_notes(Sheet_Music& sheet)
{
	std::list<Music_Note>& cantus_firmus = sheet.get_cf();
	std::list<Music_Note>& counter_point = sheet.get_cp();

	m_evaluation.clear();

	int length_cf = get_sixteenth_length(cantus_firmus);
	int length_CP = get_sixteenth_length(counter_point);
	if (length_cf != length_CP)
	{
		std::cerr << "\nCounter Point and Cantus Firmus must be the same length";
		return;
	}
	/*
	t=1
	 pos
	 |
	 CP0 CP1
	 |   |
	|--------|--------|--
	|o-------|--------|--
	|----o---|----o---|--
	|--------|o-------|o-
	|--------|--------|--

	 cf0      cf1
	 |        |
	|--------|--------|--
	|--------|o-------|--
	|--------|--------|o-
	|o-------|--------|--
	|--------|--------|--

	t=2
		 pos
		 |
		 CP0 CP1
		 |    |
	|--------|--------|--
	|o-------|--------|--
	|----o---|----o---|--
	|--------|o-------|o-
	|--------|--------|--

	 cf0      cf1
	 |        |
	|--------|--------|--
	|--------|o-------|--
	|--------|--------|o-
	|o-------|--------|--
	|--------|--------|--

	t=3
	     	  pos
	     	  |
	     	  CP0 CP1
	     	  |   |
	|--------|--------|--
	|o-------|--------|--
	|----o---|----o---|--
	|--------|o-------|o-
	|--------|--------|--

	          cf0      cf1
	          |        |
	|--------|--------|--
	|--------|o-------|--
	|--------|--------|o-
	|o-------|--------|--
	|--------|--------|--
	*/


	//cf 0 is the cf note coming from
	//cf 1 is the cf note looking at or moving to
	std::list<Music_Note>::iterator cf0_itr = cantus_firmus.begin();
	std::list<Music_Note>::iterator cf1_itr = cantus_firmus.begin();
	cf1_itr++;

	//CP 0 is the last note
	//CP 1 is the note looked at
	std::list<Music_Note>::iterator CP0_itr = counter_point.begin();
	std::list<Music_Note>::iterator CP1_itr = counter_point.begin();
	CP1_itr++;

	//positions looked at
	int CP0_sixteenth_distance = 1;
	int CP1_sixteenth_distance = 1 + 16 / static_cast<int>(CP0_itr->m_value);
	int CP0_bar = 1;
	int CP1_bar = 1;
	if (CP1_sixteenth_distance > 16)
		CP1_bar = 2;

	//add first note
	Note_Evaluation temp_eval;
	temp_eval.m_direction = Direction::No_Dir;
	temp_eval.m_jump_interval = Interval::No_Interval;

	temp_eval.m_interval = get_interval(*CP0_itr, *cf0_itr);
	temp_eval.m_interval_tied = Interval::No_Interval;
	temp_eval.m_motion = Motion::NoMo;
	temp_eval.m_position = get_bar_pos(counter_point, CP0_itr);
	temp_eval.m_beat_pos = get_beat_pos(counter_point, CP0_itr);
	temp_eval.m_sixteenth_position = CP0_sixteenth_distance;
	m_evaluation.push_back(temp_eval);
	CP0_itr->m_note_info = Utility::to_str(temp_eval);//set note info

	//add other notes
	while (CP1_itr != counter_point.end()
		&& cf1_itr != cantus_firmus.end()) //last note reached
	{
		temp_eval.m_direction = get_direction(*CP0_itr, *CP1_itr);
		temp_eval.m_jump_interval = get_interval(*CP0_itr, *CP1_itr);
		
		//
		if(CP1_bar > CP0_bar)
			temp_eval.m_interval = get_interval(*CP1_itr, *cf1_itr);
		else
			temp_eval.m_interval = get_interval(*CP1_itr, *cf0_itr);

		//TODO temp_eval.m_interval_tied = 


		temp_eval.m_motion = get_motion(*cf0_itr, *cf1_itr, *CP0_itr, *CP1_itr);
		temp_eval.m_position = get_bar_pos(counter_point, CP1_itr);
		temp_eval.m_beat_pos = get_beat_pos(counter_point, CP1_itr);
		//std::cout << "\neval: " << temp_eval;
		temp_eval.m_sixteenth_position = CP1_sixteenth_distance;
	
		m_evaluation.push_back(temp_eval);
		CP1_itr->m_note_info = Utility::to_str(temp_eval);//set note info

		//new c.p. note
		CP0_sixteenth_distance += 16 / static_cast<int>(CP0_itr->m_value);
		CP1_sixteenth_distance += 16 / static_cast<int>(CP1_itr->m_value);
		CP0_itr++;
		CP1_itr++;
		//new c.f. note?
		if (CP0_sixteenth_distance > CP0_bar * 16)
		{
			CP0_bar++;
			cf0_itr++;
			cf1_itr++;
		}
		//CP1 in new bar?
		if (CP1_sixteenth_distance > CP1_bar * 16)
			CP1_bar++;
	}	
}


Eval::Interval Eval::Evaluator_Base::get_interval(Music_Note n0, Music_Note n1)
{
	int half_tone_dist = std::abs(static_cast<int>(n0.m_pitch) - static_cast<int>(n1.m_pitch));
	Interval interval = static_cast<Interval>(half_tone_dist % 12);

	//8ths are no 1ths
	if((half_tone_dist != 0) && ((half_tone_dist % 12) == 0))
		interval = Interval::P8;

	return interval;
}

Eval::Direction Eval::Evaluator_Base::get_direction(Music_Note n0, Music_Note n1)
{
	int half_tone_dist = static_cast<int>(n0.m_pitch) - static_cast<int>(n1.m_pitch);

	if (half_tone_dist > 0)
		return Direction::Down;
	else if (half_tone_dist == 0)
		return Direction::Side;
	else if (half_tone_dist < 0)
		return Direction::Up;
}

Eval::Motion Eval::Evaluator_Base::get_motion(Music_Note cf0, Music_Note cf1, Music_Note CP0, Music_Note CP1)
{
	int cf_half_tone_dist = static_cast<int>(cf0.m_pitch) - static_cast<int>(cf1.m_pitch);
	int CP_half_tone_dist = static_cast<int>(CP0.m_pitch) - static_cast<int>(CP1.m_pitch);

	//DEBUG
	//std::cout << " (cf:" << cf0.m_pitch << ":" << cf1.m_pitch<< ":" << cf_half_tone_dist << ",cp:" << CP0.m_pitch << ":" << CP1.m_pitch << ":" << CP_half_tone_dist << ")";

	Direction cf_dir = get_direction(cf0, cf1);
	Direction CP_dir = get_direction(CP0, CP1);

	if ((cf_dir == Direction::Up
		&& CP_dir == Direction::Down)
		||
		(cf_dir == Direction::Down
		&& CP_dir == Direction::Up)
		)
		return Motion::CoMo;
	else if 
		((cf_dir == Direction::Side
		&& CP_dir != Direction::Side)
		||
		(cf_dir != Direction::Side
		&& CP_dir == Direction::Side)
		)
		return Motion::ObMo;
	else if 
		((cf_dir == Direction::Up
		&& CP_dir == Direction::Up)
		||
		(cf_dir == Direction::Down
		&& CP_dir == Direction::Down)
		)
		return Motion::DiMo;
	return Motion::NoMo;
}


Eval::Bar_Position Eval::Evaluator_Base::get_bar_pos(std::list<Music_Note>& voice, const std::list<Music_Note>::iterator& note)
{
	if (note == voice.end())
	{
		std::cerr << "\nindex not in voice!";
		return Bar_Position::No_Bar;
	}
	//get total length
	int sixteenth_total = get_sixteenth_length(voice);

	//get position of note
	std::list<Music_Note>::iterator itr = voice.begin();
	int sixteenth = 1;
	int note_counter = 0;
	for (; itr != note; itr++)
	{
		if (!itr->m_is_tied)
			note_counter++;
		sixteenth += 16 / static_cast<int>(itr->m_value);
	}
	//
	if (sixteenth <= 16 && note_counter == 0)
		return Bar_Position::First_Bar_First_Note;
	if (sixteenth <= 16)
		return Bar_Position::First_Bar;
	else if (sixteenth >= sixteenth_total - 16)
		return Bar_Position::Last_Bar_First_Note;
	else if (sixteenth >= sixteenth_total - 2 * 16)
	{
		if((sixteenth + (16 / static_cast<int>(itr->m_value))) >= sixteenth_total - 16)
			return Bar_Position::Before_Last_Last_Note;
		else
			return Bar_Position::Before_Last;
	}
	return Bar_Position::Mid_Bar;
}


Eval::Beat_Position Eval::Evaluator_Base::get_beat_pos(std::list<Music_Note>& voice, const std::list<Music_Note>::iterator& note)
{
	//get position of note
	std::list<Music_Note>::iterator itr = voice.begin();
	int sixteenth = 0;
	for (; itr != note; itr++)
	{
		sixteenth += 16 / static_cast<int>(itr->m_value);
	}

	int beat_pos = sixteenth % 16;
	//std::cout << "\nbeat_pos: " << beat_pos;
	switch (beat_pos)
	{
		case 0: return Beat_Position::Down_Beat; 
		case 4: return Beat_Position::Weak_Beat_1;
		case 8: return Beat_Position::Up_Beat;
		case 12: return Beat_Position::Weak_Beat_2;
		default: return Beat_Position::No_Beat;
	}
}

int get_sixteenth_length(std::list<Music_Note> voice)
{	
	//get total length
	int sixteenth_total = 1;//first note starts at 1, so +16 the next bar would be reached
	for (auto note : voice)
		sixteenth_total += 16 / static_cast<int>(note.m_value);
	return sixteenth_total;
}

std::ostream& Eval::operator<<(std::ostream& os, const Note_Evaluation& note)
{
	os << std::setw(4) << "\t-Position:" << note.m_position << ",\t-Beat:" << note.m_beat_pos << std::setw(12) << ",\t-Jump:" << note.m_jump_interval << std::setw(14) << ",\t-Direction:" << note.m_direction << ",\t-Motion:" << note.m_motion << ",\t-Interval:" << note.m_interval;
	return os;
}
std::ostream& Eval::operator<<(std::ostream& os, const Evaluator_Base& eval)
{
	os << "\n---------------\n";
	os << "\nEvaluation:\n";
	int i = 1;
	for (auto eval_note : eval.m_evaluation)
		os << i++ << ": " << eval_note << "\n";
	os << "---------------\n";
	return os;
}
