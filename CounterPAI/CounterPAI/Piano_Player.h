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
	//plays notes
	Keyboard keyboard;

	//fancy Event queue stuff for when to start/end a note 
	std::chrono::milliseconds whole_note_duration = std::chrono::milliseconds(2400);
	using time_point = std::chrono::system_clock::time_point;
	//A Note_Event is defined by if it's a play or stop event, when it hapens, and which
	//note is effected
	struct Note_Event
	{
		enum class Event
		{ PLAY, STOP };
		Note_Event(time_point w, Event e, Music_Note n) : when(w), event(e), note(n) {}
		time_point when;
		Event event;
		Music_Note note;
	};
	//returns bigger for the Note_Event that is futher in the future, or bigger for a PLAY
	//Event if two Events are at the same time (to ensure a note is allways stop first and 
	//the next note is played)
	struct Note_Event_Comp
	{bool operator()(const Note_Event& lhs, const Note_Event& rhs)
	{return (lhs.when == rhs.when)? ((lhs.event == Note_Event::Event::STOP)? false:true) :lhs.when > rhs.when;}};
	
	std::priority_queue<Note_Event,
		std::vector<Note_Event>,
		Note_Event_Comp> m_playing_event_q;

	void fill_in_event_q(time_point first_note_time, std::vector<Music_Note> line);
};