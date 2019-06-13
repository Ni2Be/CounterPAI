#pragma once
#include <string>
#include <SFML/Graphics.hpp>

class SVG_Parser
{
public:
	SVG_Parser(const std::string& svg_in_file, const std::string& png_out_file, float factor = 1.0f);

	sf::Texture m_texture;
	sf::Image m_image;
};

