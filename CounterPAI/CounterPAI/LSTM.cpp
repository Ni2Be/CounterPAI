#include "LSTM.h"


#include <rapidjson/document.h>
#include <rapidjson/istreamwrapper.h>



//----------------------------- LSTM -----------------------------

Eval::LSTM::LSTM(int64_t input, int64_t hidden, int64_t output, int64_t hidden_layer_count, Opti opti, Loss_F loss_f, double dropout, torch::Device device)
	:
	device(device),
	lstm(torch::nn::LSTMOptions(input, hidden).layers(hidden_layer_count).dropout(dropout)),
	s_optimizer(opti),
	s_loss_func(loss_f)
{
	out = register_module("out", torch::nn::Linear(hidden, output));
	register_module("lstm", lstm);
	if (s_optimizer == Opti::ADAM)
		optimizer = std::make_shared<torch::optim::Adam>(torch::optim::Adam(this->parameters(), s_learning_rate));
	else if (s_optimizer == Opti::SGD)
		optimizer = std::make_shared<torch::optim::SGD>(torch::optim::SGD(this->parameters(), s_learning_rate));

	this->to(device);
}

void Eval::LSTM::set_learning_rate(double learning_rate)
{
	s_learning_rate = learning_rate;
	optimizer.reset();
	if (s_optimizer == Opti::ADAM)
		optimizer = std::make_shared<torch::optim::Adam>(torch::optim::Adam(this->parameters(), s_learning_rate));
	else if (s_optimizer == Opti::SGD)
		optimizer = std::make_shared<torch::optim::SGD>(torch::optim::SGD(this->parameters(), s_learning_rate));
}

torch::Tensor Eval::LSTM::forward(torch::Tensor x)
{
	//input (sequence, batch, features) 
	torch::nn::RNNOutput lstm_out = lstm->forward(x.to(device));
	//output (seq_len, batch, num_directions * hidden_size)
	x = lstm_out.output;
	x = out->forward(x).sigmoid();
	return x;
}

torch::Tensor Eval::LSTM::learn_step(torch::Tensor learn_data, torch::Tensor target_data, bool optimize)
{
	try
	{
		this->zero_grad();
		auto new_prediction = this->forward(learn_data.to(device));

		torch::Tensor loss;
		if (s_loss_func == Loss_F::MSL)
			loss = torch::mse_loss(new_prediction, target_data.to(device));
		else if (s_loss_func == Loss_F::BCEL)
			loss = torch::binary_cross_entropy(new_prediction, target_data.to(device));
		if (optimize)
		{
			loss.backward();
			optimizer->step();
		}
		return loss;
	}
	catch (std::exception& e)
	{
		std::cout << "\n\nEXEPTION:\n" << e.what();
		char ch;
		std::cin >> ch;
	}
}

torch::Tensor Eval::LSTM::test_prediction(torch::Tensor test_data, torch::Tensor target_data)
{
	return learn_step(test_data, target_data, false);
}



//----------------------------- LSTM_Settings -----------------------------

Eval::LSTM_Settings::LSTM_Settings(const std::string& parameters_file)
{
	read_parameters(parameters_file);
}

void Eval::LSTM_Settings::read_parameters(const std::string& parameters_file_name)
{
	rapidjson::Document parameter;
	std::ifstream ifs(parameters_file_name);
	if (!ifs)
	{
		std::cerr << "\ncould not open: " + parameters_file_name + "\n";
		return;
	}
	rapidjson::IStreamWrapper isw(ifs);
	parameter.ParseStream(isw);

	test_name = parameter["name"].GetString();

	in_size = parameter["in_size"].GetInt();
	hidden_size = parameter["hidden_size"].GetInt();
	out_size = parameter["out_size"].GetInt();

	hidden_layer_count = parameter["hidden_layer_count"].GetInt();
	batch_size = parameter["batch_size"].GetInt();
	epochs = parameter["epochs"].GetInt();
	dropout = parameter["dropout"].GetDouble();
	optimizer = opti_from_string(parameter["optimizer"].GetString());
	loss_func = loss_f_from_string(parameter["loss_func"].GetString());
	learning_rate = parameter["learning_rate"].GetFloat();
	train_data_folder = parameter["train_data_folder"].GetString();
	test_data_folder = parameter["test_data_folder"].GetString();
	data_converter = parameter["data_converter"].GetString();
}

std::ostream& Eval::operator<<(std::ostream& os, LSTM_Settings& test)
{
	os << "\nTest name: " << test.test_name
		<< "\nin_size: " << test.in_size << ", hidden_size: " << test.hidden_size << ", out_size: " << test.out_size
		<< "\nhidden_layer_count: " << test.hidden_layer_count << ", batch_size: " << test.batch_size << ", epochs: " << test.epochs << ", dropout: " << test.dropout
		<< "\noptimizer: " << test.optimizer << ", loss_func: " << test.loss_func
		<< "\nlearning_rate: " << test.learning_rate
		<< "\ntrain_data_folder: " << test.train_data_folder
		<< "\ntest_data_folder: " << test.test_data_folder
		<< "\ndata_converter: " << test.data_converter
		<< "\n";
	return os;
}