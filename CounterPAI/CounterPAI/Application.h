#pragma once
#include <iostream>
#include <string>
#include "GUI.h"
#include "Piano_Player.h"
#include "Midi_Reader.h"
#include "Debug_Log.h"
#include "Rule_Eval.h"
#include "Trainings_Data_Gen.h"
#include "Test_Runner.h"

#include "AI_Evaluator.h"

class Application
{
public:
	Application();

	int run();
	void shut_down();
	Sheet_Music m_sheet;
	Piano_Player player;
	Keyboard m_feedback_piano;


	Eval::Trainings_Data_Gen generator;

	Eval::Test_Runner test_runner;

	static bool m_is_running;

	const int m_WINDOW_WIDTH = 1900;
	const int m_WINDOW_HEIGHT = 600;
	const std::string m_WINDOW_TITLE = "CounterPai";
	UI::GUI gui;

	std::shared_ptr<Eval::Evaluator_Base> m_evaluator;
	//Eval::Rule_Eval m_evaluator;
	Eval::AI_Evaluator m_ai_evaluator;

	Debug_Log m_debug_log;
};
