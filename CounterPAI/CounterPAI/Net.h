#pragma once

#include <torch/torch.h>

#include "Learn_Settings.h"

namespace Eval 
{
	struct Net : torch::nn::Module
	{
		virtual torch::Tensor forward(torch::Tensor input) = 0;

		virtual void set_learning_rate(double learning_rate) = 0;
		virtual torch::Tensor learn_step(const torch::Tensor&  learn_data, torch::Tensor target_data, bool optimize = true) = 0;
		virtual torch::Tensor test_prediction(torch::Tensor test_data, torch::Tensor target_data) = 0;

		virtual std::shared_ptr<torch::optim::Optimizer> get_optimizer() = 0;
	};
}