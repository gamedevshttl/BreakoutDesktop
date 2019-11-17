#ifndef BUTTON_H
#define BUTTON_H

#include <functional>

#include "widget.h"

class button : public widget
{
public:
	button(	widget* parent, 
			std::string tag,
			glm::vec2 position, 
			glm::vec2 size, 
			texture sprite, 
			texture press_sprite, 
			texture lock_sprite, 
			glm::vec3 color,
			const std::function<void()>& action
			);

	virtual void draw(sprite_renderer& renderer) override;
	virtual bool touch(int x, int y, touch_action action) override;

	void set_action(const std::function<void()>& acttion);

protected:
	std::function<void()> m_action;

	//texture m_sprite;
	texture m_press_sprite;
	texture m_lock_sprite;

	state m_state;
	bool m_lock;
};


#endif // !BUTTON_H
