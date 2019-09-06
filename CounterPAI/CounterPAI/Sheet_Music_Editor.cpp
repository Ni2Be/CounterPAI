#include "Sheet_Music_Editor.h"
#include "SVG_Parser.h"
#include "GUI.h"
#include "Application.h"
#include <SFML/Graphics.hpp>

#include "Utility.h"

#include "Rule_Evaluation.h"

#include "Note_Evaluation.h"

float UI::Sheet::m_button_size      = 5;
float UI::Sheet::m_note_size        = 3.0f;
float UI::Sheet::m_treble_clef_size = 7.7f;
float UI::Sheet::m_bass_clef_size   = 4.2f;
float UI::Sheet::m_bracket_size     = 0.7f;

sf::Vector2f UI::Sheet::m_treble_clef_pos = { 10.0f, 0.0f };
sf::Vector2f UI::Sheet::m_bass_clef_pos = { 50.0f, 300.0f };
sf::Vector2f UI::Sheet::m_note_offset     = { 200.0f, 0.0f };
int UI::Sheet::m_bar_count = 13;
int UI::Sheet::notes_per_bar = 4;
//int UI::Sheet::notes_per_bar = 8; TODO should be scalable
int UI::Sheet::grid_x_offset = 150;
int UI::Sheet::grid_y_offset = 20;
int UI::Sheet::grid_button_width = 27;
int UI::Sheet::grid_button_height = 10;


float UI::Sheet::m_whole_note_x = grid_button_width * 4;

UI::Sheet_Music_Editor::Sheet_Music_Editor(Sheet_Music& sheet, GUI* parent)
	:
	m_sheet(sheet),
	m_ui_sheet(sheet, this),
	m_parent(parent)
{
}

void UI::Sheet_Music_Editor::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	target.draw(m_ui_sheet, states);
}

void UI::Sheet_Music_Editor::load_resources()
{

}

UI::Note::Note(Music_Note& note)
	:
	m_note(note)
{

}

float UI::Note::get_offset() const
{
	return Sheet::m_whole_note_x / static_cast<float>(m_note.m_value);
}


sf::Sprite UI::Note::get_sprite(sf::Vector2f pos, sf::Color color) const
{
	sf::Sprite note_sprite;
	switch (m_note.m_value)
	{
	case Note_Value::Whole:
		note_sprite.setTexture(Texture_Catalog::instance().m_texture_catalog["whole_note"]); break;
	case Note_Value::Halfe:
		note_sprite.setTexture(Texture_Catalog::instance().m_texture_catalog["halfe_note"]); break;
	case Note_Value::Quarter:
		note_sprite.setTexture(Texture_Catalog::instance().m_texture_catalog["quater_note"]); break;
	case Note_Value::Eighth:
		note_sprite.setTexture(Texture_Catalog::instance().m_texture_catalog["eighth_note"]); break;
	case Note_Value::Sixteenth:
		note_sprite.setTexture(Texture_Catalog::instance().m_texture_catalog["sixteenth_note"]); break;
	default: std::cerr << "invalid notevalue\n"; break;
	}
	note_sprite.setColor(color);
	/*if(m_note.m_is_tied)
		note_sprite.setColor({ (sf::Uint8)note_sprite.getColor().r,(sf::Uint8)note_sprite.getColor().g,(sf::Uint8)note_sprite.getColor().b, 0x44});
*/
	note_sprite.setOrigin(note_sprite.getScale() / 2.0f);
	note_sprite.setPosition(pos);
	return note_sprite;
}

UI::Texture_Catalog::Texture_Catalog()
{
	load_resources();
}

