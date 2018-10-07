#ifndef RESOURCE_MANAGER_H
#define RESOURCE_MANAGER_H

#include <unordered_map>
#include <map>
#include <string>

#include <GL/glew.h>

#include "texture.h"
#include "shader.h"

class resource_manager
{
public:
	static std::map<std::string, shader> m_shaders;
	static std::map<std::string, texture> m_textures;

	static shader load_shader(const GLchar *v_shader_file, const GLchar *f_shader_file, const GLchar *g_shader_file, const std::string name);
	static shader get_shader(const std::string name);

	static texture load_texture(const GLchar *file, GLboolean alpha, const std::string name);
	static texture get_texture(const std::string name);

	static void clear();

private:
	resource_manager() {}

	static shader load_shader_from_file(const GLchar *v_shader_file, const GLchar *f_shader_file, const GLchar *g_shader_file = nullptr);
	static texture load_texture_from_file(const GLchar *file, GLboolean alpha);
};


#endif