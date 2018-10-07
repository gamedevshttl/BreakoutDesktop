#include "shader.h"

#include <iostream>

shader::shader()
{
}

shader& shader::use()
{
	glUseProgram(m_id);
	return *this;
}

void shader::compile(const GLchar *vertex_source, const GLchar *fragment_source, const GLchar *geometry_source)
{
	GLuint s_vertex, s_fragment, s_geometry;

	s_vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(s_vertex, 1, &vertex_source, NULL);
	glCompileShader(s_vertex);
	check_compile_error(s_vertex, "vertex");

	s_fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(s_fragment, 1, &fragment_source, NULL);
	glCompileShader(s_fragment);
	check_compile_error(s_fragment, "fragment");

	if (geometry_source) {
		s_geometry = glCreateShader(GL_GEOMETRY_SHADER);
		glShaderSource(s_geometry, 1, &geometry_source, NULL);
		glCompileShader(s_geometry);
		check_compile_error(s_geometry, "geometry");
	}

	m_id = glCreateProgram();
	glAttachShader(m_id, s_vertex);
	glAttachShader(m_id, s_fragment);
	if(geometry_source)
		glAttachShader(m_id, s_geometry);
	glLinkProgram(m_id);
	check_compile_error(m_id, "programm");

	glDeleteShader(s_vertex);
	glDeleteShader(s_fragment);
	if (geometry_source)
		glDeleteShader(s_geometry);
}

void shader::set_float(const GLchar* name, GLfloat value, GLboolean use_shader)
{
	if (use_shader)
		use();
	glUniform1f(glGetUniformLocation(m_id, name), value);
}

void shader::set_int(const GLchar* name, GLint value, GLboolean use_shader)
{
	if (use_shader)
		use();
	glUniform1f(glGetUniformLocation(m_id, name), value);
}

void shader::set_vector_2f(const GLchar* name, const glm::vec2& value, GLboolean use_shader)
{
	if (use_shader)
		use();
	glUniform2f(glGetUniformLocation(m_id, name), value.x, value.y);
}

void shader::set_vector_3f(const GLchar* name, GLfloat x, GLfloat y, GLfloat z, GLboolean use_shader)
{
	if (use_shader)
		use();
	glUniform3f(glGetUniformLocation(m_id, name), x, y, z);
}

void shader::set_vector_3f(const GLchar* name, const glm::vec3& value, GLboolean use_shader)
{
	if (use_shader)
		use();
	glUniform3f(glGetUniformLocation(m_id, name), value.x, value.y, value.z);
}

void shader::set_vector_4f(const GLchar* name, const glm::vec4& value, GLboolean use_shader)
{
	if (use_shader)
		use();
	glUniform4f(glGetUniformLocation(m_id, name), value.x, value.y, value.z, value.w);
}

void shader::set_matrix4(const GLchar *name, const glm::mat4& matrix, GLboolean use_shader)
{
	if (use_shader)
		use();
	glUniformMatrix4fv(glGetUniformLocation(m_id, name), 1, GL_FALSE, glm::value_ptr(matrix));
}

void shader::check_compile_error(GLuint object, const std::string& type)
{
	GLint success;
	GLchar info_log[1024];

	if (type != "programm") {
		glGetShaderiv(object, GL_COMPILE_STATUS, &success);
		if (!success) {
			glGetShaderInfoLog(object, 1024, NULL, info_log);
			std::cout << type << ": "<< info_log << std::endl;
		}
	}
	else {
		glGetProgramiv(object, GL_LINK_STATUS, &success);
		if (!success) {
			glGetProgramInfoLog(object, 1024, NULL, info_log);
			std::cout << type << ": " << info_log << std::endl;
		}
	}
};

