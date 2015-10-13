/**
 * \file Ball.h
 *
 * \author Gus Callaway
 *
 * \date 6/1/15
 *
 * \brief Declares a Ball, an object sub type
 */

#ifndef BRICKBREAKER_BALL_H
#define BRICKBREAKER_BALL_H


#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/CircleShape.hpp>
#include "Object.h"
#include "Constants.h"
#include "GraphicsRunner.h"

/**
 * \class Ball
 * \brief A circle shaped object that moves
 */

class Ball : public Object {
public:
    /**
     * \brief Parametrized constructor for a ball
     *
     * \param game          The game instance this object lies within
     *        xPos          The initial x location of the ball's center
     *        yPos          The initial y location of the ball's center
     *        xVel          The ball's initial x velocity. Must be 0 if the ball is attached!
     *        yVel          The ball's initial y velocity. Must be 0 if the ball is attached!
     *        color         The ball's fill color (will be surrounded with a thin black outline
     *        radius        The ball's radius
     */
    Ball(GraphicsRunner& game, float xPos, float yPos, float xVel, float yVel, sf::Color color = BALL_COLOR,
         float radius = BALL_RADIUS);

    /**
     * \brief Constructor for balls that should be attached to a paddle
     *
     * \param game          The game instance this object lies within
     *        color         The ball's fill color (will be surrounded with a thin black outline
     *        radius        The ball's radius
     */
    Ball(GraphicsRunner& game, sf::Color color = BALL_COLOR, float radius = BALL_RADIUS);

    /**
     * \brief Draws the ball onto a render window
     */
    void draw(sf::RenderWindow& window) const;

    /**
     * \brief Adds x and y components of velocity to shape's position
     */
    void move();

    /**
     * \brief Checks if another ball has collided with this one
     *
     * \details Checks if the two ball's centers are within 2 radii apart, then stores the current balls position and
     *      velocity data in the passed by reference boundingBox and finally returns the proper collision character.
     *
     * \param boundingBox A reference to the rectangular bounding box for the incoming object. Also used as a way to
     *                    "return" more information than just the collision type.
     *
     * \return 'y' if a collision has occurred, 'n' otherwise.
     *      If a collision has occurred, "boundingBox" is updated with the following data:
     *      boundingBox.left stores this ball's x position
     *      boundingBox.top stores this ball's y position
     *      boundingBox.width stores this ball's x velocity
     *      boundingBox.height stores this ball's y velocity
     */
    const char collision(sf::FloatRect& boundingBox) const;

    /**
     * \brief Manually set the ball's velocity
     *
     * \param velocity  The new velocity for the ball
     */
    void setVelocity(const sf::Vector2f& velocity);

    /**
     * \brief Returns whether the ball is attached to a paddle
     */
    bool isAttached() const;

    /**
     * \brief Detaches the ball from its paddle. If not attached to a paddle, does nothing.
     */
    void detach();

    /**
     * \brief This boolean flips every frame to prevent redundancy in ball to ball collisions
     *
     * \details See implementation of handleBallCollisions()
     */
    bool collisionState_;


private:
    /**
     * \brief Handles ball-paddle collisions
     *
     * \details If the ball hits the top of the paddle, responds with a simple vertical collision, otherwise responds
     *      with collision handling for essentially a free ball hitting a rigid yet moving ball.
     *
     * \param bounds    The rectangular bounds of the ball.
     *
     * \return true if a collision occurs, false otherwise
     */
    bool handlePaddleCollisions(const sf::FloatRect& bounds);

    /**
     * \brief Handles ball-barrier collisions
     *
     * \details Uses simple 4 directional collision handling, responding with either a horizontal or vertical collision.
     *
     * \param bounds    The rectangular bounds of the ball.
     *
     * \return true if a collision occurs, false otherwise
     */
    bool handleBarrierCollisions(const sf::FloatRect& bounds);

    /**
     * \brief Handles collisions between a ball and a brick
     *
     * \details First checks to see if the ball hit a side of the brick (simple vertical or horizontal collision). Then
     *      checks all 4 corners of the brick in case the ball came in diagonally to an edge, and rebounds the ball
     *      appropriately based on the angle it came in.
     *
     * \param bounds    The rectangular bounds of the ball.
     * \param objects   List of all the game's objects. Used for getting all the game's bricks.
     *
     * \return true if a collision occurs, false otherwise
     */
    bool handleBrickCollisions(const sf::FloatRect& bounds, std::vector<Object*>& objects);

    /**
     * \brief Handles ball to ball collisions
     *
     * \details Checks if a collision has occurred with each ball, and if so, rotates into the collision
     *      frame and swaps the y velocities of the two balls.
     *
     * \note Assumes balls have equal mass and the collision is perfectly elastic
     *
     * \param bounds    The rectangular bounds of the (current) ball.
     * \param objects   List of all the game's objects. Used for getting all the game's balls.
     *
     * \return true if a collision occurs, false otherwise
     */
    bool handleBallCollisions(const sf::FloatRect& bounds, std::vector<Object*>& objects);

    /**
     * \brief Handles collisions with flat surfaces
     *
     * \details Checks if the collision was horizontal or vertical using the character, then flips the x or y velocity.
     *
     * \param c    The character representing the collision type.
     *
     * \return true if the collision was handled, false otherwise (no collision or complex collision)
     */
    bool handleSimpleCollision(const char& c);

    /**
     * \brief Gets the angle from the +x axis of a line perpendicular to one connecting the two given points.
     *
     * \details Uses arctan(x distance / y distance) to get proper angle.
     *
     * \param object1   The coordinates of the center of one object
     * \param object2   The coordinates of the center of the other object stored in a float rect (.left and .top)
     *
     * \return the contact angle in degrees
     */
    float getContactAngle(const sf::Vector2f& object1, const sf::FloatRect& object2) const;


    GraphicsRunner& game_;  ///< The game instance this object lies within

    sf::CircleShape circle_;

    sf::Vector2f vel_;  ///< The x and y velocity of the ball

    const sf::Vector2f* attachedPos_;    ///< A pointer to the position that the ball should match if it attached to an object
};


#endif //BRICKBREAKER_BALL_H
