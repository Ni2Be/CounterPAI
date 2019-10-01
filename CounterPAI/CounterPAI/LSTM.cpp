#pragma once
#include "LSTM.h"


Eval::LSTM::LSTM(int64_t input, int64_t hidden, int64_t output, int64_t hidden_layer_count, Opti opti, Loss_F loss_f, double dropout, bool is_bidirectional, torch::Device device)
	:
	device(device),
	lstm(torch::nn::LSTMOptions(input, hidden).layers(hidden_layer_count).dropout(dropout).batch_first(false)),
	s_optimizer(opti),
	s_loss_func(loss_f)
{
	//not working in libtorch 1.2
	//if (is_bidirectional)
	//	lstm->options.bidirectional(true);

	register_module("lstm", lstm);
	out = register_module("out", torch::nn::Linear(hidden, output));
	if (s_optimizer == Opti::ADAM)
		optimizer = std::make_shared<torch::optim::Adam>(torch::optim::Adam(this->parameters(), torch::optim::AdamOptions(s_learning_rate).weight_decay(0.00005)));
	else if (s_optimizer == Opti::SGD)
		optimizer = std::make_shared<torch::optim::SGD>(torch::optim::SGD(this->parameters(), s_learning_rate));

	this->to(device);
}

void Eval::LSTM::set_learning_rate(double learning_rate)
{
	s_learning_rate = learning_rate;
	optimizer.reset();
	if (s_optimizer == Opti::ADAM)
		optimizer = std::make_shared<torch::optim::Adam>(torch::optim::Adam(this->parameters(), torch::optim::AdamOptions(s_learning_rate).weight_decay(0.00005)));
	else if (s_optimizer == Opti::SGD)
		optimizer = std::make_shared<torch::optim::SGD>(torch::optim::SGD(this->parameters(), s_learning_rate));
}

torch::Tensor Eval::LSTM::forward(torch::Tensor x)
{
	//input (sequence, batch, features) 
	torch::nn::RNNOutput new_prediction = lstm->forward(x.to(device));

	//output (seq_len, batch, num_directions * hidden_size)
	x = new_prediction.output[-1];
	x = out->forward(x).sigmoid();

	return x;
}

torch::Tensor Eval::LSTM::learn_step(const torch::Tensor& learn_data, torch::Tensor target_data, bool optimize)
{
	try
	{
		this->zero_grad();
		auto x = learn_data;

		torch::nn::RNNOutput new_prediction = lstm->forward(x.to(device));

		x = new_prediction.output[-1];
		x = out->forward(x).sigmoid();

		torch::Tensor loss;
		if (s_loss_func == Loss_F::MSL)
			loss = torch::mse_loss(x, target_data.view({ -1, 1 }).to(device));
		else if (s_loss_func == Loss_F::BCEL)
			loss = torch::binary_cross_entropy(x, target_data.view({-1, 1 }).to(device));
		if (optimize)
		{
			//std::cout << x << "\n";
			//std::cout << target_data.view({ -1, 1 }) << "\n";
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


std::shared_ptr<torch::optim::Optimizer> Eval::LSTM::get_optimizer()
{
	return optimizer;
}