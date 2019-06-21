#pragma once
#include "Music_Note.h"
#include <list>



class Sheet_Music
{
public:
	Sheet_Music();

	int quater_bpm = 90;

	void add_note(const Music_Note note);
	void change_note(Music_Note& note);
	void delete_note(Music_Note& note);

	std::list<Music_Note> m_bass;
	//std::vector<Music_Note> m_tenor; not used atm
	//std::vector<Music_Note> m_alto; not used atm
	std::list<Music_Note> m_soprano;
};

