#pragma once
#include "Rule_Evaluator.h"


#include "Utility.h"

Eval::Rule_Evaluator::Rule_Evaluator()
{
}


bool is_perfect(Eval::Interval interval)
{
	return (interval == Eval::Interval::P1)
		|| (interval == Eval::Interval::P5)
		|| (interval == Eval::Interval::P8);
}
bool is_imperfect(Eval::Interval interval)
{
	return (interval == Eval::Interval::m3)
		|| (interval == Eval::Interval::M3)
		|| (interval == Eval::Interval::m6)
		|| (interval == Eval::Interval::M6);
}
bool is_dissonant(Eval::Interval interval)
{
	return (interval == Eval::Interval::m2)
		|| (interval == Eval::Interval::M2)
		|| (interval == Eval::Interval::P4)
		|| (interval == Eval::Interval::Tri)
		|| (interval == Eval::Interval::m7)
		|| (interval == Eval::Interval::M7);
}
void Eval::Rule_Evaluator::evaluate_notes(Sheet_Music& sheet)
{
	Evaluator_Base::evaluate_notes(sheet);

	std::list<Music_Note>& cantus_firmus = sheet.get_cf();
	std::list<Music_Note>& counter_point = sheet.get_cp();

	//add filler notes if cf and cp are of different lenght	
	int length_cf = get_sixteenth_length(cantus_firmus);
	int length_CP = get_sixteenth_length(counter_point);

	if (length_cf < length_CP)
		for (int i = 0; i < length_CP - length_cf; i++)
			cantus_firmus.push_back(Music_Note(Note_Pitch::C4, Note_Value::Sixteenth, cantus_firmus.front().m_voice, true));
	else if (length_cf > length_CP)
		for (int i = 0; i < length_cf - length_CP; i++)
			counter_point.push_back(Music_Note(Note_Pitch::C4, Note_Value::Sixteenth, counter_point.front().m_voice, true));




	std::vector<Note_Evaluation>::iterator eval_itr = m_evaluation.begin();
	std::vector<Note_Evaluation>::iterator prev_eval_itr = m_evaluation.begin();
	std::vector<Note_Evaluation>::iterator next_eval_itr = m_evaluation.begin();
	std::vector<Note_Evaluation>::iterator last_db_eval_itr = m_evaluation.begin();
	next_eval_itr++;
	std::list<Music_Note>::iterator CP_itr = counter_point.begin();
	std::list<Music_Note>::iterator prev_CP_itr = counter_point.begin();
	std::list<Music_Note>::iterator next_CP_itr = counter_point.begin();
	std::list<Music_Note>::iterator last_db_CP_itr = counter_point.begin();
	next_CP_itr++;

	std::list<Music_Note>::iterator cf_itr		= cantus_firmus.begin();
	std::list<Music_Note>::iterator prev_cf_itr = cantus_firmus.begin();


	for (; eval_itr != m_evaluation.end() && cf_itr != cantus_firmus.end(); eval_itr++, CP_itr++)
	{
		Rule_Evaluation evaluation;
		//CP_itr->m_note_info_ += "\n";
		r1_perfect_FB_LB(*eval_itr, *CP_itr, cantus_firmus.back(), evaluation);
		r2_use_CoMo_and_ObMo(*eval_itr, *CP_itr, evaluation);
		
		r7_tied_only_on_db(*eval_itr, *CP_itr, evaluation);


		if (eval_itr != prev_eval_itr)//just skips the first note for this rules
		{
			r4_note_before_last(*eval_itr, *CP_itr, *prev_eval_itr, evaluation);

			r5_motion_to_perfect(*eval_itr, *CP_itr, *prev_eval_itr, *last_db_eval_itr, *last_db_CP_itr, *prev_cf_itr, *cf_itr, evaluation);

			r6_prohibited_skips(*eval_itr, *CP_itr, *prev_CP_itr, cantus_firmus.back(), evaluation);

			r8_prohibited_resolves(*eval_itr, *CP_itr, *prev_CP_itr, *prev_eval_itr, evaluation);

			prev_eval_itr++;
			prev_CP_itr++;
		}
		if (next_eval_itr != m_evaluation.end())//just skips the last note for this rules
		{
			r3_use_imperfects(*eval_itr, *CP_itr, *next_eval_itr, evaluation);

			r9_eights_notes(*eval_itr, *CP_itr, *next_eval_itr, evaluation);

			next_eval_itr++;
			next_CP_itr++;
		}
		if (eval_itr->m_beat_pos == Beat_Position::Down_Beat)
		{
			last_db_eval_itr = eval_itr;
			last_db_CP_itr = CP_itr;

			if (cf_itr != prev_cf_itr)
				prev_cf_itr++;
			cf_itr++;
		}

		CP_itr->add_note_info(Rule_Evaluation::C_INDEX_NAME, Utility::to_str(evaluation));
	}

	//delete filler Notes
	if (length_cf < length_CP)
		for (int i = 0; i < length_CP - length_cf; i++)
			cantus_firmus.pop_back();
	else if (length_cf > length_CP)
		for (int i = 0; i < length_cf - length_CP; i++)
			counter_point.pop_back();
}

