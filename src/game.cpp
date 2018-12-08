#include "game.h"

#include "resource_manager.h"
#include "sprite_renderer.h"
#include "game_object.h"
#include "ball_object.h"
#include "particle_generator.h"
#include "post_processor.h"
#include "text_renderer.h"

#include <iostream>
#include <sstream>
#include <string>
#include <algorithm>


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
	, m_state(game_state::game_menu)
	, m_player_size(width / 8, height / 30)
	, m_ball_radius(width / 60.0f)
	, m_player_velocity(PLAYER_VELOCITY)
	, m_x_offset(0.0f)
	, m_last_x(0.0f)	
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
	m_game_screen_top = m_height - m_height * 0.9;
	m_game_screen_low = m_height - m_height * 0.8;	

	resource_manager::load_texture("../resources/textures/background.jpg", GL_TRUE, "background");
	resource_manager::load_texture("../resources/textures/awesomeface.png", GL_TRUE, "fase");
	resource_manager::load_texture("../resources/textures/block.png", GL_TRUE, "block");
	resource_manager::load_texture("../resources/textures/block_solid.png", GL_TRUE, "block_solid");
	resource_manager::load_texture("../resources/textures/paddle.png", GL_TRUE, "paddle");
	resource_manager::load_texture("../resources/textures/particle.png", GL_TRUE, "particle");
	resource_manager::load_texture("../resources/textures/ball_icon.png", GL_TRUE, "ball_icon");
	resource_manager::load_texture("../resources/textures/white_break.png", GL_TRUE, "white_break");
	resource_manager::load_texture("../resources/textures/armor_break_1.png", GL_TRUE, "armor_break");
	resource_manager::load_texture("../resources/textures/stone_break_2.png", GL_TRUE, "stone_break");
		
	reward_manager::init();

	game_level one;
	one.load("../resources/data/level_one.json", m_width, m_height * 0.3, m_game_screen_top);
	m_levels.emplace_back(one);
	one.load("../resources/data/level_two.json", m_width, m_height * 0.3, m_game_screen_top);
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

	glm::vec2 player_pos = glm::vec2(m_width/2 - m_player_size.x/2, m_height - m_player_size.y - m_game_screen_low);
	m_player = std::make_shared<game_object>(player_pos, m_player_size, resource_manager::get_texture("paddle"));
	
	glm::vec2 ball_pos = player_pos + glm::vec2(m_player_size.x / 2 - m_ball_radius / 2, -m_ball_radius * 2);
	m_ball = std::make_shared<ball_object>(ball_pos, m_ball_radius, m_ball_velocity, resource_manager::get_texture("ball_icon"));

	m_particle_generator = std::make_shared<particle_generator>(resource_manager::get_shader("particle"),
		resource_manager::get_texture("particle"),
		500,
		m_width/70.0f);

	m_post_processor = std::make_shared<post_processor>(resource_manager::get_shader("postprocessing"), m_width, m_height);

	m_text = std::make_shared<text_renderer>(m_width, m_height);
	m_text->load("../resources/font/OCRAEXT.TTF", 24);
}

