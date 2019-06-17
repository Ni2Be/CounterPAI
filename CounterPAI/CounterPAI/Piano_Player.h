#pragma once
#include <queue>
#include <chrono>

#include "Sheet_Music.h"
#include "Keyboard.h"



class Piano_Player
{
public:
	Piano_Player();
	
	void set_quater_bpm(int bpm);
	void update();
	void play_sheet_music(Sheet_Music sheet);
	void stop();
private:
	Keyboard keyboard;
	std::chrono::milliseconds whole_note_duration = std::chrono::milliseconds(2400);

	using time_point = std::chrono::system_clock::time_point;
	//first: time when the sound should start/stop, second: Note to stop
	using note_prio = std::pair<time_point, Music_Note>;

	struct Note_Prio_Comp
	{
		bool operator()(const note_prio& lhs, const note_prio& rhs)
		{
			return lhs.first > rhs.first;
		}
	};

	std::priority_queue<note_prio,
		std::vector<note_prio>,
		Note_Prio_Comp> m_stop_playing_q;

	std::priority_queue<note_prio,
		std::vector<note_prio>,
		Note_Prio_Comp> m_start_playing_q;

	void fill_in_start_q(time_point first_note_time, std::vector<Music_Note> line);
	std::vector<note_prio> m_sheet_notes;
};

