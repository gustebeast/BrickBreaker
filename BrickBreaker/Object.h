/**
 * \file Object.hpp
 *
 * \author Gus Callaway
 *
 * \date 6/1/15
 *
 * \brief Declares an Object, an abstract class
 */

#ifndef BRICKBREAKER_OBJECT_H
#define BRICKBREAKER_OBJECT_H

#include <SFML/Graphics/RenderWindow.hpp>

/**
 * \class Object
 * \brief An abstract class to categorize objects that can be drawn and possibly moved
 */

class Object {
public:
    /**
     * \brief Just a default destructor since no object has any memory allocated in the heap
     */
    virtual ~Object() = default;

    /**
     * \brief Draws the object onto a render window.
     */
    virtual void draw(sf::RenderWindow& window) const = 0;

    /**
     * \brief Updates the object's position (could mean not moving at all). Normally ran every frame.
     */
    virtual void move() {};

    /**
     * \brief   Determines if another object collides with this one and returns the collision type
     *
     * \details Uses SFML's built in intersects() method to determine if two object's boundary boxes overlap,
     *      then determines the relative positions of the objects to return the collision type. For corner and circle
     *      collisions, the "top" and "left" values of the referenced boundingBox are updated to store the value of the
     *      corner's coordinates or the other ball's initial velocity. This data is then used in collision handling for
     *      the other object.
     *
     * \returns One of the following characters:
     *      'n' : No collision occurred
     *      'y' : A collision occurred, but not in a simple direction (i.e. two circles)
     *      'v' : The other object hit this object's top or bottom edge (a vertical collision)
     *      'h' : The other object hit this object's left or right edge (a horizontal collision)
     *      'c' : The other object hit a corner of this object
     */
    virtual const char collision(sf::FloatRect& boundingBox) const = 0;

    /**
     * \brief If true, this object should be deleted on the next frame. Otherwise it should not.
     */
    bool delete_ = false;
};


#endif //BRICKBREAKER_OBJECT_H
