# DynamicBlur
Configurable realtime blur of any sf::Texture.

## Installation

1. Include blur header 
```cpp
#include "dynamic_blur.hpp"
```

2. Instanciate a Blur object with __texture's dimensions__
```cpp
// Width and Height are texture's dimensions
Blur blur(tex_width, tex_height);
```

3. Apply the filter
```cpp
sf::Sprite filtered(blur.apply(bloom.getTexture(), intensity);
```
Where intensity is the number of __blur iterations__.

## Define a region

If you only need to blur a part of your texture you can define a region

### 1. Define a region

```cpp
// Retrieve mouse coordinates
sf::Vector2i mousePosition (sf::Mouse::getPosition(window));

// Define region
uint32_t region_width(512);
uint32_t region_height(256);
blur.setRegion(mousePosition.x - region_width/2, mousePosition.y - region_height/2, region_width, region_height);
```

### 2. Result
![region](https://github.com/johnBuffer/DynamicBlur/blob/master/img/region_blur.png)

## Quick bloom example

```cpp
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

    Blur blur(WIN_WIDTH, WIN_HEIGHT);

    float time = 0.0f;
    
    uint8_t intensity = 4;
    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        time += 0.1;
	
	render_target.clear();
        bloom.clear();
        
	// Draw a rotating rectangle
        sf::RectangleShape rectangle(sf::Vector2f(250, 100));
        rectangle.setPosition(WIN_WIDTH / 2, WIN_HEIGHT / 2);
        rectangle.setRotation(time);
        render_target.draw(rectangle);
        render_target.display();
	
	// Add it to the bloom target
        bloom.draw(rectangle);
        bloom.display();

        window.clear(sf::Color::Black);

        window.draw(sf::Sprite(render_target.getTexture()));
	// Add bloom
        window.draw(blur.apply(bloom.getTexture(), intensity), sf::BlendAdd);

        window.display();
    }

    return 0;
}

```
### Result
![bloom](https://github.com/johnBuffer/DynamicBlur/blob/master/img/bloom.png)

## Future work
 - [x] Remove border artifacts
 - [x] Allow region blur
 - [ ] Support per pixel regions
