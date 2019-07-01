#include "GUI.h"
#include <iostream>
#include "Application.h"

#include "Windows_Folder_Dialog.h"

UI::Info_Box::Info_Box(Application* app, const sf::IntRect draw_area, const std::string& info_text)
	:
	m_draw_area(draw_area),
	m_app(app)
{
	m_info_text.setFillColor(sf::Color::Black);
	m_info_text.setFont(m_app->gui.times_new_roman());
	m_info_text.setScale(0.7f, 0.7f);
	m_info_text.setString(info_text);
	m_info_text.setPosition({ (float)m_draw_area.left, (float)m_draw_area.top });
}


void UI::Info_Box::set_info_text(std::string text)
{
	int row_chars = 22;
	std::string::iterator itr = text.begin();
	for (int i = 0; itr != text.end(); i++, itr++)
	{
		std::cout << "\n" << i;
		if (*itr == '\n')
			i = 0;
		if (i > row_chars)
		{
			i = 0;
			itr = text.insert(itr, '\n');
		}
	}
	
	m_info_text.setString(text);
}

void UI::Info_Box::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	sf::RectangleShape background({ (float)m_draw_area.width, (float)m_draw_area.height });
	background.setPosition({ (float)m_draw_area.left, (float)m_draw_area.top });
	background.setOutlineThickness(1.0f);
	background.setOutlineColor(sf::Color::Black);
	target.draw(background, states);

	target.draw(m_info_text, states);
}


