#pragma once

#include <torch/torch.h>

#include "Net.h"

namespace Eval
{
	struct NN : Net
	{
		NN(int64_t input, int64_t hidden, int64_t output, int64_t hidden_layer_count, Opti opti, Loss_F loss, double dropout, torch::Device device);
		torch::Tensor forward(torch::Tensor input);
		torch::Device device;
		torch::nn::Linear in{ nullptr };
		torch::nn::Linear h0{ nullptr };
		torch::nn::Linear h1{ nullptr };
		torch::nn::Linear h2{ nullptr };
		torch::nn::Linear h3{ nullptr };
		torch::nn::Linear out{ nullptr };

		//Settings
		float s_learning_rate = 0.001;
		Opti s_optimizer;
		Loss_F s_loss_func;

		int m_hidden_layer_count = 1;

		void set_learning_rate(double learning_rate);
		torch::Tensor learn_step(const torch::Tensor&  learn_data, torch::Tensor target_data, bool optimize = true);
		torch::Tensor test_prediction(torch::Tensor test_data, torch::Tensor target_data);

		std::shared_ptr<torch::optim::Optimizer> get_optimizer();

		std::shared_ptr<torch::optim::Optimizer> optimizer;
	};
}

