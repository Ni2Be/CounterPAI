#include "Rule_Eval.h"



Eval::Rule_Eval::Rule_Eval()
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
void Eval::Rule_Eval::evaluate_notes(
	std::list<Music_Note>& cantus_firmus,
	std::list<Music_Note>& counter_point)
{
	Evaluator_Base::evaluate_notes(cantus_firmus, counter_point);

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
		CP_itr->m_note_info += "\n";
		r1_perfect_FB_LB(*eval_itr, *CP_itr, cantus_firmus.back());
		r2_use_CoMo_and_ObMo(*eval_itr, *CP_itr);
		
		r8_tied_only_on_db(*eval_itr, *CP_itr);


		if (eval_itr != prev_eval_itr)//just skips the first note for this rules
		{
			r4_note_before_last(*eval_itr, *CP_itr, *prev_eval_itr);

			r5_motion_to_perfect(*eval_itr, *CP_itr, *prev_eval_itr, *last_db_eval_itr, *last_db_CP_itr, *prev_cf_itr, *cf_itr);

			r6_prohibited_skips(*eval_itr, *CP_itr, *prev_CP_itr, cantus_firmus.back());

			r9_prohibited_resolves(*eval_itr, *CP_itr, *prev_CP_itr, *prev_eval_itr);

			prev_eval_itr++;
			prev_CP_itr++;
		}
		if (next_eval_itr != m_evaluation.end())//just skips the last note for this rules
		{
			r3_use_imperfects(*eval_itr, *CP_itr, *next_eval_itr);

			r7_allowed_dissonants(*eval_itr, *CP_itr, *next_eval_itr);

			r10_eights_notes(*eval_itr, *CP_itr, *next_eval_itr);

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

		CP_itr->m_probability = eval_itr->m_probability;
	}
}

void Eval::Rule_Eval::r1_perfect_FB_LB(Note_Evaluation& evaluated_note, Music_Note& note, Music_Note& last_cf_note)
{
	if (evaluated_note.m_position == Bar_Position::First_Bar_First_Note
		|| evaluated_note.m_position == Bar_Position::Last_Bar_First_Note)
	{
		if (!is_perfect(evaluated_note.m_interval))
		{
			note.m_note_info += "\n-Start and end must be a perfect interval.";
			evaluated_note.m_probability *= 0.0f;
		}
	}

	if ((evaluated_note.m_position == Bar_Position::First_Bar_First_Note)
		&& note.m_voice == Voice::Bass)
	{
		if (get_interval(note, last_cf_note) != Interval::P1
			&& get_interval(note, last_cf_note) != Interval::P8)
		{
			note.m_note_info += "\n-The Counter Point must be in the same mode as the c.f.";
			evaluated_note.m_probability *= 0.0f;
		}
	}
}

void Eval::Rule_Eval::r2_use_CoMo_and_ObMo(Note_Evaluation& evaluated_note, Music_Note& note)
{
	if (evaluated_note.m_beat_pos == Beat_Position::Down_Beat)
	{
		/*if (evaluated_note.m_motion == Motion::CoMo)
		{
			note.m_note_info += "\n-Contrary Motion is good!";
			evaluated_note.m_probability *= 1.0f;
		}
		else*/ if (evaluated_note.m_motion == Motion::ObMo)
		{
			note.m_note_info += "\n-Oblique Motion is Okay!";
			evaluated_note.m_probability *= 0.9f;
		}
		else if (evaluated_note.m_motion == Motion::DiMo)
		{
			note.m_note_info += "\n-Direct Motion should be avoided!";
			evaluated_note.m_probability *= 0.5f;
		}
	}
}

void Eval::Rule_Eval::r3_use_imperfects(Note_Evaluation& evaluated_note, Music_Note& note, Note_Evaluation& next_note_eval)
{
	if (evaluated_note.m_beat_pos == Beat_Position::Down_Beat
		&& evaluated_note.m_position != Bar_Position::First_Bar_First_Note
		&& evaluated_note.m_position != Bar_Position::Last_Bar_First_Note)
	{
		if (is_perfect(evaluated_note.m_interval))
		{
			note.m_note_info += "\n-To many perfect intervals are bad";
			evaluated_note.m_probability *= 0.7f;
		}
		/*else if (is_imperfect(evaluated_note.m_interval))
		{
			note.m_note_info += "\n-Good an imperfect consonance";
			evaluated_note.m_probability *= 1.0f;
		}*/
		else if (is_dissonant(evaluated_note.m_interval)
			&& !note.m_is_tied)
		{
			note.m_note_info += "\n-dissonances on the Down Beat are not allowed";
			evaluated_note.m_probability *= 0.0f;
		}
		else if (is_dissonant(evaluated_note.m_interval)
			&& note.m_is_tied)
		{
			if (next_note_eval.m_direction == Direction::Down
				&& (is_imperfect(next_note_eval.m_interval) || is_perfect(next_note_eval.m_interval))
				&& next_note_eval.m_jump_interval < Interval::M3)
			{
				//okay
			}
			else
			{
				if (next_note_eval.m_direction == Direction::Down
					&& (is_imperfect(next_note_eval.m_interval) || is_perfect(next_note_eval.m_interval)))
				{
					note.m_note_info += "\n-dissonances on the Down Beat should be resolved by a step down";
					evaluated_note.m_probability *= 0.5f;
				}
				else
				{
					note.m_note_info += "\n-dissonances on the Down Beat must be resolved";
					evaluated_note.m_probability *= 0.0f;
				}
			}
		}
	}
}

