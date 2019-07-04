#include "Music_Note.h"


Music_Note::Music_Note()
	:
	m_pitch(Note_Pitch::C1),
	m_value(Note_Value::Sixteenth),
	m_voice(Voice::Bass),
	m_is_tied(true),
	m_is_sharp(true),
	m_is_flat(true)
{};

Music_Note::Music_Note(Note_Pitch pitch, Note_Value value, Voice voice)
	:
	m_pitch(pitch),
	m_value(value),
	m_voice(voice)
{
}

Music_Note::Music_Note(Note_Pitch pitch, Note_Value value, Voice voice, bool is_tied)
	:
	m_pitch(pitch),
	m_value(value),
	m_voice(voice),
	m_is_tied(is_tied)
{
}

Music_Note::Music_Note(Note_Pitch pitch, Note_Value value, Voice voice, bool is_tied, bool is_sharp, bool is_flat)
	:
	m_pitch(pitch),
	m_value(value),
	m_voice(voice),
	m_is_tied(is_tied),
	m_is_sharp(is_sharp),
	m_is_flat(is_flat)
{
}

Note_Pitch Music_Note::get_basic_note()
{
	if (m_is_flat)
		return static_cast<Note_Pitch>(static_cast<int>(m_pitch) + 1);
	if (m_is_sharp)
		return static_cast<Note_Pitch>(static_cast<int>(m_pitch) - 1);
	return m_pitch;
}

int Music_Note::get_midi_key()
{
	return static_cast<int>(m_pitch);
}


std::ostream& operator<<(std::ostream& os, const Music_Note& note)
{
	os << note.m_pitch << " p: " << (int)note.m_pitch << " va: " << (int)note.m_value << " vo: " << (int)note.m_voice << " f: " << note.m_is_flat << " s: " << note.m_is_sharp << " t: " << note.m_is_tied << " ;\n";
	return os;
}

std::istream& operator>>(std::istream& is, Music_Note& note)
{
	std::string temp;
	
	is >> temp; //eat p:
	if (temp != "p:")
		is >> temp; //eat pitch (like C4) if still there
	std::cout << temp;
	int temp_int;
	std::cout << temp;
	is >> temp_int;
	std::cout << temp;
	note.m_pitch = static_cast<Note_Pitch>(temp_int);
	std::cout << temp;
	is >> temp; // eat va:
	std::cout << temp;
	is >> temp_int;
	std::cout << temp;
	note.m_value = static_cast<Note_Value>(temp_int);
	std::cout << temp;
	is >> temp; // eat vo:
	std::cout << temp;
	is >> temp_int;
	note.m_voice = static_cast<Voice>(temp_int);
	is >> temp; // eat f:
	std::cout << temp;
	is >> temp_int;
	note.m_is_flat = temp_int;
	is >> temp; // eat s:
	std::cout << temp;
	is >> temp_int;
	note.m_is_sharp = temp_int;
	is >> temp; // eat t:
	is >> temp_int;
	note.m_is_tied = static_cast<bool>(temp_int);
	is >> temp; // eat ;
	
	std::cout << temp << "\n";
	return is;
}

Note_Pitch_Enum_Gen::Note_Pitch_Enum_Gen()
{
	{
		std::cout << "A0 = 21, Asharp0, B0,\n";
		for (int o = 1; o < 9; o++)
		{
			for (char note = 'C'; note < 'H'; note++)
			{
				std::cout << note << o << ", ";
				if (note != 'B' && note != 'E')
					std::cout << note << "sharp" << o << ", ";
			}
			for (char note = 'A'; note < 'C'; note++)
			{
				std::cout << note << o << ", ";
				if (note != 'B' && note != 'E')
					std::cout << note << "sharp" << o << ", ";
			}
			std::cout << "\n";
		}
		std::cout << "C9, Csharp9";
	}
}