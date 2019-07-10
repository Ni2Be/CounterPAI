#pragma once
#include <iostream>
#include <string>
#include "GUI.h"
#include "Piano_Player.h"
#include "Midi_Reader.h"
#include "Debug_Log.h"
#include "Rule_Eval.h"
#include "Trainings_Data_Gen.h"

class Application
{
public:
	Application();

	int run();
	void shut_down();
	Sheet_Music m_sheet;
	Piano_Player player;
	Keyboard m_feedback_piano;
	Trainings_Data_Gen generator;

	static bool m_is_running;

	const int m_WINDOW_WIDTH = 1900;
	const int m_WINDOW_HEIGHT = 600;
	const std::string m_WINDOW_TITLE = "CounterPai";
	UI::GUI gui;

	Eval::Rule_Eval m_evaluator;

	Debug_Log m_debug_log;
};
