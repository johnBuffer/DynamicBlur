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
		// Init
		float scale = 1.0f/std::pow(2.0f, intensity);
		// Sprite of intput texture
		sf::Sprite input_sprite(texture);
		input_sprite.setScale(scale, scale);
		m_render_textures[1].draw(input_sprite);
		m_render_textures[1].display();

		while (scale < 1.0f)
		{
			// Width pass
			sf::Sprite pass_w_sprite(m_render_textures[1].getTexture());
			pass_w_sprite.setTextureRect(sf::IntRect(0, 0, m_width * scale, m_height * scale));
			pass_w_sprite.scale(2.0f, 2.0f);
			scale *= 2.0f;
			m_render_textures[0].draw(pass_w_sprite, &m_blur_w);
			m_render_textures[0].display();

			// Height pass
			sf::Sprite pass_h_sprite(m_render_textures[0].getTexture());
			pass_h_sprite.setTextureRect(sf::IntRect(0, 0, m_width * scale, m_height * scale));
			m_render_textures[1].draw(pass_h_sprite, &m_blur_h);
			m_render_textures[1].display();
		}

		return m_render_textures[1].getTexture();
	}

	const sf::Texture& apply2(const sf::Texture& texture, uint8_t intensity)
	{
		std::cout << int(intensity) << std::endl;

		sf::Sprite input_sprite(texture);
		m_render_textures[0].draw(input_sprite);
		m_render_textures[0].display();

		for (uint8_t i(0); i < intensity; ++i)
		{
			process(0, i);
		}

		return m_render_textures[0].getTexture();
		//return m_render_textures[intensity%2].getTexture();
	}

private:
	uint32_t m_width;
	uint32_t m_height;
	sf::Shader m_blur_w;
	sf::Shader m_blur_h;
	sf::RenderTexture m_render_textures[2];

	void process(uint8_t first, int it)
	{
		m_render_textures[1].clear();
		m_render_textures[1].display();

		// Downscale
		sf::Sprite down_sprite(m_render_textures[0].getTexture());
		down_sprite.scale(0.5f, 0.5f);
		m_render_textures[1].draw(down_sprite);
		m_render_textures[1].display();

		// Width pass
		sf::Sprite pass_w_sprite(m_render_textures[1].getTexture());
		pass_w_sprite.setTextureRect(sf::IntRect(0, 0, m_width * 0.5f, m_height * 0.5f));
		//pass_w_sprite.scale(2.0f, 2.0f);
		m_render_textures[0].draw(pass_w_sprite);
		m_render_textures[0].display();

		// Height pass
		/*sf::Sprite pass_h_sprite(m_render_textures[first].getTexture());
		//pass_h_sprite.scale(2.0f, 2.0f);
		m_render_textures[!first].draw(pass_h_sprite);
		m_render_textures[!first].display();*/
	}
};
