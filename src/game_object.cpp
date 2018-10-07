#include "game_object.h"

game_object::game_object()
	: m_position(0,0)
	, m_size(1,1)
	, m_velocity(0.0f)
	, m_color(1.0f)
	, m_rotation(0.0f)
	, m_sprite()
	, m_solid(false)
	, m_destroyed(false)
{}

game_object::game_object(glm::vec2 position, glm::vec2 size, texture sprite, glm::vec3 color, glm::vec2 velocity)
	: m_position(position)
	, m_size(size)
	, m_velocity(velocity)
	, m_color(color)
	, m_rotation(0.0f)
	, m_sprite(sprite)
	, m_solid(false)
	, m_destroyed(false)
{}

void game_object::draw(sprite_renderer& renderer)
{
	renderer.draw_sprite(m_sprite, m_position, m_size, m_rotation, m_color);
}
