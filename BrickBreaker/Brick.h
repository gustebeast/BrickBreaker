/**
 * \file Brick.hpp
 *
 * \author Gus Callaway
 *
 * \date 6/15/15
 *
 * \brief Declares a Brick, an object sub type
 */

#ifndef BRICKBREAKER_BRICK_H
#define BRICKBREAKER_BRICK_H


#include <SFML/Graphics/RectangleShape.hpp>
#include "Object.h"
#include "Constants.h"

/**
 * \class Brick
 * \brief A rectangular object that disappears when collided with
 */
class Brick : public Object {
public:
    /**
     * \brief Parametrized constructor for a brick
     *
     * \param xPos      The initial x location of the top left of the brick
     *        yPos      The initial y location of the top left of the brick
     *        width     The brick's width
     *        height    The brick's height
     *        special   The brick's special property (see comment on declaration of data member)
     *        color     The brick's fill color
     */
    Brick(float xPos, float yPos, float width, float height, char special = '\0',
          sf::Color color = BRICK_COLOR);

    /**
     * \brief Draws the brick onto a render window
     */
    void draw(sf::RenderWindow& window) const;

    /**
     * \brief Checks if a ball collided with the brick
     *
     * \details If the ball hit one of the bricks sides, just returns the proper collision character. If the ball hit a
     *      corner of the brick, stores the coordinates of the collided corner in the passed by reference
     *      boundingBox.
     *
     * \return 'v' or 'h' if a collision occurred with one of the sides, 'c' if a collision occurred with a corner.
     *      If it is a corner collision, "boundingBox" is updated with the following data:
     *      boundingBox.left stores the impact corner's x position
     *      boundingBox.top stores the impact corner's y position
     */
    const char collision(sf::FloatRect& boundingBox) const;

    /**
     * \brief A character representing the brick's special properties (or lack there of)
     *
     * \details ''  represents  a regular brick
     *          'b'             an extra ball brick
     *          's'             a safety brick
     */
    char special_;

private:
    sf::RectangleShape rectangle_;  ///< A brick is just a rectangle

};


#endif //BRICKBREAKER_BRICK_H
