#include "Keyboard.h"
#include <string>
#include <iostream>


Keyboard::Keyboard()
{
	//load all keys, midi code: 21 to 121
	for (int i = 21; i <= 121; i++)
	{
		sf::SoundBuffer buffer;
		if (!buffer.loadFromFile("data/sounds/" + std::to_string(i) + ".wav"))
		{
			std::cout << "could not load data/sounds/" + std::to_string(i) + ".wav\n";
			char df;
			std::cin >> df;
			std::exit(-1);
		}
		m_key_sounds[i] = buffer;
	}
}

void Keyboard::play(Music_Note note)
{
	sf::Sound* voice = &m_bass; //just to avoid nullptr
	switch (note.m_voice)
	{
	case Voice::Bass: voice = &m_bass; break;
	case Voice::Soprano: voice = &m_soprano; break;
	default: std::cerr << "invalid Voice\n"; break;
	}

	int midi_key = note.get_midi_key();
	if (midi_key < 21 || midi_key > 121)
	{
		std::cerr << "invalid key: " << midi_key << "\n"; return;
	}
	std::cout << "\nPlaying: midi: " << midi_key << "\n";
	voice->stop();
	voice->setBuffer(m_key_sounds[midi_key]);
	voice->play();
}

void Keyboard::stop(Music_Note note)
{
	sf::Sound* voice = &m_bass; //just to avoid nullptr
	switch (note.m_voice)
	{
	case Voice::Bass: voice = &m_bass; break;
	case Voice::Soprano: voice = &m_soprano; break;
	default: std::cerr << "invalid Voice\n"; break;
	}
	std::cout << "\nStoping voice\n";
	voice->stop();
}
