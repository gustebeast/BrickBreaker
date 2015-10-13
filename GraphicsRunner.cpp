/**
 * \file GraphicsRunner.cpp
 *
 * \author Gus Callaway
 *
 * \date 6/8/15
 *
 * \brief Implements the graphics runner class
 */
#include <iostream>
#include "GraphicsRunner.h"
#include "Barrier.h"
#include "Paddle.h"

using namespace sf;
using namespace std;

GraphicsRunner::GraphicsRunner(RenderWindow& window)
        : window_(window),
          windowSize_(window.getSize()),
          builder_(objects_,
                   Vector2f(window.getSize().x - 2 * BARRIER_BUFFER - 2 * BARRIER_WIDTH,
                            window.getSize().y - BANNER_HEIGHT - BARRIER_BUFFER - BARRIER_WIDTH),
                   Vector2f(BARRIER_BUFFER + BARRIER_WIDTH, BANNER_HEIGHT + BARRIER_BUFFER + BARRIER_WIDTH),
                   BRICK_HEIGHT,
                   BRICK_SEPARATION),
          status_('\0'),
          timerLength_(0),
          level_(0) // nextLevel() increments this before loading the level (so 0 -> start at level 1)
{
    float windowWidth = window_.getSize().x;
    float windowHeight = window_.getSize().y;

    // Add objects. If more objects are added/order is changed, make sure to update getters appropriately
    // Also, the first object cannot not be deletable
    objects_.push_back(new Paddle(*this, windowWidth / 2, windowHeight - 40, PADDLE_WIDTH, PADDLE_HEIGHT));

    objects_.push_back(new Barrier(windowWidth, windowHeight, BARRIER_WIDTH, BARRIER_BUFFER));

    // Record the index of the first safety brick
    indexOfFirstSafetyBrick_ = int(objects_.size());

    // Load the font and base text objects
    loadFont();
    addText("Level 0", DEFAULT_COLOR, (unsigned int)(BANNER_HEIGHT-2*BARRIER_BUFFER), false, 'l');
    addText("Brick Breaker", Color(25,200,229), (unsigned int)(BANNER_HEIGHT-2*BARRIER_BUFFER), false, 'm');
    addText("00:00", DEFAULT_COLOR, (unsigned int)(BANNER_HEIGHT-2*BARRIER_BUFFER), false, 'r');

    baseNumTextObjects_ = text_.size();

    // Populate the scores_ data member with high score data from file
    loadHighScores();

    // Load the first level, no need to clear the stage first
    nextLevel(false);

    // Instead of showing "Level 1" show the intro text, also mark that this text is displayed by setting timerLength_
    timerLength_ = -1;
    addText("Brick Breaker", Color(25,200,229), 54);
    addText("by gustebeast", DEFAULT_COLOR, 34, false);
    addText("Use J/L to move, A/D to rotate, and space to release a ball", DEFAULT_COLOR, 34, false);
}

GraphicsRunner::~GraphicsRunner() {
    // Write high score data to file
    writeHighScores();

    // And delete all the game's objects
    for (Object* object : objects_)
        delete object;
}

void GraphicsRunner::update() {
    // Clear the graphics window with a slight gray background
    window_.clear(BACKGROUND_COLOR);

    // Move and draw all objects while removing ones that need deletion
    for (auto i = objects_.begin(); i != objects_.end(); ++i) {
        Object* object = *i;

        // If the object needs to be deleted
        if (object->delete_) {
            // First check to see this object is a brick
            if (Brick* brick = dynamic_cast<Brick*>(object)) {
                // And if it is, apply any special properties the brick might have. This function also decrements either
                // numBricks_ or numSpecialBricks_ depending on the brick's type
                handleSpecialBrick(brick);
            }

            // Free the memory
            delete object;

            // Erase the object from the containing vector. The iterator now points at the next object.
            i = objects_.erase(i);

            // Decrement the iterator since it will be incremented in the for loop.
            // Note! If i == objects_.begin() then this will cause undefined behavior. The first object should never
            // be deleted!
            --i;
            continue;
        }

        // Move and draw
        object->draw(window_);
        if (status_ == '\0')
            object->move();
    }

    // Draw the game text
    for (Text text : text_)
        window_.draw(text);

    // Update the level timer if the game isn't paused
    if (status_ == '\0')
        text_[2].setString(getTimeStringFromSeconds(difftime(time(0), timerStart_) - secondsPaused_));

    // Finally display all the objects on the screen.
    window_.display();

    // And check the status of the game for the next frame
    checkStatus();
}