void game::do_collision()
{
	for (brick_object& box : m_levels[m_current_level].m_briks) {
		if (!box.m_destroyed) {
			Collision collision = check_collision(*m_ball, box);
			if(std::get<0>(collision)){
				if (!box.m_solid) {
					box.m_destroyed = true;
					spawn_rewards(box.m_position, box.m_index);
				}
				else {
					m_shake_time = 0.1;
					m_post_processor->m_shake = GL_TRUE;
				}

				Direction dir = std::get<1>(collision);
				glm::vec2 diff_vector = std::get<2>(collision);

				if (!(m_ball->m_pass_through && !box.m_solid))
				{
					if (dir == LEFT || dir == RIGHT) {
						m_ball->m_velocity.x = -m_ball->m_velocity.x;

						GLfloat penetration = m_ball->m_radius - std::abs(diff_vector.x);
						if (dir == LEFT)
							m_ball->m_position.x += penetration;
						else if (dir == RIGHT)
							m_ball->m_position.x -= penetration;
					}
					else if (dir == UP || dir == DOWN) {
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
	}

	for (auto& reward_item : m_rewards) {
		if (!reward_item.m_destroyed) {

			if (reward_item.m_position.y >= m_height)
				reward_item.m_destroyed = GL_TRUE;

			if (check_collision(*m_player, reward_item)) {				
				activate_reward(reward_item);
				reward_item.m_destroyed = GL_TRUE;
				reward_item.m_activated = GL_TRUE;
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

		m_ball->m_stuck = m_ball->m_sticky;
	}
}

void game::progress_input(GLfloat dt)
{	
	if (m_state == game_state::game_menu) {
		if (m_key[GLFW_KEY_ENTER] && !m_key_processed[GLFW_KEY_ENTER]) {
			m_state = game_state::game_active;
			m_key_processed[GLFW_KEY_ENTER] = GL_TRUE;
		}

		if (m_key[GLFW_KEY_W] && !m_key_processed[GLFW_KEY_W]) {
			++m_current_level;
			m_current_level = m_current_level%m_levels.size();
			m_key_processed[GLFW_KEY_W] = GL_TRUE;
		}

		if (m_mouse_key[GLFW_MOUSE_BUTTON_LEFT])
			m_state = game_state::game_active;

		return;
	}

	if (m_state == game_state::game_active) {
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

		if (m_mouse_key[GLFW_MOUSE_BUTTON_LEFT])
			m_ball->m_stuck = false;
	}

	if (m_state == game_state::game_win) {
		if (m_key[GLFW_KEY_ENTER]) {
			m_key_processed[GLFW_KEY_ENTER] = GL_TRUE;
			m_state = game_state::game_menu;
		}
	}


}

void game::update(GLfloat dt)
{
	m_ball->move(dt, m_width);
	do_collision();

	if (m_ball->m_position.y >= m_height) {		
		m_levels[m_current_level].lost_live();
		reset_player();

		if (m_levels[m_current_level].m_life == 0) {
			m_state = game_state::game_menu;
			m_levels[m_current_level].reset();
		}
	}

	m_particle_generator->update(dt, *m_ball, 2, glm::vec2(m_ball->m_radius/2));

	if (m_shake_time > 0.0f) {
		m_shake_time -= dt;
		if (m_shake_time < 0.0f)
			m_post_processor->m_shake = GL_FALSE;
	}

	update_reward(dt);

	if ((m_player->m_position.x + m_player->m_size.x / 2) != m_mouse_x) {
		if (m_prev_mouse_x != m_mouse_x) {
			m_diff_pos = m_mouse_x - (m_player->m_position.x + m_player->m_size.x / 2);
			m_prev_mouse_x = m_mouse_x;
			m_move_time = 0.05f;
		}

		if (m_move_time > 0) {
			if (m_diff_pos > 0) {
				if ((m_player->m_position.x + m_player->m_size.x / 2) < m_mouse_x) {
					GLfloat delta = (m_diff_pos * dt) / m_move_time;
					m_player->m_position.x += delta;

					if (m_ball->m_stuck)
						m_ball->m_position.x += delta;

					m_diff_pos -= delta;
					m_move_time -= dt;
				}
			}
			else {
				if ((m_player->m_position.x + m_player->m_size.x / 2) > m_mouse_x) {
					GLfloat delta = (m_diff_pos * dt) / m_move_time;
					m_player->m_position.x += delta;

					if (m_ball->m_stuck)
						m_ball->m_position.x += delta;


					m_diff_pos -= delta;
					m_move_time -= dt;
				}
			}

			if (m_player->m_position.x <= 0) {

				if (m_ball->m_stuck)
					m_ball->m_position.x += (1 + std::abs(m_player->m_position.x));


				m_player->m_position.x = 1;
				m_move_time = 0;
			}

			if (m_player->m_position.x >= m_width - m_player->m_size.x) {

				if (m_ball->m_stuck)
					m_ball->m_position.x -= (m_player->m_position.x - (m_width - m_player->m_size.x - 1));

				m_player->m_position.x = m_width - m_player->m_size.x - 1;
				m_move_time = 0;
			}			
		}
	}

	if (m_state == game_state::game_active && m_levels[m_current_level].is_completed()) {
		reset_player();
		reset_level();
		m_state = game_state::game_win;
	}

}

void game::key_callback(int key, int scancode, int action, int mode)
{
	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
			m_key[key] = GL_TRUE;
		else if (action == GLFW_RELEASE) {
			m_key[key] = GL_FALSE;
			m_key_processed[key] = GL_FALSE;
		}
	}
}

void game::mouse_callback(double xpos, double ypos)
{
	if (m_state == game_state::game_menu || m_state == game_state::game_win)
		return;

	m_mouse_x = xpos;
	m_mouse_y = ypos;
}

void game::mouse_key_callback(int key, int action, int mode)
{
	if (key >= 0 && key < 8) {
		if (action == GLFW_PRESS)
			m_mouse_key[key] = GL_TRUE;
		else if (action == GLFW_RELEASE)
			m_mouse_key[key] = GL_FALSE;
	}
}

void game::render()
{
	{
		m_post_processor->begin_render();

		{			
			m_sprite_renderer->draw_sprite(resource_manager::get_texture("background"), glm::vec2(0, 0), glm::vec2(m_width, m_height), 0.0f, glm::vec3(1.0f, 1.0f, 1.0f));

			m_levels[m_current_level].draw(*m_sprite_renderer);
			m_player->draw(*m_sprite_renderer);

			for (auto& reward_item : m_rewards) {
				if (!reward_item.m_destroyed)
					reward_item.draw(*m_sprite_renderer);
			}

			m_particle_generator->draw();
			m_ball->draw(*m_sprite_renderer);
		}

		m_post_processor->end_render();
		m_post_processor->render(glfwGetTime());


		std::stringstream ss; ss << m_levels[m_current_level].m_life;
		m_text->render_text("Lives:" + ss.str(), 5.0f, 5.0f, 1.0f);
	}

	if (m_state == game_state::game_menu)
	{
		m_text->render_text("Press ENTER to start", 250.0f, m_height/ 2, 1.0f);
		m_text->render_text("Press W or S to select level", 245.0f, m_height / 2 + 20.0f, 0.75f);
	}

	if (m_state == game_state::game_win)
	{
		m_text->render_text("You WON!!!", 250.0f, m_height / 2, 1.0f);
		m_text->render_text("Press ENTER to retry or ESC to quit", 245.0f, m_height / 2 + 20.0f, 0.75f);
	}
}

void game::reset_level()
{
	m_levels[m_current_level].reset();
}

void game::reset_player()
{
	m_player->m_size = m_player_size;
	m_player->m_position = glm::vec2(m_width/2 - m_player->m_size.x/2, m_height - m_player->m_size.y - m_game_screen_low);
	m_ball->reset(m_player->m_position + glm::vec2(m_player->m_size.x/2 - m_ball->m_radius/2, -m_ball_radius * 2), INITIAL_BALL_VELOCITY);
	m_ball->m_color = glm::vec3(1.0f);
	m_player->m_color = glm::vec3(1.0f);
	m_post_processor->m_chaos = GL_FALSE;
	m_post_processor->m_confuse = GL_FALSE;
	m_ball->m_pass_through = GL_FALSE;
}

void game::spawn_rewards(const glm::vec2& position, GLuint index)
{
	std::string reward_type = m_levels[m_current_level].get_reward(index);

	if (!reward_type.empty())
		m_rewards.push_back(reward(reward_type,
			reward_manager::get_color(reward_type),
			reward_manager::get_duration(reward_type),
			position,
			glm::vec2(m_width / 8, m_height / 30),
			reward_manager::get_texture(reward_type),
			glm::vec2(0.0f, 150.0f)));
}

GLboolean is_other_reward_active(std::vector<reward> rewards, const std::string type)
{
	for (const auto& reward_item : rewards) 
		if (reward_item.m_type == type && reward_item.m_activated)
			return GL_TRUE;
	return GL_FALSE;
}

void game::update_reward(GLfloat dt)
{
	for (auto& reward_item : m_rewards) {
		reward_item.m_position += reward_item.m_velocity * dt;

		if (reward_item.m_activated) {
			reward_item.m_duration -= dt;
			
			if (reward_item.m_duration <= 0.0) {
				reward_item.m_activated = GL_FALSE;

				if (reward_item.m_type == "sticky") {
					if (!is_other_reward_active(m_rewards, "sticky")) {
						m_ball->m_sticky = GL_FALSE;
						m_player->m_color = glm::vec3(1.0f);
					}
				}
				else if(reward_item.m_type == "pass-through") {
					if (!is_other_reward_active(m_rewards, "pass-through")) {
						m_ball->m_pass_through = GL_FALSE;
						m_player->m_color = glm::vec3(1.0f);
						m_ball->m_color = glm::vec3(1.0f);
					}
				}
				else if(reward_item.m_type == "confuse") {
					if (!is_other_reward_active(m_rewards, "confuse")) {
						m_post_processor->m_confuse = GL_FALSE;
					}
				}
				else if(reward_item.m_type == "chaos") {
					if (!is_other_reward_active(m_rewards, "chaos")) {
						m_post_processor->m_chaos = GL_FALSE;
					}
				}
			}
		}
	}

	m_rewards.erase(
		std::remove_if(m_rewards.begin(), m_rewards.end(), [](const reward& reward_item) 
			{ return reward_item.m_destroyed && !reward_item.m_activated; }),
		m_rewards.end());
}

void game::activate_reward(const reward& reward_item)
{
	if (reward_item.m_type == "speed") {
		m_ball->m_velocity *= 1.2;
	}
	else if (reward_item.m_type == "sticky") {
		m_ball->m_sticky = GL_TRUE;
		m_player->m_color = glm::vec3(1.0f, 0.5f, 1.0f);
	}
	else if (reward_item.m_type == "pass-through") {
		m_ball->m_pass_through = GL_TRUE;
		m_ball->m_color = glm::vec3(1.0f, 0.5f, 0.5f);
	}
	else if (reward_item.m_type == "pad-size-increase") {
		m_player->m_size.x += 50;
	}
	else if (reward_item.m_type == "confuse") {
		if (!m_post_processor->m_chaos) 
			m_post_processor->m_confuse = GL_TRUE;
	}
	else if (reward_item.m_type == "chaos") {
		if (!m_post_processor->m_confuse)
			m_post_processor->m_chaos = GL_TRUE;
	}
}