#ifndef LABEL_H
#define LABEL_H

#include <GL/glew.h>
#include <glm/glm.hpp>

#include "../texture.h"
#include "../shader.h"
#include "widget.h"

#include <map>

struct character
{
	GLuint texture_id;
	glm::ivec2 size;
	glm::ivec2 bearing;
	GLuint advance;
};

class label: public widget
{
public:
	label(	widget* parent,
			std::string tag,
			glm::vec2 position,
			GLuint width, 
			GLuint height,
			texture sprite = texture(),
			glm::vec3 color = glm::vec3(1.0f));

	void load(std::string font, GLuint font_size);
	//void render_text(std::string text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color = glm::vec3(1.0));
	void render_text();
	virtual void draw(sprite_renderer& renderer);

	std::map<GLchar, character> m_characters;
	shader m_text_shader;

	virtual void set_caption(std::string caption) override { m_caption = caption; }
	const std::string& get_label() const { return m_caption; }

private:
	GLuint m_VAO, m_VBO;
	std::string m_caption;
};

#endif
