#pragma once
#include "Evaluator_Base.h"

#include <torch/torch.h>

namespace Eval
{
	struct LSTM : torch::nn::Module
	{
		enum class Opti { ADAM, SGD };
		
		enum class Loss_F { MSL, BCEL};

		LSTM(int64_t input, int64_t hidden, int64_t output, int64_t hidden_layer_count, Opti opti, Loss_F loss);
		torch::Tensor forward(torch::Tensor input);
		torch::nn::LSTM lstm{ nullptr };
		torch::nn::Linear out{ nullptr };

		//Settings
		float s_learning_rate = 0.001;
		Opti s_optimizer;
		Loss_F s_loss_func;

		void set_learning_rate(double learning_rate);
		torch::Tensor learn_step(torch::Tensor learn_data, torch::Tensor target_data, bool optimize = true);
		torch::Tensor test_prediction(torch::Tensor test_data, torch::Tensor target_data);


		std::shared_ptr<torch::optim::Optimizer> optimizer;
	};

	class LSTM_Settings;
	class LSTM_Eval : public Evaluator_Base
	{
	public:
		LSTM_Eval();

		void train();

		void train_remember_one_cf(LSTM_Settings settings);


		//void generate_cf();

		//void load_data();
	};

	class LSTM_Settings
	{
	public:
		LSTM_Settings(const std::string& parameters_file);
		std::string test_name;

		//LSTM settings
		int in_size, hidden_size, out_size;
		int hidden_layer_count;
		int batch_size;
		int epochs;
		LSTM::Opti optimizer;
		LSTM::Loss_F loss_func;
		float learning_rate;

		std::string train_data_folder;
		std::string test_data_folder;

		//lern rules


		//load
		void read_parameters(const std::string& parameters_file_name);

		friend std::ostream& operator<<(std::ostream& os, LSTM_Settings& test);
	};

	class LSTM_Test_Runner
	{
	public:
		LSTM_Test_Runner();

		LSTM_Eval evaluator;
		void run_test(const std::string& parameters_file);
	};
}



namespace Eval
{
	std::ostream& operator<<(std::ostream& os, Eval::LSTM_Settings& test);

	inline std::ostream& operator<<(std::ostream& os, const LSTM::Loss_F& loss_f)
	{
		static const std::array<std::string, 2> loss_functions
		{
		"MSL",
		"BCEL"
		};

		os << loss_functions[static_cast<int>(loss_f)];
		return os;
	}
	inline std::ostream& operator<<(std::ostream& os, const LSTM::Opti& opti)
	{
		static const std::array<std::string, 2> optimizer
		{
		"ADAM",
		"SGD"
		};

		os << optimizer[static_cast<int>(opti)];
		return os;
	}
	static LSTM::Opti opti_from_string(std::string s)
	{
		if (s == "ADAM") return LSTM::Opti::ADAM;
		if (s == "SGD")  return LSTM::Opti::SGD;
	}
	static LSTM::Loss_F loss_f_from_string(std::string s)
	{
		if (s == "MSL") return LSTM::Loss_F::MSL;
		if (s == "BCEL")  return LSTM::Loss_F::BCEL;
	}
}