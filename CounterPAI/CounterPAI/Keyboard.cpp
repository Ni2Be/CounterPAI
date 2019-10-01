#pragma once
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
			std::cerr << "could not load data/sounds/" + std::to_string(i) + ".wav\n";
			char df;
			std::cin >> df;
			std::exit(-1);
		}
		m_key_sounds[i] = buffer;
		m_keys[i].setBuffer(m_key_sounds[i]);
		m_keys_pressed[i] = 0;
	}
}

void Keyboard::play(Music_Note note)
{
	int midi_key = note.get_midi_key();
	if (midi_key < 21 || midi_key > 121)
	{
		std::cerr << "invalid key: " << midi_key << "\n"; 
		return;
	}
	//std::cout << "\nplay: " << note.m_pitch;
	m_keys[midi_key].stop();
	m_keys[midi_key].play();
	m_keys_pressed[midi_key]++;
}

void Keyboard::stop(Music_Note note)
{
	int midi_key = note.get_midi_key();
	if (midi_key < 21 || midi_key > 121)
	{
		std::cerr << "invalid key: " << midi_key << "\n";
		return;
	}

	//std::cout << "\nstop: " << note.m_pitch;
	m_keys_pressed[midi_key]--;
	if (m_keys_pressed[midi_key] <= 0)
	{
		m_keys[midi_key].stop();
		m_keys_pressed[midi_key] = 0;
	}
}

void Keyboard::stop()
{
	for (auto& key : m_keys)
		key.second.stop();
	for (auto& key_pre : m_keys_pressed)
		key_pre.second = 0;
}
