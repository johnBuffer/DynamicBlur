#include <iostream>
#include <SFML/Graphics.hpp>

#include "dynamic_blur.hpp"

int main()
{
    int WIN_WIDTH = 1600;
    int WIN_HEIGHT = 900;

    sf::RenderWindow window(sf::VideoMode(WIN_WIDTH, WIN_HEIGHT), "BLUR", sf::Style::Default);
    window.setVerticalSyncEnabled(false);
    window.setMouseCursorVisible(true);
    window.setKeyRepeatEnabled(true);

    sf::RenderTexture render_target;
	render_target.create(WIN_WIDTH, WIN_HEIGHT);

    sf::Texture texture;
    texture.loadFromFile("C:/Users/Jean/Documents/Code/cpp/DynamicBlur/img.jpg");

	Blur blur(WIN_WIDTH, WIN_HEIGHT);

    double time = 0;

	uint8_t intensity = 1;

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

		render_target.draw(sf::Sprite(texture));
		render_target.display();

		sf::RectangleShape rectangle(sf::Vector2f(250, 100));
		rectangle.setPosition(WIN_WIDTH / 2, WIN_HEIGHT / 2);
		rectangle.setRotation(time);
		render_target.draw(rectangle);
		render_target.display();
		
		for (int i(10); i--;)
		{
			render_target.draw(sf::Sprite(blur.apply2(render_target.getTexture(), 1)));
			render_target.display();
		}

        window.clear(sf::Color::Black);

		window.draw(sf::Sprite(blur.apply2(render_target.getTexture(), intensity)));

        window.display();
    }

    return 0;
}
