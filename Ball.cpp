/**
 * \file Ball.cpp
 *
 * \author Gus Callaway
 *
 * \date 6/1/15
 *
 * \brief Implements a Ball, an object sub type
 */

#include <math.h>
#include <iostream>
#include "Ball.h"
#include "Paddle.h"

using namespace sf;

Ball::Ball(GraphicsRunner& game, float xPos, float yPos, float xVel, float yVel, Color color, float radius)
        : game_(game),
          circle_(radius, 50),    // 50 is the number of points in the polygon approximation for the circle
          vel_(xVel, yVel),
          attachedPos_(nullptr),
          collisionState_(false)
{
    // Measure position from the center of the ball
    circle_.setOrigin(radius,radius);

    // Give the ball a thin outline
    circle_.setOutlineThickness(1);
    circle_.setOutlineColor(DEFAULT_COLOR);

    circle_.setFillColor(color);

    circle_.setPosition(xPos, yPos);
}

Ball::Ball(GraphicsRunner& game, Color color, float radius)
        : game_(game),
          circle_(radius, 50),   // 50 is the number of points in the polygon approximation for the circle
          vel_(0,0),
          attachedPos_(dynamic_cast<Paddle*>(game_.getPaddle())->getPos()),
          collisionState_(false)
{
    // Measure position from the center of the ball
    circle_.setOrigin(radius,radius);

    // Give the ball a thin outline
    circle_.setOutlineThickness(1);
    circle_.setOutlineColor(DEFAULT_COLOR);

    circle_.setFillColor(color);
}

void Ball::draw(sf::RenderWindow& window) const {
    window.draw(circle_);
}

void Ball::move() {
    // Deal with collision handling:

    // First save the ball's bounds
    FloatRect bounds = circle_.getGlobalBounds();

    // Quick check to see if the ball has left the stage
    if (bounds.top > game_.windowSize_.y) {
        // If it has, mark it for deletion.
        delete_ = true;
        return;
    }

    // Save a list of the game's objects for repeated access in collision checking
    std::vector<Object*>& objects = game_.getObjects();

    // This if statement allows for collision checking to terminate if the ball is attached, or if a collision occurs.
    if (isAttached()                                   ||      // Ensure the ball isn't attached
        (handleBarrierCollisions(bounds))           ||      // Check for barrier collision, if none,
        (handlePaddleCollisions(bounds))            ||      // Check for paddle collision, if none,
        (handleBrickCollisions(bounds, objects))    ||      // check for any brick collisions, if none,
        (handleBallCollisions(bounds, objects)))            // check for a collision with any of the balls.
    {
        // If the ball did collide (wasn't attached) then move it a bit extra to help prevent it from getting stuck
        if (!isAttached()) {
            circle_.move(vel_.x * .5f, vel_.y * .5f);
        }
    }

    // Collision check is done, so mark the ball as such
    collisionState_ = !collisionState_;

    // If the ball is attached, match its position to that of its attached object
    if (isAttached()) {
        circle_.setPosition(attachedPos_->x, attachedPos_->y - circle_.getRadius());
    }

    // Slow down the ball if it is moving too fast (prevents collision issues at high speeds)
    if (vel_.x > BALL_MAX_SPEED || vel_.y > BALL_MAX_SPEED) {
        vel_.x *= .99;
        vel_.y *= .99;
    }

    // After all collision handling is complete, move the ball
    circle_.move(vel_);
}

const char Ball::collision(FloatRect& boundingBox) const {
    // Attached balls do not collide
    if (isAttached()) {return 'n';}

    // Save the center of both balls for repeated access
    Vector2f thisCenter = circle_.getPosition();
    Vector2f otherCenter(boundingBox.left + boundingBox.width / 2, boundingBox.top + boundingBox.height / 2);

    // If the two balls are less than the sum of their radii apart, and not at the same location, they have collided.
    // Use radius/4 to prevent rounding issues where the distance should be 0 but is actually a very small number.
    float distance = powf(powf(thisCenter.x - otherCenter.x, 2) + powf(thisCenter.y - otherCenter.y, 2), .5);
    if (distance < boundingBox.width / 2 + circle_.getRadius() && distance > circle_.getRadius() / 4) {
        // Store position and velocity data for this ball in the referenced bounding box
        boundingBox.left = thisCenter.x;
        boundingBox.top = thisCenter.y;
        boundingBox.width = vel_.x;
        boundingBox.height = vel_.y;

        return 'y';
    }

    // Otherwise they have not
    return 'n';
}

