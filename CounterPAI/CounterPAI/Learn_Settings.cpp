#pragma once
#include "Learn_Settings.h"

#include <rapidjson/document.h>
#include <rapidjson/istreamwrapper.h>
#include <iomanip>

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

	if (parameter.HasMember("valid_data_folder"))
		valid_data_folder = parameter["valid_data_folder"].GetString();
	else
		valid_data_folder = "null";

	data_converter = parameter["data_converter"].GetString();
	if (parameter.HasMember("data_converter_info"))
		data_converter_info = parameter["data_converter_info"].GetString();
	else
		data_converter_info = "no info";
	if (parameter.HasMember("is_bidirectional"))
		is_bidirectional = parameter["is_bidirectional"].GetBool();
	else
		is_bidirectional = false;

	if (parameter.HasMember("sequence_len"))
		sequence_len = parameter["sequence_len"].GetInt();
	else
		sequence_len = 8;
}

std::ostream& Eval::operator<<(std::ostream& os, Learn_Settings& test)
{
	os << "\n"<<  std::setw(21) << "Test name: " << test.test_name
		<< "\n"<< std::setw(21) << "NN_Type: " << test.nn_type
		<< "\n" << std::setw(21) << "in_size: " << test.in_size << ", hidden_size: " << test.hidden_size << ", out_size: " << test.out_size
		<< "\n" << std::setw(21) << "sequence_len: " << test.sequence_len
		<< "\n"<< std::setw(21) << "hidden_layer_count: " << test.hidden_layer_count << ", batch_size: " << test.batch_size << ", epochs: " << test.epochs << ", dropout: " << test.dropout
		<< "\n"<< std::setw(21) << "optimizer: " << test.optimizer << ", loss_func: " << test.loss_func
		<< "\n"<< std::setw(21) << "learning_rate: " << test.learning_rate
		<< "\n"<< std::setw(21) << "is_bidirectional: " << (test.is_bidirectional? "true" : "false")
		<< "\n"<< std::setw(21) << "train_data_folder: " << test.train_data_folder
		<< "\n"<< std::setw(21) << "test_data_folder: " << test.test_data_folder
		<< "\n"<< std::setw(21) << "valid_data_folder: " << test.valid_data_folder
		<< "\n"<< std::setw(21) << "data_converter: " << test.data_converter
		<< "\n"<< std::setw(21) << "data_converter_info: " << test.data_converter_info
		<< "\n";
	return os;
}