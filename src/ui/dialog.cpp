#include "dialog.h"
#include "button.h"
#include"../resource_manager.h"

dialog::dialog(widget* parent,
	std::string tag,
	glm::vec2 position,
	glm::vec2 size,
	texture sprite,
	glm::vec3 color	
)
	: widget(parent, tag, position, size, sprite, color)
{}

void dialog::add_action(const std::function<void()>& first_action,
						const std::function<void()>& second_action)
{
	int width_button = m_size.x / 5;
	int heigth_button = m_size.y / 8;

	int x = m_size.x / 4 - width_button / 2;
	int y = m_size.y - (m_size.y / 7 + heigth_button / 2);



}