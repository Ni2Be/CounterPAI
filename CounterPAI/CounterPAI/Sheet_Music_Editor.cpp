#include "Sheet_Music_Editor.h"
#include "SVG_Parser.h"

float UI::Sheet::m_button_size    = 5;
float UI::Sheet::m_note_size      = 3.0f;
float UI::Sheet::m_treble_clef_size = 7.7f;
float UI::Sheet::m_bass_clef_size = 4.2f;
sf::Vector2f UI::Sheet::m_treble_clef_pos = { 10.0f, 0.0f };
sf::Vector2f UI::Sheet::m_bass_clef_pos = { 50.0f, 300.0f };
sf::Vector2f UI::Sheet::m_note_offset     = { 200.0f, 0.0f };
int UI::Sheet::m_bar_count = 11;
int UI::Sheet::grid_x_offset = 150;
int UI::Sheet::grid_y_offset = 20;
int UI::Sheet::grid_button_width = 27;
int UI::Sheet::grid_button_height = 10;

float UI::Sheet::m_whole_note_x = grid_button_width * 4;

UI::Sheet_Music_Editor::Sheet_Music_Editor(Sheet_Music& sheet)
	:
	m_sheet(sheet),
	m_ui_sheet(sheet)
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


sf::Sprite UI::Note::get_sprite(sf::Vector2f pos) const
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

	//Buttons
	m_texture_catalog["b_tie"]  = SVG_Parser("data/pictures/icons/note-tie.svg", Sheet::m_button_size).m_texture;
	m_texture_catalog["b_help"] = SVG_Parser("data/pictures/icons/help-contents.svg", Sheet::m_button_size).m_texture;
}


UI::Sheet::Sheet(Sheet_Music& sheet)
	:
	m_sheet(sheet),
	//17 pos from lower A to top C i.e. for bass lower C to top E
	m_soprano_grid(sheet, Note_Pitch::A3, 17)
{
	m_bass_clef.setTexture(Texture_Catalog::instance().m_texture_catalog["bass_clef"]);
	m_bass_clef.setPosition(m_bass_clef_pos);
	m_treble_clef.setTexture(Texture_Catalog::instance().m_texture_catalog["treble_clef"]);
	m_treble_clef.setPosition(m_treble_clef_pos);
}

void UI::Sheet::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	//debug draw click_areas
	for (auto& column : m_soprano_grid.m_grid)
	{
		for (auto& button : column)
		{
			sf::RectangleShape temp({ (float)button->m_click_area.width, (float)button->m_click_area.height });
			temp.setFillColor(sf::Color::White);
			temp.setOutlineThickness(1.0f);
			temp.setOutlineColor(sf::Color::Black);
			temp.setPosition({ (float)button->m_click_area.left, (float)button->m_click_area.top });
			//target.draw(temp, states);
		}
	}

	//draw clafs
	target.draw(m_bass_clef, states);
	target.draw(m_treble_clef, states);

	//draw lines
	for (int i = 0; i < 5; i++)
	{
		sf::RectangleShape line({ 1400, 2 });
		line.setFillColor(sf::Color::Black);
		line.setPosition({ 10, m_treble_clef_pos.y + 40 + i * 20 + grid_y_offset + (grid_button_height / 2 ) - 2});

		target.draw(line, states);
	}
	//draw lines
	for (int i = 0; i < 5; i++)
	{
		sf::RectangleShape line({ 1400, 2 });
		line.setFillColor(sf::Color::Black);
		line.setPosition({ 10, m_bass_clef_pos.y - 5 + i * 20 + grid_y_offset + (grid_button_height / 2) - 2 });

		target.draw(line, states);
	}
	//draw notes
	sf::Vector2f offset = { (float)grid_x_offset - 20, 0.0f};
	for (auto& note : m_sheet.m_soprano)
	{
		Note ui_note(note);
		offset.y = -40 + 16 * grid_button_height + (grid_button_height / 2.0f) - (float)m_soprano_grid.get_distance(Note_Pitch::A3, note.m_pitch) * grid_button_height;
		target.draw(ui_note.get_sprite(offset), states);
		offset.x += ui_note.get_offset(); 
	}
	offset = { (float)grid_x_offset - 20, 0.0f };
	//for (auto& note : m_sheet.m_bass)
	//{
	//	Note ui_note(note);
	//	offset.y = -grid_button_height + 16 * grid_button_height + (grid_button_height / 2.0f) - (float)m_bass_clef_pos.get_distance(Note_Pitch::A3, note.m_pitch) * grid_button_height;
	//	target.draw(ui_note.get_sprite(offset), states);
	//	offset.x += ui_note.get_offset();
	//}
}

UI::Sheet_Grid::Sheet_Grid(Sheet_Music& sheet, Note_Pitch lowest_a_or_c, int note_count)
	:
	sheet(sheet)
{
	//4 clickpositions per bar
	for (int i = 0; i < Sheet::m_bar_count * 4; i++)
	{
		std::vector<std::shared_ptr<Sheet_Grid_Button>> temp;
		for (int n = 0; n < note_count; n++)
		{
			std::shared_ptr<Sheet_Grid_Button> temp_button(
					new Sheet_Grid_Button(
						sheet,
						sf::IntRect( { UI::Sheet::grid_x_offset + i * UI::Sheet::grid_button_width, UI::Sheet::grid_y_offset + n * UI::Sheet::grid_button_height}
						, { UI::Sheet::grid_button_width, UI::Sheet::grid_button_height} ),
						get_pitch(lowest_a_or_c, note_count - 1 - n)));

			temp.push_back(temp_button);
			temp.back()->m_debug_message = std::to_string(i) + " " + std::to_string(n);
		}
		m_grid.push_back(temp);
	}
};

