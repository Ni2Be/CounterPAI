#include "LSTM_Eval.h"



Eval::LSTM_Eval::LSTM_Eval()
{


}


Eval::LSTM_Eval::~LSTM_Eval()
{
}


void Eval::LSTM_Eval::train()
{
	int in_size = 3, hidden_size = 10, out_size = 1;
	int batch_size = 10, sequence_size = 10;

	auto model = std::make_shared<Net>(Net(in_size, hidden_size, out_size));

	model->set_learning_rate(0.04);

	//input (sequence, batch, features) 
	auto learn_data = torch::randn({ sequence_size, batch_size, in_size });
	auto test_data = torch::randn({ sequence_size, batch_size, in_size });

	std::vector<float> vec(batch_size * out_size * sequence_size, 0.33);
	assert(vec.size() == batch_size * out_size);
	auto target_data = torch::tensor(vec, at::requires_grad(false).dtype(torch::kFloat32)).view({ sequence_size, batch_size, out_size });

	std::cout << "learn_data :\n" << learn_data << std::endl << std::endl;
	std::cout << "target_data :\n" << target_data << std::endl << std::endl;


	std::cout << std::endl << "Vor lernen: " << std::endl;
	for (const auto& p : model->named_parameters())
	{
		std::cout << p.key() << std::endl << p.value() << std::endl;
	}

	auto prediction = model->forward(learn_data);
	std::cout << std::endl << std::endl << "Prediction:\n" << prediction << std::endl;

	std::cout << std::endl << "learning..." << std::endl;
	for (size_t epoch = 0; epoch < 1000; epoch++)
	{
		auto loss = model->learn_step(learn_data, target_data);

		if (epoch % 100 == 0)
		{
			auto test_data_loss = model->test_prediction(test_data, target_data);

			std::cout << "epoch: " << epoch << ", Loss: " << test_data_loss.item<float>() << std::endl;
			torch::save(model, "model.pt");
		}
	}


	std::cout << std::endl << "Nach lernen: " << std::endl;
	for (const auto& p : model->named_parameters())
	{
		std::cout << p.key() << std::endl << p.value() << std::endl;
	}


	prediction = model->forward(test_data);
	std::cout << std::endl << std::endl << "input_data :\n" << learn_data;
	std::cout << std::endl << std::endl << "target_data normal:\n" << *target_data[0][0][0].data<float>();
	std::cout << std::endl << std::endl << "Prediction normal:\n" << prediction << std::endl;
	

}