#ifndef SPRITE_RENDERER_H
#define SPRITE_RENDERER_H

#include "shader.h"
#include "texture.h"

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class sprite_renderer
{
public:
	sprite_renderer(shader& shader_item);
	~sprite_renderer();

	void draw_sprite(texture& texture_item, glm::vec2 position, glm::vec2 size = glm::vec2(10,10), GLfloat rotate = 0.0f, glm::vec3 color = glm::vec3(1.0f));
private:
	shader m_shader_item;

	GLuint m_VAO;
	void init_render_data();
};

#endif