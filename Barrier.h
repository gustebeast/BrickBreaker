/**
 * \file Barrier.h
 *
 * \author Gus Callaway
 *
 * \date 6/1/15
 *
 * \brief Declares a Barrier, an object sub type
 */

#ifndef BRICKBREAKER_BARRIER_H
#define BRICKBREAKER_BARRIER_H

#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/ConvexShape.hpp>
#include "Object.h"
#include "Constants.h"

/**
 * \class Barrier
 * \brief A thin "n" shaped wall used to block the top, left and right sides of a window and define a field of play.
 */

class Barrier : public Object {
public:
    /**
     * \brief Parametrized constructor for a barrier
     *
     * \param windowWidth   The width of the game window
     *        windowHeight  The height of the game window
     *        width         The thickness of the barrier
     *        buffer        The distance of the barrier from the edges of the graphical window
     *        color         The barrier's fill color
     */
    Barrier(float windowWidth, float windowHeight, float width, float buffer, sf::Color color = DEFAULT_COLOR);

    /**
     * \brief Draws the barrier onto a render window
     */
    void draw(sf::RenderWindow& window) const;

    /**
     * \brief Checks if the passed boundingBox intersects the bounds of the barrier
     *
     * \return 'h' if the boundingBox intersects the barrier's top wall
     *         'v' if it intersects the barrier's left or right walls
     *         'n' otherwise
     */
    const char collision(sf::FloatRect& boundingBox) const;

private:
    ///< The three walls that make up the barrier
    sf::RectangleShape left_;
    sf::RectangleShape top_;
    sf::RectangleShape right_;
};

#endif //BRICKBREAKER_BARRIER_H
