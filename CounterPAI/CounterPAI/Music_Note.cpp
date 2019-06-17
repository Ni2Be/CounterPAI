#include "Music_Note.h"

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