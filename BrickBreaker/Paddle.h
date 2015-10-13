/**
 * \file Paddle.h
 *
 * \author Gus Callaway
 *
 * \date 6/1/15
 *
 * \brief Declares a paddle, an object sub type
 */
#ifndef BRICKBREAKER_PADDLE_H
#define BRICKBREAKER_PADDLE_H

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Event.hpp>
#include "Object.h"
#include "Ball.h"
#include "Constants.h"
#include "GraphicsRunner.h"

/**
 * \class Paddle
 * \brief A thin paddle that can be moved along the bottom of the screen using arrow keys
 */
class Paddle : public Object {
public:
    /**
     * \brief Parametrized constructor for a paddle
     *
     * \param game      The game instance this object lies within
     *        xPos      The initial x location of the middle of the paddle
     *        yPos      The initial y location of the top of the paddle
     *        width     The paddle's width (actual width will be width+height due to circles on the paddle's sides)
     *        height    The paddle's height
     *        color     The paddle's fill color
     */
    Paddle(GraphicsRunner& game, float xPos, float yPos, float width, float height,
           sf::Color color = DEFAULT_COLOR);

    /**
     * \brief Draws the paddle onto a render window
     */
    void draw(sf::RenderWindow& window) const;

    /**
     * \brief Moves the paddle left or right based on its velocity and runs collision checking with the barrier
     *
     * \details Also attempts to simulate acceleration and deceleration to make for more smooth movement
     */
    void move();

    /**
     * \brief Checks if a ball is colliding with the paddle
     *
     * \details If the ball hit the top of the paddle, it is a simple vertical collision. If the ball hit one of the
     *      paddle's circular sides, update the passed by reference boundingBox to contain the center of the impacted
     *      circle and the paddle's velocity.
     *
     * \param boundingBox A reference to the rectangular bounding box for the incoming object. Also used as a way to
     *                    "return" more information than just the collision type.
     *
     * \return 'v' if the ball hit the top or bottom of the paddle, 's' if it hit the sides, 'n' otherwise.
     *      If a collision has occurred, "boundingBox" is updated with the following data:
     *      boundingBox.left stores the collided circle's x position
     *      boundingBox.top stores the collided circle's y position
     *      boundingBox.width stores this paddle's x velocity
     *      boundingBox.height stores this paddle's y velocity
     */
    const char collision(sf::FloatRect& boundingBox) const;

    /**
     * \brief Handles user input to move the paddle left and right
     *
     * \details Attempts to implement friction and inertial forces to make paddle movement more natural
     *
     * \note Both parameters are assumed to be one of their proper values (key press/release and left/right arrow)
     *
     * \param type  Either a key release or key press.
     * \param key   Either a left arrow or right arrow.
     */
    void processKey(const sf::Event::EventType& type, const sf::Keyboard::Key& key);


    /**
     * \brief Returns a vector representing the position of the top center of the paddle
     */
    const sf::Vector2f* getPos() const;

    /**
     * \brief Rotates the paddle slightly either clockwise or counter clockwise
     *
     * \note Pivots 3 degrees around the top center point of the paddle, but wont go past the maximum allowed angle
     *
     * \param direction true to rotate one tick clockwise, false to rotate counter clockwise
     */
    void rotate(bool direction);

    /**
     * \brief Either initiates a paddle elongation or returns a paddle to normal length
     *
     * \details Also moves the paddle if it is too far left or right to prevent getting stuck in the barrier
     *
     * \param type  true if the paddle is to be elongated
     *              false if the paddle is to be returned to its normal state
     */
    void changeLength(bool type);

private:
    /**
     * \brief Handles paddle-barrier collisions
     *
     * \details Moves away from the collided wall and stops moving
     */
    void handleCollision();

    GraphicsRunner& game_;   ///< The game instance this object lies within

    sf::RectangleShape rectangle_;
    sf::CircleShape leftCircle_;
    sf::CircleShape rightCircle_;

    float vel_;         ///< Y velocity will always be 0 since the paddle can only move left and right
    float accel_;       ///< The x acceleration of the paddle

    time_t timer_;      ///< Used to handle paddle elongation timing
};

#endif //BRICKBREAKER_PADDLE_H
