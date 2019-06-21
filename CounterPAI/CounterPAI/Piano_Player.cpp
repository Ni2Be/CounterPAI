#include "Piano_Player.h"


Piano_Player::Piano_Player()
{
}


void Piano_Player::set_quater_bpm(int bpm)
{
	whole_note_duration = std::chrono::milliseconds((60000 / bpm) * 4);
}

void Piano_Player::play_sheet_music(Sheet_Music sheet)
{
	set_quater_bpm(sheet.quater_bpm);

	time_point first_note_time = std::chrono::system_clock::now();

	fill_in_event_q(first_note_time, sheet.m_bass);
	fill_in_event_q(first_note_time, sheet.m_soprano);
}

void Piano_Player::fill_in_event_q(time_point first_note_time, std::list<Music_Note> line)
{
	time_point line_offset = first_note_time;
	for (Music_Note note : line)
	{
		//push note play event
		m_playing_event_q.push(
			Note_Event(line_offset, Note_Event::Event::PLAY, note));
		//move line_offset to end position of note
		line_offset += std::chrono::milliseconds(whole_note_duration.count()
			/ static_cast<int>(note.m_value));
		//push note stop event
		m_playing_event_q.push(
			Note_Event(line_offset, Note_Event::Event::STOP, note));
	}
}

void Piano_Player::stop()
{
	while (!m_playing_event_q.empty())
	{
		if(m_playing_event_q.top().event == Note_Event::Event::STOP)
			keyboard.stop(m_playing_event_q.top().note);
		m_playing_event_q.pop();
	}
}

void Piano_Player::update()
{	
	//process events
	while (
		!m_playing_event_q.empty() &&
		m_playing_event_q.top().when < std::chrono::system_clock::now())
	{
		switch (m_playing_event_q.top().event)
		{
		case Note_Event::Event::PLAY: keyboard.play(m_playing_event_q.top().note); break;
		case Note_Event::Event::STOP: keyboard.stop(m_playing_event_q.top().note); break;
		default: std::cerr << "invalid Note_Event\n"; break;
		}
		m_playing_event_q.pop();
	}
}