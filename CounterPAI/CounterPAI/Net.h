#pragma once
#include <torch/torch.h>

namespace Eval
{
	struct Net : torch::nn::Module
	{
		Net(int64_t input, int64_t hidden, int64_t output, int64_t hidden_layer_count = 2);
		torch::Tensor forward(torch::Tensor input);
		torch::nn::LSTM lstm{ nullptr };
		torch::nn::Linear out{ nullptr };


		float m_learning_rate = 0.001;

		void set_learning_rate(double learning_rate);
		torch::Tensor learn_step(torch::Tensor learn_data, torch::Tensor target_data, bool optimize = true);
		torch::Tensor test_prediction(torch::Tensor test_data, torch::Tensor target_data);


		std::shared_ptr<torch::optim::Adam> opti;
	};
}