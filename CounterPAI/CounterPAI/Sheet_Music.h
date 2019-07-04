#pragma once
#include "Music_Note.h"
#include <list>
#include <string>

#include <iostream>

class Sheet_Music;
std::ostream& operator<<(std::ostream& os, const Sheet_Music& sheet);
std::istream& operator>>(std::istream& os, Sheet_Music& sheet);
class Sheet_Music
{
public:
	Sheet_Music();

	int quater_bpm = 100;

	void add_note(const Music_Note note);
	void add_note(const Music_Note note, int sixteenth_distance);
	
	void change_note(Music_Note& note);
	void delete_note(Voice voice, int sixteenth_distance);

	bool bass_is_cf = true;
	std::list<Music_Note>& get_cf();
	std::list<Music_Note>& get_cp();

	std::string get_note_info(Voice voice, int sixteenth_distance);

	std::list<Music_Note> m_bass;
	//std::vector<Music_Note> m_tenor; not used atm
	//std::vector<Music_Note> m_alto; not used atm
	std::list<Music_Note> m_soprano;


	friend std::ostream& operator<<(std::ostream& os, const Sheet_Music& sheet);
	friend std::istream& operator>>(std::istream& os, Sheet_Music& sheet);
};

