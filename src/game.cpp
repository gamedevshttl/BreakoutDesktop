#include "game.h"

#include "resource_manager.h"
#include "sprite_renderer.h"
#include "game_object.h"
#include "ball_object.h"
#include "particle_generator.h"
#include "post_processor.h"

#include <iostream>


GLboolean check_collision(const game_object& lhs, const game_object& rhs)
{
	bool collision_x = lhs.m_position.x + lhs.m_size.x >= rhs.m_position.x &&
		rhs.m_position.x + rhs.m_size.x >= lhs.m_position.x;

	bool collision_y = lhs.m_position.y + lhs.m_size.y >= rhs.m_position.y &&
		rhs.m_position.y + rhs.m_size.y >= lhs.m_position.y;

	return collision_x && collision_y;
}

Direction vector_direction(glm::vec2 target)
{
	glm::vec2 compass[] = {
		glm::vec2(0.0f, 1.0f),
		glm::vec2(1.0f, 0.0f),
		glm::vec2(0.0f, -1.0f),
		glm::vec2(-1.0f, 0.0f),
	};

	GLfloat max = 0.0f;
	GLuint best_match = -1;

	for (GLuint i = 0; i < 4; ++i) {
		GLfloat dot_product = glm::dot(glm::normalize(target), compass[i]);
		if (dot_product > max) {
			max = dot_product;
			best_match = i;
		}
	}
	return (Direction)best_match;
}

Collision check_collision(const ball_object& lhs, const game_object& rhs)
{
	glm::vec2 center(lhs.m_position + lhs.m_radius);

	glm::vec2 aabb_half_extension(rhs.m_size.x / 2, rhs.m_size.y / 2);
	glm::vec2 aabb_center(rhs.m_position.x + aabb_half_extension.x, rhs.m_position.y + aabb_half_extension.y);

	glm::vec2 difference = center - aabb_center;
	glm::vec2 clamped = glm::clamp(difference, -aabb_half_extension, aabb_half_extension);

	glm::vec2 closest = aabb_center + clamped;
	difference = closest - center;

	//return glm::length(difference) < lhs.m_radius;

	if (glm::length(difference) < lhs.m_radius)
		return std::make_tuple(GL_TRUE, vector_direction(difference), difference);
	else
		return std::make_tuple(GL_FALSE, UP, glm::vec2(0,0));
}


game::game(GLuint width, GLuint height)
	: m_width(width)
	, m_height(height)
	, m_key()
	, m_state(game_active)
	, m_player_size(width / 8, height / 30)
	, m_player_velocity(PLAYER_VELOCITY)
	, m_x_offset(0.0f)
	, m_last_x(0.0f)
	, m_ball_radius(BALL_RADIUS)
	, m_ball_velocity(INITIAL_BALL_VELOCITY)
	, m_shake_time(0)
{
}

game::~game()
{
}

void game::init()
{
	m_ball_velocity = INITIAL_BALL_VELOCITY;

	resource_manager::load_texture("../resources/textures/background.jpg", GL_TRUE, "background");
	resource_manager::load_texture("../resources/textures/awesomeface.png", GL_TRUE, "fase");
	resource_manager::load_texture("../resources/textures/block.png", GL_TRUE, "block");
	resource_manager::load_texture("../resources/textures/block_solid.png", GL_TRUE, "block_solid");
	resource_manager::load_texture("../resources/textures/paddle.png", GL_TRUE, "paddle");
	resource_manager::load_texture("../resources/textures/particle.png", GL_TRUE, "particle");

	game_level one;
	one.load("../resources/data/level_one.json", m_width, m_height * 0.3);
	m_levels.emplace_back(one);
	m_current_level = 0;

	resource_manager::load_shader("../src/shader/sprite.vs", "../src/shader/sprite.fs", nullptr, "sprite");
	resource_manager::load_shader("../src/shader/particle.vs", "../src/shader/particle.fs", nullptr, "particle");
	resource_manager::load_shader("../src/shader/post_processor.vs", "../src/shader/post_processor.fs", nullptr, "postprocessing");

	glm::mat4 projection = glm::ortho(0.0f, static_cast<GLfloat>(m_width), static_cast<GLfloat>(m_height), 0.0f, -1.0f, 1.0f);

	resource_manager::get_shader("sprite").use().set_int("image", 0);
	resource_manager::get_shader("sprite").use().set_matrix4("projection", projection);
	resource_manager::get_shader("particle").use().set_int("image", 0);
	resource_manager::get_shader("particle").use().set_matrix4("projection", projection);

	m_sprite_renderer = std::make_shared<sprite_renderer>(resource_manager::get_shader("sprite"));

	glm::vec2 player_pos = glm::vec2(m_width/2 - m_player_size.x/2, m_height - m_player_size.y);
	m_player = std::make_shared<game_object>(player_pos, m_player_size, resource_manager::get_texture("paddle"));

	glm::vec2 ball_pos = player_pos + glm::vec2(m_player_size.x / 2 - m_ball_radius / 2, -m_ball_radius * 2);
	m_ball = std::make_shared<ball_object>(ball_pos, m_ball_radius, m_ball_velocity, resource_manager::get_texture("fase"));

	m_particle_generator = std::make_shared<particle_generator>(resource_manager::get_shader("particle"),
		resource_manager::get_texture("particle"),
		500);

	m_post_processor = std::make_shared<post_processor>(resource_manager::get_shader("postprocessing"), m_width, m_height);
}