void GraphicsRunner::handleEvent(Event &event) {
    if (event.type == Event::Closed)
        window_.close();

    // If a key is pressed or released and is unique (not the same event as the one just processed)
    if ((event.type == Event::KeyPressed || event.type == Event::KeyReleased) &&
        (event.type != lastEvent_.type || event.key.code != lastEvent_.key.code)) {

        // Then see what key was pressed and execute the appropriate action
        Keyboard::Key& key = event.key.code;

        if (status_ == '\0' && (key == Keyboard::J || key == Keyboard::L || key == Keyboard::A || key == Keyboard::D)) {
            // Move the paddle in the appropriate direction or rotate it
            dynamic_cast<Paddle*>(getPaddle())->processKey(event.type, key);
        }

        // For all other keys we only want key press events
        else if (event.type != Event::KeyReleased) {

            switch (key) {
                case Keyboard::Space: releaseBall();
                     break;

                case Keyboard::Escape: togglePause();
                     break;

                case Keyboard::Return: start();
                     break;

                default: break;
            }
        }

        // And update the previous event since this new one is different
        lastEvent_ = event;
    }
}

void GraphicsRunner::releaseBall() {
    // If the game is paused, don't release a ball
    if (status_ != '\0')
        return;

    // If the game has the into text up, remove it
    if (timerLength_ == -1)
        addText("");

    // Loop through all the game's balls
    for (long i = indexOfFirstSafetyBrick_ + numSafetyBricks_ + numBricks_; i < objects_.size(); ++i) {
        Ball* ball = dynamic_cast<Ball*>(objects_[i]);

        // If the ball is attached, release it then break out (only release one ball at a time)
        if (ball->isAttached()) {
            ball->detach();
            break;
        }
    }
}

vector<Object*>& GraphicsRunner::getObjects() {
    return objects_;
}

Object* GraphicsRunner::getPaddle() const {
    // The paddle is located at index 0
    return objects_[0];
}

Object* GraphicsRunner::getBarrier() const {
    // The barrier is located at index 1
    return objects_[1];
}

void GraphicsRunner::handleSpecialBrick(Brick* brick) {
    char& special = brick->special_;

    // Extra ball special brick
    switch(special) {
        case 's': // Safety brick
            --numSafetyBricks_;
            return; // Prevents decrementing "numBricks_" which occurs after the switch statement

        case 'b': // Extra ball
            objects_.push_back(new Ball(*this)); // Create a ball attached to the game's paddle
            break;

        case 'l': // Extra long paddle
            dynamic_cast<Paddle*>(getPaddle())->changeLength(true);
            break;


        default: break; // Do nothing in the default case
    }

    --numBricks_;
}

void GraphicsRunner::addText(string str, Color color, unsigned int size, bool needClear, char position) {
    // If necessary, reset the list of text objects
    if (needClear) {
        // Remove text objects until only the base ones remain
        while (text_.size() > baseNumTextObjects_) {
            text_.pop_back();
        }
    }

    // Nothing further to do if the string is empty
    if (str == "") {
        return;
    }

    // The bebas font needs lots of space, so make 4 space gaps between words
    replace(str.begin(), str.end(), ' ', '\t');

    // Create a new text object to add to the game's list
    Text text;
    text.setFont(font_);
    text.setString(str);
    text.setColor(color);
    text.setCharacterSize(size);

    // Now set the position of the text, which depends on alignment
    float xPos;
    float yPos;
    FloatRect bounds = text.getGlobalBounds();

    // If the text is to be center aligned
    if (position == 'c') {
        // Place it halfway across the screen (accounting for its own width)
        xPos = windowSize_.x/2 - bounds.width/2;

        // And either place it below the last non-base text object or just halfway down the screen
        if (text_.size() > baseNumTextObjects_) {
            yPos = text_.back().getPosition().y + text_.back().getGlobalBounds().height + 20;
        }
        else {
            yPos = windowSize_.y/2;
        }
    }

    // Otherwise put it at the top of the screen, either left or right justified
    else {
        yPos = BARRIER_BUFFER; // A bit of head room

        // Set the x position appropriately based on justification
        switch (position) {
            case 'l': xPos = BARRIER_BUFFER;
                      break;

            case 'm': xPos = windowSize_.x/2 - bounds.width/2;
                      break;

            case 'r': xPos = windowSize_.x - BARRIER_BUFFER - bounds.width;
                      break;

            default:  xPos = BARRIER_BUFFER;    // Should never occur, but if it does assume left justification
        }
    }

    // Apply alignment and add the text
    text.setPosition(xPos, yPos);
    text_.push_back(text);
}

void GraphicsRunner::loadFont() {
    if (!font_.loadFromFile("BrickBreakerData/bebas.ttf")) {
        abort();
    }
}

void GraphicsRunner::togglePause() {
    // If the game is not paused, pause it. If it is paused, unpause it
    if (status_ == '\0') {
        addText("Paused");

        // Display the high scores
        addText("High Scores", DEFAULT_COLOR, 20, false);
        int level = 1;
        for (string score : scores_) {
            addText(to_string(level) + " - " + getTimeStringFromSeconds(stod(score)), DEFAULT_COLOR, 18, false);
            ++level;
        }

        status_ = 'p';
        pauseStart_ = time(0);
    }
    else if (status_ == 'p') {
        addText("");
        status_ = '\0';
        secondsPaused_ += difftime(time(0), pauseStart_);
    }
}