Note_Pitch UI::Sheet_Grid::get_pitch(Note_Pitch lowest_note, int distance) const
{
	if (lowest_note == Note_Pitch::C2)
	{
		switch (distance)
		{
		case 0:  return Note_Pitch::C2;
		case 1:  return Note_Pitch::D2;
		case 2:  return Note_Pitch::E2;
		case 3:  return Note_Pitch::F2;
		case 4:  return Note_Pitch::G2;
		case 5:  return Note_Pitch::A2;
		case 6:  return Note_Pitch::B2;
		case 7:  return Note_Pitch::C3;
		case 8:  return Note_Pitch::D3;
		case 9:  return Note_Pitch::E3;
		case 10: return Note_Pitch::F3;
		case 11: return Note_Pitch::G3;
		case 12: return Note_Pitch::A3;
		case 13: return Note_Pitch::B3;
		case 14: return Note_Pitch::C4;
		case 15: return Note_Pitch::D4;
		case 16: return Note_Pitch::E4;
		default: std::cerr << "invalid bass Note\n"; break;
		}
	}
	if (lowest_note == Note_Pitch::A3)
	{
		switch (distance)
		{
		case 0:  return Note_Pitch::A3;
		case 1:  return Note_Pitch::B3;
		case 2:  return Note_Pitch::C4;
		case 3:  return Note_Pitch::D4;
		case 4:  return Note_Pitch::E4;
		case 5:  return Note_Pitch::F4;
		case 6:  return Note_Pitch::G4;
		case 7:  return Note_Pitch::A4;
		case 8:  return Note_Pitch::B4;
		case 9:  return Note_Pitch::C5;
		case 10: return Note_Pitch::D5;
		case 11: return Note_Pitch::E5;
		case 12: return Note_Pitch::F5;
		case 13: return Note_Pitch::G5;
		case 14: return Note_Pitch::A5;
		case 15: return Note_Pitch::B5;
		case 16: return Note_Pitch::C6;
		default: std::cerr << "invalid soprano Note\n"; break;
		}
	}
	return Note_Pitch::A0;
}


int UI::Sheet_Grid::get_distance(Note_Pitch lowest_note, Note_Pitch note) const
{
	if (lowest_note == Note_Pitch::C2)
	{
		switch (note)
		{
		case Note_Pitch::C2: return 0;
		case Note_Pitch::D2: return 1;
		case Note_Pitch::E2: return 2;
		case Note_Pitch::F2: return 3;
		case Note_Pitch::G2: return 4;
		case Note_Pitch::A2: return 5;
		case Note_Pitch::B2: return 6;
		case Note_Pitch::C3: return 7;
		case Note_Pitch::D3: return 8;
		case Note_Pitch::E3: return 9;
		case Note_Pitch::F3: return 10;
		case Note_Pitch::G3: return 11;
		case Note_Pitch::A3: return 12;
		case Note_Pitch::B3: return 13;
		case Note_Pitch::C4: return 14;
		case Note_Pitch::D4: return 15;
		case Note_Pitch::E4: return 16;
		default: std::cerr << "invalid bass Note\n"; break;
		}
	}
	if (lowest_note == Note_Pitch::A3)
	{
		switch (note)
		{
		case Note_Pitch::A3: return 0;
		case Note_Pitch::B3: return 1;
		case Note_Pitch::C4: return 2;
		case Note_Pitch::D4: return 3;
		case Note_Pitch::E4: return 4;
		case Note_Pitch::F4: return 5;
		case Note_Pitch::G4: return 6;
		case Note_Pitch::A4: return 7;
		case Note_Pitch::B4: return 8;
		case Note_Pitch::C5: return 9;
		case Note_Pitch::D5: return 10;
		case Note_Pitch::E5: return 11;
		case Note_Pitch::F5: return 12;
		case Note_Pitch::G5: return 13;
		case Note_Pitch::A5: return 14;
		case Note_Pitch::B5: return 15;
		case Note_Pitch::C6: return 16;
		default: std::cerr << "invalid soprano Note: " << (int)note << " \n"; break;
		}
	}
	return -1;
}


UI::Sheet_Grid_Button::Sheet_Grid_Button(Sheet_Music& sheet)
	:
	sheet(sheet)
{
}

UI::Sheet_Grid_Button::Sheet_Grid_Button(Sheet_Music& sheet, sf::IntRect click_area, Note_Pitch corrseponding_pitch)
	:
	sheet(sheet),
	m_corrseponding_pitch(corrseponding_pitch)
{
	Clickable::m_click_area = click_area;
}

void UI::Sheet_Grid_Button::on_clicked()
{
	sheet.add_note(Music_Note(m_corrseponding_pitch, Note_Value::Whole, Voice::Soprano));
}
