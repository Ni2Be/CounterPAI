#pragma once
#include "Evaluator_Base.h"

#include <torch/torch.h>

#include "LSTM.h"

namespace Eval
{
	class LSTM_Eval : public Evaluator_Base
	{
	public:
		LSTM_Eval();

		Sheet_Music testing();

		//void train();

		void train_net(LSTM_Settings settings);

		//void generate_cf();

		//void load_data();

		torch::Device device;
	};

	class LSTM_Test_Runner
	{
	public:
		LSTM_Test_Runner();

		LSTM_Eval evaluator;
		void run_test(const std::string& parameters_file);
	};
}