void game::do_collision()
{
	for (game_object& box : m_levels[m_current_level].m_briks) {
		if (!box.m_destroyed) {
			Collision collision = check_collision(*m_ball, box);
			if(std::get<0>(collision)){
				if (!box.m_solid)
					box.m_destroyed = true;
				else {
					m_shake_time = 0.1;
					m_post_processor->m_shake = GL_TRUE;
				}

				Direction dir = std::get<1>(collision);
				glm::vec2 diff_vector = std::get<2>(collision);

				if (dir == LEFT || dir == RIGHT) {
					m_ball->m_velocity.x = -m_ball->m_velocity.x;

					GLfloat penetration = m_ball->m_radius - std::abs(diff_vector.x);
					if (dir == LEFT)
						m_ball->m_position.x += penetration;
					else if (dir == RIGHT)
						m_ball->m_position.x -= penetration;
				}
				else if (dir == UP || dir == DOWN){
					m_ball->m_velocity.y = -m_ball->m_velocity.y;

					GLfloat penetration = m_ball->m_radius - std::abs(diff_vector.y);
					if (dir == UP)
						m_ball->m_position.y -= penetration;
					else if (dir == DOWN)
						m_ball->m_position.y += penetration;
				}				
			}
		}
	}

	Collision result = check_collision(*m_ball, *m_player);
	if (!m_ball->m_stuck && std::get<0>(result)) {
		GLfloat center_board = m_player->m_position.x + m_player->m_size.x/2;
		GLfloat distance = (m_ball->m_position.x + m_ball->m_radius) - center_board;
		GLfloat percentage = distance / (m_player->m_size.x/2);

		GLfloat strenght = 2.0f;
		glm::vec2 old_velosity = m_ball->m_velocity;
		m_ball->m_velocity.x = INITIAL_BALL_VELOCITY.x * percentage * strenght;
		m_ball->m_velocity = glm::normalize(m_ball->m_velocity) * glm::length(old_velosity);
		m_ball->m_velocity.y = -1 * abs(m_ball->m_velocity.y);
	}
}

void game::progress_input(GLfloat dt)
{
	if (m_state == game_active) {
		GLfloat velocity = m_player_velocity * dt;

		if (m_key[GLFW_KEY_A]) {
			if (m_player->m_position.x > 0) {
				m_player->m_position.x -= velocity;

				if (m_ball->m_stuck)
					m_ball->m_position.x -= velocity;
			}
		}
		if (m_key[GLFW_KEY_D]) {
			if (m_player->m_position.x < m_width - m_player->m_size.x) {
				m_player->m_position.x += velocity;

				if (m_ball->m_stuck)
					m_ball->m_position.x += velocity;
			}
		}

		if (m_key[GLFW_KEY_SPACE])
			m_ball->m_stuck = false;
	}

	//if (m_player->m_position.x >= 0.0f && m_player->m_position.x <= m_width - m_player->m_size.x) {
	//	m_player->m_position.x += m_x_offset * (dt * 50.0f);

	//	if (m_player->m_position.x > m_width - m_player->m_size.x)
	//		m_player->m_position.x = m_width - m_player->m_size.x;

	//	if (m_player->m_position.x < 0.0f)
	//		m_player->m_position.x = 0.0f;

	//	m_x_offset = 0.0f;
	//}	
}

void game::update(GLfloat dt)
{
	m_ball->move(dt, m_width);
	do_collision();

	if (m_ball->m_position.y >= m_height) {
		reset_level();
		reset_player();
	}

	m_particle_generator->update(dt, *m_ball, 2, glm::vec2(m_ball->m_radius/2));

	if (m_shake_time > 0.0f) {
		m_shake_time -= dt;
		if (m_shake_time < 0.0f)
			m_post_processor->m_shake = GL_FALSE;
	}
}

void game::mouse_callback(double xpos, double ypos)
{
	m_x_offset = xpos - m_last_x;
	m_last_x = xpos;
}

void game::render()
{
	m_post_processor->begin_render();

	{
		if (m_state == game_active)
			m_sprite_renderer->draw_sprite(resource_manager::get_texture("background"), glm::vec2(0, 0), glm::vec2(m_width, m_height), 0.0f, glm::vec3(1.0f, 1.0f, 1.0f));

		m_levels[m_current_level].draw(*m_sprite_renderer);
		m_player->draw(*m_sprite_renderer);
		m_particle_generator->draw();
		m_ball->draw(*m_sprite_renderer);
	}

	m_post_processor->end_render();
	m_post_processor->render(glfwGetTime());
}

void game::reset_level()
{
	m_levels[m_current_level].reset();
}

void game::reset_player()
{
	m_player->m_size = m_player_size;
	m_player->m_position = glm::vec2(m_width/2 - m_player->m_size.x/2, m_height - m_player->m_size.y);
	m_ball->reset(m_player->m_position + glm::vec2(m_player->m_size.x/2 - m_ball->m_radius/2, -m_ball_radius * 2), INITIAL_BALL_VELOCITY);
}
