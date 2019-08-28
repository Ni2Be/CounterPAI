#include "Learn_Settings.h"

#include <rapidjson/document.h>
#include <rapidjson/istreamwrapper.h>

Eval::Learn_Settings::Learn_Settings(const std::string& parameters_file)
{
	read_parameters(parameters_file);
}

void Eval::Learn_Settings::read_parameters(const std::string& parameters_file_name)
{
	rapidjson::Document parameter;
	std::ifstream ifs(parameters_file_name);
	if (!ifs)
	{
		std::cerr << "\ncould not open: " + parameters_file_name + "\n";
		return;
	}
	rapidjson::IStreamWrapper isw(ifs);
	parameter.ParseStream(isw);

	test_name = parameter["name"].GetString();

	//backward compatibility
	if (parameter.HasMember("nn_type"))
		nn_type = NN_type_from_string(parameter["nn_type"].GetString());
	else
		nn_type = NN_Type::LSTM;

	in_size = parameter["in_size"].GetInt();
	hidden_size = parameter["hidden_size"].GetInt();
	out_size = parameter["out_size"].GetInt();

	hidden_layer_count = parameter["hidden_layer_count"].GetInt();
	batch_size = parameter["batch_size"].GetInt();
	epochs = parameter["epochs"].GetInt();
	dropout = parameter["dropout"].GetDouble();
	optimizer = opti_from_string(parameter["optimizer"].GetString());
	loss_func = loss_f_from_string(parameter["loss_func"].GetString());
	learning_rate = parameter["learning_rate"].GetFloat();
	train_data_folder = parameter["train_data_folder"].GetString();
	test_data_folder = parameter["test_data_folder"].GetString();
	data_converter = parameter["data_converter"].GetString();
}

std::ostream& Eval::operator<<(std::ostream& os, Learn_Settings& test)
{
	os << "\nTest name: " << test.test_name
		<< "\nNN_Type: " << test.nn_type 
		<< "\nin_size: " << test.in_size << ", hidden_size: " << test.hidden_size << ", out_size: " << test.out_size
		<< "\nhidden_layer_count: " << test.hidden_layer_count << ", batch_size: " << test.batch_size << ", epochs: " << test.epochs << ", dropout: " << test.dropout
		<< "\noptimizer: " << test.optimizer << ", loss_func: " << test.loss_func
		<< "\nlearning_rate: " << test.learning_rate
		<< "\ntrain_data_folder: " << test.train_data_folder
		<< "\ntest_data_folder: " << test.test_data_folder
		<< "\ndata_converter: " << test.data_converter
		<< "\n";
	return os;
}