#pragma once
#include <iostream>
#include <string>
#include "GUI.h"
#include "Piano_Player.h"
#include "Midi_Reader.h"

class Application
{
public:
	Application();

	int run();
	void shut_down();
	Sheet_Music m_sheet;
	Piano_Player player;

	static bool m_is_running;

	const int m_WINDOW_WIDTH = 1400;
	const int m_WINDOW_HEIGHT = 600;
	const std::string m_WINDOW_TITLE = "CounterPai";
	UI::GUI gui;


};
