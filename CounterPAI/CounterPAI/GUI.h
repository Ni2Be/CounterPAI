#pragma once

#include <list>
#include <memory>
#include <SFML/Graphics.hpp>
#include <iostream>
#include "Sheet_Music_Editor.h"

#include "Eventsystem.h"
#include "Clickable.h"

class Application;
namespace UI {
	class Info_Box : public sf::Drawable
	{
		public:
			Info_Box(Application* app, const sf::IntRect draw_area, const std::string& info_text);

			sf::Text m_info_text;
			sf::IntRect m_draw_area;
			Application* m_app;
			void draw(sf::RenderTarget& target, sf::RenderStates states) const;

			void set_info_text(std::string text);
	};

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

		Application* m_parent;

		Button m_play_button;
		Button m_stop_button;
		Button m_clear_button;

		Button m_whole_button;
		Button m_half_button;
		Button m_quater_button;

		Button m_delete_button;
		Button m_tie_button;

		Button m_load_button;
		Button m_save_button;

		sf::Text m_cf_marker;
		Button m_soprano_cf_button;
		Button m_bass_cf_button;
		
		Button m_100_bpm_button;
		Button m_200_bpm_button;
		Button m_300_bpm_button;

		Button m_overlay_button;
		Button m_info_button;

		Info_Box m_info_text;

		void unclick_all_buttons();

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
