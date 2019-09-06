#pragma once
#include "Sheet_Music.h"
#include <list>
#include <string>
#include <array>
#include <sstream>
#include <vector>

#include "Note_Evaluation.h"

namespace Eval
{
	class Evaluator_Base;
	std::ostream& operator<<(std::ostream& os, const Evaluator_Base& eval);
	class Evaluator_Base
	{
	public:
		Evaluator_Base();

		std::list<Music_Note> cantus_firmus;

		std::list<Music_Note> counter_point;
		std::vector<Note_Evaluation> m_evaluation;

		virtual void evaluate_notes(Sheet_Music& sheet);

		Interval get_interval(Music_Note n0, Music_Note n1);
		Direction get_direction(Music_Note n0, Music_Note n1);
		Motion get_motion(Music_Note cf0, Music_Note cf1, Music_Note CP0, Music_Note CP1);
		Bar_Position get_bar_pos(std::list<Music_Note>& voice, const std::list<Music_Note>::iterator& note);
		Beat_Position get_beat_pos(std::list<Music_Note>& voice, const std::list<Music_Note>::iterator& note);

		friend std::ostream& operator<<(std::ostream& os, const Evaluator_Base& eval);
	};
}
