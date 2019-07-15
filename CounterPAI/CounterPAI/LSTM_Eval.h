#pragma once
#include "Evaluator_Base.h"

#include <torch/torch.h>

#include "Net.h"

namespace Eval
{
	class LSTM_Eval : public Evaluator_Base
	{
	public:
		LSTM_Eval();

		void train();
		void train_cf();

		void generate_cf();

		void load_data();
	};
}
