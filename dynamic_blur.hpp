#pragma once
#include <SFML/Graphics.hpp>
#include <iostream>

const std::string vert_shader =
"void main()                                                  \
{                                                             \
	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;   \
	gl_TexCoord[0] = gl_TextureMatrix[0] * gl_MultiTexCoord0; \
	gl_FrontColor = gl_Color;                                 \
}";

const std::string w_shader =
"uniform sampler2D texture;                                      \
uniform float WIDTH;                                             \
vec4 weight = vec4(0.006, 0.061, 0.242, 0.383);                  \
float WIDTH_STEP = 1.0 / WIDTH;                                  \
void main()                                                      \
{                                                                \
	vec2 pos = gl_TexCoord[0].xy;                                \
	vec2 offset = vec2(WIDTH_STEP, 0.0);                         \
	vec4 color = texture2D(texture, pos) * weight[3];            \
	color += texture2D(texture, pos + offset * 1.5) * weight[2]; \
	color += texture2D(texture, pos + offset * 2.5) * weight[1]; \
	color += texture2D(texture, pos + offset * 3.5) * weight[0]; \
	color += texture2D(texture, pos - offset * 1.5) * weight[2]; \
	color += texture2D(texture, pos - offset * 2.5) * weight[1]; \
	color += texture2D(texture, pos - offset * 3.5) * weight[0]; \
	gl_FragColor = vec4(color.xyz, 1.0);                         \
}";

const std::string h_shader =
"uniform sampler2D texture;                                      \
uniform float HEIGHT;                                            \
vec4 weight = vec4(0.006, 0.061, 0.242, 0.383);                  \
float HEIGHT_STEP = 1.0 / HEIGHT;                                \
void main()                                                      \
{                                                                \
	vec2 pos = gl_TexCoord[0].xy;                                \
	vec2 offset = vec2(0.0, HEIGHT_STEP);                        \
	vec4 color = texture2D(texture, pos) * weight[3];            \
	color += texture2D(texture, pos + offset * 1.5) * weight[2]; \
	color += texture2D(texture, pos + offset * 2.5) * weight[1]; \
	color += texture2D(texture, pos + offset * 3.5) * weight[0]; \
	color += texture2D(texture, pos - offset * 1.5) * weight[2]; \
	color += texture2D(texture, pos - offset * 2.5) * weight[1]; \
	color += texture2D(texture, pos - offset * 3.5) * weight[0]; \
	gl_FragColor = vec4(color.xyz, 1.0);                         \
}";

class Blur
{
public:
	Blur(uint32_t width, uint32_t height, float quality = 1.0f) :
		m_quality(quality),
		m_width(uint32_t(width * quality)),
		m_height(uint32_t(height * quality))
	{
		m_render_textures[0].create(m_width, m_height);
		m_render_textures[1].create(m_width, m_height);

		m_render_textures[0].setSmooth(true);
		m_render_textures[1].setSmooth(true);

		m_blur_w.loadFromMemory(vert_shader, w_shader);
		m_blur_h.loadFromMemory(vert_shader, h_shader);
		
		m_blur_w.setUniform("WIDTH" , float(m_width));
		m_blur_h.setUniform("HEIGHT", float(m_height));
	}

	const sf::Sprite apply(const sf::Texture& texture, uint8_t intensity)
	{
		// Sprite of intput texture
		sf::Sprite input_sprite(texture);
		input_sprite.scale(m_quality, m_quality);
		m_render_textures[0].draw(input_sprite);
		m_render_textures[0].display();

		const uint32_t inv_quality(1.0f / m_quality);
		float scale(1.0f);
		for (uint8_t i(0); i < intensity; ++i)
		{
			float fact(0.5f);
			if (i<intensity-1)
			{
				fact = 0.25f;
				++i;
			}

			process(0, scale, scale*fact);
			scale *= fact;
		}

		for (uint8_t i(0); i < intensity; ++i)
		{
			process(0, scale, scale*2.0f);
			scale *= 2.0f;
		}

		sf::Sprite result(m_render_textures[0].getTexture());
		result.scale(inv_quality, inv_quality);

		return result;
	}

private:
	const float m_quality;
	const uint32_t m_width;
	const uint32_t m_height;
	sf::Shader m_blur_w;
	sf::Shader m_blur_h;
	sf::RenderTexture m_render_textures[2];

	void process(uint8_t first, float in_scale, float out_scale)
	{
		float scale_fact = out_scale / in_scale;
		// Width pass
		sf::Sprite pass_w_sprite(m_render_textures[first].getTexture());
		pass_w_sprite.setTextureRect(sf::IntRect(0, 0, uint32_t(m_width * in_scale), uint32_t(m_height * in_scale)));
		pass_w_sprite.scale(scale_fact, scale_fact);
		m_render_textures[!first].draw(pass_w_sprite, &m_blur_w);
		m_render_textures[!first].display();

		// Height pass
		sf::Sprite pass_h_sprite(m_render_textures[!first].getTexture());
		m_render_textures[first].draw(pass_h_sprite, &m_blur_h);
		m_render_textures[first].display();
	}
};
