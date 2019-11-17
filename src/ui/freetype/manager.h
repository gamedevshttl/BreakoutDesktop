#ifndef MANAGER_H
#define MANAGER_H

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_CACHE_H

#include <GL/glew.h>
#include <glm/glm.hpp>

#include <map>
#include <string>

class manager
{
public:
	static manager& instance();

	FT_Face get_face(const std::string & ttf);

private:
	manager() = default;
	~manager();

	manager(const manager&) = delete;
	manager(manager&&) = delete;

	manager& operator=(const manager&) = delete;
	manager& operator=(manager&&) = delete;

	FT_Face load_face(const std::string & ttf);
	
	std::map<std::string, FT_Face> m_font_face;
};


#endif