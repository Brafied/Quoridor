#include "AIProgressBar.h"

AIProgressBar::AIProgressBar(sf::RenderWindow& window) : progress(0),
                                                         window(window) {}
                                            
void AIProgressBar::draw() const {
    sf::RectangleShape background({BAR_WIDTH, BAR_HEIGHT});
    background.setPosition(sf::Vector2f(X, Y));
    background.setFillColor(sf::Color(50, 50, 50));
    window.draw(background);
    sf::RectangleShape foreground({BAR_WIDTH * progress, BAR_HEIGHT});
    foreground.setPosition(sf::Vector2f(X, Y));
    foreground.setFillColor(sf::Color(100, 220, 100));
    window.draw(foreground);
}