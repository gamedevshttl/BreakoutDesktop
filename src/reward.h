#ifndef REWARD_H
#define REWARD_H

#include <GL/glew.h>
#include <glm/glm.hpp>

#include "game_object.h"

#include <unordered_map>

class reward : public game_object
{
public:
	reward(std::string type, glm::vec3 color, GLfloat duration, glm::vec2 position, glm::vec2 size, texture sprite, glm::vec2 velocity)
		: game_object(position, size, sprite, color, velocity)
		, m_type(type)
		, m_duration(duration)
		, m_activated(GL_FALSE)
		, m_destroyed(GL_FALSE)
	{}

    std::string m_type;
    GLfloat     m_duration;	
    GLboolean   m_activated;    
	GLboolean   m_destroyed;
}; 



class reward_manager
{
public:
	static void init();

	static const glm::vec3& get_color(const std::string& name);
	static const GLfloat get_duration(const std::string& name);
	static const texture& get_texture(const std::string& name);
protected:
	static std::unordered_map<std::string, glm::vec3> m_color;
	static std::unordered_map<std::string, GLfloat> m_duration;
	static std::unordered_map<std::string, texture> m_texture;
};

#endif