#ifndef SHADER_H
#define SHADER_H

#include <string>

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

class shader
{
public:
	shader();
	shader& use();

	void compile(const GLchar *vertex_source, const GLchar *fragment_source, const GLchar *geometry_source = nullptr);

	void set_float(const GLchar* name, GLfloat value, GLboolean use_shader = false);
	void set_int(const GLchar* name, GLint value, GLboolean use_shader = false);
	void set_vector_2f(const GLchar* name, const glm::vec2& value, GLboolean use_shader = false);
	void set_vector_3f(const GLchar* name, GLfloat x, GLfloat y, GLfloat z, GLboolean use_shader = false);
	void set_vector_3f(const GLchar* name, const glm::vec3& value, GLboolean use_shader = false);
	void set_vector_4f(const GLchar* name, const glm::vec4& value, GLboolean use_shader = false);
	void set_matrix4(const GLchar *name, const glm::mat4& matrix, GLboolean use_shader = false);
	GLuint get_id() { return m_id; }
private:
	void check_compile_error(GLuint object, const std::string& type );

	GLuint m_id;
};

#endif