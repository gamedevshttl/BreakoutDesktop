#ifndef TEXT_RENDERER_H
#define TEXT_RENDERER_H

#include <GL/glew.h>
#include <glm/glm.hpp>

#include "texture.h"
#include "shader.h"

#include <map>

struct character
{
	GLuint texture_id;
	glm::ivec2 size;
	glm::ivec2 bearing;
	GLuint advance;
};

class text_renderer
{
public:
	text_renderer(GLuint width, GLuint height);
	void load(std::string font, GLuint font_size);
	void render_text(std::string text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color = glm::vec3(1.0));

	std::map<GLchar, character> m_characters;
	shader m_text_shader;
private:
	GLuint m_VAO, m_VBO;
};

#endif