void UI::Texture_Catalog::load_resources()
{
	std::cout << "\nloading..";

	//Notes
	m_texture_catalog["whole_note"]     = SVG_Parser("data/pictures/icons/note-1.svg",       Sheet::m_note_size).m_texture;
	m_texture_catalog["halfe_note"]     = SVG_Parser("data/pictures/icons/note-2.svg",       Sheet::m_note_size).m_texture;
	m_texture_catalog["quater_note"]    = SVG_Parser("data/pictures/icons/note-4.svg",       Sheet::m_note_size).m_texture;
	m_texture_catalog["eighth_note"]    = SVG_Parser("data/pictures/icons/note-8.svg",       Sheet::m_note_size).m_texture;
	m_texture_catalog["sixteenth_note"] = SVG_Parser("data/pictures/icons/note-16.svg",      Sheet::m_note_size).m_texture;
	m_texture_catalog["flat"]           = SVG_Parser("data/pictures/icons/note-flat.svg",    Sheet::m_note_size).m_texture;
	m_texture_catalog["sharp"]          = SVG_Parser("data/pictures/icons/note-sharp.svg",   Sheet::m_note_size).m_texture;
	m_texture_catalog["natural"]        = SVG_Parser("data/pictures/icons/note-natural.svg", Sheet::m_note_size).m_texture;

	//Clef
	m_texture_catalog["bass_clef"]   = SVG_Parser("data/pictures/icons/clef-bass.svg", Sheet::m_bass_clef_size).m_texture;
	m_texture_catalog["treble_clef"] = SVG_Parser("data/pictures/icons/clef.svg", Sheet::m_treble_clef_size).m_texture;

	//Brackets
	m_texture_catalog["bracket"] = SVG_Parser("data/pictures/icons/Klammer.svg", Sheet::m_bracket_size).m_texture;

	//Buttons
	m_texture_catalog["b_tie"]  = SVG_Parser("data/pictures/icons/note-tie.svg", Sheet::m_button_size).m_texture;
	m_texture_catalog["b_help"] = SVG_Parser("data/pictures/icons/help-contents.svg", Sheet::m_button_size).m_texture;
}


UI::Sheet::Sheet(Sheet_Music& sheet, Sheet_Music_Editor* parent)
	:
	m_sheet(sheet),
	m_parent(parent),
	//17 pos from lower A to top C i.e. for bass lower C to top E
	m_soprano_grid(this, sheet.m_soprano, Voice::Soprano, Note_Pitch::A3, 17, {150, 20}),
	m_bass_grid(this, sheet.m_bass, Voice::Bass, Note_Pitch::C2, 17, { 150, 270 })
{
}

void UI::Sheet::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	target.draw(m_soprano_grid, states);
	target.draw(m_bass_grid, states);



	//draw bracket
	sf::Sprite bracket;
	bracket.setTexture(Texture_Catalog::instance().m_texture_catalog["bracket"]);
	bracket.setPosition({ (float)-215.0f, (float)-65.0f });
	target.draw(bracket, states);
	//draw vertical lines
	sf::RectangleShape line({ 1, (float)Sheet::grid_button_height * 9  + m_bass_grid.m_offset.y - m_soprano_grid.m_offset.y - 8 });
	line.setFillColor(sf::Color::Black);
	
	line.setPosition({ m_soprano_grid.m_offset.x - 110.0f ,  (float)m_soprano_grid.m_offset.y + 4 * (float)Sheet::grid_button_height + 3 });
	line.setSize({ line.getSize().x * 3, line.getSize().y });
	target.draw(line, states);

	line.setPosition({ Sheet::m_bar_count * Sheet::grid_button_width * Sheet::notes_per_bar + (float)m_soprano_grid.m_offset.x, line.getPosition().y });
	target.draw(line, states);

	line.setPosition({ line.getPosition().x + 5 ,line.getPosition().y });
	line.setSize({ line.getSize().x * 3, line.getSize().y });
	target.draw(line, states);
		
}

Note_Value get_note_value_by_grid_pos(int rel_pos)
{
	switch (rel_pos)
	{
	case 0: return Note_Value::Whole;
	case 1: return Note_Value::Quarter;
	case 2: return Note_Value::Halfe;
	case 3: return Note_Value::Quarter;
	default: std::cerr << "invalid Note_Value\n"; return Note_Value::Whole;
	}
}