void Ball::setVelocity(const Vector2f& velocity) {
    vel_ = velocity;
}

bool Ball::isAttached() const {
    return (attachedPos_ != nullptr);
}

void Ball::detach() {
    // Set the balls velocity downward
    vel_.y = BALL_MAX_SPEED*.75f;
    // and move it one step back upward so it will collide with the paddle and bounce at the appropriate angle
    circle_.move(-vel_);

    // Add a slight deviation to make the game a little harder
    vel_.x = ((rand() % 10) - 5) / 20.0f;

    // And detach the ball
    attachedPos_ = nullptr;
}

bool Ball::handlePaddleCollisions(const sf::FloatRect& bounds) {
    // Copy the other objects bounds since it could be modified by a call to collision()
    FloatRect boundsCopy = bounds;

    // Get the type of collision (vertical or circle).
    char c = game_.getPaddle()->collision(boundsCopy);

    // If the ball hit the top or bottom of the paddle
    if (c == 'v') {
        // The ball has collided with the circular sides of the paddle
        // Get the angle of collision so we can rotate into a simpler collision frame
        float contactAngle = -boundsCopy.left;

        // Create a vector out of the paddle's velocity
        Vector2f otherVel(boundsCopy.width, boundsCopy.height);

        // Create a rotation transformation and move the ball's and the paddle's velocity into the contact point frame
        Transform rotation;
        rotation.rotate(contactAngle);
        vel_ = rotation.transformPoint(vel_);
        otherVel = rotation.transformPoint(otherVel);

        // Once in this frame, invert this ball's y velocity and add the paddle's velocity to it
        vel_.y *= -1;
        vel_.y += otherVel.y * 1.1;

        // and rotate back into normal coordinates
        rotation.rotate(-2 * contactAngle);
        vel_ = rotation.transformPoint(vel_);

        // And move the ball slightly so it doesn't get stuck in the paddle
        //circle_.move(vel_);

        return true;
    }

    // If the ball hit the circular sides of the paddle
    else if (c == 's') {

        // The ball has collided with the circular sides of the paddle
        // Get the angle of collision so we can rotate into a simpler collision frame
        float contactAngle = getContactAngle(circle_.getPosition(), boundsCopy);

        // Create a vector out of the paddle's velocity
        Vector2f otherVel(boundsCopy.width, boundsCopy.height);

        // Create a rotation transformation and move both ball's velocity into the contact point frame
        Transform rotation;
        rotation.rotate(contactAngle);
        vel_ = rotation.transformPoint(vel_);
        otherVel = rotation.transformPoint(otherVel);

        // Once in this frame, invert this ball's y velocity and add the paddle's velocity to it
        vel_.y *= -1;
        vel_.y += otherVel.y * 1.1;

        // and rotate back into normal coordinates
        rotation.rotate(-2 * contactAngle);
        vel_ = rotation.transformPoint(vel_);

        // And move the ball slightly so it doesn't get stuck in the paddle
        //circle_.move(vel_);

        return true;
    }

    // Otherwise no collision has occurred
    return false;
};

bool Ball::handleBarrierCollisions(const FloatRect& bounds) {
    // Copy the other objects bounds since it could be modified by a call to collision()
    FloatRect boundsCopy = bounds;

    // Get the type of collision (vertical, horizontal or none).
    char c = game_.getBarrier()->collision(boundsCopy);

    // Colliding with a barrier is purely simple so just return the result of "handleSimpleCollision"
    return handleSimpleCollision(c);
}

