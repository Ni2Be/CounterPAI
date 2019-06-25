#pragma once
#include <SFML/Graphics.hpp>

class Application;
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

	typedef void(*Fun)(Application*);

	class Button : public UI::Clickable, public sf::Drawable
	{
	public:
		Button(Application* app, sf::IntRect click_area, std::string text = "n");
		void on_clicked();
		Fun func;
		sf::RectangleShape draw_rect;
		sf::Text m_text;
		void draw(sf::RenderTarget& target, sf::RenderStates states) const;
	private:
		Application* m_app;
	};
}