UI::Cleff_Grid::Cleff_Grid(Sheet* parent, std::list<Music_Note>& line, Voice voice, Note_Pitch lowest_a_or_c, int note_count, sf::Vector2i offset)
	:
	m_parent(parent),
	m_voice(voice),
	m_offset(offset),
	m_line(line),
	m_lowest_a_or_c(lowest_a_or_c)
{
	switch (m_voice) 
	{
	case Voice::Soprano:
		m_clef_sprite.setTexture(Texture_Catalog::instance().m_texture_catalog["treble_clef"]);
		m_clef_sprite.setPosition({ offset.x - 140.0f, offset.y - 20.0f });
		break;
	case Voice::Bass:
		m_clef_sprite.setTexture(Texture_Catalog::instance().m_texture_catalog["bass_clef"]);
		m_clef_sprite.setPosition({ offset.x - 100.0f, offset.y + 25.0f });
		break;
	default: std::cerr << "\ninvalid Voice";
		break;
	}

	for (int i = 0; i < Sheet::m_bar_count * Sheet::notes_per_bar; i++)
	{
		std::vector<std::shared_ptr<Sheet_Grid_Button>> temp;
		for (int n = 0; n < note_count; n++)
		{
			std::shared_ptr<Sheet_Grid_Button> temp_button(
					new Sheet_Grid_Button(
						this,
						sf::IntRect( { offset.x + i * UI::Sheet::grid_button_width, offset.y + n * UI::Sheet::grid_button_height}
						, { UI::Sheet::grid_button_width, UI::Sheet::grid_button_height} ),
						Music_Note::get_ACscale_pitch(lowest_a_or_c, note_count - 1 - n),
						get_note_value_by_grid_pos(i % Sheet::notes_per_bar),
						i * (16 / Sheet::notes_per_bar)
						));

			temp.push_back(temp_button);
			temp.back()->m_debug_message = "(" + std::to_string(i) + "," + std::to_string(n) + ")";
		}
		m_grid.push_back(temp);
	}
};


