#include "Application.h"
#include <iostream>
#include <sstream>
#include <fstream>


#include <filesystem>

bool Application::m_is_running = true;

Application::Application()
	:
	gui(m_WINDOW_WIDTH, m_WINDOW_HEIGHT, m_WINDOW_TITLE, this)
{
	//TESTING
	//std::cout.setstate(std::ios_base::failbit);

	//for (auto& entry : std::experimental::filesystem::directory_iterator("data/trainings_data/train"))
	//	std::experimental::filesystem::remove(entry.path());
	//generator.generate_data(5, "data/trainings_data/train");

	//for (auto& entry : std::experimental::filesystem::directory_iterator("data/trainings_data/test"))
	//	std::experimental::filesystem::remove(entry.path());
	//generator.generate_data(2, "data/trainings_data/test");

	//for (auto& entry : std::experimental::filesystem::directory_iterator("data/trainings_data/valid"))
	//	std::experimental::filesystem::remove(entry.path());
	//generator.generate_data(2, "data/trainings_data/valid");

	//std::cout.clear();


	
	//Midi_Reader midi_reader("data/piano.mid", 0);
	//m_sheet = midi_reader.sheet;
	//std::ifstream ifs;
	//ifs.open("data/trainings_data/sheet_0.sheet");
	//ifs >> m_sheet;


	lstm_evaluator.train();
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