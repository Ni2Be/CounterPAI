#include "Net.h"

Net::Net(int64_t input, int64_t output)
{
	in = register_module("in", torch::nn::LSTM(input, input));
	out = register_module("out", torch::nn::Linear(input, output));

	opti = std::make_shared<torch::optim::SGD>(torch::optim::SGD(this->parameters(), m_learning_rate));
}

void Net::set_learning_rate(double learning_rate)
{
	m_learning_rate = learning_rate;
	opti.reset();
	opti = std::make_shared<torch::optim::SGD>(torch::optim::SGD(this->parameters(), m_learning_rate));
}

at::Tensor Net::forward(at::Tensor x)
{
	x = in->forward(x).output.max();
	//x = torch::sigmoid(h->forward(x));
	x = torch::sigmoid(out->forward(x));
	return x;
}

at::Tensor Net::learn_step(at::Tensor learn_data, at::Tensor target_data)
{
	opti->zero_grad();
	auto new_prediction = this->forward(learn_data);
	auto loss = torch::mse_loss(new_prediction, target_data);
	loss.backward();
	opti->step();
	return loss;
}