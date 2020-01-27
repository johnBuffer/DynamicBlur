#include <SFML/Graphics.hpp>
#include "dynamic_blur.hpp"

int main()
{
	constexpr uint32_t  WIN_WIDTH = 1600;
    constexpr uint32_t WIN_HEIGHT = 900;

    sf::RenderWindow window(sf::VideoMode(WIN_WIDTH, WIN_HEIGHT), "TEST", sf::Style::Default);
    window.setVerticalSyncEnabled(false);
    window.setKeyRepeatEnabled(true);

    sf::RenderTexture render_target, bloom;
	render_target.create(WIN_WIDTH, WIN_HEIGHT);
	bloom.create(WIN_WIDTH, WIN_HEIGHT);

	Blur blur(WIN_WIDTH, WIN_HEIGHT, 0.25f);

    float time = 0.0f;
	uint8_t intensity = 0;

    while (window.isOpen())
    {
        sf::Vector2i mousePosition = sf::Mouse::getPosition(window);

		uint32_t region_width(512);
		uint32_t region_height(256);
		blur.setRegion(mousePosition.x - region_width/2, mousePosition.y - region_height/2, region_width, region_height);

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

		bloom.draw(rectangle);
		bloom.display();

        window.clear(sf::Color::Black);

		window.draw(sf::Sprite(render_target.getTexture()));
		window.draw(blur.apply(render_target.getTexture(), intensity));

        window.display();
    }

    return 0;
}
