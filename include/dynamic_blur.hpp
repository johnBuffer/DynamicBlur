#pragma once
#include <SFML/Graphics.hpp>
#include <iostream>

const std::string vert_shader =
"void main()                                                  \
{                                                             \
	gl_TexCoord[0] = gl_TextureMatrix[0] * gl_MultiTexCoord0; \
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
    Blur(sf::Vector2u render_size, float scale, uint32_t iterations)
        : m_render_size(render_size)
        , m_render_scale(scale)
        , m_iterations(iterations)
    {
        createTextures();
        createShaders();
    }

    sf::Sprite& apply(const sf::Texture& texture)
    {
        // Retrieve texture content and downscale it
        sf::Sprite sprite(texture);
        const float downscale = 1.0f / static_cast<float>(1 << m_iterations);
        sprite.setScale(downscale, downscale);
        uint32_t current_buffer = 0;
        m_textures[current_buffer].draw(sprite);
        // Iteratively blur
        for (uint32_t i(m_iterations); i--;) {
            current_buffer = blurIteration(current_buffer);
        }
        m_result.setTexture(m_textures[current_buffer].getTexture());
        return m_result;
    }

private:
    sf::Vector2u      m_render_size;
    float             m_render_scale;
    uint32_t          m_iterations;
    sf::RenderTexture m_textures[2];

    // Shaders
    sf::Shader m_horizontal;
    sf::Shader m_vertical;
    sf::Sprite m_result;

    // Methods
    void createTextures()
    {
        // Initialize textures
        m_textures[0].create(m_render_size.x, m_render_size.y);
        m_textures[1].create(m_render_size.x, m_render_size.y);
        clearTextures();
        // Update clear
        m_textures[0].display();
        m_textures[1].display();
    }

    void createShaders()
    {
        m_horizontal.loadFromFile("../shaders/fragment.glsl", "../shaders/vertex.glsl");
        m_horizontal.loadFromMemory(vert_shader, w_shader);
        m_vertical.loadFromMemory(vert_shader, h_shader);

        m_horizontal.setUniform("WIDTH", static_cast<float>(m_render_size.x));
        m_vertical.setUniform("HEIGHT", static_cast<float>(m_render_size.y));
    }

    void clearTextures()
    {
        m_textures[0].clear(sf::Color::Black);
        m_textures[1].clear(sf::Color::Black);
    }

    uint32_t blurIteration(uint32_t source_buffer)
    {
        const uint32_t new_source = blurPass(source_buffer);
        sf::Sprite sprite(m_textures[new_source].getTexture());
        sprite.setScale(2.0f, 2.0f);
        m_textures[!new_source].draw(sprite);
        return !new_source;
    }

    uint32_t blurPass(uint32_t source_buffer)
    {
        sf::Sprite sprite(m_textures[source_buffer].getTexture());
        m_textures[!source_buffer].draw(sprite, &m_vertical);
        m_textures[source_buffer].draw(sprite, &m_horizontal);
        return source_buffer;
    }
};
