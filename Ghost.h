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
#include "Player.h"

using namespace std;
using namespace sf;

bool GhostCollision(GhostMemory* shared)
{
    if (shared->pacman_pos[0] == shared->ghost_pos[0] && shared->pacman_pos[1] == shared->ghost_pos[1])
    {
        return true;
    }
    return false;
}

int checkChase(GhostMemory* shared)
{
    if (shared->pacman_pos[0] == shared->ghost_pos[0])
    {
        if (shared->pacman_pos[1] > shared->ghost_pos[1])
        {
            for (int i = shared->ghost_pos[1]; i <= shared->pacman_pos[1]; ++i)
            {
                if (shared->level1[shared->pacman_pos[0]][i] == 1)
                    return 5;
            }
            return 4;
        }
        else if (shared->pacman_pos[1] < shared->ghost_pos[1])
        {
            for (int i = shared->pacman_pos[1]; i <= shared->ghost_pos[1]; ++i)
            {
                if (shared->level1[shared->pacman_pos[0]][i] == 1)
                    return 5;
            }
            return 3;
        }
    }
    else if (shared->pacman_pos[1] == shared->ghost_pos[1])
    {
        if (shared->pacman_pos[0] >= shared->ghost_pos[0])
        {
            for (int i = shared->ghost_pos[0]; i <= shared->pacman_pos[0]; ++i)
            {
                if (shared->level1[i][shared->pacman_pos[1]] == 1)
                    return 5;
            }
            return 2;
        }
        else if (shared->pacman_pos[0] < shared->ghost_pos[0])
        {
            
            for (int i = shared->pacman_pos[0]; i <= shared->ghost_pos[0]; ++i)
            {
                if (shared->level1[i][shared->pacman_pos[1]] == 1)
                    return 5;
            }
            return 1;
        }
    }
    return 5;
}

void ghost_move(GhostMemory* shared)
{
    if (shared->direction == 0)
        shared->direction = 1 + rand()%4;

    /*
    int temp = checkChase(shared);
    if (temp < 5 && temp > 0)
        shared->direction = 5;
    */

    int probability = rand()%8;
    if (probability == 7)
    {
        shared->direction = 1 + rand()%4;
    }

    //Move Left
    if (shared->direction == 1)
    {
        if (shared->level1[shared->ghost_pos[0]-1][shared->ghost_pos[1]] != 1)
            shared->ghost_pos[0] += -1;
        else
            shared->direction = 1 + rand()%4;
    }
    //Move Right
    if (shared->direction == 2)
    {
        if (shared->level1[shared->ghost_pos[0]+1][shared->ghost_pos[1]] != 1)
            shared->ghost_pos[0] += 1;
        else
            shared->direction = 1 + rand()%4;
    }
    //Move Up
    if (shared->direction == 3)
    {
        if (shared->level1[shared->ghost_pos[0]][shared->ghost_pos[1]-1] != 1)
            shared->ghost_pos[1] += -1;
        else
            shared->direction = 1 + rand()%4;
    }
    //Move Down
    if (shared->direction == 4)
    {
        if (shared->level1[shared->ghost_pos[0]][shared->ghost_pos[1]+1] != 1)
            shared->ghost_pos[1] += 1;
        else
            shared->direction = 1 + rand()%4;
    }

    if (shared->level1[shared->ghost_pos[0]][shared->ghost_pos[1]] == -2)
    {
        if (shared->ghost_pos[1] >= 27)
            shared->ghost_pos[1] = 0;
        else if (shared->ghost_pos[1] <= 0)
            shared->ghost_pos[1] = 27;
    }
}

void* ghostFunctionality(void* arg)
{
    GhostMemory* shared = (GhostMemory*) arg;

    Clock ghost_delay_time;
    ghost_delay_time.restart();

    while (shared->window_open)
    {
        if (shared->paused >= 2)
        {
            if (shared->game_state == 0 && shared->loading == false)
            {
                //pthread_mutex_lock(&movement_mutex);
                float delay = ghost_delay_time.getElapsedTime().asMilliseconds();
                if (delay > shared->ghost_delay)
                {
                    ghost_move(shared);
                    ghost_delay_time.restart();
                }
                //pthread_mutex_unlock(&movement_mutex);
            }

            shared->pacManDeath = GhostCollision(shared);
        }
    }
}