void Eval::Rule_Eval::r4_note_before_last(Note_Evaluation& evaluated_note, Music_Note& note, Note_Evaluation& last_eval)
{
	if (evaluated_note.m_position == Bar_Position::Before_Last_Last_Note)
	{
		Voice cf_voice;
		if (note.m_voice == Voice::Bass)
			cf_voice = Voice::Soprano;
		else
			cf_voice = Voice::Bass;

		if ((cf_voice == Voice::Bass)
			&& (evaluated_note.m_interval != Interval::M6))
		{
			note.m_note_info += "\n-The leading tone must be a Major 6th if the c.f. is in the lower voice";
			evaluated_note.m_probability *= 0.0f;
		}
		/*else if ((cf_voice == Voice::Bass)
			&& (evaluated_note.m_interval == Interval::M6))
		{
			note.m_note_info += "\n-Good the leading tone is a Major 6th an the c.f. is in the lower voice";
		}*/
		else if ((cf_voice == Voice::Soprano)
			&& (evaluated_note.m_interval != Interval::m3))
		{
			note.m_note_info += "\n-The leading tone must be a minor 3rd if the c.f. is in the upper voice";
			evaluated_note.m_probability *= 0.0f;
		}
		/*else if ((cf_voice == Voice::Soprano)
			&& (evaluated_note.m_interval == Interval::m3))
		{
			note.m_note_info += "\n-Good the leading tone is a minor 3rd an the c.f. is in the upper voice";
		}*/

		if (evaluated_note.m_beat_pos != Beat_Position::Down_Beat)
		{
			if (last_eval.m_interval != Interval::P5)
			{
				note.m_note_info += "\n-The leading tone should be prepared by a fith, if not on the downbeat";
				evaluated_note.m_probability *= 0.25f;
			}
		}
	}

}	


void Eval::Rule_Eval::r5_motion_to_perfect(
	Note_Evaluation& evaluated_note, 
	Music_Note& note,
	Note_Evaluation& last_note_eval, 
	Note_Evaluation& last_db_eval, 
	Music_Note& last_db_note,
	Music_Note& last_cf_note,
	Music_Note& cf_note)
{
	if (is_perfect(evaluated_note.m_interval)
		&& evaluated_note.m_motion != Motion::CoMo
		&& evaluated_note.m_position != Bar_Position::First_Bar_First_Note
		&& evaluated_note.m_beat_pos == Beat_Position::Down_Beat)
	{
		if(!is_perfect(last_note_eval.m_interval))
		{
			note.m_note_info += "\n-From an imperfect to a perfect interval one should only move in contrary motion";
			evaluated_note.m_probability *= 0.0f;
		}
		else if (is_perfect(last_note_eval.m_interval)
			&& evaluated_note.m_motion != Motion::ObMo)
		{
			note.m_note_info += "\n-From a perfect to a perfect interval one should only move in contrary or oblique motion";
			evaluated_note.m_probability *= 0.0f;
		}
	}

	//R5a
	std::cout << "\nn: " << get_motion(last_cf_note, cf_note, last_db_note, note);
	if ((evaluated_note.m_beat_pos == Beat_Position::Down_Beat)
		&& is_perfect(last_db_eval.m_interval)
		&& is_perfect(evaluated_note.m_interval)
		&& (last_note_eval.m_jump_interval < Interval::P4)
		&& (evaluated_note.m_jump_interval < Interval::P4))
	{
		
		if (get_motion(last_cf_note, cf_note, last_db_note, note) == Motion::DiMo)
		{
			note.m_note_info += "\n-The skip of less then a fourth cannot prevent a succession of either two fifth or two octaves in direct motion";
			evaluated_note.m_probability *= 0.0f;
		}
	}
}

