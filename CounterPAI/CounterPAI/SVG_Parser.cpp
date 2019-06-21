#include "SVG_Parser.h"
#define DEBUG

#include <iostream>
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


SVG_Parser::SVG_Parser(const std::string& svg_in_file, const std::string& png_out_file, float factor)
{
	parse(svg_in_file, png_out_file, factor, true);
}

SVG_Parser::SVG_Parser(const std::string& svg_in_file, float factor)
{
	parse(svg_in_file, "SVG_Parser_temp_png_out_file", factor, false);
}


void SVG_Parser::parse(const std::string& svg_in_file, const std::string& png_out_file, float factor, bool save_png)
{

	NSVGimage *image = NULL;
	NSVGrasterizer *rast = NULL;
	unsigned char* img = NULL;
	int w, h;
	const char* filename = svg_in_file.c_str();

#ifdef DEBUG
	printf("parsing %s\n", filename);
#endif
	image = nsvgParseFromFile(filename, "px", 96.0f);
	if (image == NULL) {
		printf("Could not open SVG image.\n");
		return;
	}
	w = (int)(image->width * factor);
	h = (int)(image->height * factor);

	rast = nsvgCreateRasterizer();
	if (rast == NULL) {
		printf("Could not init rasterizer.\n");
		return;
	}

	img = (unsigned char*)malloc(w*h * 4);
	if (img == NULL) {
		printf("Could not alloc image buffer.\n");
		return;
	}

#ifdef DEBUG
	printf("rasterizing image %d x %d\n", w, h);
#endif
	nsvgRasterize(rast, image, 0, 0, factor, img, w, h, w * 4);

#ifdef DEBUG
	printf("writing %s\n", png_out_file.c_str());
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