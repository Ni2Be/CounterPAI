#pragma once
#include "Sheet_Music.h"
#include "SFML/Graphics.hpp"

#include <map>
#include <list>
#include <memory>

#include "Clickable.h"

namespace UI {
	class Texture_Catalog
	{
	public:
		static Texture_Catalog& instance()
		{
			static Texture_Catalog m_instance;
			return m_instance;
		}
		void load_resources();
		std::map<std::string, sf::Texture> m_texture_catalog;
	private:
		Texture_Catalog();
		Texture_Catalog(const Texture_Catalog&);
		Texture_Catalog &operator=(const Texture_Catalog&);
	};

	class Note
	{
	public:
		Note(Music_Note& note);
		Music_Note& m_note;

		float get_offset() const;
		sf::Sprite get_sprite(sf::Vector2f pos) const;
	};

	class Sheet_Grid_Button : public UI::Clickable
	{
	public:
		Sheet_Grid_Button(Sheet_Music& sheet);
		Sheet_Grid_Button(Sheet_Music& sheet, sf::IntRect click_area, Note_Pitch corrseponding_pitch);
		void on_clicked();

		Note_Pitch m_corrseponding_pitch;

		std::string m_debug_message;
	private:
		Sheet_Music& sheet;
	};

	class Sheet_Grid
	{
	public:
		Sheet_Grid(Sheet_Music& sheet, Note_Pitch lowest_note, int note_count);
		std::vector<std::vector<std::shared_ptr<Sheet_Grid_Button>>> m_grid;
		Note_Pitch get_pitch(Note_Pitch lowest_note, int distance) const;
		int get_distance(Note_Pitch lowest_note, Note_Pitch note) const;
	private:
		Sheet_Music& sheet;
	};

	class Sheet : public sf::Drawable
	{
	public:
		Sheet(Sheet_Music& sheet);
		
		Sheet_Music& m_sheet;

		Sheet_Grid m_soprano_grid;
		//Sheet_Grid m_bass_grid;

		sf::Sprite m_treble_clef;
		sf::Sprite m_bass_clef;

		void draw(sf::RenderTarget& target, sf::RenderStates states) const;

		static sf::Vector2f m_bass_clef_pos;
		static sf::Vector2f m_treble_clef_pos;
		static sf::Vector2f m_note_offset;
		static float m_whole_note_x;
		static float m_note_size;
		static float m_treble_clef_size;
		static float m_bass_clef_size;
		static float m_button_size;
		static int m_bar_count;

		static int grid_x_offset;
		static int grid_y_offset;
		static int grid_button_width;
		static int grid_button_height;
	};

	class Sheet_Music_Editor : public sf::Drawable
	{
	public:
		Sheet_Music_Editor(Sheet_Music& sheet);


		void draw(sf::RenderTarget& target, sf::RenderStates states) const;

	private:
		Sheet_Music& m_sheet;
		Sheet m_ui_sheet;

		//Resources
		void load_resources();
	};

}
