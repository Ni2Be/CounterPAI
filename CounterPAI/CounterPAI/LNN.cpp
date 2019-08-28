#include "LNN.h"


Eval::LNN::LNN(int64_t input, int64_t hidden, int64_t output, int64_t hidden_layer_count, Opti opti, Loss_F loss_f, double dropout, torch::Device device)
	:
	device(device),
	s_optimizer(opti),
	m_hidden_layer_count(hidden_layer_count),
	s_loss_func(loss_f)
{
	in = register_module("in", torch::nn::Linear(input, hidden));

	h0 = register_module("h0", torch::nn::Linear(hidden, hidden));
	if(hidden_layer_count > 1)
		h1 = register_module("h1", torch::nn::Linear(hidden, hidden));
	if (hidden_layer_count > 2)
		h2 = register_module("h2", torch::nn::Linear(hidden, hidden));
	if (hidden_layer_count > 3)
		h3 = register_module("h3", torch::nn::Linear(hidden, hidden));
	if (hidden_layer_count > 4)
		std::cerr << "\nMax 4 hidden layers supported!\n";

	out = register_module("out", torch::nn::Linear(hidden, output));


	if (s_optimizer == Opti::ADAM)
		optimizer = std::make_shared<torch::optim::Adam>(torch::optim::Adam(this->parameters(), s_learning_rate));
	else if (s_optimizer == Opti::SGD)
		optimizer = std::make_shared<torch::optim::SGD>(torch::optim::SGD(this->parameters(), s_learning_rate));

	this->to(device);
}

void Eval::LNN::set_learning_rate(double learning_rate)
{
	s_learning_rate = learning_rate;
	optimizer.reset();
	if (s_optimizer == Opti::ADAM)
		optimizer = std::make_shared<torch::optim::Adam>(torch::optim::Adam(this->parameters(), s_learning_rate));
	else if (s_optimizer == Opti::SGD)
		optimizer = std::make_shared<torch::optim::SGD>(torch::optim::SGD(this->parameters(), s_learning_rate));
}

torch::Tensor Eval::LNN::forward(torch::Tensor x)
{
	x = in->forward(x.to(device));
	x = h0->forward(x).relu();
	if (m_hidden_layer_count > 1)
		x = h1->forward(x).relu();;
	if (m_hidden_layer_count > 2)
		x = h2->forward(x).relu();;
	if (m_hidden_layer_count > 3)
		x = h3->forward(x).relu();;

	x = out->forward(x).sigmoid();
	return x;
}

torch::Tensor Eval::LNN::learn_step(torch::Tensor learn_data, torch::Tensor target_data, bool optimize)
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

torch::Tensor Eval::LNN::test_prediction(torch::Tensor test_data, torch::Tensor target_data)
{
	return learn_step(test_data, target_data, false);
}

std::shared_ptr<torch::optim::Optimizer> Eval::LNN::get_optimizer()
{
	return optimizer;
}