#ifndef BALL_OBJECT_H
#define BALL_OBJECT_H

#include "game_object.h"

class ball_object:public game_object
{
public:
	GLfloat m_radius;
	GLboolean m_stuck;

	ball_object();
	ball_object(glm::vec2 pos, GLfloat radius, glm::vec2 velocity, texture sprite);

	glm::vec2 move(GLfloat dt, GLuint window_width);
	void reset(glm::vec2 position, glm::vec2 velocity);
};

#endif