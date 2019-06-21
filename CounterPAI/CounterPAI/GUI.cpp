#include "GUI.h"
#include <iostream>
#include "Application.h"

UI::GUI::GUI(int width, int height, const std::string& title, Application* parent)
	:
	m_window(sf::VideoMode(width, height), title),
	m_parent(parent),
	m_sheet_editor(parent->m_sheet)
{
	load_resources();
	m_window.setActive(false);
}


void UI::GUI::render()
{
	sf::sleep(sf::milliseconds(33));
	m_window.clear(sf::Color::White);
	for (auto& drawable : m_drawables)
	{
		m_window.draw(*drawable);
	}
	m_window.draw(m_sheet_editor);

	m_window.display();
}

void UI::GUI::check_events()
{
	Event_System::instance().check_events(m_window);
}


void UI::GUI::load_resources()
{
	m_times_new_roman.loadFromFile("data/times-new-roman.ttf");
}

