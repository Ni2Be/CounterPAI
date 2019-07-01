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
	std::list<Music_Note>::iterator CP_itr = counter_point.begin();
	std::list<Music_Note>::iterator prev_CP_itr = counter_point.begin();
	
	for (; eval_itr != m_evaluation.end(); eval_itr++, CP_itr++)
	{
		CP_itr->m_note_info += "\n";
		r1_perfect_FB_LB(*eval_itr, *CP_itr, cantus_firmus.back());
		r2_use_CoMo_and_ObMo(*eval_itr, *CP_itr);
		r3_use_imperfects(*eval_itr, *CP_itr);
		r4_note_before_last(*eval_itr, *CP_itr);
		r5_motion_to_perfect(*eval_itr, *CP_itr);
		
		if (eval_itr != prev_eval_itr)//just skips the first note for this rule
		{
			r6_prohibited_skips(*eval_itr, *CP_itr, *prev_CP_itr, cantus_firmus.back());
			prev_eval_itr++;
			prev_CP_itr++;
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
		if (evaluated_note.m_motion == Motion::CoMo)
		{
			note.m_note_info += "\n-Contrary Motion is good!";
			evaluated_note.m_probability *= 1.0f;
		}
		else if (evaluated_note.m_motion == Motion::ObMo)
		{
			note.m_note_info += "\n-Oblique Motion is Okay!";
			evaluated_note.m_probability *= 0.9f;
		}
		else if (evaluated_note.m_motion == Motion::DiMo)
		{
			note.m_note_info += "\n-Direct Motion should be avoided!";
			evaluated_note.m_probability *= 0.3f;
		}
	}
}

void Eval::Rule_Eval::r3_use_imperfects(Note_Evaluation& evaluated_note, Music_Note& note)
{
	if (evaluated_note.m_beat_pos == Beat_Position::Down_Beat
		&& evaluated_note.m_position != Bar_Position::First_Bar_First_Note
		&& evaluated_note.m_position != Bar_Position::Last_Bar_First_Note)
	{
		if (is_perfect(evaluated_note.m_interval))
		{
			note.m_note_info += "\n-Use more imperfect consonances";
			evaluated_note.m_probability *= 0.7f;
		}
		else if (is_imperfect(evaluated_note.m_interval))
		{
			note.m_note_info += "\n-Good an imperfect consonance";
			evaluated_note.m_probability *= 1.0f;
		}
		else if (is_dissonant(evaluated_note.m_interval))
		{
			note.m_note_info += "\n-No dissonances on the Down Beat";
			evaluated_note.m_probability *= 0.0f;
		}
	}
}

void Eval::Rule_Eval::r4_note_before_last(Note_Evaluation& evaluated_note, Music_Note& note)
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
		else if ((cf_voice == Voice::Bass)
			&& (evaluated_note.m_interval == Interval::M6))
		{
			note.m_note_info += "\n-Good the leading tone is a Major 6th an the c.f. is in the lower voice";
		}
		else if ((cf_voice == Voice::Soprano)
			&& (evaluated_note.m_interval != Interval::m3))
		{
			note.m_note_info += "\n-The leading tone must be a minor 3rd if the c.f. is in the upper voice";
			evaluated_note.m_probability *= 0.0f;
		}
		else if ((cf_voice == Voice::Soprano)
			&& (evaluated_note.m_interval == Interval::m3))
		{
			note.m_note_info += "\n-Good the leading tone is a minor 3rd an the c.f. is in the upper voice";
		}
	}
}	


void Eval::Rule_Eval::r5_motion_to_perfect(Note_Evaluation& evaluated_note, Music_Note& note)
{
	if (is_perfect(evaluated_note.m_interval)
		&& evaluated_note.m_motion != Motion::CoMo
		&& evaluated_note.m_position != Bar_Position::First_Bar_First_Note)
	{
		note.m_note_info += "\n-To a perfect interval one should only move in contrary motion";
		evaluated_note.m_probability *= 0.0f;
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
		&& is_perfect(evaluated_note.m_jump_interval))
	{
		note.m_note_info += "\n-No perfect skips from an unison";
		evaluated_note.m_probability *= 0.0f;
	}
}
