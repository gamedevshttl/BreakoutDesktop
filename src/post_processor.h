#ifndef POST_PROCESSOR_H
#define POST_PROCESSOR_H

#include <GL/glew.h>
#include <glm/glm.hpp>

#include "shader.h"
#include "texture.h"

class post_processor
{
public:
	shader m_post_processor_shader;
	texture m_texture;
	GLuint m_width, m_height;

	GLboolean m_confuse, m_chaos, m_shake;

	post_processor(shader shader, GLuint width, GLuint height);

	void begin_render();
	void end_render();
	void render(GLfloat time);

private:
	GLuint MSFBO, FBO;
	GLuint RBO;
	GLuint m_VAO;
	void init_render_data();
};


#endif // POST_PROCESSOR


