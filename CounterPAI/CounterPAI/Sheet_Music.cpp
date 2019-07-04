#include "Sheet_Music.h"

#include <string>
#include <sstream>
#include <regex>

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

enum class find_note_flag
{
	WAS_BETWEEN_NOTES,
	WAS_AT_NOTE,
	WAS_AFTER_LAST_NOTE
};

find_note_flag find_note_position(
	std::list<Music_Note>& voice, 
	std::list<Music_Note>::iterator& pos, 
	int& sixteenth_distance)
{
	pos = voice.begin();
	if (sixteenth_distance == 0)
	{
		std::cout << "\nwas at!";
		return find_note_flag::WAS_AT_NOTE;
	}
	for (;
		pos != voice.end();
		pos++)
	{
		sixteenth_distance -= 16 / static_cast<int>(pos->m_value);
		if (sixteenth_distance == 0)
		{
			pos++;
			std::cout << "\nwas at!";
			return find_note_flag::WAS_AT_NOTE;
		}
		if (sixteenth_distance < 0)
		{
			sixteenth_distance += 16 / static_cast<int>(pos->m_value);
			pos++;
			std::cout << "\nwas between!";
			return find_note_flag::WAS_BETWEEN_NOTES;
		}
	}
	std::cout << "\nwas after!";
	return find_note_flag::WAS_AFTER_LAST_NOTE;

}

//TODO support eight and sixteenth note cases
void arrange_notes(
	std::list<Music_Note>& voice, 
	std::list<Music_Note>::iterator& predecessor, 
	std::list<Music_Note>::iterator& new_note,
	int distance_to_predecessor)
{
	std::cout << "\ndist to pre: " << distance_to_predecessor;

	switch (distance_to_predecessor)
	{
	case 16: predecessor->m_value = Note_Value::Whole; break;
	case 12: 
		predecessor->m_value = Note_Value::Quarter;
		voice.insert(predecessor, Music_Note(predecessor->m_pitch, Note_Value::Halfe, predecessor->m_voice, predecessor->m_is_tied));
		predecessor->m_is_tied = true;
		break;
	case 8: predecessor ->m_value = Note_Value::Halfe; break;
	case 4: predecessor ->m_value = Note_Value::Quarter;break;
	case 2: predecessor ->m_value = Note_Value::Eighth; break;
	case 1: predecessor ->m_value = Note_Value::Sixteenth; break;
	default: std::cout << "\ninvalid dist\n"; break;
	}

	new_note++;
	while (new_note != voice.end() && new_note->m_is_tied)
		new_note = voice.erase(new_note);

}



void insert_note(std::list<Music_Note>& voice, const Music_Note new_note, int sixteenth_distance)
{
	std::cout << "\ninserting.. dist: " << sixteenth_distance << "\n";

	//find position in list
	std::list<Music_Note>::iterator note;
	int position_to_pre = sixteenth_distance;
	find_note_flag note_pos = find_note_position(voice, note, position_to_pre);

	std::cout << "\nfind_note_position dist: " << sixteenth_distance << "\n";
	std::cout << "\nfind_note_position dist: " << position_to_pre << "\n";

	//inserting between two notes, cut predecessor, stretch or cut new note
	if (note_pos == find_note_flag::WAS_BETWEEN_NOTES)
	{
		note--;
		std::list<Music_Note>::iterator predecessor = note;
		note++;
		note = voice.insert(note, new_note);
		std::list<Music_Note>::iterator new_note_itr = note;
		note++;
		std::list<Music_Note>::iterator successor = note;
		arrange_notes(voice, predecessor, new_note_itr, position_to_pre);
	}
	//inserting after the last note has ended, filling empty space
	else if (note_pos == find_note_flag::WAS_AFTER_LAST_NOTE)
	{

		auto fill_in = [&voice, &note, new_note, &position_to_pre](int dist, Note_Value val)
		{
			if (position_to_pre >= dist && voice.empty())
			{
				position_to_pre -= dist;
				voice.push_back(new_note);
				note = voice.end();
			}		
			note--;
			std::list<Music_Note>::iterator predecessor = note;
			note++;
			for (; position_to_pre >= dist; position_to_pre -= dist)
			{
				std::cout << "\nnote pitch: " << static_cast<int>(new_note.m_pitch);
				std::cout << "\npre pitch: " << static_cast<int>( predecessor->m_pitch);
				voice.push_back(Music_Note(predecessor->m_pitch, val, new_note.m_voice, true));
			}
		};
		fill_in(16, Note_Value::Whole);
		fill_in(8, Note_Value::Halfe);
		fill_in(4, Note_Value::Quarter);
		std::cout << "\nNEW DIST" << sixteenth_distance;
		voice.push_back(new_note);
	}
	//note at the end of the list, insert it,
	//or there is already a Note at that spot, change it
	else if (note_pos == find_note_flag::WAS_AT_NOTE)
	{
		if (voice.empty() || note == voice.end())
		{
			voice.push_back(new_note);
			return;
		}
		*note = new_note;
		note++;
		while (note != voice.end() && note->m_is_tied)
			note = voice.erase(note);
	}

	//set all tied notes pitches
	std::list<Music_Note>::iterator suc_note = voice.begin();
	suc_note++;
	std::list<Music_Note>::iterator note_note = voice.begin();
	while (suc_note != voice.end())
	{
		if (suc_note->m_is_tied)
			suc_note->m_pitch = note_note->m_pitch;
		note_note++;
		suc_note++;
	}
}

