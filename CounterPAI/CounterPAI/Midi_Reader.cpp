#include "Midi_Reader.h"

#include <queue>


Midi_Reader::Midi_Reader(const std::string& file_name, int track)
	:
	m_file_name(file_name)
{
	m_midifile.read(file_name);
	m_midifile.doTimeAnalysis();
	m_midifile.linkNotePairs();
	m_tracks = m_midifile.getTrackCount();
	convert_to_sheet(track);
}


void Midi_Reader::convert_to_sheet(int track)
{
	std::cout << "\nConverting..\n";
	std::vector<start_end_note> all_notes;

	parse_value_pitch(all_notes, track);

	set_voice(all_notes, track);

	//Convert to Sheet
	for (auto s_e_note : all_notes)
	{
		switch (s_e_note.second.m_voice)
		{
		case Voice::Bass: sheet.m_bass.push_back(s_e_note.second); break;
		case Voice::Soprano: sheet.m_soprano.push_back(s_e_note.second); break;
		default: std::cerr << "invalid voice\n"; break;
		}
	}
}

void Midi_Reader::parse_value_pitch(std::vector<start_end_note>& all_notes, int track)
{
	Music_Note temp_note(Note_Pitch::A0, Note_Value::Whole, Voice::Bass);
	for (int event = 0; event < m_midifile[track].size(); event++)
	{
		if (m_midifile[track][event].isNoteOn())
		{
			//Note is palyed for a duration
			double duration = m_midifile[track][event].getDurationInSeconds();

			//Note Value
			double deviation = 5.0;
			if (abs(duration - 2.4) <= 2.4 / deviation) {
				temp_note.m_value = Note_Value::Whole;
				duration = 2.4;
			}
			else if (abs(duration - 1.2) <= 1.2 / deviation) {
				temp_note.m_value = Note_Value::Halfe;
				duration = 1.2;
			}
			else if (abs(duration - 0.6) <= 0.6 / deviation) {
				temp_note.m_value = Note_Value::Quarter;
				duration = 0.6;
			}
			else if (abs(duration - 0.3) <= 0.3 / deviation) {
				temp_note.m_value = Note_Value::Eighth;
				duration = 0.3;
			}
			else if (abs(duration - 0.15) <= 0.15 / deviation) {
				temp_note.m_value = Note_Value::Sixteenth;
				duration = 0.15;
			}
			else
			{
				std::cerr << "invalid Note duration: " << duration << "\n";
			}
			//Note Pitch
			temp_note.m_pitch = static_cast<Note_Pitch>((int)m_midifile[track][event][1]);
			//push
			double start = m_midifile[track][event].seconds;
			all_notes.push_back(start_end_note({ start, start + duration }, temp_note));
		}
	}
}

void Midi_Reader::set_voice(std::vector<start_end_note>& all_notes, int track)
{
	std::sort(std::begin(all_notes), std::end(all_notes),
		[](const start_end_note& lhs, const start_end_note& rhs)
	{ return lhs.first.first < rhs.first.first; });
	for (int i = 0; i < all_notes.size(); i++)
	{
		std::cout << "note:" << (int)all_notes[i].second.m_pitch << ", ";
		std::cout << "start: " << all_notes[i].first.first << ", end: " << all_notes[i].first.second << std::endl; 
		//TODO check for union starts
		//find the soprano notes
		for(int other = 0; other < all_notes.size(); other++)
		{
			if (all_notes[i].first.second > all_notes[other].first.first + 0.1
				&& all_notes[i].first.first < all_notes[other].first.second - 0.1)
			{
				std::cout << "other: " <<(int)all_notes[other].second.m_pitch << ", ";
				std::cout << "start: " << all_notes[other].first.first << ", end: " << all_notes[other].first.second << std::endl;
				if (all_notes[i].second.m_pitch > all_notes[other].second.m_pitch)
				{
					std::cout << "Soprano\n";
					all_notes[i].second.m_voice = Voice::Soprano;
				}
				else
				{
					std::cout << "Bass\n";
				}
			}
		}
	}
}


std::ostream& operator<<(std::ostream &out, const Midi_Reader& midi_reader)
{
	out << std::setprecision(4) << "TPQ: " << midi_reader.m_midifile.getTicksPerQuarterNote() << "\n";
	if (midi_reader.m_tracks > 1) out << "TRACKS: " << midi_reader.m_tracks << "\n";
	for (int track = 0; track < midi_reader.m_tracks; track++) {
		if (midi_reader.m_tracks > 1) out << "\nTrack " << track << "\n";
		out << "Tick\tSeconds\tDur\tMessage" << "\n";
		for (int event = 0; event < midi_reader.m_midifile[track].size(); event++) {
			out << std::dec << midi_reader.m_midifile[track][event].tick;
			out << '\t' << std::dec << midi_reader.m_midifile[track][event].seconds;
			out << '\t';
			if (midi_reader.m_midifile[track][event].isNoteOn())
				out << midi_reader.m_midifile[track][event].getDurationInSeconds();
			out << '\t';

			for (int i = 0; i < midi_reader.m_midifile[track][event].size(); i++)
				out << (int)midi_reader.m_midifile[track][event][i] << ' ';
			out << "\n";
		}
	}
	return out;
}