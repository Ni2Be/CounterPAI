#pragma once
#include <torch/torch.h>

struct Net : torch::nn::Module
{
	Net(int64_t input, int64_t hidden, int64_t output);
	at::Tensor forward(at::Tensor input);
	torch::nn::Linear in{ nullptr };
	torch::nn::Linear h{ nullptr };
	torch::nn::Linear out{ nullptr };
	at::Tensor another_bias;


	float m_learning_rate = 0.001;

	void set_learning_rate(double learning_rate);
	at::Tensor learn_step(at::Tensor learn_data, at::Tensor target_data);
	std::shared_ptr<torch::optim::SGD> opti;
};