UI::GUI::GUI(int width, int height, const std::string& title, Application* parent)
	:
	m_window(sf::VideoMode(width, height), title),
	m_parent(parent),
	m_play_button(parent,   { {0  ,500},{100,100} }, "play"),
	m_stop_button(parent,   { {110,500},{100,100} }, "stop"),
	m_clear_button(parent,  { {220,500},{100,100} }, "clear"),
	m_sheet_editor(parent->m_sheet, this),
	m_whole_button(parent,  { {400,500},{100,100} }, "1/1"),
	m_half_button(parent, { {510,500},{100,100} }, "1/2"),
	m_quater_button(parent, { {620,500},{100,100} }, "1/4"),
	m_tie_button(parent, { {800,500},{100,100} }, "tie"),
	m_delete_button(parent, { {910,500},{100,100} }, "delete"),
	m_overlay_button(parent, { {1110,500},{100,100} }, "overlay"),
	m_info_button(parent, { {1220,500},{100,100} }, "note info"),
	m_info_text(parent, { { 1400, 0 }, { 200, 600 } }, "no Note")
{
	load_resources();
	m_play_button.func = [](Application* app) {
		std::cout << "\nPLAY!\n";
		app->m_debug_log.log("Play Button");
		app->player.stop();
		app->player.play_sheet_music(app->m_sheet);
	};
	attach_drawable(m_play_button);
	m_play_button.draw_rect.setFillColor({ 0x33,0x33,0x33 });
	
	m_stop_button.func = [](Application* app) {
		std::cout << "\nSTOP!\n";
		app->m_debug_log.log("Stop Button");
		app->player.stop();
	};
	attach_drawable(m_stop_button);
	m_stop_button.draw_rect.setFillColor({ 0x33,0x33,0x33 });

	m_clear_button.func = [](Application* app) {
		std::cout << "\nCLEAR!\n";
		app->m_debug_log.log("Clear Button");
		app->m_sheet.m_bass.clear();
		app->m_sheet.m_soprano.clear();
	};
	attach_drawable(m_clear_button);
	m_clear_button.draw_rect.setFillColor({ 0x33,0x33,0x33 });

	m_whole_button.func = [](Application* app) {
		std::cout << "\nWhole!\n";
		app->m_debug_log.log("Whole Button");
		app->gui.m_sheet_editor.selected_value = Note_Value::Whole;
		app->gui.m_whole_button.draw_rect.setFillColor({ 0x00,0x00,0x00 });
		app->gui.m_half_button.draw_rect.setFillColor({ 0x33,0x33,0x33 });
		app->gui.m_quater_button.draw_rect.setFillColor({ 0x33,0x33,0x33 });
	};
	attach_drawable(m_whole_button);
	m_whole_button.draw_rect.setFillColor({ 0x00,0x00,0x00 });

	m_half_button.func = [](Application* app) {
		std::cout << "\nHalf!\n";
		app->m_debug_log.log("Half Button");
		app->gui.m_sheet_editor.selected_value = Note_Value::Halfe;
		app->gui.m_whole_button.draw_rect.setFillColor({ 0x33,0x33,0x33 });
		app->gui.m_half_button.draw_rect.setFillColor({ 0x00,0x00,0x00 });
		app->gui.m_quater_button.draw_rect.setFillColor({ 0x33,0x33,0x33 });
	};
	attach_drawable(m_half_button);
	m_half_button.draw_rect.setFillColor({ 0x33,0x33,0x33 });

	m_quater_button.func = [](Application* app) {
		std::cout << "\nQuater!\n";
		app->m_debug_log.log("Quater Button");
		app->gui.m_sheet_editor.selected_value = Note_Value::Quarter;

		app->gui.m_whole_button.draw_rect.setFillColor({ 0x33,0x33,0x33 });
		app->gui.m_half_button.draw_rect.setFillColor({ 0x33,0x33,0x33 });
		app->gui.m_quater_button.draw_rect.setFillColor({ 0x00,0x00,0x00 });
	};
	attach_drawable(m_quater_button);
	m_quater_button.draw_rect.setFillColor({ 0x33,0x33,0x33 });

	m_tie_button.func = [](Application* app) {
		std::cout << "\nTie!\n";
		app->m_debug_log.log("Tie Button");
		app->gui.m_sheet_editor.is_tying = !app->gui.m_sheet_editor.is_tying;
		if (app->gui.m_sheet_editor.is_tying)
			app->gui.m_tie_button.draw_rect.setFillColor({ 0x00,0x00,0x00 });
		else
			app->gui.m_tie_button.draw_rect.setFillColor({ 0x33,0x33,0x33 });
	};
	attach_drawable(m_tie_button);
	m_tie_button.draw_rect.setFillColor({ 0x33,0x33,0x33 });


	m_delete_button.func = [](Application* app) {
		std::cout << "\nDelete!\n";
		app->m_debug_log.log("Delete Button");
		app->gui.m_sheet_editor.is_deleting = !app->gui.m_sheet_editor.is_deleting;
		if(app->gui.m_sheet_editor.is_deleting)
			app->gui.m_delete_button.draw_rect.setFillColor({ 0x00,0x00,0x00 });
		else
			app->gui.m_delete_button.draw_rect.setFillColor({ 0x33,0x33,0x33 });
	};
	attach_drawable(m_delete_button);
	m_delete_button.draw_rect.setFillColor({ 0x33,0x33,0x33 });


	m_overlay_button.func = [](Application* app) {
		std::cout << "\nOverlay!\n";
		app->m_debug_log.log("Overlay Button");
		app->gui.m_sheet_editor.draw_overlay = !app->gui.m_sheet_editor.draw_overlay;

		////DEBUG
		//for (auto& note : app->m_sheet.m_soprano)
		//	note.m_note_probability = (rand() % 1000) / 1000.0f;
		//for (auto& note : app->m_sheet.m_bass)
		//	note.m_note_probability = (rand() % 1000) / 1000.0f;
		////ENDDEBUG


		if (app->gui.m_sheet_editor.draw_overlay)
			app->gui.m_overlay_button.draw_rect.setFillColor({ 0x00,0x00,0x00 });
		else
			app->gui.m_overlay_button.draw_rect.setFillColor({ 0x33,0x33,0x33 });
	};
	attach_drawable(m_overlay_button);
	m_overlay_button.draw_rect.setFillColor({ 0x33,0x33,0x33 });

	m_info_button.func = [](Application* app) {
		std::cout << "\nInfo!\n";
		app->m_debug_log.log("Info Button");



		app->gui.m_sheet_editor.wants_info = !app->gui.m_sheet_editor.wants_info;

		if (app->gui.m_sheet_editor.wants_info)
		{
			app->m_evaluator.evaluate_notes(app->m_sheet.m_bass, app->m_sheet.m_soprano);
			std::cout << "\nEvaluated:\n" << app->m_evaluator << "\n";

			app->gui.m_info_button.draw_rect.setFillColor({ 0x00,0x00,0x00 });
		}
		else
		{
			//TODO should not be set in gui
			for (auto& note : app->m_sheet.m_bass)
				note.m_note_info = "no message!";
			for (auto& note : app->m_sheet.m_soprano)
				note.m_note_info = "no message!";
			
			app->gui.m_info_button.draw_rect.setFillColor({ 0x33,0x33,0x33 });
		}
	};
	attach_drawable(m_info_button);
	m_info_button.draw_rect.setFillColor({ 0x33,0x33,0x33 });


	attach_drawable(m_info_text);


	m_window.setActive(false);

}


void UI::GUI::render()
{
	sf::sleep(sf::milliseconds(33));
	m_window.clear(sf::Color::White);
	m_window.draw(m_sheet_editor);
	for (auto& drawable : m_drawables)
	{
		m_window.draw(*drawable);
	}

	m_window.display();
}

void UI::GUI::check_events()
{
	Event_System::instance().check_events(m_window);
}


void UI::GUI::load_resources()
{
	m_times_new_roman.loadFromFile("data/times-new-roman.ttf");
}

