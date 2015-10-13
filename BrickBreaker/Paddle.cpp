/**
 * \file Paddle.cpp
 *
 * \author Gus Callaway
 *
 * \date 6/1/15
 *
 * \brief Implements a paddle, an object sub type
 */

#include <math.h>
#include "Paddle.h"

using namespace std;
using namespace sf;

Paddle::Paddle(GraphicsRunner& game, float xPos, float yPos, float width, float height, Color color)
        : game_(game),
          rectangle_(Vector2f(width, height)),
          leftCircle_(height / 2, 50),
          rightCircle_(height / 2, 50),
          vel_(0),
          accel_(0),
          timer_(time(0) - 30) // Just to make sure that the timer is behind us when we start
{
    // The origin of the paddle is on the top halfway along its width, the origin for the circles is their center
    rectangle_.setOrigin(width / 2, 0);
    leftCircle_.setOrigin(height / 2, height / 2);
    rightCircle_.setOrigin(height / 2, height / 2);

    // Position the circles on the ends of the rectangle
    rectangle_.setPosition(xPos, yPos);
    leftCircle_.setPosition(xPos - width / 2, yPos + height / 2);
    rightCircle_.setPosition(xPos + width / 2, yPos + height / 2);

    rectangle_.setFillColor(color);
    leftCircle_.setFillColor(color);
    rightCircle_.setFillColor(color);
}

void Paddle::draw(RenderWindow &window) const {
    window.draw(rectangle_);
    window.draw(leftCircle_);
    window.draw(rightCircle_);
}

void Paddle::move() {
    // If the elongation timer is over, return the paddle to normal
    if (difftime(time(0), timer_) == 0) {
        changeLength(false);
    }

    // A reference is needed for the call to collision()
    FloatRect leftCircle = leftCircle_.getGlobalBounds();
    FloatRect rightCircle = rightCircle_.getGlobalBounds();
    FloatRect rectangle = rectangle_.getGlobalBounds();

    // Paddle only needs to check for a collision with the barrier
    if (game_.getBarrier()->collision(leftCircle) != 'n' || game_.getBarrier()->collision(rightCircle) != 'n'
            || game_.getBarrier()->collision(rectangle) != 'n') {
        handleCollision();
    }

    // Apply acceleration
    vel_ += accel_;

    // Apply friction
    vel_ -= vel_/13;

    // Move the paddle left or right based on the velocity
    rectangle_.move(vel_, 0);

    // Move the circular sides to the edges of the paddle using some temporary variables to prevent unnecessary calls
    const Vector2f& size = rectangle_.getSize();
    Vector2f center(rectangle_.getPosition().x, rectangle_.getPosition().y + size.y/2);
    float rotation = rectangle_.getRotation()*.017453294f; // Convert to radians

    leftCircle_.setPosition(center.x - size.x/2*cosf(rotation), center.y - size.x/2*sinf(rotation));
    rightCircle_.setPosition(center.x + size.x/2*cosf(rotation), center.y + size.x/2*sinf(rotation));
}

