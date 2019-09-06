#pragma once
#include <iostream>
#include <array>
#include <string>

namespace Eval
{

	enum class Motion;
	enum class Direction;
	enum class Interval;
	enum class Bar_Position;
	enum class Beat_Position;

	class Note_Evaluation;
	std::ostream& operator<<(std::ostream& os, const Note_Evaluation& note);
	std::istream& operator>>(std::istream& is, Note_Evaluation& note);
	class Note_Evaluation
	{
	public:
		static const std::string C_INDEX_NAME;

		Note_Evaluation();
		Note_Evaluation(const std::string& eval_str);

		/*Motion between the last and current note in CP and cf*/
		Motion    m_motion;
		/*Direction from the last note in CP to the current note in CP*/
		Direction m_direction;
		/*Interval between the last note in CP and the current note in CP*/
		Interval  m_jump_interval;
		/*Interval between current note in CP and the cf note that is sounding
		at the same time*/
		Interval  m_interval;
		/*Position of the CP Note. First Bar, Mid, Before Last Bar or Last Bar*/
		Bar_Position m_position;
		/*Position in the Bar*/
		Beat_Position m_beat_pos;

		/*The starting position of the note counted from the first cf note
		0 = start at the same time as the cf
		17 = first note in the 2. bar
		...*/
		int   m_sixteenth_position;

		friend std::ostream& operator<<(std::ostream& os, const Note_Evaluation& note);
		friend std::istream& operator>>(std::istream& is, Note_Evaluation& note);
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

	inline std::istream& operator>>(std::istream& is, Motion& motion)
	{
		std::string s;
		is >> s;
		if (s == "CoMo")
			motion = Motion::CoMo;
		else if (s == "ObMo")
			motion = Motion::ObMo;
		else if (s == "DiMo")
			motion = Motion::DiMo;
		else if (s == "NoMo")
			motion = Motion::NoMo;
		return is;
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
		"No_Dir"
		};

		os << direction_str[static_cast<int>(direction)];
		return os;
	}

	inline std::istream& operator>>(std::istream& is, Direction& direction)
	{
		std::string s;
		is >> s;
		if (s == "Up")
			direction = Direction::Up;
		else if (s == "Side")
			direction = Direction::Side;
		else if (s == "Down")
			direction = Direction::Down;
		else if (s == "No_Dir")
			direction = Direction::No_Dir;
		return is;
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
		"No_Itvl"
		};
		os << interval_str[static_cast<int>(interval)];
		return os;
	}

	inline std::istream& operator>>(std::istream& is, Interval& interval)
	{
		std::string s;
		is >> s;

		if (s == "P1")
			interval = Interval::P1;
		else if (s == "m2"	   )
			interval = Interval::m2;
		else if (s == "M2"	   )
			interval = Interval::M2;
		else if (s == "m3"	   )
			interval = Interval::m3;
		else if (s == "M3"	   )
			interval = Interval::M3;
		else if (s == "P4"	   )
			interval = Interval::P4;
		else if (s == "Tri"	   )
			interval = Interval::Tri;
		else if (s == "P5"	   )
			interval = Interval::P5;
		else if (s == "m6"	   )
			interval = Interval::m6;
		else if (s == "M6"	   )
			interval = Interval::M6;
		else if (s == "m7"	   )
			interval = Interval::m7;
		else if (s == "M7"	   )
			interval = Interval::M7;
		else if (s == "P8"	   )
			interval = Interval::P8;
		else if (s == "No_Itvl")
			interval = Interval::No_Interval;
		return is;
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
		"First_Bar_First_Note",
		"First_Bar",
		"Mid_Bar",
		"Before_Last",
		"Before_Last_Last_Note",
		"Last_Bar_First_Note",
		"No_Bar"
		};
		os << position_str[static_cast<int>(position)];
		return os;
	}

	inline std::istream& operator>>(std::istream& is, Bar_Position& bar_pos)
	{
		std::string s;
		is >> s;
		if (s == "First_Bar_First_Note")
			bar_pos = Bar_Position::First_Bar_First_Note;
		else if (s == "First_Bar")
			bar_pos = Bar_Position::First_Bar;
		else if (s == "Mid_Bar")
			bar_pos = Bar_Position::Mid_Bar;
		else if (s == "Before_Last")
			bar_pos = Bar_Position::Before_Last;
		else if (s == "Before_Last_Last_Note")
			bar_pos = Bar_Position::Before_Last_Last_Note;
		else if (s == "Last_Bar_First_Note")
			bar_pos = Bar_Position::Last_Bar_First_Note;
		else if (s == "No_Bar")
			bar_pos = Bar_Position::No_Bar;
		return is;
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
		"Down_Beat",
		"Weak_Beat_1",
		"Up_Beat",
		"Weak_Beat_2",
		"No_Beat"
		};
		os << beat_str[static_cast<int>(beat)];
		return os;
	}

	inline std::istream& operator>>(std::istream& is, Beat_Position& beat_pos)
	{
		std::string s;
		is >> s;
		if (s == "Down_Beat")
			beat_pos = Beat_Position::Down_Beat;
		else if (s == "Weak_Beat_1")
			beat_pos = Beat_Position::Weak_Beat_1;
		else if (s == "Up_Beat")
			beat_pos = Beat_Position::Up_Beat;
		else if (s == "Weak_Beat_2")
			beat_pos = Beat_Position::Weak_Beat_2;
		else if (s == "No_Beat")
			beat_pos = Beat_Position::No_Beat;
		return is;
	}

}
