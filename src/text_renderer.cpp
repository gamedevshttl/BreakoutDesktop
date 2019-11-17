#include <glm/gtc/matrix_transform.hpp>
#include <ft2build.h>
#include FT_FREETYPE_H

#include "text_renderer.h"
#include "resource_manager.h"

#include <iostream>

namespace text {

	text_renderer::text_renderer(GLuint width, GLuint height)
	{
		m_text_shader = resource_manager::load_shader("../src/shader/text.vs", "../src/shader/text.fs", nullptr, "text");
		m_text_shader.set_matrix4("projection", glm::ortho(0.0f, static_cast<GLfloat>(width), static_cast<GLfloat>(height), 0.0f), GL_TRUE);
		m_text_shader.set_int("text", 0);

		glGenVertexArrays(1, &m_VAO);
		glGenBuffers(1, &m_VBO);
		glBindVertexArray(m_VAO);
		glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}

	void text_renderer::load(std::string font, GLuint font_size)
	{
		m_characters.clear();
		FT_Library ft;

		if (FT_Init_FreeType(&ft))
			std::cout << "ERROR::FREETYPE could not init FreeType library" << std::endl;

		FT_Face face;
		if (FT_New_Face(ft, font.c_str(), 0, &face))
			std::cout << "ERROR::FREETYPE faled load font: " << font << std::endl;

		FT_Set_Pixel_Sizes(face, 0, font_size);

		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

		for (GLubyte c = 0; c < 128; ++c) {
			if (FT_Load_Char(face, c, FT_LOAD_RENDER))
			{
				std::cout << "ERROR::FREETYPE faled load glyph: " << c << std::endl;
				continue;
			}

			GLuint texture;
			glGenTextures(1, &texture);
			glBindTexture(GL_TEXTURE_2D, texture);
			glTexImage2D(
				GL_TEXTURE_2D,
				0,
				GL_RED,
				face->glyph->bitmap.width,
				face->glyph->bitmap.rows,
				0,
				GL_RED,
				GL_UNSIGNED_BYTE,
				face->glyph->bitmap.buffer);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			character character_item = {
				texture,
				glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
				glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
				face->glyph->advance.x
			};

			m_characters[c] = character_item;
		}

		glBindTexture(GL_TEXTURE_2D, 0);
		FT_Done_Face(face);
		FT_Done_FreeType(ft);

	}

	void text_renderer::render_text(std::string text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color)
	{
		m_text_shader.use();
		m_text_shader.set_vector_3f("textColor", color);
		glActiveTexture(GL_TEXTURE0);
		glBindVertexArray(m_VAO);

		for (auto c = text.begin(); c != text.end(); ++c) {
			character ch = m_characters[*c];

			GLfloat xpos = x + ch.bearing.x * scale;
			GLfloat ypos = y + (m_characters['H'].bearing.y - ch.bearing.y) * scale;

			GLfloat w = ch.size.x * scale;
			GLfloat h = ch.size.y * scale;

			GLfloat vertices[6][4] = {
				{xpos,		ypos + h,	0.0f, 1.0f},
				{xpos + w,	ypos,		1.0f, 0.0f},
				{xpos,		ypos,		0.0f, 0.0f},

				{ xpos,		ypos + h,	0.0f, 1.0f },
				{ xpos + w,	ypos + h,	1.0f, 1.0f },
				{ xpos + w,	ypos,		1.0f, 0.0f },
			};

			glBindTexture(GL_TEXTURE_2D, ch.texture_id);
			glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
			glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);

			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glDrawArrays(GL_TRIANGLES, 0, 6);

			x += (ch.advance >> 6) * scale;
		}

		glBindVertexArray(0);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

}