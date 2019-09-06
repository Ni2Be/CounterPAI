#include "Application.h"
#include <iostream>
#include <sstream>
#include <fstream>

#include "Folder_Dialog.h"

#include <filesystem>



bool Application::m_is_running = true;

Application::Application()
	:
	gui(m_WINDOW_WIDTH, m_WINDOW_HEIGHT, m_WINDOW_TITLE, this)
{
	m_evaluator.reset(new Eval::Rule_Eval());
	//m_evaluator.reset(new Eval::AI_Evaluator());


	//TESTING
	//std::cout << "\nGENERATE DATA!";
	//std::cout.setstate(std::ios_base::failbit);

	for (auto& entry : std::experimental::filesystem::directory_iterator("data/trainings_data/train"))
		std::experimental::filesystem::remove(entry.path());
	generator.generate_data(10, "data/trainings_data/train");

	for (auto& entry : std::experimental::filesystem::directory_iterator("data/trainings_data/test"))
		std::experimental::filesystem::remove(entry.path());
	generator.generate_data(10, "data/trainings_data/test");

	for (auto& entry : std::experimental::filesystem::directory_iterator("data/trainings_data/valid"))
		std::experimental::filesystem::remove(entry.path());
	generator.generate_data(2, "data/trainings_data/valid");

	//std::cout.clear();


	////////////////////// update .sheet data
	//////////////////////for (auto& entry : std::experimental::filesystem::directory_iterator("data/sheets"))
	//////////////////////{
	//////////////////////	std::ifstream fs;
	//////////////////////	fs.open(entry.path().c_str());
	//////////////////////	fs >> m_sheet;
	//////////////////////	fs.close();

	//////////////////////	m_sheet.clear_note_infos();
	//////////////////////	m_evaluator->evaluate_notes(m_sheet);

	//////////////////////	std::ofstream ofs;
	//////////////////////	ofs.open(entry.path().c_str());
	//////////////////////	if (!ofs)
	//////////////////////		std::cerr << "could not open file\n";

	//////////////////////	ofs << m_sheet;
	//////////////////////}

	
	//Midi_Reader midi_reader("data/piano.mid", 0);
	//m_sheet = midi_reader.sheet;
	//std::ifstream ifs;
	//ifs.open("data/trainings_data/sheet_0.sheet");
	//ifs >> m_sheet;


	//lstm_evaluator.train_remember_one_cf();

	//test_runner.run_test(Windows_File_Loader::get_exe_path() + "/data/trainings_settings/test_1.json");
	//test_runner.run_test(Windows_File_Loader::get_exe_path() + "/data/trainings_settings/evaluate_fux_rules_1.json");

	for (auto& entry : std::experimental::filesystem::directory_iterator("data/trainings_settings"))
		if (!is_directory(entry.path()))
			test_runner.run_test(entry.path().string());


	//Eval::Learn_Settings settings("data/trainings_settings/evaluate_fux_rules_from_two_sides_6.json");
	//m_ai_evaluator.test_net(settings);

	//m_sheet = lstm_evaluator.testing();
}

int Application::run()
{
	while (m_is_running)
	{
		player.update();
		gui.check_events();
		gui.render();
	}
	return 0;
}

void Application::shut_down()
{
	m_is_running = false;
}