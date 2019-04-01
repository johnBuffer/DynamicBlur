#pragma once
#include <SFML/Graphics.hpp>
#include <iostream>

class Blur
{
public:
	Blur(uint32_t width, uint32_t height) :
		m_width(width),
		m_height(height)
	{
		m_render_textures[0].create(width, height);
		m_render_textures[1].create(width, height);

		m_blur_w.loadFromFile("C:/Users/Jean/Documents/Code/cpp/DynamicBlur/vert.vert", "C:/Users/Jean/Documents/Code/cpp/DynamicBlur/blur_w.frag");
		m_blur_h.loadFromFile("C:/Users/Jean/Documents/Code/cpp/DynamicBlur/vert.vert", "C:/Users/Jean/Documents/Code/cpp/DynamicBlur/blur_h.frag");
		
		m_blur_w.setUniform("WIDTH" , float(width));
		m_blur_w.setUniform("HEIGHT", float(height));

		m_blur_h.setUniform("WIDTH" , float(width));
		m_blur_h.setUniform("HEIGHT", float(height));
	}

	const sf::Texture& apply(const sf::Texture& texture, uint8_t intensity)
	{
		// Sprite of intput texture
		sf::Sprite input_sprite(texture);
		m_render_textures[0].draw(input_sprite);
		m_render_textures[0].display();

		float scale(1.0f);
		for (uint8_t i(0); i < intensity; ++i)
		{
			process(0, scale, scale*0.5f);
			scale *= 0.5f;
		}

		for (uint8_t i(0); i < intensity; ++i)
		{
			process(0, scale, scale*2.0f);
			scale *= 2.0f;
		}

		return m_render_textures[0].getTexture();
	}

private:
	uint32_t m_width;
	uint32_t m_height;
	sf::Shader m_blur_w;
	sf::Shader m_blur_h;
	sf::RenderTexture m_render_textures[2];

	void process(uint8_t first, float in_scale, float out_scale)
	{
		float scale_fact = out_scale / in_scale;
		// Width pass
		sf::Sprite pass_w_sprite(m_render_textures[first].getTexture());
		pass_w_sprite.setTextureRect(sf::IntRect(0, 0, m_width * in_scale, m_height * in_scale));
		pass_w_sprite.scale(scale_fact, scale_fact);
		m_render_textures[!first].draw(pass_w_sprite, &m_blur_w);
		m_render_textures[!first].display();

		// Height pass
		sf::Sprite pass_h_sprite(m_render_textures[!first].getTexture());
		m_render_textures[first].draw(pass_h_sprite, &m_blur_h);
		m_render_textures[first].display();
	}
};
