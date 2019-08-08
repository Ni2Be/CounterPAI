#pragma once

#include <SFML/Audio.hpp>
#include <map>
#include "Music_Note.h"

class Keyboard
{
public:
	Keyboard();

	void play(Music_Note note);
	void stop(Music_Note note);
	void stop();
private:
	//<midi number, sound>
	std::map<int, sf::SoundBuffer> m_key_sounds;
	std::map<int, sf::Sound> m_keys;
	std::map<int, int> m_keys_pressed;
	sf::Sound m_bass;
	//sf::Sound m_tenor; not used atm
	//sf::Sound m_alto; not used atm
	sf::Sound m_soprano;

};

