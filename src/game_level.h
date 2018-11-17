#ifndef GAMELEVEL_H
#define GAMELEVEL_H

#include <GL/glew.h>
#include "sprite_renderer.h"
#include "game_object.h"

#include <vector>
#include <string>
#include <unordered_map>

class brick_object: public game_object
{
public:
	brick_object(glm::vec2 position, glm::vec2 size, texture sprite, glm::vec3 color = glm::vec3(1.0f), glm::vec2 velocity = glm::vec2(0.0f, 0.0f))
		: game_object(position, size, sprite, color, velocity)
		, m_index(0)
	{}

	GLuint m_index;
};

class game_level
{
public:
	game_level();
	void load(const GLchar* file, GLuint level_width, GLuint level_height, GLuint screen_height);
	void draw(sprite_renderer& renderer);	
	void reset();
	GLboolean is_completed();
	std::string get_reward(GLuint index);
	
	std::vector<brick_object> m_briks;

	std::unordered_map<GLuint, std::string> m_reward;

private:
	void init(const std::vector<std::vector<GLuint>>& tile_data, GLuint level_width, GLuint level_height, GLuint screen_height);
};

#endif