void Eval::Rule_Evaluator::r1_perfect_FB_LB(Note_Evaluation& evaluated_note, Music_Note& note, Music_Note& last_cf_note, Rule_Evaluation& evaluation)
{
	if (evaluated_note.m_position == Bar_Position::First_Bar_First_Note
		&& note.m_is_tied)
		return;

	//Start and end must be a perfect interval.
	if (evaluated_note.m_position == Bar_Position::First_Bar_First_Note
		|| evaluated_note.m_position == Bar_Position::Last_Bar_First_Note)
	{
		if (!is_perfect(evaluated_note.m_interval))
		{
			evaluation.broken_rules.push_back(Fux_Rule::R1a);
			evaluation.m_probability *= 0.0f;
		}
	}

	//The Counter Point must be in the same mode as the c.f.
	if ((evaluated_note.m_position == Bar_Position::First_Bar_First_Note)
		&& note.m_voice == Voice::Bass)
	{
		if (get_interval(note, last_cf_note) != Interval::P1
			&& get_interval(note, last_cf_note) != Interval::P8)
		{
			evaluation.broken_rules.push_back(Fux_Rule::R1b);
			evaluation.m_probability *= 0.0f;
		}
	}
}

void Eval::Rule_Evaluator::r2_use_CoMo_and_ObMo(Note_Evaluation& evaluated_note, Music_Note& note, Rule_Evaluation& evaluation)
{
	if (evaluated_note.m_beat_pos == Beat_Position::Down_Beat)
	{
		//Contrary Motion should be preferred
		if (evaluated_note.m_motion == Motion::ObMo)
		{
			evaluation.broken_rules.push_back(Fux_Rule::R2a);
			evaluation.m_probability *= 0.9f;
		}
		//Direct Motion should be avoided
		else if (evaluated_note.m_motion == Motion::DiMo)
		{
			evaluation.broken_rules.push_back(Fux_Rule::R2b);
			evaluation.m_probability *= 0.2f;
		}
	}
}

void Eval::Rule_Evaluator::r3_use_imperfects(Note_Evaluation& evaluated_note, Music_Note& note, Note_Evaluation& next_note_eval, Rule_Evaluation& evaluation)
{
	if (evaluated_note.m_beat_pos == Beat_Position::Down_Beat
		&& evaluated_note.m_position != Bar_Position::First_Bar_First_Note
		&& evaluated_note.m_position != Bar_Position::Last_Bar_First_Note)
	{
		//Use perfect intervals sparingly
		if (is_perfect(evaluated_note.m_interval))
		{
			evaluation.broken_rules.push_back(Fux_Rule::R3a);
			evaluation.m_probability *= 0.7f;
		}
		//Dissonances on the Down Beat are not allowed
		else if (is_dissonant(evaluated_note.m_interval)
			&& !note.m_is_tied)
		{
			evaluation.broken_rules.push_back(Fux_Rule::R3b);
			evaluation.m_probability *= 0.0f;
		}
		else if (is_dissonant(evaluated_note.m_interval)
			&& note.m_is_tied)
		{
			if (next_note_eval.m_direction == Direction::Down
				&& (is_imperfect(next_note_eval.m_interval) || is_perfect(next_note_eval.m_interval))
				&& (next_note_eval.m_jump_interval < Interval::M3
					|| (next_note_eval.m_jump_interval == Interval::P5 && next_note_eval.m_beat_pos == Beat_Position::Weak_Beat_1)))
			{
				//okay
			}
			else
			{
				//Dissonants on the Down Beat should be resolved by a step down.
				if (next_note_eval.m_direction == Direction::Down
					&& (is_imperfect(next_note_eval.m_interval) || is_perfect(next_note_eval.m_interval)))
				{
					evaluation.broken_rules.push_back(Fux_Rule::R3c);
					evaluation.m_probability *= 0.5f;
				}
				//Dissonants on the Down Beat must be resolved.
				else
				{
					evaluation.broken_rules.push_back(Fux_Rule::R3d);
					evaluation.m_probability *= 0.0f;
				}
			}
		}
	}
}