const char Paddle::collision(sf::FloatRect& boundingBox) const {
    // First check for collision with the rectangular part of the paddle, so get that part's bounds
    FloatRect rectBounds = rectangle_.getGlobalBounds();

    // If the incoming object is within the paddle's bounding box and is between the sides, do further checking
    if (rectBounds.intersects(boundingBox) && boundingBox.left + boundingBox.width/2 > leftCircle_.getPosition().x
                                           && boundingBox.left + boundingBox.width/2 < rightCircle_.getPosition().x) {
        // This math calculates the distance of the center of the incoming object to the line cutting lengthwise through
        // the rectangle of the paddle.
        float ax = boundingBox.left + boundingBox.width/2 - leftCircle_.getPosition().x;
        float ay = leftCircle_.getPosition().y - (boundingBox.top + boundingBox.width/2);
        float bx = rightCircle_.getPosition().x - leftCircle_.getPosition().x;
        float by = leftCircle_.getPosition().y - rightCircle_.getPosition().y;
        float projFactor = (ax*bx + ay*by)/(bx*bx + by*by);
        float dist = powf(powf(ax - bx*projFactor, 2) + powf(ay - by*projFactor, 2), .5);

        // If the distance is less than the radius of the incoming object plus half the width of the paddle, it has hit
        if (dist < boundingBox.width/2 + rectangle_.getSize().y/2) {
            // a collision has occurred so store velocity and position data in the bounding box and return out
            boundingBox.left = rectangle_.getRotation(); // Rotation needed for the incoming ball's collision handling
            boundingBox.width = vel_;
            boundingBox.height = 0;
            return 'v';
        }
    }

    // Next check for possible side circle collision
    FloatRect leftCircleBounds = leftCircle_.getGlobalBounds();
    FloatRect rightCircleBounds = rightCircle_.getGlobalBounds();

    // This boolean prevents calling intersects() twice
    bool temp = leftCircleBounds.intersects(boundingBox);

    // If the ball hits one of the circles on the side of the paddle
    if ( temp || rightCircleBounds.intersects(boundingBox)) {
        Vector2f thisCenter;

        // If the left circle was hit
        if (temp) {
            thisCenter = leftCircle_.getPosition();
        }
            // If the right circle was hit
        else {
            thisCenter = rightCircle_.getPosition();
        }

        // Get the center of the incoming circle
        Vector2f otherCenter(boundingBox.left + boundingBox.width / 2, boundingBox.top + boundingBox.height / 2);

        // If the distance between the two circles is less than the sum of their radii
        if (powf(powf(thisCenter.x - otherCenter.x, 2) + powf(thisCenter.y - otherCenter.y, 2), .5)
            < boundingBox.width / 2 + rightCircle_.getRadius()) {

            // a collision has occurred so store velocity and position data in the bounding box and return out
            boundingBox.left = thisCenter.x;
            boundingBox.top = thisCenter.y;
            boundingBox.width = vel_;
            boundingBox.height = 0;
            return 's';
        }

    }

    return 'n';     // Otherwise no collision
}

void Paddle::processKey(const Event::EventType &type, const Keyboard::Key &key) {
    // Left or right movement keys
    if (key == Keyboard::J || key == Keyboard::L) {
        // Upon any key release stop the paddle from accelerating
        if (type == Event::KeyReleased) {
            // Only if the key released matches the current direction of acceleration
            if ((key == Keyboard::L && accel_ > 0) || (key == Keyboard::J && accel_ < 0)) {
                accel_ = 0;
            }
        }
        else {
            // If key is for right motion, then make accel_ be positive, otherwise negative
            accel_ = PADDLE_ACCELERATION*((key == Keyboard::L)*2-1);
        }
    }
    // Otherwise assumes either rotate clockwise or counterclockwise keys
    else {
        // Only consider the key press
        if (type == Event::KeyPressed) {
            // Rotate right when true, left when false
            rotate(key == Keyboard::D);
        }
    }
}

const sf::Vector2f* Paddle::getPos() const {
    return &rectangle_.getPosition();
}

void Paddle::handleCollision() {
    // If the paddle is on the left half of the stage, move it to the right
    if (rectangle_.getPosition().x < game_.windowSize_.x/2) {
        rectangle_.move(1, 0);
    }
    // Otherwise move it to the left
    else {
        rectangle_.move(-1, 0);
    }
    // and stop moving
    accel_ = 0;
    vel_ = 0;
}

void Paddle::rotate(bool direction) {
    // This turns rotations counter clockwise into negative degrees
    float rotation = rectangle_.getRotation();
    if (rotation > PADDLE_MAX_ROTATION) {
        rotation -= 360;
    }

    // Don't rotate further than the maximum paddle rotation
    if ((direction && rotation < PADDLE_MAX_ROTATION) || (!direction && rotation > -PADDLE_MAX_ROTATION)) {
        // 3 is the amount to increment for each key press
        rectangle_.rotate(3*(direction*2-1));
    }
}

void Paddle::changeLength(bool type) {
    // Only elongate if the existing timer has passed
    if (type && difftime(time(0), timer_) > 0) {
        // Elongate the paddle for the appropriate amount of time
        timer_ = time(0) + PADDLE_ELONGATION_TIME;
        rectangle_.setSize(Vector2f(PADDLE_WIDTH * PADDLE_ELONGATION_FACTOR, rectangle_.getSize().y));
        rectangle_.setOrigin(rectangle_.getSize().x / 2, 0);
    }
    else if (!type && rectangle_.getSize().x != PADDLE_WIDTH) {
        rectangle_.setSize(Vector2f(PADDLE_WIDTH, rectangle_.getSize().y));
        rectangle_.setOrigin(rectangle_.getSize().x / 2, 0);
    }
}
