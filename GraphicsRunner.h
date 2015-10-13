/**
 * \file GraphicsRunner.hpp
 *
 * \author Gus Callaway
 *
 * \date 6/8/15
 *
 * \brief Declares the graphics runner class
 */
#ifndef BRICKBREAKER_GRAPHICSRUNNER_H
#define BRICKBREAKER_GRAPHICSRUNNER_H


#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Window/Event.hpp>
#include "Object.h"
#include "StageBuilder.h"
#include "Brick.h"
#include "Constants.h"

/**
 * \class GraphicsRunner
 * \brief The main game instance. Contains all the game's objects and deals with moving and drawing them.
 */
class GraphicsRunner {
public:
    /**
     * \brief Parametrized constructor for a barrier
     *
     * \param window    The graphics window on which to display the game
     */
    GraphicsRunner(sf::RenderWindow& window);

    /**
     * \brief Destructor for the game
     *
     * \details Called after the game is over. Goes through and deletes all the objects on the heap.
     */
    ~GraphicsRunner();

    /**
     * \brief Updates the graphics and game state for the next frame
     *
     * \details Clears the window, moves and draws all objects, and handles any other needed checks (checking if the
     *      game is over for example)
     */
    void update();

    /**
     * \brief Takes in an event and handles it appropriately
     *
     * \param event The event to be handled. Could for example be a button press.
     *
     * \details Determines the event's type and either deals with it directly or sends it off to whatever object it
     *      pertains to.
     */
    void handleEvent(sf::Event& event);

    /**
     * \brief Releases a ball attached to the game's paddle (if there is one)
     *
     * \param event The event to be handled. Could for example be a button press.
     *
     * \details Determines the event's type and either deals with it directly or sends it off to whatever object it
     *      pertains to.
     */
    void releaseBall();

    /**
     * \brief Returns a vector of all the objects active in the game
     */
    std::vector<Object*>& getObjects();

    /**
     * \brief Gets the game's paddle
     *
     * \return A pointer to the game's paddle as an abstract Object
     */
    Object* getPaddle() const;

    /**
     * \brief Gets the game's barrier
     *
     * \return A pointer to the game's barrier as an abstract Object
     */
    Object* getBarrier() const;

    int indexOfFirstSafetyBrick_;   ///< The index in the objects_ vector of the first safety brick
    int numSafetyBricks_;           ///< The number of safety bricks on the stage
    int numBricks_;                 ///< The number of bricks on the stage
    sf::Vector2u windowSize_;       ///< Holds the original window size to handle window resizing


private:
    std::vector<Object*> objects_;
    sf::RenderWindow& window_;
    sf::Event lastEvent_;    ///< Stores what the last event was to prevent registering the same event multiple times
    StageBuilder builder_;

    /**
     * \brief Checks if a brick is special, and if it is, applies the correct behavior
     *
     * \param brick The brick that has just been hit and could possibly have special properties
     */
    void handleSpecialBrick(Brick* brick);

    /**
     * \brief Changes the text to be displayed in the center of the screen
     *
     * \param str       The string to be displayed
     *        color     The color of the text
     *        size      How large the text should be in pixels
     *        needClear True if all other text should be cleared before adding
     *        position  Describes where to place the text, possible values are:
     *                      'l' for the top left of the screen
     *                      'm' for the top middle of the screen
     *                      'r' for the top right of the screen
     *                      'c' for the center of the screen
     *
     * \note The empty string can be passed for 'str' to remove all text from the screen
     */
    void addText(std::string str, sf::Color color = DEFAULT_COLOR, unsigned int size = 44,
                 bool needClear = true, char position = 'c');

    /**
     * \brief Loads the game's font from file.
     */
    void loadFont();

    /**
     * \brief Pauses or unpauses the game
     *
     * \details Flips the value of paused_ and sets the game's text appropriately
     */
    void togglePause();

    /**
     * \brief Checks the game's status and updates 'status_' accordingly. See declaration of data member below.
     *
     * \details Also checks the game's timer for displaying text in the center of the screen. Either clears away the
     *          text or loads the next level depending on the situation.
     */
     void checkStatus();

    /**
     * \brief Deletes all the game's balls and bricks to start a new round
     */
    void clear();

    /**
     * \brief Completely loads the next level
     *
     * \details Clears the last level (if necessary), decreases the number of safety bricks by one, loads safety bricks,
     *          loads regular bricks if possible (and if not possible, ends the game), updates numBricks_, and finally
     *          adds a ball to mark the start of the next level.
     *
     * \param needClear If true, clear the stage before loading the next one
     */
    void nextLevel(bool needClear = true);

    /**
     * \brief Ends the game either as a win or a loss
     *
     * \details Sets appropriate text to appear in the center of the screen and prepares the game to be played again
     *
     * \param won   true if the player won
     *              false if the player lost
     */
    void gameOver(bool won = false);

    /**
     * \brief Attempts to start the game
     *
     * \details Resets the game and loads the first level.
     */
    void start();

    /**
     * \brief Takes in a number of seconds and returns the time duration in a more human readable format
     *
     * \param totalSeconds  The amount of time passed in seconds
     *
     * \return  The time passed in the form <minutes>:<seconds>
     */
    std::string getTimeStringFromSeconds(double totalSeconds);

    /**
     * \brief Saves a high score for the current level if it beats the current high score
     *
     * \param score The score for the current level in seconds
     *
     * \note Assumes that scores for all levels other than the current one are filled (cant beat level 5 before level 4)
     */
    void saveHighScore(double score);

    /**
     * \brief Writes the high score list to file
     *
     * \details Clears the current file first, then puts each entry of scores_ on a different line
     */
    void writeHighScores();

    /**
     * \brief Loads high scores from file into the scores_ data member
     *
     * \details If no high score file exists, scores_ is left untouched
     */
    void loadHighScores();

    /**
     * \brief Stores the current state of the game
     *
     * \details Possible values:
     *              'p'     - The game is paused, objects do not move
     *              'c'     - The player just completed a level
     *              'o'     - The game is over
     *              '\0'    - Normal status (game is under way)
     */
    char status_;

    sf::Font font_;                     ///< The default font for text displayed in the game

    std::vector<sf::Text> text_;        ///< Holds all the game's text objects

    std::vector<std::string> scores_;   ///< The first entry is the high score for level 1, the second for level 2, etc

    unsigned long baseNumTextObjects_;  ///< The number of text objects permanently present (ones in the banner)

    time_t pauseStart_;                 ///< Holds the time at which the player paused the game

    double secondsPaused_;               ///< How many seconds the game has been paused for

    time_t timerStart_;                 ///< Holds the time at which a timer started. Otherwise when the level started

    float timerLength_;                 ///< Holds how long the current timer should go for

    int level_;                         ///< 1 is the first level as well as the loneliest number
};


#endif //BRICKBREAKER_GRAPHICSRUNNER_H
