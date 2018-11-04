#include "reward.h"
#include "resource_manager.h"

std::unordered_map<std::string, glm::vec3> reward_manager::m_color;
std::unordered_map<std::string, GLfloat> reward_manager::m_duration;
std::unordered_map<std::string, texture> reward_manager::m_texture;

void reward_manager::init()
{
	m_color["speed"] = glm::vec3(0.5f, 0.5f, 1.0f);
	m_color["sticky"] = glm::vec3(1.0f, 0.5f, 1.0f);
	m_color["pass-through"] = glm::vec3(0.5f, 1.0f, 0.5f);
	m_color["pad-size-increase"] = glm::vec3(1.0f, 0.6f, 0.4f);
	m_color["confuse"] = glm::vec3(1.0f, 0.3f, 0.3f);
	m_color["chaos"] = glm::vec3(0.9f, 0.25f, 0.25f);
	
	m_duration["speed"] = 0.0f;
	m_duration["sticky"] = 20.0f;
	m_duration["pass-through"] = 10.0f;
	m_duration["pad-size-increase"] = 0.0f;
	m_duration["confuse"] = 15.0f;
	m_duration["chaos"] = 15.0f;

	m_texture["speed"] = resource_manager::load_texture("../resources/textures/powerup_speed.png", GL_TRUE, "powerup_speed");
	m_texture["sticky"] = resource_manager::load_texture("../resources/textures/powerup_sticky.png", GL_TRUE, "powerup_sticky");
	m_texture["pass-through"] = resource_manager::load_texture("../resources/textures/powerup_passthrough.png", GL_TRUE, "powerup_passthrough");
	m_texture["pad-size-increase"] = resource_manager::load_texture("../resources/textures/powerup_increase.png", GL_TRUE, "powerup_increase");
	m_texture["confuse"] = resource_manager::load_texture("../resources/textures/powerup_confuse.png", GL_TRUE, "powerup_confuse");
	m_texture["chaos"] = resource_manager::load_texture("../resources/textures/powerup_chaos.png", GL_TRUE, "powerup_chaos");	
}


const glm::vec3& reward_manager::get_color(const std::string& name)
{
	return m_color[name];
}

const GLfloat reward_manager::get_duration(const std::string& name)
{
	return m_duration[name];
}

const texture& reward_manager::get_texture(const std::string& name)
{
	return m_texture[name];
}
