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
        uint32_t current_buffer = 0;
        m_textures[current_buffer].draw(sprite);
        //current_buffer = blurIteration(current_buffer, 1);
        for (uint32_t i(m_iterations); i--;) {
            current_buffer = blurPass(current_buffer, m_iterations - i);
        }
        for (uint32_t i(m_iterations); i--;) {
            current_buffer = blurPass(current_buffer, i);
        }
        m_textures[current_buffer].display();
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
        m_textures[0].setSmooth(true);
        m_textures[1].setSmooth(true);
        // Update clear
        m_textures[0].display();
        m_textures[1].display();
    }

    void createShaders()
    {
        m_horizontal.loadFromFile("../shaders/vertex.glsl", "../shaders/fragment_hori.glsl");
        m_vertical.loadFromFile("../shaders/vertex.glsl", "../shaders/fragment_vert.glsl");
        // Set pixel steps in shader
        m_horizontal.setUniform("WIDTH", static_cast<float>(m_render_size.x));
        m_vertical.setUniform("HEIGHT", static_cast<float>(m_render_size.y));
    }

    void clear(uint32_t texture_id)
    {
        m_textures[texture_id].clear(sf::Color::Black);
    }

    void draw(const sf::Sprite& sprite, uint32_t dest_buffer, const sf::Shader& shader)
    {
        clear(dest_buffer);
        m_textures[dest_buffer].draw(sprite, &shader);
    }

    void draw(const sf::Sprite& sprite, uint32_t dest_buffer)
    {
        clear(dest_buffer);
        m_textures[dest_buffer].draw(sprite);
    }

    const sf::Texture& getTexture(uint32_t source_buffer)
    {
        return m_textures[source_buffer].getTexture();
    }

    uint32_t blurPass(uint32_t source_buffer, uint32_t downscale)
    {
        // Initialize scales and rectangle
        const float inv_scale = static_cast<float>(1 << downscale);
        const float scale = 1.0f / inv_scale;
        const int32_t current_pass_size_x = m_render_size.x >> downscale;
        const int32_t current_pass_size_y = m_render_size.y >> downscale;
        // Draw from source to target with separate blur passes
        sf::Sprite sprite;
        sprite.setScale(scale, scale);
        sprite.setTexture(getTexture(source_buffer));
        draw(sprite, !source_buffer);

        sprite.setScale(1.0f, 1.0f);
        sprite.setTexture(getTexture(!source_buffer));
        sprite.setTextureRect({0, 0, current_pass_size_x, current_pass_size_y});
        draw(sprite, source_buffer, m_horizontal);

        sprite.setTexture(getTexture(source_buffer));
        draw(sprite, !source_buffer, m_vertical);

        const float safe_scale = 1.0f;
        sprite.setScale(inv_scale * safe_scale, inv_scale * safe_scale);
        sprite.setTexture(getTexture(!source_buffer));
        draw(sprite, source_buffer);

        return source_buffer;
    }
};
