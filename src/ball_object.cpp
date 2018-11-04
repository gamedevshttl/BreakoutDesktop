#include "ball_object.h"

ball_object::ball_object()
	: game_object()
	, m_radius(12.5f)
	, m_stuck(true)
	, m_sticky(GL_FALSE)
	, m_pass_through(GL_FALSE)
{}

ball_object::ball_object(glm::vec2 pos, GLfloat radius, glm::vec2 velocity, texture sprite)
	: game_object(pos, glm::vec2(radius* 2, radius * 2), sprite, glm::vec3(1.0f), velocity)
	, m_radius(radius)
	, m_stuck(true)
	, m_sticky(GL_FALSE)
	, m_pass_through(GL_FALSE)
{}

glm::vec2 ball_object::move(GLfloat dt, GLuint window_width)
{
	if (!m_stuck) {
		m_position += m_velocity * dt;

		if (m_position.x < 0.0f){
			m_velocity.x = -m_velocity.x;
			m_position.x = 0.0f;
		}
		else if (m_position.x + m_size.x >= window_width) {
			m_velocity.x = -m_velocity.x;
			m_position.x = window_width - m_size.x;
		}
		else if (m_position.y <= 0.0f) {
			m_velocity.y = -m_velocity.y;
			m_position.y = 0.0f;
		}
	}

	return m_position;
}

void ball_object::reset(glm::vec2 position, glm::vec2 velocity)
{
	m_position = position;
	m_velocity = velocity;
	m_stuck = true;
}