bool Ball::handleBrickCollisions(const FloatRect& bounds, std::vector<Object*>& objects) {

    // Loop through all the brick objects (safety and regular)
    for (int i = game_.indexOfFirstSafetyBrick_;
         i < game_.indexOfFirstSafetyBrick_ + game_.numSafetyBricks_ + game_.numBricks_; ++i) {

        // Copy the other objects bounds since it could be modified by a call to collision()
        FloatRect boundsCopy = bounds;

        // Get the type of collision (vertical, horizontal or with a point)
        char c = objects[i]->collision(boundsCopy);

        // First get rid of the brick if necessary
        if (c != 'n') {
            // Collision occurred with the brick, mark it for deletion
            objects[i]->delete_ = true;
        }
        // Next handle the impact of the collision on the ball

        // If the collision is simple (ball hitting the sides of a brick) this call will handle it appropriately
        if (handleSimpleCollision(c)) {
            return true;
        }

        // If the ball hit a corner of the brick
        else if (c == 'c') {
            // Note: "boundsCopy" holds the coordinates of the corner collided (coming from the call to collision())

            // Get the angle of collision so we can rotate into a simpler collision frame
            float contactAngle = getContactAngle(circle_.getPosition(), boundsCopy);

            // Create a rotation transformation and move the ball's velocity into the contact point frame
            Transform rotation;
            rotation.rotate(contactAngle);
            vel_ = rotation.transformPoint(vel_);

            // Once in this frame, simply invert the ball's y velocity
            vel_.y *= -1;

            // and rotate back into normal coordinates
            rotation.rotate(-2 * contactAngle);
            vel_ = rotation.transformPoint(vel_);
            return true;
        }
        // Otherwise the ball did not collide with this brick
    }

    // The ball did not collide with any brick
    return false;
}

bool Ball::handleBallCollisions(const FloatRect& bounds, std::vector<Object*>& objects) {
    // Loop through all the ball objects
    // Balls occur after the index of the last brick
    for (int i = game_.indexOfFirstSafetyBrick_ + game_.numSafetyBricks_ + game_.numBricks_;
         i < objects.size(); ++i) {

        // Get the object
        Object* object = objects[i];

        if (Ball* ball = dynamic_cast<Ball*>(object)) {
            // If the other ball has already changed its collision state, it has done collision checking and thus
            // this ball does not need to check if it has collided with the other one
            if (ball->collisionState_ != collisionState_) {
                continue;
            }
        }

        // Copy the other objects bounds since it could be modified by a call to collision()
        FloatRect boundsCopy = bounds;

        // Get the type of collision (yes or no)
        char c = objects[i]->collision(boundsCopy);

        // Collision has occurred, bounds now holds the x and y coordinates and velocity of the incoming ball
        if (c == 'y') {
            // Get the angle of collision so we can rotate into a simpler collision frame
            float contactAngle = getContactAngle(circle_.getPosition(), boundsCopy);

            // Create a vector out of the other balls velocity
            Vector2f otherVel(boundsCopy.width, boundsCopy.height);

            // Create a rotation transformation and move both ball's velocity into the contact point frame
            Transform rotation;
            rotation.rotate(contactAngle);
            vel_ = rotation.transformPoint(vel_);
            otherVel = rotation.transformPoint(otherVel);

            // Once in this frame, simply swap the two ball's y velocities
            float temp = vel_.y;
            vel_.y = otherVel.y;
            otherVel.y = temp;

            // and rotate back into normal coordinates
            rotation.rotate(-2*contactAngle);
            vel_ = rotation.transformPoint(vel_);
            otherVel = rotation.transformPoint(otherVel);

            // Now dynamic cast the current object to a ball
            Ball* other = dynamic_cast<Ball*>(objects[i]);
            // and set the other ball's velocity to its new value
            other->setVelocity(otherVel);

            return true;
        }
        // Otherwise no collision so do nothing
    }

    return false;
}

bool Ball::handleSimpleCollision(const char& c) {
    // Vertical collision, flip y velocity
    if (c == 'v') {
        vel_.y *= -1;
        return true;
    }

    // Horizontal collision, flip x velocity
    else if (c == 'h') {
        vel_.x *= -1;
        return true;
    }

    // Otherwise, either no collision has occurred or more complex collision handling is needed.
    return false;
}

float Ball::getContactAngle(const sf::Vector2f& object1, const sf::FloatRect& object2) const {
    // The contact angle represents the angle from the positive x axis to the line tangent to the contact point
    float contactAngle;

    // Pre-calculate the denominator before calculating the arctan to handle the case of a 90° contact angle
    float denominator = object1.y - object2.top;

    // In a purely tangential collision the contact angle is 90°
    if (denominator == 0) {
        contactAngle = 1.570796327f;
    }
    // Otherwise need to use arctan to find the angle
    else {
        contactAngle =
                atanf((object1.x - object2.left) / denominator);
    }

    return contactAngle * 57.295779513f;   // Angle must be in degrees to be used by the rotate() function
}