void UI::Cleff_Grid::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	//draw click_areas
	if (m_parent->m_parent->m_draw_grid)
	{
		for (auto& column : m_grid)
		{
			for (auto& button : column)
			{
				sf::RectangleShape temp({ (float)button->m_click_area.width, (float)button->m_click_area.height });
				temp.setFillColor(sf::Color::White);
				temp.setOutlineThickness(1.0f);
				temp.setOutlineColor({ 0xcc,0xcc,0xcc });
				temp.setPosition({ (float)button->m_click_area.left, (float)button->m_click_area.top });
				target.draw(temp, states);
			}
		}
	}


	//draw claf
	target.draw(m_clef_sprite, states);

	//draw horizontal lines
	for (int i = 0; i < 5; i++)
	{
		sf::RectangleShape line({(Sheet::m_bar_count + 1) * Sheet::notes_per_bar * Sheet::grid_button_width + 10.0f, 2 });
		line.setFillColor(sf::Color::Black);
		line.setPosition({ m_offset.x - 110.0f,  40 + i * 20 + (float) m_offset.y + (Sheet::grid_button_height / 2) - 2 });
		target.draw(line, states);
	}
	//draw vertical lines
	for (int i = 0; i <= Sheet::m_bar_count; i++)
	{
		sf::RectangleShape line({ 1, (float)Sheet::grid_button_height * 9 - 10});
		line.setFillColor(sf::Color::Black);
		line.setPosition({ i * Sheet::grid_button_width * Sheet::notes_per_bar + (float)m_offset.x,  (float)m_offset.y + 4 * (float)Sheet::grid_button_height + 4});
		target.draw(line, states);
		if (i == 0)
		{
			line.setPosition({ m_offset.x - 110.0f ,line.getPosition().y });
			line.setSize({ line.getSize().x * 3, line.getSize().y });
			target.draw(line, states);
		}
		if (i == Sheet::m_bar_count)
		{
			line.setPosition({ line.getPosition().x + 5 ,line.getPosition().y }); 
			line.setSize({ line.getSize().x * 3, line.getSize().y });
			target.draw(line, states);
		}
	}

	//draw notes
	sf::Vector2f offset = { (float)m_offset.x - 20, 0.0f };
	sf::Vector2f last_note_pos = offset;
	for (auto& note : m_line)
	{
		Note ui_note(note);

		offset.y = m_offset.y + -60 + 16 * Sheet::grid_button_height + (Sheet::grid_button_height / 2.0f) - (float)Music_Note::get_ACscale_distance(m_lowest_a_or_c, note.get_basic_note()) * Sheet::grid_button_height;
		
		std::string note_info = ui_note.m_note.get_note_info(Eval::Rule_Evaluation::C_INDEX_NAME);

		Eval::Rule_Evaluation note_evaluation;
		if (note_info != "null")
			note_evaluation = Eval::Rule_Evaluation(note_info);

		if (m_parent->m_parent->draw_overlay)
			target.draw(ui_note.get_sprite(offset, 
				{ (sf::Uint8)(255 * (1.0f - note_evaluation.m_probability)),
					(sf::Uint8)(255 * note_evaluation.m_probability),
						(sf::Uint8)0 }), states);
		else
			target.draw(ui_note.get_sprite(offset), states);

		sf::Text symbol;
		symbol.setFont(m_parent->m_parent->m_parent->times_new_roman());
		if (m_parent->m_parent->draw_overlay)
			symbol.setFillColor({ (sf::Uint8)(255 * (1.0f - note_evaluation.m_probability)),
					(sf::Uint8)(255 * note_evaluation.m_probability),
						(sf::Uint8)0 });
		else
			symbol.setFillColor(sf::Color::Black);
		symbol.setPosition({ offset.x + 20, offset.y + 22 });
		

		if (note.m_is_flat)
			symbol.setString("b");
		else if (note.m_is_sharp)
			symbol.setString("#");
		target.draw(symbol, states);

		if (note.m_is_tied)
		{
			auto curve = sf::VertexArray(sf::PrimitiveType::LineStrip);
			auto curve2 = sf::VertexArray(sf::PrimitiveType::LineStrip);
			
			for (float x = -(offset.x - last_note_pos.x) / 2 + 5; x < (offset.x - last_note_pos.x) / 2 - 5; x += 0.1f)
			{
				sf::Vertex vertex(sf::Vector2f{ last_note_pos.x + 40 + 5 + (offset.x - last_note_pos.x) / 2 + x - 5, last_note_pos.y + std::pow(x * (10.0f / (offset.x - last_note_pos.x)), 2) + 4 + 25 });
				vertex.color = sf::Color::Black;
				curve.append(vertex);
				vertex.position = sf::Vector2f{ last_note_pos.x + 40 + 5 + (offset.x - last_note_pos.x) / 2 + x - 5, last_note_pos.y + std::pow(x * (10.0f / (offset.x - last_note_pos.x)) * 1.01f, 2) - 1 + 4 + 25};
				curve2.append(vertex);
			}
			target.draw(curve);
			target.draw(curve2);
		}

		//draw helper lines Soprano
		if ((note.m_pitch == Note_Pitch::C4 && note.m_voice == Voice::Soprano)
			|| (note.m_pitch == Note_Pitch::B3 && note.m_voice == Voice::Soprano)
			|| (note.m_pitch == Note_Pitch::A3 && note.m_voice == Voice::Soprano))
		{

			sf::RectangleShape line({ 44.0f, 2 });
			line.setFillColor(sf::Color::Black);
			line.setPosition({ offset.x + 10, m_offset.y + -60 + 16 * Sheet::grid_button_height + (Sheet::grid_button_height / 2.0f) + 4 * Sheet::grid_button_height - 2 });
			target.draw(line, states);
			if (note.m_pitch == Note_Pitch::A3)
			{
				line.setPosition({ offset.x + 10, m_offset.y + -60 + 16 * Sheet::grid_button_height + (Sheet::grid_button_height / 2.0f) + 6 * Sheet::grid_button_height - 2 });
				target.draw(line, states);
			}
		}
		if (note.m_pitch == Note_Pitch::C6
			|| note.m_pitch == Note_Pitch::B5
			|| note.m_pitch == Note_Pitch::A5)
		{

			sf::RectangleShape line({ 44.0f, 2 });
			line.setFillColor(sf::Color::Black);
			line.setPosition({ offset.x + 10, m_offset.y + -60 + 0 * Sheet::grid_button_height + (Sheet::grid_button_height / 2.0f) + 8 * Sheet::grid_button_height - 2 });
			target.draw(line, states);
			if (note.m_pitch == Note_Pitch::C6)
			{
				line.setPosition({ offset.x + 10, m_offset.y + -60 + 0 * Sheet::grid_button_height + (Sheet::grid_button_height / 2.0f) + 6 * Sheet::grid_button_height - 2 });
				target.draw(line, states);
			}
		}

		//draw helper lines Bass
		if (note.m_pitch == Note_Pitch::C2
			|| note.m_pitch == Note_Pitch::D2
			|| note.m_pitch == Note_Pitch::E2)
		{

			sf::RectangleShape line({ 44.0f, 2 });
			line.setFillColor(sf::Color::Black);
			line.setPosition({ offset.x + 10, m_offset.y + -60 + 16 * Sheet::grid_button_height + (Sheet::grid_button_height / 2.0f) + 4 * Sheet::grid_button_height - 2 });
			target.draw(line, states);
			if (note.m_pitch == Note_Pitch::C2)
			{
				line.setPosition({ offset.x + 10, m_offset.y + -60 + 16 * Sheet::grid_button_height + (Sheet::grid_button_height / 2.0f) + 6 * Sheet::grid_button_height - 2 });
				target.draw(line, states);
			}
		}
		if ((note.m_pitch == Note_Pitch::C4 && note.m_voice == Voice::Bass)
			|| (note.m_pitch == Note_Pitch::D4 && note.m_voice == Voice::Bass)
			|| (note.m_pitch == Note_Pitch::E4 && note.m_voice == Voice::Bass))
		{

			sf::RectangleShape line({ 44.0f, 2 });
			line.setFillColor(sf::Color::Black);
			line.setPosition({ offset.x + 10, m_offset.y + -60 + 0 * Sheet::grid_button_height + (Sheet::grid_button_height / 2.0f) + 8 * Sheet::grid_button_height - 2 });
			target.draw(line, states);
			if (note.m_pitch == Note_Pitch::E4)
			{
				line.setPosition({ offset.x + 10, m_offset.y + -60 + 0 * Sheet::grid_button_height + (Sheet::grid_button_height / 2.0f) + 6 * Sheet::grid_button_height - 2 });
				target.draw(line, states);
			}
		}



		last_note_pos = offset;
		offset.x += ui_note.get_offset();
	}
}


