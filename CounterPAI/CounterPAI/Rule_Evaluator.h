#pragma once
#include "Evaluator_Base.h"

#include "Rule_Evaluation.h"
namespace Eval
{

	class Rule_Evaluator : public Evaluator_Base
	{
	public:
		Rule_Evaluator();

		void evaluate_notes(Sheet_Music& sheet);

		
		void r1_perfect_FB_LB(Note_Evaluation& evaluated_note, Music_Note& note, Music_Note& last_cf_note, Rule_Evaluation& evaluation);
		void r2_use_CoMo_and_ObMo(Note_Evaluation& evaluated_note, Music_Note& note, Rule_Evaluation& evaluation);
		void r3_use_imperfects(Note_Evaluation& evaluated_note, Music_Note& note, Note_Evaluation& next_note_eval, Rule_Evaluation& evaluation);
		void r4_note_before_last(Note_Evaluation& evaluated_note, Music_Note& note, Note_Evaluation& last_eval, Rule_Evaluation& evaluation);
		void r5_motion_to_perfect(
			Note_Evaluation& evaluated_note, 
			Music_Note& note, 
			Note_Evaluation& last_note_eval, 
			Note_Evaluation& last_db_eval, 
			Music_Note& last_db_note, 
			Music_Note& last_cf_note, 
			Music_Note& cf_note, 
			Rule_Evaluation& evaluation);
		void r6_prohibited_skips(Note_Evaluation& evaluated_note, Music_Note& note, Music_Note& last_note, Music_Note& last_cf_note, Rule_Evaluation& evaluation);
		//void r7_allowed_dissonants(Note_Evaluation& evaluated_note, Music_Note& note, Note_Evaluation& next_note_eval, Rule_Evaluation& evaluation);
		void r7_tied_only_on_db(Note_Evaluation& evaluated_note, Music_Note& note, Rule_Evaluation& evaluation);
		void r8_prohibited_resolves(Note_Evaluation& evaluated_note, Music_Note& note, Music_Note& last_note, Note_Evaluation& last_evaluated_note, Rule_Evaluation& evaluation);
		void r9_eights_notes(Note_Evaluation& evaluated_note, Music_Note& note, Note_Evaluation& next_note_eval, Rule_Evaluation& evaluation);
	};
}
