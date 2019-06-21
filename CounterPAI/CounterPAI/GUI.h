#pragma once

#include <list>
#include <memory>
#include <SFML/Graphics.hpp>
#include <iostream>
#include "Sheet_Music_Editor.h"

#include "Eventsystem.h"

class Application;
namespace UI {
	class GUI
	{
	public:
		GUI(int width, int height, const std::string& title, Application* parent);

		void render();
		void check_events();

		/*attaches a drawable to the gui, ownership
		of the drawable stayes by the caller*/
		template <class DRAWABLE>
		void attach_drawable(DRAWABLE& drawable);
		/*detaches a drawable from the gui*/
		template <class DRAWABLE>
		void detach_drawable(DRAWABLE& drawable);

		sf::Font& times_new_roman() { return m_times_new_roman; }

	private:
		Application* m_parent;

		Sheet_Music_Editor m_sheet_editor;

		typedef std::list<sf::Drawable*> Draw_Container;
		Draw_Container m_drawables;
		sf::RenderWindow m_window;

		//Resources
		void load_resources();
		sf::Font m_times_new_roman;
	};
}

template <class DRAWABLE>
void UI::GUI::attach_drawable(DRAWABLE& drawable)
{
	m_drawables.push_back(&drawable);
}

template <class DRAWABLE>
void UI::GUI::detach_drawable(DRAWABLE& drawable)
{
	auto new_end = std::remove_if(std::begin(m_drawables), std::end(m_drawables),
		[&drawable](const sf::Drawable* e)
	{
		return e == &drawable;
	});
	m_drawables.erase(new_end, m_drawables.end());
}
