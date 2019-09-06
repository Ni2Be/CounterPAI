#include "Music_Note.h"

#include <rapidjson/istreamwrapper.h>
#include <rapidjson/writer.h>

#include "Utility.h"

Music_Note::Music_Note()
	:
	m_pitch(Note_Pitch::C1),
	m_value(Note_Value::Sixteenth),
	m_voice(Voice::Bass),
	m_is_tied(true),
	m_is_sharp(true),
	m_is_flat(true)
{
	m_note_info_->SetObject();
};

Music_Note::Music_Note(Note_Pitch pitch, Note_Value value, Voice voice)
	:
	m_pitch(pitch),
	m_value(value),
	m_voice(voice)
{
	m_note_info_->SetObject();
}

Music_Note::Music_Note(Note_Pitch pitch, Note_Value value, Voice voice, bool is_tied)
	:
	m_pitch(pitch),
	m_value(value),
	m_voice(voice),
	m_is_tied(is_tied)
{
	m_note_info_->SetObject();
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
	m_note_info_->SetObject();
}

void Music_Note::add_note_info(std::string sender, std::string message)
{
	using namespace rapidjson;
	Value::MemberIterator itr = m_note_info_->FindMember(sender);
	if (itr == m_note_info_->MemberEnd())
	{
		Value new_value(sender, m_note_info_->GetAllocator());
		Value m(message, m_note_info_->GetAllocator());
		m_note_info_->AddMember(new_value, m, m_note_info_->GetAllocator());
	}
	else
	{
		Value::StringRefType m(message.c_str());
		(*m_note_info_)[sender.c_str()].SetString(m);
	}
}

std::string Music_Note::get_note_info(std::string index)
{
	using namespace rapidjson;
	Value::MemberIterator itr = m_note_info_->FindMember(index.c_str());
	if (itr == m_note_info_->MemberEnd())
	{
		return "null";
	}
	return std::string(itr->value.GetString());
}

void Music_Note::clear_note_info()
{
	m_note_info_->RemoveAllMembers();
}


Note_Pitch Music_Note::get_ACscale_pitch(Note_Pitch lowest_note, int distance)
{
	
	if (lowest_note == Note_Pitch::C2)
	{
		switch (distance)
		{
		case 0:  return Note_Pitch::C2;
		case 1:  return Note_Pitch::D2;
		case 2:  return Note_Pitch::E2;
		case 3:  return Note_Pitch::F2;
		case 4:  return Note_Pitch::G2;
		case 5:  return Note_Pitch::A2;
		case 6:  return Note_Pitch::B2;
		case 7:  return Note_Pitch::C3;
		case 8:  return Note_Pitch::D3;
		case 9:  return Note_Pitch::E3;
		case 10: return Note_Pitch::F3;
		case 11: return Note_Pitch::G3;
		case 12: return Note_Pitch::A3;
		case 13: return Note_Pitch::B3;
		case 14: return Note_Pitch::C4;
		case 15: return Note_Pitch::D4;
		case 16: return Note_Pitch::E4;
		default: std::cerr << "invalid bass Note\n"; break;
		}
	}
	else if (lowest_note == Note_Pitch::A3)
	{
		switch (distance)
		{
		case 0:  return Note_Pitch::A3;
		case 1:  return Note_Pitch::B3;
		case 2:  return Note_Pitch::C4;
		case 3:  return Note_Pitch::D4;
		case 4:  return Note_Pitch::E4;
		case 5:  return Note_Pitch::F4;
		case 6:  return Note_Pitch::G4;
		case 7:  return Note_Pitch::A4;
		case 8:  return Note_Pitch::B4;
		case 9:  return Note_Pitch::C5;
		case 10: return Note_Pitch::D5;
		case 11: return Note_Pitch::E5;
		case 12: return Note_Pitch::F5;
		case 13: return Note_Pitch::G5;
		case 14: return Note_Pitch::A5;
		case 15: return Note_Pitch::B5;
		case 16: return Note_Pitch::C6;
		default: std::cerr << "invalid soprano Note\n"; break;
		}
	}
	else
	{
		std::cerr << "\nget_ACscale_pitch() only supports lowest_note: A3 or C2";
	}
	return Note_Pitch::A0;
}


