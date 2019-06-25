#include "Clickable.h"
#include <iostream>
#include "Eventsystem.h"
#include "Application.h"

UI::Clickable::Clickable()
{
	Event_System::instance().add_clickable(*this);
}

UI::Clickable::~Clickable()
{
	Event_System::instance().remove_clickable(*this);
}

void UI::Clickable::check_if_clicked(sf::Vector2i click_pos)
{
	if (m_click_area.contains(click_pos))
	{
		on_clicked();
	}
}

UI::Button::Button(Application* app, sf::IntRect click_area, std::string text)
	:
	m_app(app)
{
	Clickable::m_click_area = click_area;
	m_text.setString(text);

	m_text.setFont(m_app->gui.times_new_roman());
	m_text.setPosition({ (float)m_click_area.left, (float)m_click_area.top });
}
void UI::Button::on_clicked()
{
	func(m_app);
}
void UI::Button::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	sf::RectangleShape button = draw_rect;
	button.setSize({ (float)m_click_area.width, (float)m_click_area.height });
	button.setPosition({ (float)m_click_area.left, (float)m_click_area.top });
	target.draw(button, states);

	target.draw(m_text, states);
}
