/**
 * \file Barrier.cpp
 *
 * \author Gus Callaway
 *
 * \date 6/1/15
 *
 * \brief Implements a Barrier, an object sub type
 */

#include "Barrier.h"

using namespace sf;

Barrier::Barrier(float windowWidth, float windowHeight, float width, float buffer, Color color)
        : // Colon initialize the three rectangles that make up the barrier by setting their width and height.
          left_(Vector2f(width, windowHeight-buffer-BANNER_HEIGHT)),
          top_(Vector2f(windowWidth - 2 * buffer-width, width)), // 2*buffer for extra space on right and left
          right_(left_) // Right barrier has same dimensions as left one
{
    // Now that the rectangles are the right size, move them to the right position
    left_.setPosition(buffer, buffer + BANNER_HEIGHT);
    top_.setPosition(buffer, buffer + BANNER_HEIGHT);
    right_.setPosition(windowWidth-buffer-width, buffer + BANNER_HEIGHT);

    left_.setFillColor(color);
    top_.setFillColor(color);
    right_.setFillColor(color);
}

void Barrier::draw(sf::RenderWindow& window) const {
    // Simply draw the three rectangles
    window.draw(left_);
    window.draw(top_);
    window.draw(right_);
}

const char Barrier::collision(FloatRect& boundingBox) const {
    // Check if the object is colliding with the barrier's left right or top portion
    if (left_.getGlobalBounds().intersects(boundingBox) || right_.getGlobalBounds().intersects(boundingBox)) {
        return 'h';     // The other object has hit the barrier's sides, so it is a horizontal collision
    }
    else if (top_.getGlobalBounds().intersects(boundingBox)) {
        return 'v';     // The other object has hit the top of the barrier, so it is a vertical collision
    }
    else {
        return 'n';     // No collision has occurred
    }
}
