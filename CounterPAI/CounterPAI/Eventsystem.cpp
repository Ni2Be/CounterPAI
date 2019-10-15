#pragma once
#include "Eventsystem.h"
#include <iostream>
#include "Application.h"

UI::Event_System::Event_System()
{}

void UI::Event_System::add_clickable(const Clickable& clickable)
{
	m_clickables.push_back(const_cast<Clickable*>(&clickable));
}

void UI::Event_System::remove_clickable(const Clickable& clickable)
{
	auto new_end = std::remove_if(std::begin(m_clickables), std::end(m_clickables),
		[&clickable](const Clickable* e)
	{
		return e == &clickable;
	});
	m_clickables.erase(new_end, m_clickables.end());
}

void UI::Event_System::check_events(sf::RenderWindow& window, Application* app)
{
	sf::Event event;
	while (window.pollEvent(event))
	{
		if (event.type == sf::Event::Closed)
		{
			Application::m_is_running = false;
			window.close();
		}
		if (event.type == sf::Event::MouseButtonPressed)
		{
			for (auto& clickable : m_clickables)
				clickable->check_if_clicked(sf::Mouse::getPosition(window));
		}
		if (event.type == sf::Event::KeyPressed)
		{
			if (event.key.control && event.key.code == sf::Keyboard::Z)
				app->undo();
			if (event.key.control && event.key.code == sf::Keyboard::Y)
				app->redo();
		}
	}
}

