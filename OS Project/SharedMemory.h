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
#include <semaphore.h>

using namespace std;
using namespace sf;

pthread_mutex_t movement_mutex = PTHREAD_MUTEX_INITIALIZER;
sem_t key, permit;
pthread_mutex_t speed_mutex;
int available_boosts = 2;


struct PowerPosition
{
public:
    int positions[2];
};

struct SharedMemory
{
public:
    int pacman_pos[2] = {14,21};
    bool poweredUp = false;
    int pacman_direction = 0;
    int lives = 3;
    int pacman_speed = 150;

    int ghost1_pos[2] = {13,14};
    int ghost1_direction = 0;
    int ghost2_pos[2] = {14,14};
    int ghost2_direction = 0;
    int ghost3_pos[2] = {15,14};
    int ghost3_direction = 0;
    int ghost4_pos[2] = {14,15};
    int ghost4_direction = 0;

    int score = 0;
    bool window_open = true;
    int paused = 0;
    int game_state = -1;
    bool game_win = false;
    bool loading = false;
    PowerPosition powerupPositions[12];
    PowerPosition permits[2];
    PowerPosition keys[2];
    int level1[31][28] = {
                            {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
                            {1,2,0,0,0,0,0,0,0,0,0,0,2,1,1,2,0,0,0,0,0,0,0,0,0,0,2,1},
                            {1,0,1,1,1,1,0,1,1,1,1,1,0,1,1,0,1,1,1,1,1,0,1,1,1,1,0,1},
                            {1,0,1,-1,-1,1,0,1,-1,-1,-1,1,0,1,1,0,1,-1,-1,-1,1,0,1,-1,-1,1,0,1},
                            {1,0,1,1,1,1,0,1,1,1,1,1,0,1,1,0,1,1,1,1,1,0,1,1,1,1,0,1},
                            {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
                            {1,0,1,1,1,1,0,1,1,0,1,1,1,1,1,1,1,1,0,1,1,0,1,1,1,1,0,1},
                            {1,0,1,1,1,1,0,1,1,0,1,1,1,1,1,1,1,1,0,1,1,0,1,1,1,1,0,1},
                            {1,0,0,0,0,0,0,1,1,2,0,0,0,1,1,0,0,0,2,1,1,0,0,0,0,0,0,1},
                            {1,1,1,1,1,1,0,1,1,1,1,1,-1,1,1,-1,1,1,1,1,1,0,1,1,1,1,1,1},
                            {-1,-1,-1,-1,-1,1,0,1,1,1,1,1,-1,1,1,-1,1,1,1,1,1,0,1,-1,-1,-1,-1,-1},
                            {-1,-1,-1,-1,-1,1,0,1,1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,1,1,0,1,-1,-1,-1,-1,-1},
                            {-1,-1,-1,-1,-1,1,0,1,1,-1,1,1,1,1,1,1,1,1,-1,1,1,0,1,-1,-1,-1,-1,-1},
                            {1,1,1,1,1,1,0,1,1,-1,3,-1,-1,-1,-1,-1,-1,1,-1,1,1,0,1,1,1,1,1,1},
                            {-2,-1,-1,-1,-1,-1,-1,-1,-1,-1,3,-1,-1,-1,-1,-1,-1,1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-2},
                            {1,1,1,1,1,1,0,1,1,-1,3,-1,-1,-1,-1,-1,-1,1,-1,1,1,0,1,1,1,1,1,1},
                            {-1,-1,-1,-1,-1,1,0,1,1,-1,1,1,1,1,1,1,1,1,-1,1,1,0,1,-1,-1,-1,-1,-1},
                            {-1,-1,-1,-1,-1,1,0,1,1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,1,1,0,1,-1,-1,-1,-1,-1},
                            {-1,-1,-1,-1,-1,1,0,1,1,-1,1,1,1,1,1,1,1,1,-1,1,1,0,1,-1,-1,-1,-1,-1},
                            {1,1,1,1,1,1,0,1,1,-1,1,1,1,1,1,1,1,1,-1,1,1,0,1,1,1,1,1,1},
                            {1,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,1},
                            {1,0,1,1,1,1,0,1,1,1,1,1,0,1,1,0,1,1,1,1,1,0,1,1,1,1,0,1},
                            {1,0,1,1,1,1,0,1,1,1,1,1,0,1,1,0,1,1,1,1,1,0,1,1,1,1,0,1},
                            {1,0,0,2,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,2,0,0,1},
                            {1,1,1,0,1,1,0,1,1,0,1,1,1,1,1,1,1,1,0,1,1,0,1,1,0,1,1,1},
                            {1,1,1,0,1,1,0,1,1,0,1,1,1,1,1,1,1,1,0,1,1,0,1,1,0,1,1,1},
                            {1,0,0,0,0,0,2,1,1,0,0,0,0,1,1,0,0,0,0,1,1,2,0,0,0,0,0,1},
                            {1,0,1,1,1,1,1,1,1,1,1,1,0,1,1,0,1,1,1,1,1,1,1,1,1,1,0,1},
                            {1,0,1,1,1,1,1,1,1,1,1,1,0,1,1,0,1,1,1,1,1,1,1,1,1,1,0,1},
                            {1,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,1},
                            {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
                         };
};

struct GhostMemory
{
public:
    int pacman_pos[2] = {23,14};
    int ghost_pos[2];
    int direction = 0;
    bool pacManDeath = false;
    int ghost_delay = 100;
    bool key = false;
    bool permit = false;
    bool boost = false;
    bool left_box = false;

    bool window_open = true;
    int paused = 0;
    int game_state = -1;
    bool game_win = false;
    bool loading = false;
    int level1[31][28] = {
                            {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
                            {1,2,0,0,0,0,0,0,0,0,0,0,2,1,1,2,0,0,0,0,0,0,0,0,0,0,2,1},
                            {1,0,1,1,1,1,0,1,1,1,1,1,0,1,1,0,1,1,1,1,1,0,1,1,1,1,0,1},
                            {1,0,1,-1,-1,1,0,1,-1,-1,-1,1,0,1,1,0,1,-1,-1,-1,1,0,1,-1,-1,1,0,1},
                            {1,0,1,1,1,1,0,1,1,1,1,1,0,1,1,0,1,1,1,1,1,0,1,1,1,1,0,1},
                            {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
                            {1,0,1,1,1,1,0,1,1,0,1,1,1,1,1,1,1,1,0,1,1,0,1,1,1,1,0,1},
                            {1,0,1,1,1,1,0,1,1,0,1,1,1,1,1,1,1,1,0,1,1,0,1,1,1,1,0,1},
                            {1,0,0,0,0,0,0,1,1,2,0,0,0,1,1,0,0,0,2,1,1,0,0,0,0,0,0,1},
                            {1,1,1,1,1,1,0,1,1,1,1,1,-1,1,1,-1,1,1,1,1,1,0,1,1,1,1,1,1},
                            {-1,-1,-1,-1,-1,1,0,1,1,1,1,1,-1,1,1,-1,1,1,1,1,1,0,1,-1,-1,-1,-1,-1},
                            {-1,-1,-1,-1,-1,1,0,1,1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,1,1,0,1,-1,-1,-1,-1,-1},
                            {-1,-1,-1,-1,-1,1,0,1,1,-1,1,1,1,1,1,1,1,1,-1,1,1,0,1,-1,-1,-1,-1,-1},
                            {1,1,1,1,1,1,0,1,1,-1,3,-1,-1,-1,-1,-1,-1,1,-1,1,1,0,1,1,1,1,1,1},
                            {-2,-1,-1,-1,-1,-1,-1,-1,-1,-1,3,-1,-1,-1,-1,-1,-1,1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-2},
                            {1,1,1,1,1,1,0,1,1,-1,3,-1,-1,-1,-1,-1,-1,1,-1,1,1,0,1,1,1,1,1,1},
                            {-1,-1,-1,-1,-1,1,0,1,1,-1,1,1,1,1,1,1,1,1,-1,1,1,0,1,-1,-1,-1,-1,-1},
                            {-1,-1,-1,-1,-1,1,0,1,1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,1,1,0,1,-1,-1,-1,-1,-1},
                            {-1,-1,-1,-1,-1,1,0,1,1,-1,1,1,1,1,1,1,1,1,-1,1,1,0,1,-1,-1,-1,-1,-1},
                            {1,1,1,1,1,1,0,1,1,-1,1,1,1,1,1,1,1,1,-1,1,1,0,1,1,1,1,1,1},
                            {1,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,1},
                            {1,0,1,1,1,1,0,1,1,1,1,1,0,1,1,0,1,1,1,1,1,0,1,1,1,1,0,1},
                            {1,0,1,1,1,1,0,1,1,1,1,1,0,1,1,0,1,1,1,1,1,0,1,1,1,1,0,1},
                            {1,0,0,2,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,2,0,0,1},
                            {1,1,1,0,1,1,0,1,1,0,1,1,1,1,1,1,1,1,0,1,1,0,1,1,0,1,1,1},
                            {1,1,1,0,1,1,0,1,1,0,1,1,1,1,1,1,1,1,0,1,1,0,1,1,0,1,1,1},
                            {1,0,0,0,0,0,2,1,1,0,0,0,0,1,1,0,0,0,0,1,1,2,0,0,0,0,0,1},
                            {1,0,1,1,1,1,1,1,1,1,1,1,0,1,1,0,1,1,1,1,1,1,1,1,1,1,0,1},
                            {1,0,1,1,1,1,1,1,1,1,1,1,0,1,1,0,1,1,1,1,1,1,1,1,1,1,0,1},
                            {1,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,1},
                            {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
                         };
};