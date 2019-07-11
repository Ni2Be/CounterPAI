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
		~LSTM_Eval();

		void train();

		void load_data();
	};
}
