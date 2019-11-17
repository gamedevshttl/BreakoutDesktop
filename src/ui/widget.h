#ifndef WIDGET_H
#define WIDGET_H
 
#include <GL/glew.h>
#include <glm/glm.hpp>

#include <memory>
#include <vector>

#include "../texture.h"
#include "../sprite_renderer.h"

template <typename Type>
using pointer = std::shared_ptr<Type>;

template <typename Type>
using link = std::weak_ptr<Type>;

template <typename Type, typename... Args>
inline pointer<Type> _make_shared(Args&&... args)
{
	return std::make_shared<Type>(std::forward<Args>(args)...);
}

enum class touch_action :short {
	press,
	release,
	move
};

enum class state : short {
	none,
	pressed
};

class widget : public std::enable_shared_from_this<widget>
{
public:
	widget(widget* parent, std::string tag, glm::vec2 position, glm::vec2 size, texture sprite, glm::vec3 color = glm::vec3(1.0f));

	virtual void draw(sprite_renderer& renderer);
	virtual bool touch(int x, int y, touch_action action);

	const glm::vec2& get_position() const { return m_position; }
	const glm::vec2& get_size() const { return m_size; }

	glm::vec2 get_global_position() const;
	virtual const widget* get_parent() const { return m_parent; }

	template<typename Type, typename... Args>
	std::shared_ptr<Type> add(Args... args)
	{
		auto control = _make_shared<Type>(this, std::forward<Args>(args)...);
		m_child.push_back(control);
		return control;
	}

	void set_visible(bool visible) { m_visible = visible; }
	bool get_visible() const { return m_visible; }

	widget* find(const std::string tag);

	virtual void set_caption(std::string caption) {}

	const std::string& get_tag() const {return m_tag;}

protected:
	glm::vec2 m_position, m_global_position, m_size;
	glm::vec3 m_color;
	texture m_sprite;
	GLfloat m_rotation;

	widget* m_parent;
	std::vector<std::shared_ptr<widget>> m_child;

	bool m_visible;

	std::string m_tag;
};

class system
{
public:
	static void set_capture(std::shared_ptr<widget> capture_widget);
	static void remove_capture();
	static std::shared_ptr<widget> get_capture();

	static void set_active_widget(std::shared_ptr<widget> capture_widget);
	static std::shared_ptr<widget> get_active_widget();
	static void reset_active_w	idget();
private:
	static std::shared_ptr<widget> m_capture_widget;
	static std::shared_ptr<widget> m_active_widget;
};

#endif // !WIDGET_H
