#pragma once
#include "Note_Evaluation.h"

#include <sstream>

const std::string Eval::Note_Evaluation::C_INDEX_NAME = "NOTE_EVALUATION";

Eval::Note_Evaluation::Note_Evaluation()
	:
	m_motion(Motion::NoMo),
	m_direction(Direction::No_Dir),
	m_jump_interval(Interval::No_Interval),
	m_interval(Interval::No_Interval),
	m_position(Bar_Position::No_Bar),
	m_beat_pos(Beat_Position::No_Beat),
	m_sixteenth_position(-1)
{}

Eval::Note_Evaluation::Note_Evaluation(const std::string& eval_str)
{
	std::stringstream ss;
	ss << eval_str;
	ss >> *this;
}


std::ostream& Eval::operator<<(std::ostream& os, const Note_Evaluation& note)
{
	os << "p: " << note.m_position << " b: " << note.m_beat_pos << " j: " << note.m_jump_interval << " d: " << note.m_direction << " m: " << note.m_motion << " i: " << note.m_interval;
	return os;
}

std::istream& Eval::operator>>(std::istream& is, Note_Evaluation& note)
{
	std::string temp;
	is >> temp; //eat p:
	is >> note.m_position;
	is >> temp; //eat b:
	is >> note.m_beat_pos;
	is >> temp; //eat j:
	is >> note.m_jump_interval;
	is >> temp; //eat d:
	is >> note.m_direction;
	is >> temp; //eat m:
	is >> note.m_motion;
	is >> temp; //eat i:
	is >> note.m_interval;

	return is;
}
