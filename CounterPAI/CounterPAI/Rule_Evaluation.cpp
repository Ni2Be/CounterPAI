#pragma once
#include "Rule_Evaluation.h"
#include <iostream>
#include <sstream>
#include <algorithm>

#include "Utility.h"

const std::string Eval::Rule_Evaluation::C_INDEX_NAME = "RULE_EVALUATION";

bool is_main_rule(Eval::Fux_Rule rule)
{
	if (rule == Eval::Fux_Rule::R1
		|| rule == Eval::Fux_Rule::R2
		|| rule == Eval::Fux_Rule::R3
		|| rule == Eval::Fux_Rule::R4
		|| rule == Eval::Fux_Rule::R5
		|| rule == Eval::Fux_Rule::R6
		|| rule == Eval::Fux_Rule::R7
		|| rule == Eval::Fux_Rule::R8
		|| rule == Eval::Fux_Rule::R9)
	{
		return true;
	}
	return false;
}

bool is_sub_rule(Eval::Fux_Rule main_rule, Eval::Fux_Rule sub_rule)
{
	if (main_rule == Eval::Fux_Rule::R1)
		if (sub_rule == Eval::Fux_Rule::R1a
			|| sub_rule == Eval::Fux_Rule::R1b)
			return true;
	if (main_rule == Eval::Fux_Rule::R2)
		if (sub_rule == Eval::Fux_Rule::R2a
			|| sub_rule == Eval::Fux_Rule::R2b)
			return true;
	if (main_rule == Eval::Fux_Rule::R3)
		if (sub_rule == Eval::Fux_Rule::R3a
			|| sub_rule == Eval::Fux_Rule::R3b
			|| sub_rule == Eval::Fux_Rule::R3c
			|| sub_rule == Eval::Fux_Rule::R3d)
			return true;
	if (main_rule == Eval::Fux_Rule::R4)
		if (sub_rule == Eval::Fux_Rule::R4a
			|| sub_rule == Eval::Fux_Rule::R4b
			|| sub_rule == Eval::Fux_Rule::R4c)
			return true;
	if (main_rule == Eval::Fux_Rule::R5)
		if (sub_rule == Eval::Fux_Rule::R5a
			|| sub_rule == Eval::Fux_Rule::R5b)
			return true;
	if (main_rule == Eval::Fux_Rule::R6)
		if (sub_rule == Eval::Fux_Rule::R6a
			|| sub_rule == Eval::Fux_Rule::R6b
			|| sub_rule == Eval::Fux_Rule::R6c
			|| sub_rule == Eval::Fux_Rule::R6d)
			return true;
	if (main_rule == Eval::Fux_Rule::R8)
		if (sub_rule == Eval::Fux_Rule::R8a
			|| sub_rule == Eval::Fux_Rule::R8b
			|| sub_rule == Eval::Fux_Rule::R8c)
			return true;
	if (main_rule == Eval::Fux_Rule::R9)
		if (sub_rule == Eval::Fux_Rule::R9a
			|| sub_rule == Eval::Fux_Rule::R9b)
			return true;

	return false;
}

//R1 as input
bool Eval::Rule_Evaluation::was_rule_broken(Fux_Rule rule)
{
	for (const auto& r : broken_rules)
	{
		//rule directly broken
		if (rule == r)
			return true;

		//sub rule broken: (rule == R1 and r == R1a) means R1 broken
		if (is_main_rule(rule))
			if (is_sub_rule(rule, r))
				return true;
	}
	return false;
}



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
	case Eval::Fux_Rule::R1a:
		return "R1: Start and end must be a perfect interval";
	case Eval::Fux_Rule::R1b:
		return "R1: The Counter Point must be in the same mode as the c.f.";
	case Eval::Fux_Rule::R2a:
		return "R2: Contrary Motion should be preferred";
	case Eval::Fux_Rule::R2b:
		return "R2: Direct Motion should be avoided";
	case Eval::Fux_Rule::R3a:
		return "R3: Use perfect intervals sparingly";
	case Eval::Fux_Rule::R3b:
		return "R3: Dissonances on the Down Beat are not allowed";
	case Eval::Fux_Rule::R3c:
		return "R3: Dissonants on the Down Beat should be resolved by a step down";
	case Eval::Fux_Rule::R3d:
		return "R3: Dissonants on the Down Beat must be resolved";
	case Eval::Fux_Rule::R4a:
		return "R4: The leading tone must be a Major 6th if the c.f. is in the lower voice";
	case Eval::Fux_Rule::R4b:
		return "R4: The leading tone must be a minor 3rd if the c.f. is in the upper voice";
	case Eval::Fux_Rule::R4c:
		return "R4: The leading tone should be prepared by a fith or a suspension, if not on the downbeat";
	case Eval::Fux_Rule::R5a:
		return "R5: To a perfect interval one should move in Contrary Motion";
	case Eval::Fux_Rule::R5b:
		return "R5: The skip of less then a fourth cannot prevent a succession of either two fifth or two octaves in direct motion";
	case Eval::Fux_Rule::R6a:
		return "R6: No Tritone skips";
	case Eval::Fux_Rule::R6b:
		return "R6: No M6 skips";
	case Eval::Fux_Rule::R6c:
		return "R6: No perfect skips from an unison";
	case Eval::Fux_Rule::R6d:
		return "R6: No skips bigger than an octave";
	case Eval::Fux_Rule::R7:
		return "R7: Tied notes are only allowed on the down beat";
	case Eval::Fux_Rule::R8a:
		return "R8: Tied notes should not be resolved from an unison to a second, if c.f. is in bass";
	case Eval::Fux_Rule::R8b:
		return "R8: Tied notes should not be resolved from a second to an unison, if c.f. is in bass";
	case Eval::Fux_Rule::R8c:
		return "R8: Tied notes should not be resolved from a seventh to an octave, if c.f. is in soprano";
	case Eval::Fux_Rule::R9a:
		return "R9: Eight notes are only allowed on weak beats";
	case Eval::Fux_Rule::R9b:
		return "R9: Eight notes are only allowed by a stepwise approach";
	default:
		return Utility::to_str(rule);
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
