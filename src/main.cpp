#include <SFML/Graphics.hpp>
#include "dynamic_blur.hpp"
#include "event_manager.hpp"


int main()
{
	constexpr uint32_t WIN_WIDTH  = 1600;
    constexpr uint32_t WIN_HEIGHT = 900;

    sf::RenderWindow window(sf::VideoMode(WIN_WIDTH, WIN_HEIGHT), "TEST", sf::Style::Default);
    window.setVerticalSyncEnabled(false);

    sf::RenderTexture render_target, bloom;
	render_target.create(WIN_WIDTH, WIN_HEIGHT);
	bloom.create(WIN_WIDTH, WIN_HEIGHT);

	Blur blur({WIN_WIDTH, WIN_HEIGHT}, 1.0f, 5);

    sfev::EventManager event_manager(window, true);
    event_manager.addKeyPressedCallback(sf::Keyboard::Escape, [&](sfev::CstEv){window.close();});
    event_manager.addEventCallback(sf::Event::Closed, [&](sfev::CstEv){window.close();});

    while (window.isOpen())
    {
        event_manager.processEvents();

        const sf::Vector2i mouse_position = sf::Mouse::getPosition(window);

		render_target.clear();
        const float circle_radius = 100.0f;
        sf::CircleShape mouse_circle(circle_radius);
        mouse_circle.setOrigin(circle_radius, circle_radius);
        mouse_circle.setPosition(static_cast<float>(mouse_position.x), static_cast<float>(mouse_position.y));
        render_target.draw(mouse_circle);
        render_target.display();

        window.clear(sf::Color::Black);

        window.draw(sf::Sprite(render_target.getTexture()));
        window.draw(blur.apply(render_target.getTexture()), sf::BlendAdd);

        window.display();
    }

    return 0;
}
