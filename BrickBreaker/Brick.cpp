/**
 * \file Brick.cpp
 *
 * \author Gus Callaway
 *
 * \date 6/15/15
 *
 * \brief Implements a brick, an object sub type
 */
#include <SFML/Graphics/CircleShape.hpp>
#include <math.h>
#include "Brick.h"

using namespace sf;

Brick::Brick(float xPos, float yPos, float width, float height, char special, Color color)
        : rectangle_(Vector2f(width, height)), special_(special)
{
    rectangle_.setPosition(xPos, yPos);

    // Safety bricks are colored red, otherwise use the passed color
    if (special_ == 's') {
        rectangle_.setFillColor(SAFETY_BRICK_COLOR);
    }

    // Special bricks have a special color
    else if (special_ != '\0') {
        //rectangle_.setFillColor(Color(0xFFFFFF00u ^ color.toInteger())); // This uses an inverted regular brick color
        rectangle_.setFillColor(SPECIAL_BRICK_COLOR);
    }

    // Normal bricks just use the normal fill color
    else {
        rectangle_.setFillColor(color);
    }
}

void Brick::draw(sf::RenderWindow& window) const {
    window.draw(rectangle_);
}

const char Brick::collision(sf::FloatRect& boundingBox) const {
    // Save the current object's bounds and the center of the incoming object for repeated access
    FloatRect bounds = rectangle_.getGlobalBounds();
    Vector2f center(boundingBox.left + boundingBox.width / 2, boundingBox.top + boundingBox.height / 2);

    // Also since the incoming object must be a ball, we get its radius using the boundingBox
    float radius = boundingBox.width / 2;

    // Ensure the two collision boxes at least overlap (not necessarily a collision though)
    if (bounds.intersects(boundingBox)) {

        // If the brick contains the top or bottom middle point of the incoming object
        if (bounds.contains(center.x, boundingBox.top)
                || bounds.contains(center.x, boundingBox.top + boundingBox.height)) {

            return 'v';     // it is a vertical collision with the top or bottom of the brick
        }

        // If the brick contains the left or right middle point of the incoming object
        else if (bounds.contains(boundingBox.left, center.y)
            || bounds.contains(boundingBox.left + boundingBox.width, center.y)) {

            return 'h';     // it is a horizontal collision with the left or right side of the brick
        }

        // If the incoming object hits the top left corner of the brick
        else if (pow(pow(center.x - bounds.left, 2) + pow(center.y - bounds.top, 2), .5) < radius) {

            // Save the coordinates of the contact point
            boundingBox.left = bounds.left;
            boundingBox.top = bounds.top;
            return 'c';
        }

        // If the incoming object hits the top right corner of the brick
        else if (pow(pow(center.x - (bounds.left + bounds.width), 2) + pow(center.y - bounds.top, 2), .5) < radius) {

            // Save the coordinates of the contact point
            boundingBox.left = bounds.left + bounds.width;
            boundingBox.top = bounds.top;
            return 'c';
        }

        // If the incoming object hits the bottom right corner of the brick
        else if (pow(pow(center.x - (bounds.left + bounds.width), 2)
                     + pow(center.y - (bounds.top + bounds.height), 2), .5) < radius) {

            // Save the coordinates of the contact point
            boundingBox.left = bounds.left + bounds.width;
            boundingBox.top = bounds.top + bounds.height;
            return 'c';
        }

        // If the incoming object hits the bottom left corner of the brick
        else if (pow(pow(center.x - bounds.left, 2) + pow(center.y - (bounds.top + bounds.height), 2), .5) < radius) {

            // Save the coordinates of the contact point
            boundingBox.left = bounds.left;
            boundingBox.top = bounds.top + bounds.height;
            return 'c';
        }

        // The corner of the collision box intersects the brick, but the ball itself is not touching so no collision
    }

    return 'n';     // No collision has occurred
}
