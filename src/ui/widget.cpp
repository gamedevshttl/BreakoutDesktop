#include "widget.h"

widget::widget(widget* parent, std::string tag, glm::vec2 position, glm::vec2 size, texture sprite, glm::vec3 color)
	: m_parent(parent)
	, m_tag(tag)
	, m_position(position)
	, m_size(size)
	, m_rotation(0.0f)
	, m_sprite(sprite)
	, m_color(color)
	, m_visible(true)
{	
	m_global_position = get_global_position();	
}


void widget::draw(sprite_renderer& renderer)
{
	if (!get_visible())
		return;
	renderer.draw_sprite(m_sprite, m_global_position, m_size, m_rotation, m_color);
	for (auto& child : m_child)
		child->draw(renderer);
}

bool widget::touch(int x, int y, touch_action action)
{
	if (!get_visible())
		return false;

	for (auto& child : m_child)
		if (child->get_visible())
			child->touch(x, y, action);

	return false;
}

glm::vec2 widget::get_global_position() const
{
	glm::vec2 position = m_position;

	const widget* parent = m_parent;
	while (parent) {
		position += parent->get_position();
		parent = parent->get_parent();
	}

	return position;
}

widget* widget::find(const std::string tag)
{
	for (auto& child : m_child) {
		if (child->get_tag() == tag)
			return child.get();
	}

	return nullptr;
}


std::shared_ptr<widget> system::m_capture_widget = nullptr;
std::shared_ptr<widget> system::m_active_widget = nullptr;

void system::set_capture(std::shared_ptr<widget> capture_widget)
{
	m_capture_widget = capture_widget;
}

void system::remove_capture()
{
	m_capture_widget = nullptr;
}

std::shared_ptr<widget> system::get_capture()
{
	return m_capture_widget;
}

void system::set_active_widget(std::shared_ptr<widget> active_widget)
{
	m_active_widget = active_widget;
}

std::shared_ptr<widget> system::get_active_widget()
{
	return m_active_widget;
}

void system::reset_active_widget()
{
	m_active_widget = nullptr;
}