void Sheet_Music::add_note(const Music_Note note, int sixteenth_distance)
{

	std::cout << "\n..adding";
	switch (note.m_voice)
	{
	case Voice::Bass: insert_note(m_bass, note, sixteenth_distance); break;
	case Voice::Soprano: insert_note(m_soprano, note, sixteenth_distance); break;
	default: std::cerr << "invalid voice\n"; break;
	}
}

void Sheet_Music::change_note(Music_Note& note)
{
	std::cerr << "TODO change_note\n";
}

void erase_note(std::list<Music_Note>& voice, int sixteenth_distance)
{
	//find position in list
	std::list<Music_Note>::iterator note;
	int position_to_pre = sixteenth_distance;
	find_note_flag note_pos = find_note_position(voice, note, position_to_pre);

	if (note_pos == find_note_flag::WAS_AT_NOTE)
	{
		std::cout << "\ndeleting";
		if (voice.empty())
			return;
		voice.erase(note);
	}
}

void Sheet_Music::delete_note(Voice voice, int sixteenth_distance)
{
	switch (voice)
	{
	case Voice::Bass: erase_note(m_bass, sixteenth_distance); break;
	case Voice::Soprano: erase_note(m_soprano, sixteenth_distance); break;
	default: std::cerr << "invalid voice\n"; break;
	}
}


std::string get_info(std::list<Music_Note>& voice, int sixteenth_distance)
{
	//find position in list
	std::list<Music_Note>::iterator note;
	int position_to_pre = sixteenth_distance;
	find_note_flag note_pos = find_note_position(voice, note, position_to_pre);

	if (note_pos == find_note_flag::WAS_AT_NOTE)
	{
		std::cout << "\ngettin info";
		if (voice.empty())
			return "";

		std::string note_info = note->m_note_info;
		std::replace(note_info.begin(), note_info.end(), '\t', '\n');
		std::replace(note_info.begin(), note_info.end(), ':', '\n');
		std::replace(note_info.begin(), note_info.end(), ',', ' ');
		
		return  note_info + "\n\nProbability\n" + std::to_string(note->m_probability);
	}
	return "";
}

std::string Sheet_Music::get_note_info(Voice voice, int sixteenth_distance)
{
	switch (voice)
	{
	case Voice::Bass: return get_info(m_bass, sixteenth_distance); break;
	case Voice::Soprano: return get_info(m_soprano, sixteenth_distance); break;
	default: std::cerr << "invalid voice\n"; break;
	}
}

Music_Note get_note_in_voice(std::list<Music_Note>& voice, int sixteenth_distance)
{
	//find position in list
	std::list<Music_Note>::iterator note;
	int position_to_pre = sixteenth_distance;
	find_note_flag note_pos = find_note_position(voice, note, position_to_pre);

	if (note_pos == find_note_flag::WAS_AT_NOTE)
	{
		std::cout << "\ngettin info";
		if (voice.empty())
			return Music_Note();

		return *note;
	}
	return Music_Note();
}

Music_Note Sheet_Music::get_note(Voice voice, int sixteenth_distance)
{
	switch (voice)
	{
	case Voice::Bass: return get_note_in_voice(m_bass, sixteenth_distance); break;
	case Voice::Soprano: return get_note_in_voice(m_soprano, sixteenth_distance); break;
	default: std::cerr << "invalid voice\n"; break;
	}
}

std::list<Music_Note>& Sheet_Music::get_cf()
{
	if (bass_is_cf)
		return m_bass;
	else
		return m_soprano;
}

std::list<Music_Note>& Sheet_Music::get_cp()
{
	if (bass_is_cf)
		return m_soprano;
	else
		return m_bass;
}


std::ostream& operator<<(std::ostream& os, const Sheet_Music& sheet)
{
	std::string temp;

	os << "tempo:\n";
	os << sheet.quater_bpm << "\n";
	os << "cf:\n";
	if (sheet.bass_is_cf)
		os << "bass\n";
	else
		os << "soprano\n";
	
	os << "soprano: \n";
	for (const auto& note : sheet.m_soprano)
		os << note;
	os << "bass: \n";
	for (const auto& note : sheet.m_bass)
		os << note;
	os << "end\n";
	return os;
}

std::istream& operator>>(std::istream& is, Sheet_Music& sheet)
{
	std::string temp;

	is >> temp; //eat "tempo:"
	is >> sheet.quater_bpm;
	is >> temp; //eat "cf:"
	is >> temp;
	if (temp == "bass")
		sheet.bass_is_cf = true;
	else
		sheet.bass_is_cf = false;
	is >> temp; //eat "soprano:"
	std::list<Music_Note>* voice = &sheet.m_soprano;
	sheet.m_soprano.clear();
	sheet.m_bass.clear();

	std::cout << "\nstart\n" << temp << "\n";
	while (is)
	{
		is >> temp; 
		if (temp == "bass:")
		{
			voice = &sheet.m_bass;
			std::cout << "\nstart\n" << temp << "\n";
			continue;
		}
		if (temp == "end")
			break;
		Music_Note temp_note;
		is >> temp_note;
		voice->push_back(temp_note);
	}
}