int Music_Note::get_ACscale_distance(Note_Pitch lowest_note, Note_Pitch note) 
{
	if (lowest_note == Note_Pitch::C2)
	{
		switch (note)
		{
		case Note_Pitch::C2: return 0;
		case Note_Pitch::D2: return 1;
		case Note_Pitch::E2: return 2;
		case Note_Pitch::F2: return 3;
		case Note_Pitch::G2: return 4;
		case Note_Pitch::A2: return 5;
		case Note_Pitch::B2: return 6;
		case Note_Pitch::C3: return 7;
		case Note_Pitch::D3: return 8;
		case Note_Pitch::E3: return 9;
		case Note_Pitch::F3: return 10;
		case Note_Pitch::G3: return 11;
		case Note_Pitch::A3: return 12;
		case Note_Pitch::B3: return 13;
		case Note_Pitch::C4: return 14;
		case Note_Pitch::D4: return 15;
		case Note_Pitch::E4: return 16;
		default: std::cerr << "invalid bass Note\n"; break;
		}
	}
	else if (lowest_note == Note_Pitch::A3)
	{
		switch (note)
		{
		case Note_Pitch::A3: return 0;
		case Note_Pitch::B3: return 1;
		case Note_Pitch::C4: return 2;
		case Note_Pitch::D4: return 3;
		case Note_Pitch::E4: return 4;
		case Note_Pitch::F4: return 5;
		case Note_Pitch::G4: return 6;
		case Note_Pitch::A4: return 7;
		case Note_Pitch::B4: return 8;
		case Note_Pitch::C5: return 9;
		case Note_Pitch::D5: return 10;
		case Note_Pitch::E5: return 11;
		case Note_Pitch::F5: return 12;
		case Note_Pitch::G5: return 13;
		case Note_Pitch::A5: return 14;
		case Note_Pitch::B5: return 15;
		case Note_Pitch::C6: return 16;
		default: std::cerr << "invalid soprano Note: " << (int)note << " \n"; break;
		}
	}
	else
	{
		std::cerr << "\nget_ACscale_distance() only supports lowest_note: A3 or C2";
	}
	return -1;
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
	os << note.m_pitch
		<< " p: " << (int)note.m_pitch
		<< " va: " << (int)note.m_value
		<< " vo: " << (int)note.m_voice
		<< " f: " << note.m_is_flat
		<< " s: " << note.m_is_sharp
		<< " t: " << note.m_is_tied;
		//<< " ;\n";
	
	using namespace rapidjson;
	StringBuffer strbuf;
	strbuf.Clear();
	Writer<StringBuffer> writer(strbuf);
	note.m_note_info_->Accept(writer);
	std::string note_info_string(strbuf.GetString());

	if (note_info_string.size() > Music_Note::C_MAX_NOTE_INFO_STING_SIZE)
		std::cerr << "\nnote info string to long, save file will be corrupted!";
	//std::replace(note_info_string.begin(), note_info_string.end(), '\n', ' ');

	os << " | " << note_info_string << "\n";

	return os;
}

std::istream& operator>>(std::istream& is, Music_Note& note)
{
	std::string temp;
	
	is >> temp; //eat p:
	if (temp != "p:")
		is >> temp; //eat pitch (like C4) if still there
	int temp_int;
	is >> temp_int;
	note.m_pitch = static_cast<Note_Pitch>(temp_int);
	is >> temp; // eat va:
	is >> temp_int;
	note.m_value = static_cast<Note_Value>(temp_int);
	is >> temp; // eat vo:
	is >> temp_int;
	note.m_voice = static_cast<Voice>(temp_int);
	is >> temp; // eat f:
	is >> temp_int;
	note.m_is_flat = temp_int;
	is >> temp; // eat s:
	is >> temp_int;
	note.m_is_sharp = temp_int;
	is >> temp; // eat t:
	is >> temp_int;
	note.m_is_tied = static_cast<bool>(temp_int);

	is >> temp; // eat |

	char buffer[Music_Note::C_MAX_NOTE_INFO_STING_SIZE];
	is.getline(buffer, Music_Note::C_MAX_NOTE_INFO_STING_SIZE);
	note.m_note_info_->Parse(buffer);

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