void GraphicsRunner::checkStatus() {
    // In normal status, check to make sure the game isn't over
    if (status_ == '\0') {
        // If no bricks remain, the player won
        if (numBricks_ == 0) {
            addText("Level Cleared!", WIN_COLOR, 54);
            status_ = 'c';
            saveHighScore(difftime(time(0), timerStart_) - secondsPaused_);
            timerStart_ = time(0);  // Timer for the break between levels
            timerLength_ = LEVEL_BREAK_TIME;
        }
        // If no balls remain, the player lost
        else if (objects_.size() == indexOfFirstSafetyBrick_ + numSafetyBricks_ + numBricks_) {
            gameOver();
        }
    }

    // Also check the timer to see if we need to clear off any text, or perhaps load the next level
    if (timerLength_ > 0) {
        // If the timer is over
        if (difftime(time(0), timerStart_) > timerLength_) {
            timerLength_ = 0;

            // If the player just completed a level, load the next one
            if (status_ == 'c') {
                nextLevel();
            }
            // Otherwise we just need to get rid of the text on the screen
            else if (status_ != 'p') {
                addText("");
            }
        }
    }
}

void GraphicsRunner::clear() {
    // Create an iterator pointing at the last element
    auto i = objects_.end();
    --i;

    // While there are objects other than the paddle and the barrier
    while (objects_.size() > indexOfFirstSafetyBrick_) {
        // Deallocate the memory and pop the object off the objects vector
        delete *i;
        --i;
        objects_.pop_back();
    }
}

void GraphicsRunner::nextLevel(bool needClear) {
    // If a player is already paused going into a level, make it as though they paused right when the level begins
    secondsPaused_ = 0;
    pauseStart_ = time(0);

    // Increment the level and update the display text
    ++level_;
    text_[0].setString("Level " + to_string(level_));

    // Return game to normal status
    status_ = '\0';

    if (needClear) {
        clear();
    }

    addText("level " + to_string(level_), DEFAULT_COLOR, 54);
    timerStart_ = time(0);  // Marks the start of the level timer
    timerLength_ = LEVEL_BREAK_TIME/2;

    // One fewer safety brick every level
    numSafetyBricks_ = NUM_SAFETY_BRICKS - level_ + 1;

    // Add the safety bricks
    builder_.addSafetyBricks(numSafetyBricks_);

    // Try to load the regular bricks for the next stage, if the next stage doesn't exist end the game
    if (!builder_.getLevel(level_)) {
        gameOver(true); // No next level, the player won the game
    }

    // Record the number of bricks before adding the ball
    numBricks_ = int(objects_.size()) - (indexOfFirstSafetyBrick_ + numSafetyBricks_);

    // Create a ball attached to the paddle
    objects_.push_back(new Ball(*this));
}

void GraphicsRunner::gameOver(bool won) {
    if (won) {
        addText("You Win!", WIN_COLOR, 54);
        addText("Press return to play again", DEFAULT_COLOR, 44, false);
        timerLength_ = 0;
        status_ = 'o';
    }
    else {
        addText("You Lose!", LOSE_COLOR, 54);
        addText("Press return to restart", DEFAULT_COLOR, 44, false);
        status_ = 'o';
    }
}

void GraphicsRunner::start() {
    if (status_ == 'o') {
        // Reset the level and load the first one
        level_ = 0;
        nextLevel();
    }
}

string GraphicsRunner::getTimeStringFromSeconds(double totalSeconds) {
    // Get the number of minutes, making sure that it is a two digit number
    string minuteString = to_string(int(totalSeconds/60));
    if (minuteString.size() == 1) {
        minuteString = "0" + minuteString;
    }

    // Get the number of seconds, making sure that it is a two digit number
    string secondString = to_string(int(totalSeconds)%60);
    if (secondString.size() == 1) {
        secondString = "0" + secondString;
    }

    // Replace the timer string with an updated one
    return minuteString + ":" + secondString;
}

void GraphicsRunner::saveHighScore(double score) {
    // If no high score exists yet for this level, create a new one
    if (scores_.size() < level_) {
        // NOTE! This assumes that level_ = scores_.size() + 1
        scores_.push_back(to_string(score));
    }
    // Otherwise, update the best score if this one beats it
    else {
        if (stod(scores_[level_-1]) > score) {
            scores_[level_-1] = to_string(score);
            addText("New High Score!", DEFAULT_COLOR, 24, false);
        }
    }
}

void GraphicsRunner::writeHighScores() {
    ofstream scoresFile("BrickBreakerData/.scores.txt");
    // Clear the file if it exists to make room for the updated scores
    if (scoresFile.is_open())
        scoresFile.clear();

    // And write each score to file
    for (string& score : scores_)
        scoresFile << score << endl;

    scoresFile.close();
}

void GraphicsRunner::loadHighScores() {
    ifstream scoresFile("BrickBreakerData/.scores.txt");

    // Only add entries if the file has already been created
    if (scoresFile.is_open()) {
        string line;
        while (getline(scoresFile, line))
            scores_.push_back(line);
    }
}
