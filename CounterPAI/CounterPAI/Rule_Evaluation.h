#pragma once
#include <vector>
#include <array>
#include <string>

namespace Eval
{
	enum class Fux_Rule;

	class Rule_Evaluation;
	std::ostream& operator<<(std::ostream& os, const Rule_Evaluation& note);
	std::istream& operator>>(std::istream& is, Rule_Evaluation& note);
	class Rule_Evaluation
	{
	public:
		Rule_Evaluation()
		{}

		Rule_Evaluation(const std::string& eval_str);
		static std::string get_rule_text(Fux_Rule rule);

		static const std::string C_INDEX_NAME;

		std::vector<Fux_Rule> broken_rules;

		bool was_rule_broken(Fux_Rule rule);

		/*Probability that the CP note is a good note.*/
		float m_probability = 1.0f;

		friend std::ostream& operator<<(std::ostream& os, const Rule_Evaluation& note);
		friend std::istream& operator>>(std::istream& is, Rule_Evaluation& note);
	};



	enum class Fux_Rule
	{
		R1,
		R1a,
		R1b,
		R2,
		R2a,
		R2b,
		R3,
		R3a,
		R3b,
		R3c,
		R3d,
		R4,
		R4a,
		R4b,
		R4c,
		R5,
		R5a,
		R5b,
		R6,
		R6a,
		R6b,
		R6c,
		R6d,
		R7,
		R8,
		R8a,
		R8b,
		R8c,
		R9,
		R9a,
		R9b
	};

	inline std::ostream& operator<<(std::ostream& os, const Fux_Rule& rule)
	{
		static const std::array<std::string, 31> rule_str
		{
			"R1",
			"R1a",
			"R1b",
			"R2",
			"R2a",
			"R2b",
			"R3",
			"R3a",
			"R3b",
			"R3c",
			"R3d",
			"R4",
			"R4a",
			"R4b", 
			"R4c",
			"R5",
			"R5a",
			"R5b",
			"R6",
			"R6a",
			"R6b",
			"R6c",
			"R6d",
			"R7",
			"R8",
			"R8a",
			"R8b",
			"R8c",
			"R9",
			"R9a",
			"R9b"
		};
		os << rule_str[static_cast<int>(rule)];
		return os;
	}
	inline std::istream& operator>>(std::istream& is, Fux_Rule& rule)
	{
		std::string s;
		is >> s;
		if (s == "R1")
			rule = Fux_Rule::R1;
		else if (s == "R1a")
			rule = Fux_Rule::R1a;
		else if (s == "R1b")
			rule = Fux_Rule::R1b;
		else if (s == "R2")
			rule = Fux_Rule::R2;
		else if (s == "R2a")
			rule = Fux_Rule::R2a;
		else if (s == "R2b")
			rule = Fux_Rule::R2b;
		else if (s == "R3")
			rule = Fux_Rule::R3;
		else if (s == "R3a")
			rule = Fux_Rule::R3a;
		else if (s == "R3b")
			rule = Fux_Rule::R3b;
		else if (s == "R3c")
			rule = Fux_Rule::R3c;
		else if (s == "R3d")
			rule = Fux_Rule::R3d;
		else if (s == "R4")
			rule = Fux_Rule::R4;
		else if (s == "R4a")
			rule = Fux_Rule::R4a;
		else if (s == "R4b")
			rule = Fux_Rule::R4b;
		else if (s == "R4c")
			rule = Fux_Rule::R4c;
		else if (s == "R5")
			rule = Fux_Rule::R5;
		else if (s == "R5a")
			rule = Fux_Rule::R5a;
		else if (s == "R5b")
			rule = Fux_Rule::R5b;
		else if (s == "R6")
			rule = Fux_Rule::R6;
		else if (s == "R6a")
			rule = Fux_Rule::R6a;
		else if (s == "R6b")
			rule = Fux_Rule::R6b;
		else if (s == "R6c")
			rule = Fux_Rule::R6c;
		else if (s == "R6d")
			rule = Fux_Rule::R6d;
		else if (s == "R7")
			rule = Fux_Rule::R7;
		else if (s == "R8")
			rule = Fux_Rule::R8;
		else if (s == "R8a")
			rule = Fux_Rule::R8a;
		else if (s == "R8b")
			rule = Fux_Rule::R8b;
		else if (s == "R8c")
			rule = Fux_Rule::R8c;
		else if (s == "R9")
			rule = Fux_Rule::R9;
		else if (s == "R9a")
			rule = Fux_Rule::R9a;
		else if (s == "R9b")
			rule = Fux_Rule::R9b;
		return is;
	}
	inline int get_main_rule_index(Fux_Rule rule)
	{
		switch (rule)
		{
		case Fux_Rule::R1: return 1;
		case Fux_Rule::R2: return 2;
		case Fux_Rule::R3: return 3;
		case Fux_Rule::R4: return 4;
		case Fux_Rule::R5: return 5;
		case Fux_Rule::R6: return 6;
		case Fux_Rule::R7: return 7;
		case Fux_Rule::R8: return 8;
		case Fux_Rule::R9: return 9;
		}
	}

	inline Fux_Rule get_main_rule_from_index(int index)
	{
		switch (index)
		{
		case 1: return Fux_Rule::R1;
		case 2: return Fux_Rule::R2;
		case 3: return Fux_Rule::R3;
		case 4: return Fux_Rule::R4;
		case 5: return Fux_Rule::R5;
		case 6: return Fux_Rule::R6;
		case 7: return Fux_Rule::R7;
		case 8: return Fux_Rule::R8;
		case 9: return Fux_Rule::R9;
		}
	}
}
