#pragma once
#include <iostream>
#include <string>
#include <list>

#include "GUI.h"
#include "Piano_Player.h"
#include "Midi_Reader.h"
#include "Debug_Log.h"
#include "Rule_Evaluator.h"
#include "Trainings_Data_Gen.h"
#include "Test_Runner.h"

#include "AI_Evaluator.h"
#include "AI_Config_CLI.h"

class Application
{
public:
	Application(bool start_ai_cli = false);

	int run();
	void shut_down();

	bool m_start_ai_cli;

	//Application
	static bool m_is_running;

	const int m_WINDOW_WIDTH = 1900;
	const int m_WINDOW_HEIGHT = 600;
	const std::string m_WINDOW_TITLE = "CounterPai";

	Debug_Log m_debug_log;

	//CounterPai
	Sheet_Music m_sheet;
	Piano_Player player;
	Keyboard m_feedback_piano;

	std::list<Sheet_Music> m_undo_sheets;
	std::list<Sheet_Music> m_redo_sheets;
	void log_undo_sheet();
	void undo();
	void redo();

	std::shared_ptr<Eval::Evaluator_Base> m_evaluator;
	//UI
	Eval::AI_Config_CLI ai_config_cli;
	UI::GUI gui;
};
