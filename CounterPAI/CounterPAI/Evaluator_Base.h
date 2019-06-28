#pragma once
#include "Sheet_Music.h"
#include <list>
#include <string>
#include <array>


namespace Eval
{
	enum class Motion
	{
		CoMo,
		ObMo,
		DiMo,
		NoMo
	};

	static std::string get_Motrion_str(Motion motion)
	{
		static const std::array<std::string, 4> motion_str
		{
			"CoMo",
			"ObMo",
			"DiMo",
			"NoMo"
		};
		return motion_str[static_cast<int>(motion)];
	}

	enum class Direction
	{
		Up,
		Side,
		Down,
		No_Dir
	};

	static std::string get_Motrion_str(Direction direction)
	{
		static const std::array<std::string, 4> direction_str
		{
		"Up",
		"Side",
		"Down",
		"No_Dir"
		};
		return direction_str[static_cast<int>(direction)];
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

	static std::string get_Interval_str(Interval interval)
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
		"No_Interval"
		};
		return interval_str[static_cast<int>(interval)];
	}

	enum class Bar_Position
	{
		First_Bar,
		Mid_Bar,
		Before_Last,
		Last_Bar,
		No_Bar
	};

	static std::string get_Position_str(Bar_Position position)
	{
		static const std::array<std::string, 4> position_str
		{
		"First_Bar",
		"Before_Last",
		"Last_Bar",
		"No_Bar"
		};
		return position_str[static_cast<int>(position)];
	}

	class Note_Evaluation
	{
	public:
		Motion    m_motion;
		Direction m_direction;
		Interval  m_jump_interval;
		Interval  m_interval;
		Interval  m_interval_tied;
		Bar_Position m_position;

		float m_probability;
		int   m_sixteenth_position;
	};

	class Evaluator_Base
	{
	public:
		Evaluator_Base(
			std::list<Music_Note> cantus_firmus,
			std::list<Music_Note> counter_point);

		std::list<Music_Note> m_cantus_firmus;

		std::list<Music_Note> m_counter_point;
		std::list<Note_Evaluation> m_evaluation;

		void evaluate_notes();

		Interval get_interval(Music_Note n0, Music_Note n1);
		Direction get_direction(Music_Note n0, Music_Note n1);
		Motion get_motion(Music_Note cf0, Music_Note cf1, Music_Note CP0, Music_Note CP1);
		Bar_Position get_bar_pos(std::list<Music_Note> voice, std::list<Music_Note>::iterator note);
	};

}
