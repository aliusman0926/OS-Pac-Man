#pragma once

#include<X11/Xlib.h>

#undef Drawable
#undef Window
#undef Font
#undef Status
#undef None
#undef Default

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <pthread.h>
#include <string.h>
#include <iostream>
#include <cstdlib>
#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>

//My Header Files
#include "SharedMemory.h"
#include "Ghost.h"

using namespace std;
using namespace sf;

void pacman_move(int direction, int board[31][28], int* position)
{
    //Move Right
    if (direction == 1)
    {
        if (board[position[0]+1][position[1]] != 1)
            position[0] += 1;
    }
    //Move Left
    if (direction == 2)
    {
        if (board[position[0]-1][position[1]] != 1)
            position[0] -= 1;
    }
    //Move Down
    if (direction == 3)
    {
        if (board[position[0]][position[1]+1] != 1)
            position[1] += 1;
    }
    //Move Up
    if (direction == 4)
    {
        if (board[position[0]][position[1]-1] != 1)
            position[1] -= 1;
    }

    if (board[position[0]][position[1]] == -2)
    {
        if (position[1] >= 27)
            position[1] = 0;
        else if (position[1] <= 0)
            position[1] = 27;
    }
}

bool checkDirection(int* position, int board[31][28], int direction)
{
    //Move Right
    if (direction == 1)
    {
        if (board[position[0]+1][position[1]] != 1)
            return true;
    }
    //Move Left
    if (direction == 2)
    {
        if (board[position[0]-1][position[1]] != 1)
            return true;
    }
    //Move Down
    if (direction == 3)
    {
        if (board[position[0]][position[1]+1] != 1)
            return true;
    }
    //Move Up
    if (direction == 4)
    {
        if (board[position[0]][position[1]-1] != 1)
            return true;
    }

    return false;
}

void eatPellet(int* position, int board[31][28], int& score, bool& powerUp, Clock& power_time, Sound& eat_sound, Sound& power_sound)
{
    if (board[position[0]][position[1]] == 0)
    {
        score += 5;
        board[position[0]][position[1]] = -1;
        eat_sound.play();
    }
    else if (board[position[0]][position[1]] == 2)
    {
        if (powerUp == false)
        {
            score += 15;
            powerUp = true;
            power_time.restart();
            board[position[0]][position[1]] = -1;
            power_sound.play();
        }
    }
}

void PacManFunctionality(SharedMemory* shared, Clock& pacman_delay_time, Clock& power_time, Sound& eat_sound, Sound& power_sound)
{
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
    {
        if (shared->paused == 0)
            shared->paused = 2;
        // move left...
        if (checkDirection(shared->pacman_pos, shared->level1, 2))
            shared->pacman_direction = 2;
    }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
    {
        if (shared->paused == 0)
            shared->paused = 2;
        // move right...
        if (checkDirection(shared->pacman_pos, shared->level1, 1))
            shared->pacman_direction = 1;
    }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
    {
        if (shared->paused == 0)
            shared->paused = 2;
        // move right...
        if (checkDirection(shared->pacman_pos, shared->level1, 4))
            shared->pacman_direction = 4;
    }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
    {
        if (shared->paused == 0)
            shared->paused = 2;
        // move right...
        if (checkDirection(shared->pacman_pos, shared->level1, 3))
            shared->pacman_direction = 3;
    }

    float pacman_delay = pacman_delay_time.getElapsedTime().asMilliseconds();

    if (shared->paused >= 2)
    {
        if (pacman_delay > shared->pacman_speed)
        {
            pacman_move(shared->pacman_direction, shared->level1, shared->pacman_pos);
            pacman_delay_time.restart();
        }
        eatPellet(shared->pacman_pos, shared->level1, shared->score, shared->poweredUp, power_time, eat_sound, power_sound);
    }
    float powerDown = power_time.getElapsedTime().asSeconds();
    if (powerDown > 10)
        shared->poweredUp = false;

}