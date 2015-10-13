/**
 * \file StageBuilder.hpp
 *
 * \author Gus Callaway
 *
 * \date 6/18/15
 *
 * \brief Declares the game's stage builder
 */

#ifndef BRICKBREAKER_STAGEBUILDER_H
#define BRICKBREAKER_STAGEBUILDER_H


#include "Object.h"

/**
 * \class StageBuilder
 * \brief Adds bricks to the game and positions them to set up the stage.
 */
class StageBuilder {
public:
    /**
     * \brief Parametrized constructor for a brick
     *
     * \param objects           A reference to the vector containing the game's objects. Needed to add bricks
     *        stageSize         The width and height of the area inside the barrier where the stage builder will work
     *        origin            The position of the top left point of the building area
     *        brickHeight       How tall each brick should be
     *        separation        How much space should be between the bricks
     */
    StageBuilder(std::vector<Object*>& objects, sf::Vector2f stageSize,
                 sf::Vector2f origin, float brickHeight, float separation);

    /**
     * \brief Completely loads the specified level
     *
     * \details For level 1, it randomly fills the first few rows with bricks and special bricks. For any other level
     *          it makes a call to loadLevelFromFile
     *
     * \param level The level to load (1 is the first level)
     */
    bool getLevel(int level);

    /**
     * \brief Adds a row of safety bricks to the bottom of the stage
     *
     * \details Adds the specified number of safety bricks to the game's list of objects, positioning each appropriately
     *
     * \param numBricks The number of safety bricks
     */
    void addSafetyBricks(int numBricks);

private:
    std::vector<Object*>& objects_; ///< The stage builder needs access to the game's list of objects to add bricks
    sf::Vector2f stageSize_;
    sf::Vector2f origin_;
    float brickHeight_;
    float separation_;

    /**
     * \brief Ensure the BrickBreakerData folder exists, and if it doesn't, populate it
     *
     * \details Can create a hard coded readme and sample level in case they were not included with the executable
     */
    void checkDataFile();

    /**
     * \brief Attempts to load the specified level from file
     *
     * \details Goes through the file line by line. If a dash is found, a brick is added to the game. If a tilda is
     *          found, a random special brick is added to the game. If a space is found, the next brick will be
     *          positioned to create an empty space. The number of bricks per line is determined using the first line of
     *          the file. Any other lines that are not long enough will be assumed to have trailing spaces. Lines that
     *          are too long will be cut short.
     *
     * \return True if the load was successful, false otherwise
     */
    bool loadLevelFromFile(int level);
};


#endif //BRICKBREAKER_STAGEBUILDER_H
