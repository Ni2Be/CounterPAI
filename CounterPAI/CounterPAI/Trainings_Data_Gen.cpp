#include "Trainings_Data_Gen.h"
#include <list>
#include <fstream>

Trainings_Data_Gen::Trainings_Data_Gen()
	:
	m_gen(m_rd())
{
}

void save_sheet(Sheet_Music& sheet, const std::string folder)
{
	std::ofstream ofs;
	ofs.open(folder.c_str());
	if (!ofs)
	{
		std::cerr << "could not open " << folder << "\n";
		return;
	}
	ofs << sheet;
}

void Trainings_Data_Gen::generate_data(int count, const std::string folder)
{
	for (int i = 0; i < count; i++)
	{
		Sheet_Music sheet = generate_sheet();
		float score = 0;
		for (auto& note : sheet.get_cp())
		{
			score += note.m_probability;
		}
		score /= (float)sheet.get_cp().size();
		save_sheet(sheet, folder + "/sheet" + "_score_" + std::to_string(score) + "_" + std::to_string(i) + ".sheet");
	}
}

Sheet_Music Trainings_Data_Gen::generate_sheet()
{
	Sheet_Music sheet;
	std::uniform_int_distribution<> dist(0, 1);
	sheet.bass_is_cf = dist(m_gen);

	if (sheet.bass_is_cf)
	{
		generate_cf(sheet.m_bass, Note_Pitch::C2, Voice::Bass);
		generate_cp(sheet, Note_Pitch::A3, Voice::Soprano);

		m_evaluator.evaluate_notes(sheet.m_bass, sheet.m_soprano);
	}
	else
	{
		generate_cf(sheet.m_soprano, Note_Pitch::A3, Voice::Soprano);
		generate_cp(sheet, Note_Pitch::C2, Voice::Bass);

		m_evaluator.evaluate_notes(sheet.m_soprano, sheet.m_bass);
	}
	return sheet;
}


void Trainings_Data_Gen::generate_cf(std::list<Music_Note>& voice_line, Note_Pitch min, Voice voice)
{
	//gen first note
	std::uniform_int_distribution<> all_pitches_dist(0, 16);
	voice_line.push_back(
		Music_Note(Music_Note::get_ACscale_pitch(min, all_pitches_dist(m_gen)), Note_Value::Whole, voice)
	);


	//gen mid notes
	std::uniform_int_distribution<> pitch_distance_dist(0, 7);
	std::uniform_int_distribution<> pitch_distance_sml_dist(1, 2);
	std::uniform_int_distribution<> pitch_prob_dist(0, 100);
	std::uniform_int_distribution<> up_down_dist(0, 1);
	for (int i = 1; i < m_sheet_bars - 1; i++)
	{
		int distance = Music_Note::get_ACscale_distance(min, voice_line.back().m_pitch);

		int jump_height = pitch_prob_dist(m_gen);
		if (jump_height < 80)
			jump_height = pitch_distance_sml_dist(m_gen);
		else
			jump_height = pitch_distance_dist(m_gen);
		if(up_down_dist(m_gen) == 0)
			distance = (distance + jump_height) % 16;
		else
		{
			if (distance - jump_height < 0)
				distance = (16 - std::abs(distance - jump_height)) % 16;
			else
				distance = (distance - jump_height) % 16;
		}

		voice_line.push_back(
			Music_Note(Music_Note::get_ACscale_pitch(min, distance), Note_Value::Whole, voice)
		);
	}


	//gen last note / set front to unison fith or octave
	std::uniform_int_distribution<> unifioc_dist(0, 2);
	int unifioc = unifioc_dist(m_gen);

	Music_Note note = voice_line.front();
	voice_line.push_back(note);
	voice_line.pop_front();
	std::cout << "\nuznifi: " << unifioc;
	if (unifioc == 0)
		voice_line.push_front(note);
	else if (unifioc == 1)
	{
		int distance = Music_Note::get_ACscale_distance(min, note.m_pitch);
		std::cout << "\ndi: " << distance;
		if (distance + 4 > 16)
			distance = (distance + 7) % 17;
		else
			distance = (distance + 4);
		std::cout << "\ndi: " << distance;
		voice_line.push_front(
			Music_Note(Music_Note::get_ACscale_pitch(min, distance), Note_Value::Whole, voice)
		);
	}
	else if (unifioc == 2)
	{
		int distance = Music_Note::get_ACscale_distance(min, note.m_pitch);
		std::cout << "\ndi: " << distance;
		if (distance + 7 > 16)
			distance = (distance + 10) % 17;
		else
			distance = (distance + 7);
		std::cout << "\ndi: " << distance;
		voice_line.push_front(
			Music_Note(Music_Note::get_ACscale_pitch(min, distance), Note_Value::Whole, voice)
		);
	}
}

void Trainings_Data_Gen::generate_cp(Sheet_Music& sheet, Note_Pitch min, Voice voice)
{
	int sixteenths = (m_sheet_bars - 2) * 16; // before last bar will be filled / last bar will be a whole note
	Note_Value val;

	std::uniform_int_distribution<> note_value_dist(0, 9);
	std::uniform_int_distribution<> note_value_under_whole_dist(1, 2);

	//use less eights
	Note_Value note_values[10] = { Note_Value::Whole, Note_Value::Halfe, Note_Value::Quarter, 
		Note_Value::Whole, Note_Value::Halfe, Note_Value::Quarter,
		Note_Value::Whole, Note_Value::Halfe, Note_Value::Quarter,
		Note_Value::Eighth };

	std::uniform_int_distribution<> pitch_dist(0, 16);
	std::uniform_int_distribution<> flat_sharp_dist(0, 40);
	std::uniform_int_distribution<> tied_dist(0, 10);

	Note_Pitch pitch;

	auto add_note = [&sheet, &val, &pitch, &min, &voice, &pitch_dist, &flat_sharp_dist, &tied_dist](std::mt19937& gen) {
		pitch = Music_Note::get_ACscale_pitch(min, pitch_dist(gen));

		
		int flat_sharp = flat_sharp_dist(gen);
		if (pitch < min)
			flat_sharp = 0;
		if (pitch > Music_Note::get_ACscale_pitch(min, 16))
			flat_sharp = 0;

		bool is_tied = !tied_dist(gen);
		if (flat_sharp == 39)
			sheet.add_note(Music_Note(static_cast<Note_Pitch>((int)pitch + 1), val, voice, is_tied, true, false));
		else if (flat_sharp == 40)
			sheet.add_note(Music_Note(static_cast<Note_Pitch>((int)pitch - 1), val, voice, is_tied, false, true));
		else
			sheet.add_note(Music_Note(pitch, val, voice, is_tied));
	};

	do
	{
		if(sixteenths == -4)
			val = note_values[note_value_under_whole_dist(m_gen)];
		else
			val = note_values[note_value_dist(m_gen)];

		add_note(m_gen);

		sixteenths -= 16 / static_cast<int>(val);

		if (val == Note_Value::Eighth)//eights are allways a pair
		{
			add_note(m_gen);

			sixteenths -= 16 / static_cast<int>(val);
		}
	} while (sixteenths >= -4 && sixteenths != 0);

	int sixteenth_left = 16 + sixteenths;
	std::cout << "\nsixteenth: " << sixteenths;
	std::cout << "\nsixteenth_left: " << sixteenth_left;
	if (sixteenth_left != 0)
	{
		val = static_cast<Note_Value>(16 / sixteenth_left);

		add_note(m_gen);

	}
	val = Note_Value::Whole;
	add_note(m_gen);
}