void Eval::Rule_Evaluator::r4_note_before_last(Note_Evaluation& evaluated_note, Music_Note& note, Note_Evaluation& last_eval, Rule_Evaluation& evaluation)
{
	if (evaluated_note.m_position == Bar_Position::Before_Last_Last_Note)
	{
		Voice cf_voice;
		if (note.m_voice == Voice::Bass)
			cf_voice = Voice::Soprano;
		else
			cf_voice = Voice::Bass;

		//The leading tone must be a Major 6th if the c.f. is in the lower voice
		if ((cf_voice == Voice::Bass)
			&& (evaluated_note.m_interval != Interval::M6))
		{
			evaluation.broken_rules.push_back(Fux_Rule::R4a);
			evaluation.m_probability *= 0.0f;
		}
		//The leading tone must be a minor 3rd if the c.f. is in the upper voice
		else if ((cf_voice == Voice::Soprano)
			&& (evaluated_note.m_interval != Interval::m3))
		{
			evaluation.broken_rules.push_back(Fux_Rule::R4b);
			evaluation.m_probability *= 0.0f;
		}
		//The leading tone should be prepared by a fith or a suspension, if not on the downbeat
		if (evaluated_note.m_beat_pos != Beat_Position::Down_Beat)
		{
			if ((evaluated_note.m_jump_interval == Interval::M2
				|| evaluated_note.m_jump_interval == Interval::m2)
				&& evaluated_note.m_direction == Direction::Down)
			{
				//okay
			}
			else if (last_eval.m_interval != Interval::P5)
			{
				evaluation.broken_rules.push_back(Fux_Rule::R4c);
				evaluation.m_probability *= 0.25f;
			}
		}
	}
}	


void Eval::Rule_Evaluator::r5_motion_to_perfect(
	Note_Evaluation& evaluated_note, 
	Music_Note& note,
	Note_Evaluation& last_note_eval, 
	Note_Evaluation& last_db_eval, 
	Music_Note& last_db_note,
	Music_Note& last_cf_note,
	Music_Note& cf_note,
	Rule_Evaluation& evaluation)
{
	if (!note.m_is_tied)
	{
		if (is_perfect(evaluated_note.m_interval)
			&& evaluated_note.m_motion != Motion::CoMo
			&& evaluated_note.m_position != Bar_Position::First_Bar_First_Note
			&& evaluated_note.m_beat_pos == Beat_Position::Down_Beat)
		{
			//to a perfect interval one should only move in contrary motion
			if (!is_perfect(last_note_eval.m_interval))
			{
				evaluation.broken_rules.push_back(Fux_Rule::R5a);
				evaluation.m_probability *= 0.0f;
			}
		}
	}

	//The skip of less then a fourth cannot prevent a succession of either two fifth or two octaves in direct motion
	if ((evaluated_note.m_beat_pos == Beat_Position::Down_Beat)
		&& is_perfect(last_db_eval.m_interval)
		&& is_perfect(evaluated_note.m_interval)
		&& (last_note_eval.m_jump_interval < Interval::P4)
		&& (evaluated_note.m_jump_interval < Interval::P4))
	{
		
		if (get_motion(last_cf_note, cf_note, last_db_note, note) == Motion::DiMo)
		{
			evaluation.broken_rules.push_back(Fux_Rule::R5b);
			evaluation.m_probability *= 0.0f;
		}
	}
}

