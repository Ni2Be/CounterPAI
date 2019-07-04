#pragma once
#include "Sheet_Music.h"
#include <list>
#include <string>
#include <array>
#include <sstream>
#include <vector>

namespace Eval
{

	enum class Motion;
	enum class Direction;
	enum class Interval;
	enum class Bar_Position;
	enum class Beat_Position;

	class Note_Evaluation;
	std::ostream& operator<<(std::ostream& os, const Note_Evaluation& note);
	class Note_Evaluation
	{
	public:
		Note_Evaluation();

		/*Motion between the last and current note in CP and cf*/
		Motion    m_motion;
		/*Direction from the last note in CP to the current note in CP*/
		Direction m_direction;
		/*Interval between the last note in CP and the current note in CP*/
		Interval  m_jump_interval;
		/*Interval between current note in CP and the cf note that is sounding
		at the same time*/
		Interval  m_interval;
		/*only set if a CP not is sounding into the next cf note. (tied into 
		the next bar)
		Interval between current note in CP and the cf note that is sounding
		at the same time*/
		Interval  m_interval_tied;
		/*Position of the CP Note. First Bar, Mid, Before Last Bar or Last Bar*/
		Bar_Position m_position;
		/*Position in the Bar*/
		Beat_Position m_beat_pos;

		/*Probability that the CP note is a good note.*/
		float m_probability = 1.0f;
		/*The starting position of the note counted from the first cf note
		0 = start at the same time as the cf
		17 = first note in the 2. bar
		...*/
		int   m_sixteenth_position;

		friend std::ostream& operator<<(std::ostream& os, const Note_Evaluation& note);
	};

	class Evaluator_Base;
	std::ostream& operator<<(std::ostream& os, const Evaluator_Base& eval);
	class Evaluator_Base
	{
	public:
		Evaluator_Base();

		std::list<Music_Note> cantus_firmus;

		std::list<Music_Note> counter_point;
		std::vector<Note_Evaluation> m_evaluation;

		virtual void evaluate_notes(
			std::list<Music_Note>& cantus_firmus,
			std::list<Music_Note>& counter_point);

		Interval get_interval(Music_Note n0, Music_Note n1);
		Direction get_direction(Music_Note n0, Music_Note n1);
		Motion get_motion(Music_Note cf0, Music_Note cf1, Music_Note CP0, Music_Note CP1);
		Bar_Position get_bar_pos(std::list<Music_Note>& voice, const std::list<Music_Note>::iterator& note);
		Beat_Position get_beat_pos(std::list<Music_Note>& voice, const std::list<Music_Note>::iterator& note);

		friend std::ostream& operator<<(std::ostream& os, const Evaluator_Base& eval);
	};

	enum class Motion
	{
		CoMo,
		ObMo,
		DiMo,
		NoMo
	};

	inline std::ostream& operator<<(std::ostream& os, const Motion& motion)
	{
		static const std::array<std::string, 4> motion_str
		{
			"CoMo",
			"ObMo",
			"DiMo",
			"NoMo"
		};
		os << motion_str[static_cast<int>(motion)];
		return os;
	}

	enum class Direction
	{
		Up,
		Side,
		Down,
		No_Dir
	};


	inline std::ostream& operator<<(std::ostream& os, const Direction& direction)
	{
		static const std::array<std::string, 4> direction_str
		{
		"Up",
		"Side",
		"Down",
		"No Dir"
		};

		os << direction_str[static_cast<int>(direction)];
		return os;
	}

	enum class Interval
	{
		P1 = 0,
		m2,
		M2,
		m3,
		M3,
		P4,
		Tri,
		P5,
		m6,
		M6,
		m7,
		M7,
		P8,
		No_Interval
	};


	inline std::ostream& operator<<(std::ostream& os, const Interval& interval)
	{
		static const std::array<std::string, 14> interval_str
		{
		"P1",
		"m2",
		"M2",
		"m3",
		"M3",
		"P4",
		"Tri",
		"P5",
		"m6",
		"M6",
		"m7",
		"M7",
		"P8",
		"No Itvl"
		};
		os << interval_str[static_cast<int>(interval)];
		return os;
	}


	enum class Bar_Position
	{
		First_Bar_First_Note,
		First_Bar,
		Mid_Bar,
		Before_Last,
		Before_Last_Last_Note,
		Last_Bar_First_Note,
		No_Bar
	};

	inline std::ostream& operator<<(std::ostream& os, const Bar_Position& position)
	{
		static const std::array<std::string, 7> position_str
		{
		"First Bar First Note",
		"First Bar",
		"Mid Bar",
		"Before Last",
		"Before Last Last Note",
		"Last Bar First Note",
		"No Bar"
		};
		os << position_str[static_cast<int>(position)];
		return os;
	}

	enum class Beat_Position
	{
		Down_Beat,
		Weak_Beat_1,
		Up_Beat,
		Weak_Beat_2,
		No_Beat,
	};

	inline std::ostream& operator<<(std::ostream& os, const Beat_Position& beat)
	{
		static const std::array<std::string, 5> beat_str
		{
		"Down Beat",
		"Weak Beat 1",
		"Up Beat",
		"Weak Beat 2",
		"No Beat"
		};
		os << beat_str[static_cast<int>(beat)];
		return os;
	}

}
