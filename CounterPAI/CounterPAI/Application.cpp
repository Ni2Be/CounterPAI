#include "Application.h"
#include <iostream>


bool Application::m_is_running;

Application::Application()
	:
	gui(m_WINDOW_WIDTH, m_WINDOW_HEIGHT, m_WINDOW_TITLE, this)
{
	m_is_running = true;

	//TESTING
	Midi_Reader midi_reader("data/piano.mid", 0);
	m_sheet = midi_reader.sheet;
	player.play_sheet_music(m_sheet);
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