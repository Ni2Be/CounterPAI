#pragma once

#include <torch/torch.h>

#include "Net.h"

namespace Eval
{
	struct LSTM : Net
	{
		LSTM(int64_t input, int64_t hidden, int64_t output, int64_t hidden_layer_count, Opti opti, Loss_F loss, double dropout, torch::Device device);
		torch::Tensor forward(torch::Tensor input);
		torch::Device device;
		torch::nn::LSTM lstm{ nullptr };
		torch::nn::Linear out{ nullptr };

		//Settings
		float s_learning_rate = 0.001;
		Opti s_optimizer;
		Loss_F s_loss_func;

		void set_learning_rate(double learning_rate);
		torch::Tensor learn_step(const torch::Tensor& learn_data, torch::Tensor target_data, bool optimize = true);
		torch::Tensor test_prediction(torch::Tensor test_data, torch::Tensor target_data);

		std::shared_ptr<torch::optim::Optimizer> get_optimizer();

		std::shared_ptr<torch::optim::Optimizer> optimizer;
	};
}
