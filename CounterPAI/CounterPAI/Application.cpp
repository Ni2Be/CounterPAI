#pragma once
#include "Application.h"
#include <iostream>
#include <sstream>
#include <fstream>

#include "Folder_Dialog.h"


bool Application::m_is_running = true;

Application::Application()
	:
	gui(m_WINDOW_WIDTH, m_WINDOW_HEIGHT, m_WINDOW_TITLE, this)
{
	m_evaluator.reset(new Eval::Rule_Evaluator());

#ifdef AI_EVALUATOR
	m_evaluator.reset(new Eval::AI_Evaluator());
#endif


	//!!!!!!!!!update .sheet-data!!!!!!!!!
	//for (auto& entry : std::experimental::filesystem::directory_iterator("data/sheets"))
	//{

	//	if (!is_directory(entry.path()))
	//	{
	//		std::ifstream fs;
	//		fs.open(entry.path().c_str());
	//		fs >> m_sheet;
	//		fs.close();

	//		m_sheet.clear_note_infos();
	//		m_evaluator->evaluate_notes(m_sheet);

	//		std::ofstream ofs;
	//		ofs.open(entry.path().c_str());
	//		if (!ofs)
	//			std::cerr << "could not open file\n";

	//		ofs << m_sheet;
	//	}
	//}

	//TESTING
	//std::cout << "\nGENERATE DATA!";




	//std::cout.setstate(std::ios_base::failbit);
	//std::cout.clear();

	//std::cout.clear();
	
	//Midi_Reader midi_reader("data/piano.mid", 0);
	//m_sheet = midi_reader.sheet;
	//std::ifstream ifs;
	//ifs.open("data/trainings_data/sheet_0.sheet");
	//ifs >> m_sheet;



}

int Application::run()
{

#ifdef AI_EVALUATOR
	ai_config_cli.run_dialog();
#endif
	std::cout << "\nCounterPai\n";
	gui.m_window.setFramerateLimit(30);
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