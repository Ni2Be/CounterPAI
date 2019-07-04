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
		sf::Sprite get_sprite(sf::Vector2f pos, sf::Color color = sf::Color::Black) const;
	};


	class Sheet;
	class Cleff_Grid;
	class Sheet_Grid_Button : public UI::Clickable
	{
	public:
		//Sheet_Grid_Button(Sheet_Music& sheet);
		Sheet_Grid_Button(Cleff_Grid* parent, sf::IntRect click_area, Note_Pitch pitch, Note_Value value, int sixteenth_distance);
		void on_clicked();

		Note_Pitch m_pitch;
		Note_Value m_value;
		int m_sixteenth_distance;
		std::string m_debug_message;
	private:
		Cleff_Grid* m_parent;
	};

	class Cleff_Grid : public sf::Drawable
	{
	public:
		Cleff_Grid(Sheet* parent, std::list<Music_Note>& line, Voice voice, Note_Pitch lowest_note, int note_count, sf::Vector2i offset);
		std::vector<std::vector<std::shared_ptr<Sheet_Grid_Button>>> m_grid;
		Note_Pitch get_pitch(Note_Pitch lowest_note, int distance) const;
		int get_distance(Note_Pitch lowest_note, Note_Pitch note) const;

		Voice m_voice;
		Sheet* m_parent;

		std::list<Music_Note>& m_line;

		sf::Vector2i m_offset;
		Note_Pitch m_lowest_a_or_c;
		sf::Sprite m_clef_sprite;
		void draw(sf::RenderTarget& target, sf::RenderStates states) const;
	private:
	};

	class Sheet_Music_Editor;
	class Sheet : public sf::Drawable
	{
	public:
		Sheet(Sheet_Music& sheet, Sheet_Music_Editor* parent);
		
		Sheet_Music_Editor* m_parent;
		Sheet_Music& m_sheet;

		Cleff_Grid m_soprano_grid;
		Cleff_Grid m_bass_grid;

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
		static float m_bracket_size;
		static int m_bar_count;
		static int notes_per_bar;

		static int grid_x_offset;
		static int grid_y_offset;
		static int grid_button_width;
		static int grid_button_height;
	};

	class GUI;
	class Sheet_Music_Editor : public sf::Drawable
	{
	public:
		Sheet_Music_Editor(Sheet_Music& sheet, GUI* m_parent);


		void draw(sf::RenderTarget& target, sf::RenderStates states) const;

		GUI* m_parent;
		Note_Value selected_value = Note_Value::Whole;
		bool is_deleting = false;
		bool is_tying = false;
		bool m_draw_grid = true;
		bool draw_overlay = false;
		bool wants_info = false;
		bool set_sharp = false;
		bool set_flat = false;
	private:




		Sheet_Music& m_sheet;
		Sheet m_ui_sheet;

		//Resources
		void load_resources();
	};

}