UI::Sheet_Grid_Button::Sheet_Grid_Button(Cleff_Grid* parent, sf::IntRect click_area, Note_Pitch pitch, Note_Value value, int sixteenth_distance)
	:
	m_parent(parent),
	m_pitch(pitch),
	m_value(value),//not used
	m_sixteenth_distance(sixteenth_distance)
{
	Clickable::m_click_area = click_area;
}

std::string generate_info_text(Music_Note note)
{
	std::string info_text = "";

	info_text += Utility::to_str(note.get_basic_note());

	if (note.m_is_flat)
		info_text += "b";
	if (note.m_is_sharp)
		info_text += "#";

	//Note Evaluation
	std::stringstream ss;
	ss << note.get_note_info(Eval::Note_Evaluation::C_INDEX_NAME);
	Eval::Note_Evaluation evaluation;
	ss >> evaluation;
	info_text += "\nDirection: " + Utility::to_str(evaluation.m_direction);
	info_text += "\nInterval: " + Utility::to_str(evaluation.m_interval);
	info_text += "\nJump: " + Utility::to_str(evaluation.m_jump_interval);
	info_text += "\nMotion: " + Utility::to_str(evaluation.m_motion);


	//Fux Rule Evaluation
	info_text += "\n\n";
	std::stringstream ss2;
	ss2 << note.get_note_info(Eval::Rule_Evaluation::C_INDEX_NAME);
	Eval::Rule_Evaluation rule_evaluation;
	ss2 >> rule_evaluation;
	for (const auto& r : rule_evaluation.broken_rules)
		info_text += Eval::Rule_Evaluation::get_rule_text(r) + "\n";
	info_text += "\nProbability: " + std::to_string(rule_evaluation.m_probability);


	return info_text;
}