void Eval::Rule_Eval::r6_prohibited_skips(Note_Evaluation& evaluated_note, Music_Note& note, Music_Note& last_note, Music_Note& last_cf_note)
{
	if (evaluated_note.m_jump_interval == Interval::Tri)
	{
		note.m_note_info += "\n-No Tritone skips";
		evaluated_note.m_probability *= 0.0f;
	}
	else if (evaluated_note.m_jump_interval == Interval::M6)
	{
		note.m_note_info += "\n-No M6 skips";
		evaluated_note.m_probability *= 0.0f;
	}
	else if (get_interval(last_note, last_cf_note) == Interval::P1
		&& evaluated_note.m_beat_pos == Beat_Position::Down_Beat
		&& is_perfect(evaluated_note.m_jump_interval))
	{
		note.m_note_info += "\n-No perfect skips from an unison";
		evaluated_note.m_probability *= 0.0f;
	}
}

void Eval::Rule_Eval::r7_allowed_dissonants(Note_Evaluation& evaluated_note, Music_Note& note, Note_Evaluation& next_note_eval)
{
	if (evaluated_note.m_beat_pos != Beat_Position::Down_Beat)
	{
		if (is_dissonant(evaluated_note.m_interval))
		{
			if (evaluated_note.m_jump_interval > Interval::M2)
			{
				note.m_note_info += "\n-Dissonance on the upbeat is not prepared by a step";
				evaluated_note.m_probability *= 0.0f;
			}
			/*else
			{
				note.m_note_info += "\n-Dissonance on the upbeat is prepared by a step";
			}*/
			if (next_note_eval.m_jump_interval > Interval::M2)
			{
				note.m_note_info += "\n-Dissonance on the upbeat is not released by a step";
				evaluated_note.m_probability *= 0.0f;
			}
			/*else
			{
				note.m_note_info += "\n-Dissonance on the upbeat is released by a step";
			}*/
		}
	}
}

void Eval::Rule_Eval::r8_tied_only_on_db(Note_Evaluation& evaluated_note, Music_Note& note)
{
	if (evaluated_note.m_beat_pos != Beat_Position::Down_Beat
		&& note.m_is_tied)
	{
		note.m_note_info += "\n-Tied notes are only allowed on the down beat";
		evaluated_note.m_probability *= 0.0f;
	}
}


void Eval::Rule_Eval::r9_prohibited_resolves(Note_Evaluation& evaluated_note, Music_Note& note, Music_Note& last_note, Note_Evaluation& last_evaluated_note)
{
	Voice cf_voice;
	if (note.m_voice == Voice::Bass)
		cf_voice = Voice::Soprano;
	else
		cf_voice == Voice::Bass;

	if (last_note.m_is_tied)
	{
		if (cf_voice == Voice::Bass)
		{
			if (last_evaluated_note.m_interval == Interval::P1
				&& (evaluated_note.m_interval == Interval::m2 || evaluated_note.m_interval == Interval::M2))
			{
				note.m_note_info += "\n-Tied notes should not be resolved from an unison to a second";
				evaluated_note.m_probability *= 0.5f;
			}
			if (evaluated_note.m_interval == Interval::P1
				&& (last_evaluated_note.m_interval == Interval::m2 || last_evaluated_note.m_interval == Interval::M2))
			{
				note.m_note_info += "\n-Tied notes should not be resolved from a second to an unison";
				evaluated_note.m_probability *= 0.5f;
			}
		}
		else if (cf_voice == Voice::Soprano)
		{
			if (evaluated_note.m_interval == Interval::P8
				&& (last_evaluated_note.m_interval == Interval::m7 || last_evaluated_note.m_interval == Interval::M7))
			{
				note.m_note_info += "\n-Tied notes should not be resolved from a seventh to an octave";
				evaluated_note.m_probability *= 0.5f;
			}
		}
	}
}


void Eval::Rule_Eval::r10_eights_notes(Note_Evaluation& evaluated_note, Music_Note& note, Note_Evaluation& next_note_eval)
{
	if (note.m_value == Note_Value::Eighth)
	{
		if ((evaluated_note.m_beat_pos == Beat_Position::Down_Beat
			|| evaluated_note.m_beat_pos == Beat_Position::Up_Beat))
		{
			note.m_note_info += "\n-Eight notes are only allowed on weak beats";
			evaluated_note.m_probability *= 0.1f;
		}
		if (evaluated_note.m_jump_interval > Interval::M2
			|| next_note_eval.m_jump_interval > Interval::M2)
		{
			note.m_note_info += "\n-Eight notes are only allowed by a stepwise approach";
			evaluated_note.m_probability *= 0.0f;
		}
	}
}