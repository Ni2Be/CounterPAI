#include "Net.h"

Eval::Net::Net(int64_t input, int64_t hidden, int64_t output, int64_t hidden_layer_count)
	:
	lstm(torch::nn::LSTMOptions(input, hidden).layers(hidden_layer_count))
{
	//lstm = register_module("lstm", torch::nn::LSTM(input, hidden));
	//lstm->options = torch::nn::LSTMOptions(input, hidden).layers(3);
	out = register_module("out", torch::nn::Linear(hidden, output));
	
	;

	opti = std::make_shared<torch::optim::SGD>(torch::optim::SGD(this->parameters(), m_learning_rate));
}

void Eval::Net::set_learning_rate(double learning_rate)
{
	m_learning_rate = learning_rate;
	opti.reset();
	opti = std::make_shared<torch::optim::SGD>(torch::optim::SGD(this->parameters(), m_learning_rate));
}

torch::Tensor Eval::Net::forward(torch::Tensor x)
{
	//input (sequence, batch, features) 
	torch::nn::RNNOutput lstm_out = lstm->forward(x);
	//output (seq_len, batch, num_directions * hidden_size)
	x = lstm_out.output;
	//std::cout << "\noutput: " << x;
	//input ()
	x = out->forward(x).sigmoid();
	//std::cout << "\nreturn: " << x;
	return x;
}

torch::Tensor Eval::Net::learn_step(torch::Tensor learn_data, torch::Tensor target_data, bool optimize)
{
	try
	{
		this->zero_grad();
		auto new_prediction = this->forward(learn_data);
		//std::cout << "\nnew_prediction: " << new_prediction;
		//std::cout << "\ntarget_data: " << target_data;
		auto loss = torch::mse_loss(new_prediction, target_data);
		if (optimize)
		{
			loss.backward();
			opti->step();
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

torch::Tensor Eval::Net::test_prediction(torch::Tensor test_data, torch::Tensor target_data)
{
	return learn_step(test_data, target_data, false);
}