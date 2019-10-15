#pragma once
#include "Application.h"
#include <iostream>
#include <sstream>
#include <fstream>

#include "Folder_Dialog.h"
#include "Defines.h"

bool Application::m_is_running = true;

Application::Application(bool start_ai_cli)
	:
	m_start_ai_cli(start_ai_cli),
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

void Application::log_undo_sheet()
{
	m_undo_sheets.push_back(m_sheet);

	if (m_undo_sheets.size() > 500)
		m_undo_sheets.pop_front();
}

void Application::undo()
{
	if (m_undo_sheets.size() > 0)
	{
		m_redo_sheets.push_back(m_sheet);
		m_sheet = m_undo_sheets.back();
		m_undo_sheets.pop_back();
	}
}

void Application::redo()
{
	if (m_redo_sheets.size() > 0)
	{
		m_undo_sheets.push_back(m_sheet);
		m_sheet = m_redo_sheets.back();
		m_redo_sheets.pop_back();
	}
}

int Application::run()
{
	if(m_start_ai_cli)
		ai_config_cli.run_dialog();

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