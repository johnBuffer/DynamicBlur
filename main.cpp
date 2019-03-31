#include <SFML/Graphics.hpp>

void downscale(sf::RenderTexture& tex, sf::RenderTexture& tmp)
{
	// Downscale
	sf::Sprite down_sprite(tex.getTexture());
	down_sprite.scale(0.5f, 0.5f);
	tmp.draw(down_sprite);
	tmp.display();

	// Upscale
	sf::Sprite up_sprite(tmp.getTexture());
	up_sprite.scale(2.0f, 2.0f);
	tex.draw(up_sprite);
	tex.display();
}

int main()
{
	constexpr uint32_t  WIN_WIDTH = 1600;
    constexpr uint32_t WIN_HEIGHT = 900;

    sf::RenderWindow window(sf::VideoMode(WIN_WIDTH, WIN_HEIGHT), "TEST", sf::Style::Default);
    window.setVerticalSyncEnabled(false);
    window.setKeyRepeatEnabled(true);

    sf::RenderTexture render_target, tmp;
	render_target.create(WIN_WIDTH, WIN_HEIGHT);
	tmp.create(WIN_WIDTH, WIN_HEIGHT);

    float time = 0.0f;
	uint8_t intensity = 0;

    while (window.isOpen())
    {
        sf::Vector2i mousePosition = sf::Mouse::getPosition(window);

        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();

            if (event.type == sf::Event::KeyPressed)
            {
                switch (event.key.code)
                {
                case sf::Keyboard::Escape:
                    window.close();
                    break;
                case sf::Keyboard::A:
                    ++intensity;
					break;
				case sf::Keyboard::E:
					--intensity;
					break;
                default:
                    break;
                }
            }
        }

        time += 0.1;

		render_target.clear();

		// Debug rectangle
		sf::RectangleShape rectangle(sf::Vector2f(250, 100));
		rectangle.setPosition(WIN_WIDTH / 2, WIN_HEIGHT / 2);
		rectangle.setRotation(time);

		render_target.draw(rectangle);
		render_target.display();

		for (uint8_t i(0); i < intensity; ++i)
		{
			downscale(render_target, tmp);
		}

        window.clear(sf::Color::Black);

		window.draw(sf::Sprite(render_target.getTexture()));

        window.display();
    }

    return 0;
}
