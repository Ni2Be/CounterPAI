#pragma once

#include <MidiFile.h>

#include <vector>
#include <iostream>
#include <iomanip>

#include "Sheet_Music.h"

//converts midi files to sheet music
//the midi file has to be recorded at 100 bpm
//and threre must be 2 distinct voices playing the whole
//song.
//if there are more then 2 voices, all notes between the
//top and bottom note will be cut
class Midi_Reader
{
public:
	Midi_Reader(const std::string& file_name, int track = 0);

	friend std::ostream& operator<<(std::ostream &out, const Midi_Reader& midi_reader);
	Sheet_Music sheet;
private:
	void convert_to_sheet(int track);

	using start_end_note = std::pair<std::pair<double, double>, Music_Note>;
	void parse_value_pitch(std::vector<start_end_note>& all_notes, int track);
	void set_voice(std::vector<start_end_note>& all_notes, int track);

	std::string m_file_name;
	smf::MidiFile m_midifile;
	int m_tracks;
};

