#include "Sheet_Music.h"



Sheet_Music::Sheet_Music()
{
}

void Sheet_Music::add_note(const Music_Note note)
{
	switch (note.m_voice)
	{
	case Voice::Bass: m_bass.push_back(note); break;
	case Voice::Soprano: m_soprano.push_back(note); break;
	default: std::cerr << "invalid voice\n"; break;
	}
}

void Sheet_Music::change_note(Music_Note& note)
{
	std::cerr << "TODO change_note\n";
}

void Sheet_Music::delete_note(Music_Note& note)
{
	std::cerr << "TODO delete_note\n";
}