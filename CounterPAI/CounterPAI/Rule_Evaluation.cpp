#include "Rule_Evaluation.h"
#include <iostream>
#include <sstream>

const std::string Eval::Rule_Evaluation::C_INDEX_NAME = "RULE_EVALUATION";


Eval::Rule_Evaluation::Rule_Evaluation(const std::string& eval_str)
{
	std::stringstream ss;
	ss << eval_str;
	ss >> *this;
}

std::string Eval::Rule_Evaluation::get_rule_text(Fux_Rule rule)
{
	switch (rule)
	{
	case Eval::Fux_Rule::R1:
		return "R1: One should use perfet intervals to begin and end a piece";
	case Eval::Fux_Rule::R2:
		return "R2: ";
	case Eval::Fux_Rule::R3:
		return "R3: ";
	case Eval::Fux_Rule::R4:
		return "R4: ";
	case Eval::Fux_Rule::R5:
		return "R5: ";
	case Eval::Fux_Rule::R6:
		return "R6: ";
	case Eval::Fux_Rule::R7:
		return "R7: ";
	case Eval::Fux_Rule::R8:
		return "R8: ";
	case Eval::Fux_Rule::R9:
		return "R9: ";
	case Eval::Fux_Rule::R10:
		return "R10: ";
	default:
		return "No rule";
	}
}


std::ostream& Eval::operator<<(std::ostream& os, const Rule_Evaluation& note)
{
	os << "prb: " << note.m_probability;
	for (const auto& r : note.broken_rules)
		os << " " << r;

	return os;
}

std::istream& Eval::operator>>(std::istream& is, Rule_Evaluation& note)
{
	std::string temp;
	is >> temp; // eat prb:
	is >> note.m_probability;

	while (is.peek() == ' ')
	{
		Fux_Rule temp;
		is >> temp;
		note.broken_rules.push_back(temp);
	}
	return is;
}
