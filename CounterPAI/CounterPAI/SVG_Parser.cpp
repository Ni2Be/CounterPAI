#include "SVG_Parser.h"
//#define DEBUG

#include <iostream>
#include <fstream>
#include <regex>
#include <cstdio>

//nanosvg
#include <stdio.h> //nanosvg dependency
#include <string.h> //nanosvg dependency
#include <math.h> //nanosvg dependency
#define NANOSVG_IMPLEMENTATION	// Expands implementation
#define NANOSVG_ALL_COLOR_KEYWORDS	// Include full list of color keywords.
#include "nanosvg.h"
#define NANOSVGRAST_IMPLEMENTATION
#include "nanosvgrast.h"

//STB
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <STB/stb_image_write.h>


SVG_Parser::SVG_Parser(const std::string& svg_in_file, const std::string& png_out_file, float factor, std::string hex_color)
{
	parse(svg_in_file, png_out_file, factor, true, hex_color);
}

SVG_Parser::SVG_Parser(const std::string& svg_in_file, float factor, std::string hex_color)
{
	parse(svg_in_file, "SVG_Parser_temp_png_out_file", factor, false, hex_color);
}

void SVG_Parser::parse(const std::string& svg_in_file, const std::string& png_out_file, float factor, bool save_png, std::string hex_color)
{
	std::ifstream is(svg_in_file.c_str());
	if (!is)
		std::cerr << "could not open: " + svg_in_file << "\n";
	std::string file_data_string;
	while (is)
	{
		std::string temp;
		is >> temp;
		file_data_string.append(temp + "\n" );
	}

	std::regex rege("fill:#[a-fA-F0-9]{6}");
	file_data_string = std::regex_replace(file_data_string, rege, "fill:#" + hex_color);
	std::regex rege2("fill=\"#[a-fA-F0-9]{6}");
	file_data_string = std::regex_replace(file_data_string, rege2, "fill=\"#" + hex_color);

	std::ofstream temp_file("temp_svg_file.svg");
	temp_file << file_data_string;
	temp_file.close();
#ifdef DEBUG
	std::cout << "parsing: " + svg_in_file + "\n";
#endif
	NSVGimage *image = NULL;
	image = nsvgParseFromFile("temp_svg_file.svg", "px", 96.0f);
	std::remove("temp_svg_file.svg");

	if (image == NULL) {
		std::cerr << "Could not open temp SVG image.\n";
		return;
	}
	int w, h;
	w = (int)(image->width * factor);
	h = (int)(image->height * factor);

	NSVGrasterizer *rast = NULL;
	rast = nsvgCreateRasterizer();
	if (rast == NULL) {
		std::cerr << "Could not init rasterizer.\n";
		return;
	}

	unsigned char* img = NULL;
	img = (unsigned char*)malloc(w*h * 4);
	if (img == NULL) {
		std::cerr << "Could not alloc image buffer.\n";
		return;
	}

#ifdef DEBUG
	std::cout << "rasterizing image " << w << ", " << h << "\n";
#endif
	nsvgRasterize(rast, image, 0, 0, factor, img, w, h, w * 4);

#ifdef DEBUG
	std::cout << "writing " + png_out_file + "\n";
#endif
	//pars png 
	stbi_write_png(png_out_file.c_str(), w, h, 4, img, w * 4);
	//load to texture
	m_texture.loadFromFile(png_out_file);
	//delete all res
	if(!save_png)
		std::remove(png_out_file.c_str());
	nsvgDeleteRasterizer(rast);
	nsvgDelete(image);
}