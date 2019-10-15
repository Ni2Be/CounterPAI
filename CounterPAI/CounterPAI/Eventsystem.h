#pragma once
#include <SFML/Graphics.hpp>
#include "Clickable.h"
#include <list>

namespace UI {
	class Event_System
	{
	public:
		static Event_System& instance()
		{
			static Event_System m_instance;
			return m_instance;
		}
		void check_events(sf::RenderWindow& window, Application* app);
		void add_clickable(const Clickable& clickable);
		void remove_clickable(const Clickable& clickable);
	private:
		Event_System();
		Event_System(const Event_System&);
		Event_System &operator=(const Event_System&);

		std::list<Clickable*> m_clickables;
	};
}
