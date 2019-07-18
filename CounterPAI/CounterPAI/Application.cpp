#include "Application.h"
#include <iostream>
#include <sstream>
#include <fstream>

#include "Windows_Folder_Dialog.h"

#include <filesystem>

bool Application::m_is_running = true;

Application::Application()
	:
	gui(m_WINDOW_WIDTH, m_WINDOW_HEIGHT, m_WINDOW_TITLE, this)
{
	//TESTING
	//std::cout << "\nGENERATE DATA!";
	//std::cout.setstate(std::ios_base::failbit);

	//for (auto& entry : std::experimental::filesystem::directory_iterator("data/trainings_data/train"))
	//	std::experimental::filesystem::remove(entry.path());
	//generator.generate_data(1000, "data/trainings_data/train");

	//for (auto& entry : std::experimental::filesystem::directory_iterator("data/trainings_data/test"))
	//	std::experimental::filesystem::remove(entry.path());
	//generator.generate_data(100, "data/trainings_data/test");

	//for (auto& entry : std::experimental::filesystem::directory_iterator("data/trainings_data/valid"))
	//	std::experimental::filesystem::remove(entry.path());
	//generator.generate_data(2, "data/trainings_data/valid");

	//std::cout.clear();


	
	//Midi_Reader midi_reader("data/piano.mid", 0);
	//m_sheet = midi_reader.sheet;
	//std::ifstream ifs;
	//ifs.open("data/trainings_data/sheet_0.sheet");
	//ifs >> m_sheet;


	//lstm_evaluator.train_remember_one_cf();
	
	//test_runner.run_test(Windows_File_Loader::get_exe_path() + "/data/trainings_settings/test_1.json");
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