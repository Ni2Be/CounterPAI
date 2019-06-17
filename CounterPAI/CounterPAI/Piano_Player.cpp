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

	fill_in_start_q(first_note_time, sheet.m_bass);
	fill_in_start_q(first_note_time, sheet.m_soprano);
}

void Piano_Player::fill_in_start_q(time_point first_note_time, std::vector<Music_Note> line)
{
	time_point line_offset = first_note_time;
	for (Music_Note note : line)
	{
		m_start_playing_q.push(
			note_prio(line_offset,
				note));
		line_offset += std::chrono::milliseconds(whole_note_duration.count()
			/ static_cast<int>(note.m_value));
	}
}

void Piano_Player::stop()
{
	while (!m_start_playing_q.empty())
		m_start_playing_q.pop();
	while (!m_stop_playing_q.empty())
	{
		keyboard.stop(m_stop_playing_q.top().second);
		m_stop_playing_q.pop();
	}
}

void Piano_Player::update()
{
	//check if a note should start to play
	while (
		!m_start_playing_q.empty() &&
		m_start_playing_q.top().first < std::chrono::system_clock::now())
	{	
		Music_Note note = m_start_playing_q.top().second;
		keyboard.play(note);
		m_stop_playing_q.push(
			note_prio(std::chrono::system_clock::now() 
					+ std::chrono::milliseconds(whole_note_duration.count() 
						/ static_cast<int>(note.m_value)), 
				note));
		m_start_playing_q.pop();
	}

	//check if a note has to stop to play
	//while (
	//	!m_stop_playing_q.empty() &&
	//	m_stop_playing_q.top().first > std::chrono::system_clock::now())
	//{		
	//	keyboard.stop(m_stop_playing_q.top().second);
	//	m_stop_playing_q.pop();
	//}
}