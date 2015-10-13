/**
 * \file Constants.h
 *
 * \author Gus Callaway
 *
 * \date 8/16/15
 *
 * \brief Defines all the game's constants
 */

#ifndef BRICKBREAKER_CONSTANTS_H
#define BRICKBREAKER_CONSTANTS_H

#include <SFML/Graphics/RenderWindow.hpp>
#include <fstream>

const unsigned int WINDOW_WIDTH = 1300;

const unsigned int WINDOW_HEIGHT = 800;

const float LEVEL_BREAK_TIME = 5;               ///< How many seconds to wait between levels

const float PADDLE_WIDTH = 100;

const float PADDLE_HEIGHT = 13;

const float PADDLE_ELONGATION_TIME = 15;        ///< In seconds

const float PADDLE_ELONGATION_FACTOR = 1.5f;    ///< In seconds

const float PADDLE_MAX_ROTATION = 15;           ///< This is in degrees

const float PADDLE_ACCELERATION = 1;

const float BARRIER_WIDTH = 10;                  ///< How thick the boundary should be

const float BARRIER_BUFFER = 5;                 ///< How far the boundary should be from the edges of the window

const float BANNER_HEIGHT = 30;                 ///< How much space above the barrier for displaying information

const float BRICK_HEIGHT = 20;

const unsigned int NUM_SAFETY_BRICKS = 10;               ///< Decremented by one each level

const float BRICK_SEPARATION = 1;               ///< How much space to put between each brick when creating a stage

const float BALL_RADIUS = 10;

const float BALL_MAX_SPEED = 8.0;               ///< A soft cap on ball velocity

const char SPECIALS[] = {'b', 'l'};                     ///< A list of all special brick characters.

// Colors //
const sf::Color DEFAULT_COLOR = sf::Color(51,51,51);            ///< Used for the paddle, barrier, and normal text

const sf::Color BACKGROUND_COLOR = sf::Color(245,245,245);      ///< A slight gray, better on the eyes

const sf::Color BALL_COLOR = sf::Color(122,122,122);

const sf::Color BRICK_COLOR = sf::Color(25,130,196);

const sf::Color SPECIAL_BRICK_COLOR = sf::Color(255,202,58);

const sf::Color SAFETY_BRICK_COLOR = sf::Color(255,102,0);

const sf::Color LOSE_COLOR = sf::Color(255,0,0);                ///< Text color for failure messages

const sf::Color WIN_COLOR = sf::Color(138,201,38);              ///< Text color for failure messages

// Used for creating the first level

const unsigned int NUM_BRICKS_PER_LINE = 20;

const unsigned int NUM_EMPTY_ROWS = 2;

const unsigned int NUM_BRICK_ROWS = 3;

const unsigned int NUM_SPECIAL_BRICKS = 6;

#endif //BRICKBREAKER_CONSTANTS_H