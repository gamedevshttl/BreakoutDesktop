#include "manager.h"

#include <iostream>

manager::~manager()
{
	for (auto elem : m_font_face) {
		FT_Done_Face(elem.second);
	}
}

manager& manager::instance()
{
	static manager manager;
	return manager;
}

FT_Face manager::get_face(const std::string & font) 
{
	if (m_font_face.count(font))
		return m_font_face[font];

	return load_face(font);
}

FT_Face manager::load_face(const std::string & font)
{
	FT_Library ft;

	if (FT_Init_FreeType(&ft)) {
		std::cout << "ERROR::FREETYPE could not init FreeType library" << std::endl;
		return nullptr;
	}

	FT_Face face;
	if (FT_New_Face(ft, font.c_str(), 0, &face)) {
		std::cout << "ERROR::FREETYPE faled load font: " << font << std::endl;
		return nullptr;
	}

	m_font_face[font] = face;

	return face;
}
