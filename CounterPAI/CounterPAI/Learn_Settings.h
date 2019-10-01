#pragma once

#include <torch/torch.h>


namespace Eval
{
	enum class NN_Type { LSTM, LNN };
	
	enum class Opti { ADAM, SGD };

	enum class Loss_F { MSL, BCEL };



	class Learn_Settings
	{
	public:
		Learn_Settings(const std::string& parameters_file);
		std::string test_name;

		//LSTM settings
		NN_Type nn_type;
		int in_size, hidden_size, out_size;
		int hidden_layer_count;
		int batch_size;
		int epochs;
		double dropout;
		Opti optimizer;
		Loss_F loss_func;
		float learning_rate;
		bool is_bidirectional;

		std::string data_converter_info;

		std::string train_data_folder;
		std::string test_data_folder;
		std::string valid_data_folder;

		//lern data
		std::string data_converter;

		//load
		void read_parameters(const std::string& parameters_file_name);


		friend std::ostream& operator<<(std::ostream& os, Learn_Settings& test);
	};
}

//INPUT OUTPUT
namespace Eval
{
	std::ostream& operator<<(std::ostream& os, Eval::Learn_Settings& test);

	inline std::ostream& operator<<(std::ostream& os, const NN_Type& nn_type)
	{
		static const std::array<std::string, 2> nn_types
		{
		"LSTM",
		"LNN"
		};

		os << nn_types[static_cast<int>(nn_type)];
		return os;
	}

	inline std::ostream& operator<<(std::ostream& os, const Loss_F& loss_f)
	{
		static const std::array<std::string, 2> loss_functions
		{
		"MSL",
		"BCEL"
		};

		os << loss_functions[static_cast<int>(loss_f)];
		return os;
	}
	inline std::ostream& operator<<(std::ostream& os, const Opti& opti)
	{
		static const std::array<std::string, 2> optimizer
		{
		"ADAM",
		"SGD"
		};

		os << optimizer[static_cast<int>(opti)];
		return os;
	}
	static NN_Type NN_type_from_string(std::string s)
	{
		if (s == "LSTM") return NN_Type::LSTM;
		if (s == "LNN")  return NN_Type::LNN;
	}
	static Opti opti_from_string(std::string s)
	{
		if (s == "ADAM") return Opti::ADAM;
		if (s == "SGD")  return Opti::SGD;
	}
	static Loss_F loss_f_from_string(std::string s)
	{
		if (s == "MSL") return Loss_F::MSL;
		if (s == "BCEL")  return Loss_F::BCEL;
	}
}