void Eval::Rule_Evaluator::r6_prohibited_skips(Note_Evaluation& evaluated_note, Music_Note& note, Music_Note& last_note, Music_Note& last_cf_note, Rule_Evaluation& evaluation)
{
	//No Tritone skips
	if (evaluated_note.m_jump_interval == Interval::Tri)
	{
		evaluation.broken_rules.push_back(Fux_Rule::R6a);
		evaluation.m_probability *= 0.0f;
	}
	//No M6 skips
	else if (evaluated_note.m_jump_interval == Interval::M6)
	{
		evaluation.broken_rules.push_back(Fux_Rule::R6b);
		evaluation.m_probability *= 0.0f;
	}
	//No perfect skips from an unison
	else if (!note.m_is_tied
		&& get_interval(last_note, last_cf_note) == Interval::P1
		&& evaluated_note.m_beat_pos == Beat_Position::Down_Beat
		&& is_perfect(evaluated_note.m_jump_interval))
	{
		evaluation.broken_rules.push_back(Fux_Rule::R6c);
		evaluation.m_probability *= 0.0f;
	}
	//No skips bigger than an octave
	else if (std::abs((int)last_note.m_pitch - (int)note.m_pitch) > (int) Interval::P8)
	{
		evaluation.broken_rules.push_back(Fux_Rule::R6d);
		evaluation.m_probability *= 0.1f;
	}
}

void Eval::Rule_Evaluator::r7_tied_only_on_db(Note_Evaluation& evaluated_note, Music_Note& note, Rule_Evaluation& evaluation)
{
	//Tied notes are only allowed on the down beat
	if (evaluated_note.m_beat_pos != Beat_Position::Down_Beat
		&& note.m_is_tied)
	{
		evaluation.broken_rules.push_back(Fux_Rule::R7);
		evaluation.m_probability *= 0.0f;
	}
}


void Eval::Rule_Evaluator::r8_prohibited_resolves(Note_Evaluation& evaluated_note, Music_Note& note, Music_Note& last_note, Note_Evaluation& last_evaluated_note, Rule_Evaluation& evaluation)
{
	bool rule_was_broken = false;
	Voice cf_voice;
	if (note.m_voice == Voice::Bass)
		cf_voice = Voice::Soprano;
	else
		cf_voice = Voice::Bass;

	if (last_note.m_is_tied)
	{
		if (cf_voice == Voice::Bass)
		{
			//Tied notes should not be resolved from an unison to a second
			if (last_evaluated_note.m_interval == Interval::P1
				&& (evaluated_note.m_interval == Interval::m2 || evaluated_note.m_interval == Interval::M2))
			{
				evaluation.broken_rules.push_back(Fux_Rule::R8a);
				evaluation.m_probability *= 0.5f;
			}
			//Tied notes should not be resolved from a second to an unison
			if (evaluated_note.m_interval == Interval::P1
				&& (last_evaluated_note.m_interval == Interval::m2 || last_evaluated_note.m_interval == Interval::M2))
			{
				evaluation.broken_rules.push_back(Fux_Rule::R8b);
				evaluation.m_probability *= 0.5f;
			}
		}
		else if (cf_voice == Voice::Soprano)
		{
			//Tied notes should not be resolved from a seventh to an octave
			if (evaluated_note.m_interval == Interval::P8
				&& (last_evaluated_note.m_interval == Interval::m7 || last_evaluated_note.m_interval == Interval::M7))
			{
				evaluation.broken_rules.push_back(Fux_Rule::R8c);
				evaluation.m_probability *= 0.5f;
			}
		}
	}
}


void Eval::Rule_Evaluator::r9_eights_notes(Note_Evaluation& evaluated_note, Music_Note& note, Note_Evaluation& next_note_eval, Rule_Evaluation& evaluation)
{
	if (note.m_value == Note_Value::Eighth)
	{
		//Eight notes are only allowed on weak beats
		if ((evaluated_note.m_beat_pos == Beat_Position::Down_Beat
			|| evaluated_note.m_beat_pos == Beat_Position::Up_Beat))
		{
			evaluation.broken_rules.push_back(Fux_Rule::R9a);
			evaluation.m_probability *= 0.1f;
		}
		//Eight notes are only allowed by a stepwise approach
		if (evaluated_note.m_jump_interval > Interval::M2
			|| next_note_eval.m_jump_interval > Interval::M2)
		{
			evaluation.broken_rules.push_back(Fux_Rule::R9b);
			evaluation.m_probability *= 0.0f;
		}
	}
}