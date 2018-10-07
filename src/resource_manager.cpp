#include "resource_manager.h"

#include <iostream>
#include <sstream>
#include <fstream>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"


std::map<std::string, shader> resource_manager::m_shaders;
std::map<std::string, texture> resource_manager::m_textures;

shader resource_manager::load_shader(const GLchar *v_shader_file, const GLchar *f_shader_file, const GLchar *g_shader_file, const std::string name)
{
	m_shaders[name] = load_shader_from_file(v_shader_file, f_shader_file, g_shader_file);
	return  m_shaders[name];
}

shader resource_manager::get_shader(const std::string name)
{
	return  m_shaders[name];
}

texture resource_manager::load_texture(const GLchar *file, GLboolean alpha, const std::string name)
{
	m_textures[name] = load_texture_from_file(file, alpha);
	return m_textures[name];
}

texture resource_manager::get_texture(const std::string name)
{
	return m_textures[name];
}

void resource_manager::clear()
{
	for (auto iter : m_shaders) {
		glDeleteProgram(iter.second.get_id());
	}

	for (auto iter : m_textures) {
		glDeleteTextures(1, &iter.second.m_id);
	}
}

shader resource_manager::load_shader_from_file(const GLchar *v_shader_file, const GLchar *f_shader_file, const GLchar *g_shader_file)
{
	std::string vertex_code;
	std::string fragment_code;
	std::string geometry_code;

	std::ifstream vertex_shader_file(v_shader_file);
	std::ifstream fragment_shader_file(f_shader_file);

	std::stringstream v_shader_stream, f_shader_stream;

	v_shader_stream << vertex_shader_file.rdbuf();
	f_shader_stream << fragment_shader_file.rdbuf();

	vertex_shader_file.close();
	fragment_shader_file.close();

	vertex_code = v_shader_stream.str();
	fragment_code = f_shader_stream.str();

	if (g_shader_file) {
		std::ifstream geometry_shader_file(g_shader_file);
		std::stringstream g_shader_stream;
		g_shader_stream << geometry_shader_file.rdbuf();
		geometry_shader_file.close();
		geometry_code = g_shader_stream.str();
	}

	shader shader_value;
	shader_value.compile(vertex_code.c_str(), fragment_code.c_str(), g_shader_file != nullptr ? geometry_code.c_str() : nullptr);
	return shader_value;
}

texture resource_manager::load_texture_from_file(const GLchar *file, GLboolean alpha)
{
	texture texture_value;
	if (alpha) {
		texture_value.m_internal_format = GL_RGBA;
		texture_value.m_image_format = GL_RGBA;
	}

	int width, height, nr_channels;
	unsigned char* image = stbi_load(file, &width, &height, &nr_channels, 0);

	if (image) {

		GLenum format;
		if (nr_channels == 1)
			texture_value.m_image_format = GL_RED;
		else if (nr_channels == 3)
			texture_value.m_image_format = GL_RGB;
		else if (nr_channels == 4)
			texture_value.m_image_format = GL_RGBA;

		texture_value.generate(width, height, image);
		stbi_image_free(image);
	}
	else {
		std::cout << "Failed load texture " << file << std::endl;
	}

	return texture_value;
}