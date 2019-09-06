#pragma once
#include <iostream>
#include <string>
#include <array>
#include <memory>

#ifndef RAPIDJSON_HAS_STDSTRING
#define RAPIDJSON_HAS_STDSTRING 1
#endif
#include <rapidjson/document.h>


enum class Note_Pitch;
enum class Note_Value;
enum class Voice;

class Music_Note;
std::ostream& operator<<(std::ostream& os, const Music_Note& note);
std::istream& operator>>(std::istream& is, Music_Note& note);
class Music_Note
{
public:
	Music_Note();
	Music_Note(Note_Pitch pitch, Note_Value value, Voice voice);
	Music_Note(Note_Pitch pitch, Note_Value value, Voice voice, bool is_tied);
	Music_Note(Note_Pitch pitch, Note_Value value, Voice voice, bool is_tied, bool is_sharp, bool is_flat);

	void add_note_info(std::string index, std::string message);
	std::string get_note_info(std::string index);
	void clear_note_info();

	static Note_Pitch get_ACscale_pitch(Note_Pitch lowest_note, int distance);
	static int get_ACscale_distance(Note_Pitch lowest_note, Note_Pitch note);

	Note_Pitch get_basic_note();
	int get_midi_key();

	Note_Pitch m_pitch;
	Note_Value m_value;
	Voice m_voice;
	bool m_is_tied = false; //true if a note is tied to the previous note
	bool m_is_sharp = false;
	bool m_is_flat = false;
	
	friend std::ostream& operator<<(std::ostream& os, const Music_Note& note);
	friend std::istream& operator>>(std::istream& is, Music_Note& note);
	
	static const int C_MAX_NOTE_INFO_STING_SIZE = 512;
	bool m_is_corrupted = false;

private:
	std::shared_ptr<rapidjson::Document> m_note_info_ = std::make_shared<rapidjson::Document>(rapidjson::Document());
};



enum class Note_Value
{
	Whole = 1, Halfe = 2, Quarter = 4, Eighth = 8, Sixteenth = 16
};

enum class Voice
{
	Bass, 
	//Tenor, not used atm
	//Alto, not used atm
	Soprano
};

enum class Note_Pitch : int
{
	A0 = 21, Asharp0, B0,
	C1, Csharp1, D1, Dsharp1, E1, F1, Fsharp1, G1, Gsharp1, A1, Asharp1, B1,
	C2, Csharp2, D2, Dsharp2, E2, F2, Fsharp2, G2, Gsharp2, A2, Asharp2, B2,
	C3, Csharp3, D3, Dsharp3, E3, F3, Fsharp3, G3, Gsharp3, A3, Asharp3, B3,
	C4, Csharp4, D4, Dsharp4, E4, F4, Fsharp4, G4, Gsharp4, A4, Asharp4, B4,
	C5, Csharp5, D5, Dsharp5, E5, F5, Fsharp5, G5, Gsharp5, A5, Asharp5, B5,
	C6, Csharp6, D6, Dsharp6, E6, F6, Fsharp6, G6, Gsharp6, A6, Asharp6, B6,
	C7, Csharp7, D7, Dsharp7, E7, F7, Fsharp7, G7, Gsharp7, A7, Asharp7, B7,
	C8, Csharp8, D8, Dsharp8, E8, F8, Fsharp8, G8, Gsharp8, A8, Asharp8, B8,
	C9, Csharp9
};

inline std::ostream& operator<<(std::ostream& os, const Note_Pitch& pitch)
{
	int pitch_pos = static_cast<int>(pitch) - 21;
	static const std::array<std::string, 101> pitch_str
	{
	"A0", "Asharp0", "B0",
	"C1", "Csharp1", "D1", "Dsharp1", "E1", "F1", "Fsharp1", "G1", "Gsharp1", "A1", "Asharp1", "B1",
	"C2", "Csharp2", "D2", "Dsharp2", "E2", "F2", "Fsharp2", "G2", "Gsharp2", "A2", "Asharp2", "B2",
	"C3", "Csharp3", "D3", "Dsharp3", "E3", "F3", "Fsharp3", "G3", "Gsharp3", "A3", "Asharp3", "B3",
	"C4", "Csharp4", "D4", "Dsharp4", "E4", "F4", "Fsharp4", "G4", "Gsharp4", "A4", "Asharp4", "B4",
	"C5", "Csharp5", "D5", "Dsharp5", "E5", "F5", "Fsharp5", "G5", "Gsharp5", "A5", "Asharp5", "B5",
	"C6", "Csharp6", "D6", "Dsharp6", "E6", "F6", "Fsharp6", "G6", "Gsharp6", "A6", "Asharp6", "B6",
	"C7", "Csharp7", "D7", "Dsharp7", "E7", "F7", "Fsharp7", "G7", "Gsharp7", "A7", "Asharp7", "B7",
	"C8", "Csharp8", "D8", "Dsharp8", "E8", "F8", "Fsharp8", "G8", "Gsharp8", "A8", "Asharp8", "B8",
	"C9", "Csharp9"
	};
	os << pitch_str[pitch_pos];
	return os;
};

class Note_Pitch_Enum_Gen
{
public:
	Note_Pitch_Enum_Gen();
};

