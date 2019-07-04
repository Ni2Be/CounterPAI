#pragma once
#include "Evaluator_Base.h"

namespace Eval
{

	class Rule_Eval : public Evaluator_Base
	{
	public:
		Rule_Eval();


		void evaluate_notes(
			std::list<Music_Note>& cantus_firmus,
			std::list<Music_Note>& counter_point);

		void r1_perfect_FB_LB(Note_Evaluation& evaluated_note, Music_Note& note, Music_Note& last_cf_note);
		void r2_use_CoMo_and_ObMo(Note_Evaluation& evaluated_note, Music_Note& note);
		void r3_use_imperfects(Note_Evaluation& evaluated_note, Music_Note& note);
		void r4_note_before_last(Note_Evaluation& evaluated_note, Music_Note& note, Note_Evaluation& last_eval);
		void r5_motion_to_perfect(
			Note_Evaluation& evaluated_note, 
			Music_Note& note, 
			Note_Evaluation& last_note_eval, 
			Note_Evaluation& last_db_eval, 
			Music_Note& last_db_note, 
			Music_Note& last_cf_note, 
			Music_Note& cf_note);
		void r6_prohibited_skips(Note_Evaluation& evaluated_note, Music_Note& note, Music_Note& last_note, Music_Note& last_cf_note);
		void r7_allowed_dissonants(Note_Evaluation& evaluated_note, Music_Note& note, Note_Evaluation& next_note_eval);
	};
}