void UI::Sheet_Grid_Button::on_clicked()
{
	//std::cout << "info: " << m_parent->m_parent->m_parent->wants_info << ", del: " << m_parent->m_parent->m_parent->is_deleting << ", ty: " << m_parent->m_parent->m_parent->is_tying << "\n";
	if (m_parent->m_parent->m_parent->wants_info)
	{
		Music_Note note = m_parent->m_parent->m_sheet.get_note(m_parent->m_voice, m_sixteenth_distance);
		if (note.m_is_corrupted)
			return;
		m_parent->m_parent->m_parent->m_parent->m_parent->m_feedback_piano.stop();
		m_parent->m_parent->m_parent->m_parent->m_parent->m_feedback_piano.play(m_parent->m_parent->m_sheet.get_note(m_parent->m_voice, m_sixteenth_distance));

		m_parent->m_parent->m_parent->m_parent->m_parent->m_debug_log.log("Info N:" + this->m_debug_message);
		std::string info_text = generate_info_text(note);

		m_parent->m_parent->m_parent->m_parent->m_info_text.set_info_text(info_text);
	}
	else
	{
		if (m_parent->m_parent->m_parent->is_deleting)
		{
			m_parent->m_parent->m_parent->m_parent->m_parent->m_debug_log.log("Delete N:" + this->m_debug_message);
			m_parent->m_parent->m_sheet.delete_note(m_parent->m_voice, m_sixteenth_distance);
		}
		else if (m_parent->m_parent->m_parent->is_tying)
		{
			m_parent->m_parent->m_parent->m_parent->m_parent->m_debug_log.log("Tie N:" + this->m_debug_message);
			m_parent->m_parent->m_sheet.add_note(Music_Note(m_pitch, m_parent->m_parent->m_parent->selected_value, m_parent->m_voice, true), m_sixteenth_distance);
		}
		else
		{
			if (m_parent->m_parent->m_parent->set_flat)
			{
				if (m_parent->m_parent->m_parent->selected_value == Note_Value::Eighth
					&& m_parent->m_parent->m_sheet.get_note(m_parent->m_voice, m_sixteenth_distance + 2).m_value != Note_Value::Eighth)
					m_parent->m_parent->m_sheet.add_note(Music_Note(static_cast<Note_Pitch>((int)m_pitch - 1), m_parent->m_parent->m_parent->selected_value, m_parent->m_voice, false, false, true), m_sixteenth_distance + 2);
				m_parent->m_parent->m_parent->m_parent->m_parent->m_debug_log.log("Add b N:" + this->m_debug_message);
				m_parent->m_parent->m_parent->m_parent->m_info_text.set_info_text(Utility::to_str(static_cast<Note_Pitch>((int)m_pitch - 1)));
				m_parent->m_parent->m_sheet.add_note(Music_Note(static_cast<Note_Pitch>((int)m_pitch - 1), m_parent->m_parent->m_parent->selected_value, m_parent->m_voice, false, false, true), m_sixteenth_distance);
			}
			else if (m_parent->m_parent->m_parent->set_sharp)
			{
				if (m_parent->m_parent->m_parent->selected_value == Note_Value::Eighth
					&& m_parent->m_parent->m_sheet.get_note(m_parent->m_voice, m_sixteenth_distance + 2).m_value != Note_Value::Eighth)
					m_parent->m_parent->m_sheet.add_note(Music_Note(static_cast<Note_Pitch>((int)m_pitch + 1), m_parent->m_parent->m_parent->selected_value, m_parent->m_voice, false, true, false), m_sixteenth_distance + 2);
				m_parent->m_parent->m_parent->m_parent->m_parent->m_debug_log.log("Add # N:" + this->m_debug_message);
				m_parent->m_parent->m_parent->m_parent->m_info_text.set_info_text(Utility::to_str(static_cast<Note_Pitch>((int)m_pitch + 1)));
				m_parent->m_parent->m_sheet.add_note(Music_Note(static_cast<Note_Pitch>((int)m_pitch + 1), m_parent->m_parent->m_parent->selected_value, m_parent->m_voice, false, true, false), m_sixteenth_distance);
			}
			else
			{
				if (m_parent->m_parent->m_parent->selected_value == Note_Value::Eighth
					&& m_parent->m_parent->m_sheet.get_note(m_parent->m_voice, m_sixteenth_distance + 2).m_value != Note_Value::Eighth)
					m_parent->m_parent->m_sheet.add_note(Music_Note(m_pitch, m_parent->m_parent->m_parent->selected_value, m_parent->m_voice), m_sixteenth_distance + 2);
				m_parent->m_parent->m_parent->m_parent->m_parent->m_debug_log.log("Add N:" + this->m_debug_message);
				m_parent->m_parent->m_parent->m_parent->m_info_text.set_info_text(Utility::to_str(m_pitch));
				m_parent->m_parent->m_sheet.add_note(Music_Note(m_pitch, m_parent->m_parent->m_parent->selected_value, m_parent->m_voice), m_sixteenth_distance);

			}
			m_parent->m_parent->m_parent->m_parent->m_parent->m_feedback_piano.stop();
			m_parent->m_parent->m_parent->m_parent->m_parent->m_feedback_piano.play(m_parent->m_parent->m_sheet.get_note(m_parent->m_voice, m_sixteenth_distance));
		}
	}
}
