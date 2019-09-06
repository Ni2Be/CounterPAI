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

		/*Probability that the CP note is a good note.*/
		float m_probability = 1.0f;

		friend std::ostream& operator<<(std::ostream& os, const Rule_Evaluation& note);
		friend std::istream& operator>>(std::istream& is, Rule_Evaluation& note);
	};

	enum class Fux_Rule
	{
		R1,
		R2,
		R3,
		R4,
		R5,
		R6,
		R7,
		R8,
		R9,
		R10
	};

	inline std::ostream& operator<<(std::ostream& os, const Fux_Rule& rule)
	{
		static const std::array<std::string, 10> rule_str
		{
			"R1",
			"R2",
			"R3",
			"R4",
			"R5",
			"R6",
			"R7",
			"R8",
			"R9",
			"R10"
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
		else if (s == "R2")
			rule = Fux_Rule::R2;
		else if (s == "R3")
			rule = Fux_Rule::R3;
		else if (s == "R4")
			rule = Fux_Rule::R4;
		else if (s == "R5")
			rule = Fux_Rule::R5;
		else if (s == "R6")
			rule = Fux_Rule::R6;
		else if (s == "R7")
			rule = Fux_Rule::R7;
		else if (s == "R8")
			rule = Fux_Rule::R8;
		else if (s == "R9")
			rule = Fux_Rule::R9;
		else if (s == "R10")
			rule = Fux_Rule::R10;
		return is;
	}

}
