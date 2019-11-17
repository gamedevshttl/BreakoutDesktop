#ifndef DIALOG_H
#define DIALOG_H

#include <functional>

#include "widget.h"

class dialog : public widget
{
public:
	dialog(widget* parent,
		std::string tag,
		glm::vec2 position,
		glm::vec2 size,
		texture sprite,
		glm::vec3 color
	);

	void dialog::add_action(const std::function<void()>& first_action,
		const std::function<void()>& second_action);

protected:
	std::function<void()> m_first_action;
	std::function<void()> m_second_action;
};

#endif