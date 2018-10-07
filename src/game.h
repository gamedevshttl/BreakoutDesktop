#ifndef  GAME_H
#define GAME_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <memory>
#include <vector>

#include "game_level.h"

class sprite_renderer;
class game_object;
class ball_object;
class particle_generator;

enum game_state {
	game_active,
	game_menu,
	game_win
};

enum Direction {
	UP,
	RIGHT,
	DOWN,
	LEFT
};

typedef std::tuple<GLboolean, Direction, glm::vec2> Collision;

// Initial velocity of the player paddle
const GLfloat PLAYER_VELOCITY(500.0f);
// Initial velocity of the Ball
const glm::vec2 INITIAL_BALL_VELOCITY(100.0f, -350.0f);
// Radius of the ball object
const GLfloat BALL_RADIUS = 12.5f;

class game {
public:
	game(GLuint width, GLuint height);
	~game();

	void init();
	void do_collision();
	void progress_input(GLfloat dt);
	void update(GLfloat dt);
	void mouse_callback(double xpos, double ypos);
	void render();
	void reset_level();
	void reset_player();

	

	game_state m_state;
	GLboolean m_key[1024];
	GLuint m_width, m_height;

	std::shared_ptr<sprite_renderer> m_sprite_renderer;

	std::vector<game_level> m_levels;
	GLuint m_current_level;

	std::shared_ptr<game_object> m_player;
	glm::vec2 m_player_size;

	GLfloat m_player_velocity;

	GLfloat m_x_offset, m_last_x;

	GLfloat m_ball_radius;
	glm::vec2 m_ball_velocity;

	std::shared_ptr<ball_object> m_ball;
	std::shared_ptr<particle_generator> m_particle_generator;

};



#endif // ! GAME_H
