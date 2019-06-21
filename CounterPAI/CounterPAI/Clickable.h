#pragma once
#include <SFML/Graphics.hpp>

namespace UI {
	class Clickable
	{
		friend class Event_System;
	public:
		//TODO should copy properly
		Clickable();
		~Clickable();
		sf::Rect<int> m_click_area;
	protected:
		virtual void on_clicked() = 0;
	private:
		void check_if_clicked(sf::Vector2i click_pos);
	};
}