/*

NOT USED AT THE MOMENT

*/

#pragma once
#include "Evaluator_Base.h"

#include <torch/torch.h>

#include "Learn_Settings.h"

namespace Eval
{

	class AI_Evaluator : public Evaluator_Base
	{
	public:
		AI_Evaluator();

		Sheet_Music testing();

		//void train();

		void train_net(Learn_Settings settings);
		void test_net(Learn_Settings settings);
		//void generate_cf();

		//void load_data();

		torch::Device device;


		void evaluate_notes(Sheet_Music& sheet);
	};
}
