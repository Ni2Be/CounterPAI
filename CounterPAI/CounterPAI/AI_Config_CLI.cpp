#pragma once

#include "AI_Config_CLI.h"

#include <filesystem>
#include <iostream>


#include "Folder_Dialog.h"

char get_user_input()
{
	char ch;
	ch = std::cin.peek();
	std::cin.clear();
	std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	return ch;
}

void clear_cin()
{
	std::cin.clear();
	std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

void Eval::AI_Config_CLI::run_dialog()
{
	std::cout << "\n-------AI Config Dialog-------"
		<< "\noptions:"
		<< "\n\"r\": [r]un tests"
		<< "\n\"g\": [g]enerate sheets"
		<< "\n\"a\": [a]nalyse sheets"
		<< "\n\"v\": [v]alidate net"
		<< "\n\"e\": [e]xit ai config dialog"
		<< "\n";

	char ch = get_user_input();

	switch (ch)
	{
	case 'r': run_tests(); run_dialog(); return;
	case 'g': generate_trainings_sheets_dialog(); run_dialog(); return;
	case 'a': analyse_sheets_dialog(); run_dialog(); return;
	case 'v': validate_net_dialog(); run_dialog(); return;
	case 'e': return;
	default: run_dialog(); return;
	}
}


void Eval::AI_Config_CLI::run_tests()
{
	std::cout << "\n-------Run Tests-------\n";
	for (auto& entry : std::experimental::filesystem::directory_iterator("data/trainings_settings"))
	{
		if (!is_directory(entry.path()))
		{

			test_runner.run_test(entry.path().string());

			//ensure libtorch python parts have cleaned up their memory mess
			std::this_thread::sleep_for(std::chrono::seconds(2));
		}
	}
}

void Eval::AI_Config_CLI::generate_trainings_sheets_dialog()
{
	std::cout << "\n-------Generate Trainings Sheets-------"
		<< "\noptions:"
		<< "\n\"a\": [a]lter sheets count"
		<< "\n\"s\": [s]witch generators"
		<< "\n\"c\": [c]hange folder names"
		<< "\n\"g\": [g]enerate sheets"
		<< "\n\"o\": [o]ne rule subset"
		<< "\n\"e\": [e]xit generate trainings sheets dialog"
		<< "\n";

	char ch = get_user_input();

	int rule_num = 0;
	int count = 0;

	switch (ch)
	{
	case 'a':
		std::cout << "\nSheet counts:"
			<< "\ntrain_sheets_count: " << train_sheets_count
			<< "\ntest_sheets_count: " << test_sheets_count
			<< "\nvalid_sheets_count: " << valid_sheets_count
			<< "\n"
			<< "\nnew train_sheets_count: ";
		std::cin >> train_sheets_count;
		std::cout << "\nnew test_sheets_count: ";
		std::cin >> test_sheets_count;
		std::cout << "\nnew test_sheets_count: ";
		std::cin >> valid_sheets_count;
		clear_cin();

		std::cout << "\nSheet counts:"
			<< "\ntrain_sheets_count: " << train_sheets_count
			<< "\ntest_sheets_count: " << test_sheets_count
			<< "\nvalid_sheets_count: " << valid_sheets_count
			<< "\n";
		generate_trainings_sheets_dialog(); return;	
	case 's':
			std::cout << "\nGenerator:"
				<< "\ntrain: " << train_settings
				<< "\ntest: " << test_settings
				<< "\nvalid: " << valid_settings
				<< "\n\nSettings:"
				<< "\n[0]: " << Eval::Trainings_Data_Gen::Settings::Stochastic
				<< "\n[1]: " << Eval::Trainings_Data_Gen::Settings::Post_Sep
				<< "\n[2]: " << Eval::Trainings_Data_Gen::Settings::Pre_Sep
				<< "\nnew train generator: ";
			int temp;
			std::cin >> temp;
			if(temp == 0) train_settings = Eval::Trainings_Data_Gen::Settings::Stochastic;
			else if (temp == 1) train_settings = Eval::Trainings_Data_Gen::Settings::Post_Sep;
			else if (temp == 2) train_settings = Eval::Trainings_Data_Gen::Settings::Pre_Sep;
			std::cout << "\nnew test generator: ";
			std::cin >> temp;
			if (temp == 0) test_settings = Eval::Trainings_Data_Gen::Settings::Stochastic;
			else if (temp == 1) test_settings = Eval::Trainings_Data_Gen::Settings::Post_Sep;
			else if (temp == 2) test_settings = Eval::Trainings_Data_Gen::Settings::Pre_Sep;
			std::cout << "\nnew valid generator: ";
			std::cin >> temp;
			if (temp == 0) valid_settings = Eval::Trainings_Data_Gen::Settings::Stochastic;
			else if (temp == 1) valid_settings = Eval::Trainings_Data_Gen::Settings::Post_Sep;
			else if (temp == 2) valid_settings = Eval::Trainings_Data_Gen::Settings::Pre_Sep;
			clear_cin();

			std::cout << "\nGenerator:"
				<< "\ntrain: " << train_settings
				<< "\ntest: " << test_settings
				<< "\nvalid: " << valid_settings
				<< "\n";
			generate_trainings_sheets_dialog(); return;
	case 'c': 
		std::cout << "\nFolders:"
			<< "\ntrain_data_folder: " << train_data_folder
			<< "\ntest_data_folder: " << test_data_folder
			<< "\nvalid_data_folder: " << valid_data_folder
			<< "\n"
			<< "\nnew train_data_folder: ";
		std::cin >> train_data_folder;
		std::cout << "\n new test_data_folder: ";
		std::cin >> test_data_folder;
		std::cout << "\n new valid_data_folder: ";
		std::cin >> valid_data_folder;
		clear_cin();

		std::cout << "\nFolders:"
			<< "\ntrain_data_folder: " << train_data_folder
			<< "\ntest_data_folder: " << test_data_folder
			<< "\nvalid_data_folder: " << valid_data_folder
			<< "\n";
		 generate_trainings_sheets_dialog(); return;
	case 'g': generate_trainings_sheets(); return;
	case 'o':
		std::cout << "\nwhich rule (1-9): ";
		std::cin >> rule_num;
		clear_cin();
		std::cout << "\nhow many: ";
		std::cin >> count;
		clear_cin();
		generator.select_sheets_subset(train_data_folder, selected_data_folder, count, Eval::get_main_rule_from_index(rule_num)); return;
	case 'e': return;
	default: generate_trainings_sheets_dialog(); return;
	}
}

void Eval::AI_Config_CLI::generate_trainings_sheets()
{
	for (auto& entry : std::experimental::filesystem::directory_iterator(train_data_folder))
		std::experimental::filesystem::remove(entry.path());
	if(train_settings == Eval::Trainings_Data_Gen::Settings::Post_Sep 
		|| train_settings == Eval::Trainings_Data_Gen::Settings::Stochastic)
		generator.generate_data(train_sheets_count, train_data_folder, train_settings);
	else if(train_settings == Eval::Trainings_Data_Gen::Settings::Pre_Sep)
		generator.generate_data(train_sheets_count, train_data_folder, train_settings, "data/sheets/train"); 


	if (test_settings == Eval::Trainings_Data_Gen::Settings::Post_Sep
		|| test_settings == Eval::Trainings_Data_Gen::Settings::Stochastic)
		generator.generate_data(test_sheets_count, test_data_folder, test_settings);
	else if (test_settings == Eval::Trainings_Data_Gen::Settings::Pre_Sep)
		generator.generate_data(test_sheets_count, test_data_folder, test_settings, "data/sheets/test_valid");


	if (valid_settings == Eval::Trainings_Data_Gen::Settings::Post_Sep
		|| valid_settings == Eval::Trainings_Data_Gen::Settings::Stochastic)
		generator.generate_data(valid_sheets_count, valid_data_folder, valid_settings);
	else if (valid_settings == Eval::Trainings_Data_Gen::Settings::Pre_Sep)
		generator.generate_data(valid_sheets_count, valid_data_folder, valid_settings, "data/sheets/test_valid");
}



void Eval::AI_Config_CLI::validate_net_dialog()
{
	std::cout << "\n-------Validate Net-------"
		<< "\noptions:"
		<< "\n\"v\": [v]alidate on net"
		<< "\n\"b\": [b]atch validation"
		<< "\n\"e\": [e]xit validation dialog"
		<< "\n";

	char ch = get_user_input();

	switch (ch)
	{
	case 'v':
		validate_one_net();
		validate_net_dialog(); return;
	case 'b':
		batch_validation();
		validate_net_dialog(); return;
	case 'e': return;
	default: generate_trainings_sheets_dialog(); return;
	}
}

void Eval::AI_Config_CLI::batch_validation()
{
	std::vector<std::string> net_paths;
	std::function<void(std::string)> get_nets;

	std::vector<std::string> folder_paths;
	bool validate_folders = false;

	get_nets =
		[&get_nets, &net_paths, &folder_paths, &validate_folders](std::string path)
	{
		std::cout << "\nNets:\n";
		net_paths.clear();
		folder_paths.clear();

		for (auto& entry : std::experimental::filesystem::directory_iterator(path))
		{
			if (is_directory(entry.path()))
				folder_paths.push_back(entry.path().string());
			else if (*(entry.path().string().end() - 4) != 'i')//don't list the optimizers
				net_paths.push_back(entry.path().string());
		}
		for (int i = 0; i < net_paths.size(); i++)
			std::cout << "\n[" << i << "]: " << net_paths[i];
		if(net_paths.size() == 0)
			for (int i = 0; i < folder_paths.size(); i++)
				std::cout << "\n[" << i << "]: " << folder_paths[i];

		std::cout << "\nValidate all Nets?"
			<< "\noptions:"
			<< "\n\"y\": [y]es"
			<< "\n\"c\": [c]ontinue search"
			<< "\n\"e\": [e]xit"
			<< "\n";

		char ch = get_user_input();
		int folder_number = 0;
		switch (ch)
		{
		case 'y':
			if (net_paths.size() == 0)
				validate_folders = true;
			return;
		case 'c':
			for (int i = 0; i < folder_paths.size(); i++)
				std::cout << "\n[" << i << "]: " << folder_paths[i];
			std::cout << "\nenter folder number: ";
			std::cin >> folder_number;
			clear_cin();

			return get_nets(folder_paths[folder_number]);
		case 'e': return;
		default: return;
		}
	};

	get_nets("data/trainings_results/models");
	std::cout << "\nValidating:\n";
	if (validate_folders)
	{
		for (int i = 0; i < folder_paths.size(); i++)
			std::cout << "\n[" << i << "]: " << folder_paths[i];

		std::vector<std::string> settings_paths;		
		for (auto& entry : std::experimental::filesystem::directory_iterator("data/trainings_settings"))
		{
			if(!is_directory(entry.path()))
				settings_paths.push_back(entry.path().string());
		}
		std::cout << "\nWith Settings:\n";
		for (int i = 0; i < settings_paths.size(); i++)
			std::cout << "\n[" << i << "]: " << settings_paths[i];

		while (true)
		{
			char ch;
			std::cout << "\ncontinue?: y/n";
			std::cin >> ch;
			clear_cin();
			if (ch == 'n')
				return;
			else if (ch == 'y')
				break;
		}
		for (int i = 0; i < folder_paths.size(); i++)
		{
			net_paths.clear();
			for (auto& entry : std::experimental::filesystem::directory_iterator(folder_paths[i]))
			{
				if (!is_directory(entry.path()) && (*(entry.path().string().end() - 4) != 'i'))//don't list the optimizers
					net_paths.push_back(entry.path().string());
			}
			for (int n = 0; n < net_paths.size(); n++)
			{
				ai_evaluator.validate_net(net_paths[n], settings_paths[i], settings_paths[i]);
			}
		}
	}
	else
	{
		for (int i = 0; i < net_paths.size(); i++)
			std::cout << "\n[" << i << "]: " << net_paths[i];

		std::vector<std::string> settings_paths;
		std::function<int(std::string)> get_settings;
		get_settings =
			[&get_settings, &settings_paths](std::string path) -> int
		{
			std::cout << "\nSettings:\n";
			settings_paths.clear();

			for (auto& entry : std::experimental::filesystem::directory_iterator(path))
				settings_paths.push_back(entry.path().string());

			for (int i = 0; i < settings_paths.size(); i++)
			{
				std::cout << "\n[" << i << "]: " << settings_paths[i];
			}
			std::cout << "\nSettings number: ";
			int settings_number = 0;
			std::cin >> settings_number;
			clear_cin();

			int counter = 0;
			for (auto& entry : std::experimental::filesystem::directory_iterator(path))
			{
				if (counter == settings_number && is_directory(entry.path()))
					return get_settings(entry.path().string());
				counter++;
			}
			return settings_number;
		};

		int settings_number = get_settings("data/trainings_settings");

		std::cout << "\nValidation of:";
		for (int i = 0; i < net_paths.size(); i++)
			std::cout << "\n[" << i << "]: " << net_paths[i];
		std::cout << "\nwith settings " << settings_paths[settings_number] << "\n";


		for (int i = 0; i < net_paths.size(); i++)
		{
			ai_evaluator.validate_net(net_paths[i], settings_paths[settings_number], settings_paths[settings_number]);
		}
	}
}



void Eval::AI_Config_CLI::validate_one_net()
{
	std::vector<std::string> net_paths;
	std::function<int(std::string)> get_net;
	get_net =
		[&get_net, &net_paths](std::string path) -> int
	{
		std::cout << "\nNets:\n";
		net_paths.clear();

		for (auto& entry : std::experimental::filesystem::directory_iterator(path))
		{
			if(*(entry.path().string().end() - 4) != 'i')//don't list the optimizers
				net_paths.push_back(entry.path().string());
		}
		for (int i = 0; i < net_paths.size(); i++)
		{
			std::cout << "\n[" << i << "]: " << net_paths[i];
		}
		std::cout << "\nNet number: ";
		int net_number = 0;
		std::cin >> net_number;
		clear_cin();

		int counter = 0;
		for (auto& entry : std::experimental::filesystem::directory_iterator(path))
		{
			if (counter == net_number && is_directory(entry.path()))
				return get_net(entry.path().string());
			counter++;
		}
		return net_number;
	};

	int net_number = get_net("data/trainings_results/models");

	std::cout << "\nNet " << net_paths[net_number] << "\n";

	std::vector<std::string> settings_paths;
	std::function<int(std::string)> get_settings;
	get_settings =
		[&get_settings, &settings_paths](std::string path) -> int
	{
		std::cout << "\nSettings:\n";
		settings_paths.clear();

		for (auto& entry : std::experimental::filesystem::directory_iterator(path))
			settings_paths.push_back(entry.path().string());

		for (int i = 0; i < settings_paths.size(); i++)
		{
			std::cout << "\n[" << i << "]: " << settings_paths[i];
		}
		std::cout << "\nSettings number: ";
		int settings_number = 0;
		std::cin >> settings_number;
		clear_cin();

		int counter = 0;
		for (auto& entry : std::experimental::filesystem::directory_iterator(path))
		{
			if (counter == settings_number && is_directory(entry.path()))
				return get_settings(entry.path().string());
			counter++;
		}
		return settings_number;
	};

	int settings_number = get_settings("data/trainings_settings");

	std::cout << "\nValidation of:";
	std::cout << "\nNet " << net_paths[net_number];
	std::cout << "\nwith settings " << settings_paths[settings_number] << "\n";

	ai_evaluator.validate_net(net_paths[net_number], settings_paths[settings_number], settings_paths[settings_number]);
}

void Eval::AI_Config_CLI::analyse_sheets_dialog()
{

	std::cout << "\n-------Analyse Sheets-------"
		<< "\noptions:"
		<< "\n\"a\": [a]nalyse"
		<< "\n\"c\": [c]hange folder names"
		<< "\n\"e\": [e]xit analyse sheets dialog"
		<< "\n";

	char ch = get_user_input();

	switch (ch)
	{
	case 'a':
		analyse_sheets(); return;
	case 'c':
		std::cout << "\nFolders:"
			<< "\ntrain_data_folder: " << train_data_folder
			<< "\ntest_data_folder: " << test_data_folder
			<< "\nvalid_data_folder: " << valid_data_folder
			<< "\n"
			<< "\nnew train_data_folder: ";
		std::cin >> train_data_folder;
		std::cout << "\nnew test_data_folder: ";
		std::cin >> test_data_folder;
		std::cout << "\nnew valid_data_folder: ";
		std::cin >> valid_data_folder;
		clear_cin();

		std::cout << "\nFolders:"
			<< "\ntrain_data_folder: " << train_data_folder
			<< "\ntest_data_folder: " << test_data_folder
			<< "\nvalid_data_folder: " << valid_data_folder
			<< "\n";
		analyse_sheets_dialog(); return;
	case 'e': return;
	default: analyse_sheets_dialog(); return;
	}
}

void Eval::AI_Config_CLI::analyse_sheets()
{
	std::cout << "\nTrainings Sheets:";
	generator.analyse_sheets(train_data_folder);
	std::cout << "\nTest Sheets:";
	generator.analyse_sheets(test_data_folder);
	std::cout << "\nValid Sheets:";
	generator.analyse_sheets(valid_data_folder);
}
