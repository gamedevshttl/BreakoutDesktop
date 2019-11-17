#include "button.h"

button::button(	widget* parent, 
				std::string tag,
				glm::vec2 position, 
				glm::vec2 size, 
				texture sprite, 
				texture press_sprite, 
				texture lock_sprite, 
				glm::vec3 color,
				const std::function<void()>& action
				)
	: widget(parent, tag, position, size, sprite, color)
	, m_press_sprite(press_sprite)
	, m_lock_sprite(lock_sprite)
	, m_state(state::none)
	, m_action(nullptr)
	, m_lock(false)
{
	m_action = action;
}

void button::draw(sprite_renderer& renderer)
{
	if (m_lock) {
		renderer.draw_sprite(m_lock_sprite, m_global_position, m_size, m_rotation, m_color);
		return;
	}

	if (m_state == state::none)
		renderer.draw_sprite(m_sprite, m_global_position, m_size, m_rotation, m_color);
	else if (m_state == state::pressed)
		renderer.draw_sprite(m_press_sprite, m_global_position, m_size, m_rotation, m_color);

	for (auto& child : m_child)
		child->draw(renderer);
}

bool button::touch(int x, int y, touch_action action)
{
	if (m_lock)
		return false;

	glm::vec2 pos = m_global_position;

	if (pos.x < x && x < pos.x + m_size.x &&
		pos.y < y && y < pos.y + m_size.y)
	{
		if (action == touch_action::press) {
			m_state = state::pressed;

			system::set_capture(shared_from_this());
			return true;
		}

		if (action == touch_action::release) {
			if (m_state == state::pressed) {
				m_state = state::none;

				if (m_action)
					m_action();

				system::remove_capture();
			}
		}
	}

	return false;
}

void button::set_action(const std::function<void()>& action)
{
	m_action = action;
}
