#include "game_level.h"
#include "resource_manager.h"

#include <fstream>
#include <sstream>
#include <string>

#include "rapidjson/document.h"


game_level::game_level()
{}

void game_level::load(const GLchar* file, GLuint level_width, GLuint level_height, GLuint screen_height)
{
	std::string level_source;
	std::ifstream level_source_file(file);
	std::stringstream level_source_stream;
	level_source_stream << level_source_file.rdbuf();
	level_source = level_source_stream.str();

	rapidjson::Document document;
	document.Parse(level_source.c_str());

	assert(document.IsObject());

	const rapidjson::Value& object_array = document["level"];
	assert(object_array.IsArray());
	
	std::vector<std::vector<GLuint>> tile_data;
	for (rapidjson::SizeType i = 0; i < object_array.Size(); ++i) {
		std::string key = "line" + std::to_string(i);
		if (object_array[i].HasMember(key.c_str())){
			const rapidjson::Value& vector_item = object_array[i][key.c_str()];
			assert(vector_item.IsArray());

			std::vector<GLuint> block_line;
			for (rapidjson::SizeType j = 0; j < vector_item.Size(); ++j) {
				GLuint block_type = vector_item[j].GetUint();
				block_line.emplace_back(block_type);
			}
			tile_data.emplace_back(block_line);
		}		
	}

	init(tile_data, level_width, level_height, screen_height);

	const rapidjson::Value& reward_array = document["reward"];
	assert(reward_array.IsArray());

	for (rapidjson::Value::ConstValueIterator it = reward_array.Begin(); it != reward_array.End(); ++it)
	{		
		for (rapidjson::Value::ConstMemberIterator obj_it = it->GetObject().MemberBegin();
			obj_it != it->GetObject().MemberEnd();
			++obj_it) {
			
			std::string reward_key = obj_it->name.GetString();

			const rapidjson::Value& value = obj_it->value;
			assert(value.IsArray());
			for (rapidjson::Value::ConstValueIterator value_it = value.Begin(); value_it != value.End(); ++value_it) {
				GLuint index = value_it->GetInt();
				m_reward[index] = reward_key;
			}
		}
	}
}

void game_level::reset()
{
	for (auto& brick : m_briks)
		brick.m_destroyed = false;
}

void game_level::draw(sprite_renderer& renderer)
{
	for (auto& brick : m_briks)
		if (!brick.m_destroyed)
			brick.draw(renderer);
}

GLboolean game_level::is_completed()
{
	return false;
}

void game_level::init(const std::vector<std::vector<GLuint>>& tile_data, GLuint level_width, GLuint level_height, GLuint screen_height)
{
	if (tile_data.empty())
		return;

	GLuint height = tile_data.size();
	GLuint width = tile_data[0].size();

	GLfloat unit_width = level_width / static_cast<GLfloat>(width);
	GLfloat unit_height = level_height / static_cast<GLfloat>(height);

	GLuint index = 0;
	for (GLuint y = 0; y < height; ++y) {
		for (GLuint x = 0; x < width; ++x) {
			glm::vec2 pos(unit_width * x, unit_height * y + screen_height);
			glm::vec2 size(unit_width, unit_height);

			if (tile_data[y][x] == 1) {
				brick_object brick(pos, size, resource_manager::get_texture("block_solid"), glm::vec3(0.8f, 0.8f, 0.7f));
				brick.m_solid = true;
				brick.m_index = index;
				m_briks.push_back(brick);
			}
			else if(tile_data[y][x] > 1){
				glm::vec3 color(1.0f);
				if (tile_data[y][x] == 2)
					color = glm::vec3(0.2f, 0.6f, 1.0f);
				else if (tile_data[y][x] == 3)
					color = glm::vec3(0.0f, 0.7f, 0.0f);
				else if (tile_data[y][x] == 4)
					color = glm::vec3(0.8f, 0.8f, 0.4f);
				else if (tile_data[y][x] == 5)
					color = glm::vec3(1.0f, 0.5f, 0.0f);

				brick_object brick(pos, size, resource_manager::get_texture("block"), color);
				brick.m_index = index;
				m_briks.push_back(brick);
			}
			++index;
		}
	}
}

std::string game_level::get_reward(GLuint index)
{
	auto reward_it = m_reward.find(index);
	if (reward_it != m_reward.end())
		return reward_it->second;

	return "";
}