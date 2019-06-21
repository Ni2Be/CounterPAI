#include "Clickable.h"
#include <iostream>
#include "Eventsystem.h"

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
