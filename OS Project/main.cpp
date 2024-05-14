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

//My Header Files
#include "SharedMemory.h"
#include "Player.h"
#include "Ghost.h"

using namespace std;
using namespace sf;

void setPermitsandKeys(SharedMemory* shared)
{
    for (int i = 0; i < 2; ++i)
    {
        if (shared->keys[i].positions[0] == -1 || shared->keys[i].positions[1] == -1)
        {
            int x;
            int y;

            bool taken = false;
            do {
                taken = false;
                x = 13 + rand()%3;
                y = 11 + rand()%6;
                for (int i = 0; i < 2; ++i)
                {
                    if (shared->permits[i].positions[0] == x && shared->permits[i].positions[0] == y)
                    {
                        taken = true;
                        break;
                    }
                    else if (shared->keys[i].positions[0] == x && shared->keys[i].positions[0] == y)
                    {
                        taken = true;
                        break;
                    }
                }
            } while (taken == true);
            shared->keys[i].positions[0] = x;
            shared->keys[i].positions[1] = y;
        }
        if (shared->permits[i].positions[0] == -1 || shared->permits[i].positions[1] == -1)
        {
            int x;
            int y;

            bool taken = false;
            do {
                taken = false;
                x = 13 + rand()%3;
                y = 11 + rand()%6;
                for (int i = 0; i < 2; ++i)
                {
                    if (shared->permits[i].positions[0] == x && shared->permits[i].positions[0] == y)
                    {
                        taken = true;
                        break;
                    }
                    if (shared->keys[i].positions[0] == x && shared->keys[i].positions[0] == y)
                    {
                        taken = true;
                        break;
                    }
                }
            } while (taken == true);
            shared->permits[i].positions[0] = x;
            shared->permits[i].positions[1] = y;
        }
    }
    return;
}

void setNewKeys(SharedMemory* shared)
{
    int key_count = 0;
    int permit_count = 0;
    for (int i = 0; i < 2; ++i)
    {
        if (key_count < 1)
        {
            if (shared->keys[i].positions[0] == -1 || shared->keys[i].positions[1] == -1)
            {
                int x;
                int y;

                bool taken = false;
                do {
                    taken = false;
                    x = 13 + rand()%3;
                    y = 11 + rand()%6;
                    for (int i = 0; i < 2; ++i)
                    {
                        if (shared->permits[i].positions[0] == x && shared->permits[i].positions[0] == y)
                        {
                            taken = true;
                            break;
                        }
                        else if (shared->keys[i].positions[0] == x && shared->keys[i].positions[0] == y)
                        {
                            taken = true;
                            break;
                        }
                    }
                } while (taken == true);
                shared->keys[i].positions[0] = x;
                shared->keys[i].positions[1] = y;
                key_count += 1; 
            }
        }
        if (permit_count < 1)
        {
            if (shared->permits[i].positions[0] == -1 || shared->permits[i].positions[1] == -1)
            {
                int x;
                int y;

                bool taken = false;
                do {
                    taken = false;
                    x = 13 + rand()%3;
                    y = 11 + rand()%6;
                    for (int i = 0; i < 2; ++i)
                    {
                        if (shared->permits[i].positions[0] == x && shared->permits[i].positions[0] == y)
                        {
                            taken = true;
                            break;
                        }
                        if (shared->keys[i].positions[0] == x && shared->keys[i].positions[0] == y)
                        {
                            taken = true;
                            break;
                        }
                    }
                } while (taken == true);
                shared->permits[i].positions[0] = x;
                shared->permits[i].positions[1] = y;
                permit_count += 1;
            }
        }
    }
    return;
}

void savePermitPositions(SharedMemory* shared, int level1[31][28])
{
    for (int i = 0; i < 2; ++i)
    {
        level1[shared->permits[i].positions[0]][shared->permits[i].positions[1]] = 4;
        level1[shared->keys[i].positions[0]][shared->keys[i].positions[1]] = 5;
    }
}

void checkKey(SharedMemory* shared, GhostMemory* ghost_mem)
{
    if (ghost_mem->key == false)
    {
        if (shared->level1[ghost_mem->ghost_pos[0]][ghost_mem->ghost_pos[1]] == 4)
        {
            shared->level1[ghost_mem->ghost_pos[0]][ghost_mem->ghost_pos[1]] = -1;
            for (int i = 0; i < 2; ++i)
            {
                if (shared->keys[i].positions[0] == ghost_mem->ghost_pos[0]
                   && shared->keys[i].positions[1] == ghost_mem->ghost_pos[1])
                {
                    shared->keys[i].positions[0] = -1;
                    shared->keys[i].positions[1] = -1;
                }
            }
            ghost_mem->key = true;
        }
    }
}

void checkPermit(SharedMemory* shared, GhostMemory* ghost_mem)
{
    if (ghost_mem->permit == false)
    {
        if (ghost_mem->key == true)
        {
            if (shared->level1[ghost_mem->ghost_pos[0]][ghost_mem->ghost_pos[1]] == 5)
            {
                shared->level1[ghost_mem->ghost_pos[0]][ghost_mem->ghost_pos[1]] = -1;
                for (int i = 0; i < 2; ++i)
                {
                    if (shared->permits[i].positions[0] == ghost_mem->ghost_pos[0]
                       && shared->permits[i].positions[1] == ghost_mem->ghost_pos[1])
                    {
                        shared->permits[i].positions[0] = -1;
                        shared->permits[i].positions[1] = -1;
                    }
                }
                ghost_mem->permit = true;
            }
        }
    }
}

void freePermitsKeys(SharedMemory* shared, GhostMemory* ghost_mem)
{
    if (ghost_mem->key == true && ghost_mem->permit == true)
    {
        if (ghost_mem->ghost_pos[1] == 9)
        {
            ghost_mem->key = false;
            ghost_mem->permit = false;
            ghost_mem->left_box = true;
            setNewKeys(shared);
            savePermitPositions(shared, shared->level1);
            savePermitPositions(shared, ghost_mem->level1);
        }
    }
}

bool checkWin(SharedMemory* shared)
{
    if (shared->lives == 0)
        return true;

    for (int i = 0; i < 31; ++i)
    {
        for (int j = 0; j < 28; ++j)
        {
            if (shared->level1[i][j] == 0 || shared->level1[i][j] == 2)
                return false;
        }
    }
    shared->game_win = true;
    return true;
}

bool checkCoins(SharedMemory* shared, GhostMemory* ghost1)
{
    for (int i = 0; i < 31; ++i)
    {
        for (int j = 0; j < 28; ++j)
        {
            if (shared->level1[i][j] == 0)
            {
                return false;
            }
        }
    }
    for (int i = 0; i < 31; ++i)
    {
        for (int j = 0; j < 28; ++j)
        {
            shared->level1[i][j] = ghost1->level1[i][j];
        }
    }
    shared->pacman_pos[0] = 14;
    shared->pacman_pos[1] = 21;
    shared->poweredUp = false;
    shared->pacman_direction = 0;
    shared->ghost1_pos[0] = 13;
    shared->ghost1_pos[1] = 14;
    shared->ghost1_direction = 0;
    shared->ghost2_pos[0] = 14;
    shared->ghost2_pos[1] = 14;
    shared->ghost2_direction = 0;
    shared->ghost3_pos[0] = 15;
    shared->ghost3_pos[1] = 14;
    shared->ghost3_direction = 0;
    shared->ghost4_pos[0] = 14;
    shared->ghost4_pos[1] = 15;
    shared->ghost4_direction = 0;
    return true;
}

void UpdateGhostData(SharedMemory* shared, GhostMemory* ghost1,
                     GhostMemory* ghost2, GhostMemory* ghost3, GhostMemory* ghost4)
{
    shared->ghost1_pos[0] = ghost1->ghost_pos[0];
    shared->ghost1_pos[1] = ghost1->ghost_pos[1];
    shared->ghost1_direction = ghost1->direction;

    shared->ghost2_pos[0] = ghost2->ghost_pos[0];
    shared->ghost2_pos[1] = ghost2->ghost_pos[1];
    shared->ghost2_direction = ghost2->direction;

    shared->ghost3_pos[0] = ghost3->ghost_pos[0];
    shared->ghost3_pos[1] = ghost3->ghost_pos[1];
    shared->ghost3_direction = ghost3->direction;

    shared->ghost4_pos[0] = ghost4->ghost_pos[0];
    shared->ghost4_pos[1] = ghost4->ghost_pos[1];
    shared->ghost4_direction = ghost4->direction;

    ghost1->pacman_pos[0] = shared->pacman_pos[0];
    ghost1->pacman_pos[1] = shared->pacman_pos[1];

    ghost2->pacman_pos[0] = shared->pacman_pos[0];
    ghost2->pacman_pos[1] = shared->pacman_pos[1];

    ghost3->pacman_pos[0] = shared->pacman_pos[0];
    ghost3->pacman_pos[1] = shared->pacman_pos[1];

    ghost4->pacman_pos[0] = shared->pacman_pos[0];
    ghost4->pacman_pos[1] = shared->pacman_pos[1];

    ghost1->window_open = shared->window_open;
    ghost1->game_state = shared->game_state;
    ghost1->game_win = shared->game_win;
    ghost1->loading = shared->loading;
    ghost1->paused = shared->paused;

    ghost2->window_open = shared->window_open;
    ghost2->game_state = shared->game_state;
    ghost2->game_win = shared->game_win;
    ghost2->loading = shared->loading;
    ghost2->paused = shared->paused;

    ghost3->window_open = shared->window_open;
    ghost3->game_state = shared->game_state;
    ghost3->game_win = shared->game_win;
    ghost3->loading = shared->loading;
    ghost3->paused = shared->paused;

    ghost4->window_open = shared->window_open;
    ghost4->game_state = shared->game_state;
    ghost4->game_win = shared->game_win;
    ghost4->loading = shared->loading;
    ghost4->paused = shared->paused;
}

bool checkDeath(SharedMemory* shared, GhostMemory* ghost1,
                GhostMemory* ghost2, GhostMemory* ghost3, GhostMemory* ghost4,
                Sound& ghost_sound)
{

    if (ghost1->pacManDeath)
    {
        if (shared->poweredUp)
        {
            if (ghost1->boost == true)
                releaseBoost(ghost1);

            ghost1->left_box = false;
            shared->score += 20;
            ghost1->ghost_pos[0] = 14;
            ghost1->ghost_pos[1] = 14;
            ghost_sound.play();
        }
        else
        {
            shared->pacman_pos[0] = 14;
            shared->pacman_pos[1] = 21;
            shared->lives += -1;
            return true;
        }
        ghost1->pacManDeath = false;
    }
    if (ghost2->pacManDeath)
    {
        if (shared->poweredUp)
        {
            if (ghost2->boost == true)
                releaseBoost(ghost2);
            
            ghost2->left_box = false;
            shared->score += 20;
            ghost2->ghost_pos[0] = 14;
            ghost2->ghost_pos[1] = 14;
            ghost_sound.play();
        }
        else
        {
            shared->pacman_pos[0] = 14;
            shared->pacman_pos[1] = 21;
            shared->lives += -1;
            return true;
        }
        ghost2->pacManDeath = false;
    }
    if (ghost3->pacManDeath)
    {
        if (shared->poweredUp)
        {
            if (ghost3->boost == true)
                releaseBoost(ghost3);
            
            ghost3->left_box = false;
            shared->score += 20;
            ghost3->ghost_pos[0] = 14;
            ghost3->ghost_pos[1] = 14;
            ghost_sound.play();
        }
        else
        {
            shared->pacman_pos[0] = 14;
            shared->pacman_pos[1] = 21;
            shared->lives += -1;
            return true;
        }
        ghost3->pacManDeath = false;
    }
    if (ghost4->pacManDeath)
    {
        if (shared->poweredUp)
        {
            if (ghost4->boost == true)
                releaseBoost(ghost4);
              
            ghost4->left_box = false;
            shared->score += 20;
            ghost4->ghost_pos[0] = 14;
            ghost4->ghost_pos[1] = 14;
            ghost_sound.play();
        }
        else
        {
            shared->pacman_pos[0] = 14;
            shared->pacman_pos[1] = 21;
            shared->lives += -1;
            return true;
        }
        ghost4->pacManDeath = false;
    }

    return false;
}

void respawnPowerUps(SharedMemory* shared, Clock& timer)
{
    float time_check = timer.getElapsedTime().asSeconds();

    if (time_check > 45)
    {
        for (int i = 0; i < 12; ++i)
        {
            if (shared->level1[shared->powerupPositions[i].positions[0]][shared->powerupPositions[i].positions[1]] != 2)
            {
                shared->level1[shared->powerupPositions[i].positions[0]][shared->powerupPositions[i].positions[1]] = 2;
                timer.restart();
                return;
            }
        }
        timer.restart();
    }
}

void* Engine_Thread(void* arg)
{
    SharedMemory* shared = (SharedMemory*) arg;
    
    Clock pacman_delay_time;
    pacman_delay_time.restart();

    Clock power_time;
    power_time.restart();

    GhostMemory* ghost1 = new GhostMemory;
    ghost1->ghost_pos[0] = shared->ghost1_pos[0];
    ghost1->ghost_pos[1] = shared->ghost1_pos[1];
    savePermitPositions(shared, ghost1->level1);

    GhostMemory* ghost2 = new GhostMemory;
    ghost2->ghost_pos[0] = shared->ghost2_pos[0];
    ghost2->ghost_pos[1] = shared->ghost2_pos[1];
    savePermitPositions(shared, ghost2->level1);

    GhostMemory* ghost3 = new GhostMemory;
    ghost3->ghost_pos[0] = shared->ghost3_pos[0];
    ghost3->ghost_pos[1] = shared->ghost3_pos[1];
    savePermitPositions(shared, ghost3->level1);

    GhostMemory* ghost4 = new GhostMemory;
    ghost4->ghost_pos[0] = shared->ghost4_pos[0];
    ghost4->ghost_pos[1] = shared->ghost4_pos[1];
    savePermitPositions(shared, ghost4->level1);

    SoundBuffer eat_buff;
    Sound eat_sound;
    eat_buff.loadFromFile("audio/pellet.wav");
    eat_sound.setBuffer(eat_buff);
    eat_sound.setVolume(40);

    SoundBuffer power_buff;
    Sound power_sound;
    power_buff.loadFromFile("audio/powerup.wav");
    power_sound.setBuffer(power_buff);
    power_sound.setVolume(100);

    SoundBuffer death_buff;
    Sound death_sound;
    death_buff.loadFromFile("audio/death.wav");
    death_sound.setBuffer(death_buff);
    death_sound.setVolume(100);

    SoundBuffer ghost_buff;
    Sound ghost_sound;
    ghost_buff.loadFromFile("audio/ghost_death.wav");
    ghost_sound.setBuffer(ghost_buff);
    ghost_sound.setVolume(100);

    for (int i = 0; i < 31; ++i)
    {
        for (int j = 0; j < 28; ++j)
        {
            shared->level1[i][j] = ghost1->level1[i][j];
        }
    }

    pthread_t ghost1_t;
    pthread_create( &ghost1_t, NULL, ghostFunctionality, (void*) ghost1);
    pthread_t ghost2_t;
    pthread_create( &ghost2_t, NULL, ghostFunctionality, (void*) ghost2);
    pthread_t ghost3_t;
    pthread_create( &ghost3_t, NULL, ghostFunctionality, (void*) ghost3);
    pthread_t ghost4_t;
    pthread_create( &ghost4_t, NULL, ghostFunctionality, (void*) ghost4);

    Clock death_timer;
    death_timer.restart();

    Clock powerup_timer;
    powerup_timer.restart();

    while (shared->window_open)
    {
        if (shared->paused == 0)
        {
            sleep(3);
            shared->paused = 2;
        }
        if (shared->paused == -2)
        {
            sleep(3);
            shared->paused = 2;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::P))
        {
            shared->paused = 1;
            ghost1->paused = 1;
            ghost2->paused = 1;
            ghost3->paused = 1;
            ghost4->paused = 1;
        }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::R))
        {
            shared->paused = 2;
            ghost1->paused = 2;
            ghost2->paused = 2;
            ghost3->paused = 2;
            ghost4->paused = 2;
        }

        if (shared->paused >= 2)
        {
            if (shared->game_state == 0 && shared->loading == false)
            {
                //pthread_mutex_lock(&movement_mutex);
                PacManFunctionality(shared, pacman_delay_time, power_time, eat_sound, power_sound);
                respawnPowerUps(shared, powerup_timer);
                //pthread_mutex_unlock(&movement_mutex);
            }

            float death_delay = death_timer.getElapsedTime().asSeconds();

            if (death_delay > 5)
            {
                if (checkDeath(shared, ghost1, ghost2, ghost3, ghost4, ghost_sound))
                {
                    death_sound.play();
                    shared->pacman_direction = 0;
                    shared->paused = -2;
                    death_timer.restart();
                }
            }

            pthread_mutex_lock(&movement_mutex);
            UpdateGhostData(shared, ghost1, ghost2, ghost3, ghost4);

            checkKey(shared, ghost1);
            checkPermit(shared, ghost1);
            freePermitsKeys(shared, ghost1);

            checkKey(shared, ghost2);
            checkPermit(shared, ghost2);
            freePermitsKeys(shared, ghost2);

            checkKey(shared, ghost3);
            checkPermit(shared, ghost3);
            freePermitsKeys(shared, ghost3);

            checkKey(shared, ghost4);
            checkPermit(shared, ghost4);
            freePermitsKeys(shared, ghost4);

            pthread_mutex_unlock(&movement_mutex);

            if (checkWin(shared))
            {
                if (shared->game_win)
                    shared->game_state = 1;
                else 
                    shared->game_state = 2;

                pthread_cancel(ghost1_t);
                pthread_cancel(ghost2_t);
                pthread_cancel(ghost3_t);
                pthread_cancel(ghost4_t);
                return NULL;
            }
            if (checkCoins(shared, ghost1))
            {
                shared->pacman_speed -= 10;
                ghost1->ghost_delay -= 10;
                ghost2->ghost_delay -= 10;
                ghost3->ghost_delay -= 10;
                ghost4->ghost_delay -= 10;

                ghost1->ghost_pos[0] = shared->ghost1_pos[0];
                ghost1->ghost_pos[1] = shared->ghost1_pos[1];
                ghost2->ghost_pos[0] = shared->ghost2_pos[0];
                ghost2->ghost_pos[1] = shared->ghost2_pos[1];
                ghost3->ghost_pos[0] = shared->ghost3_pos[0];
                ghost3->ghost_pos[1] = shared->ghost3_pos[1];
                ghost4->ghost_pos[0] = shared->ghost4_pos[0];
                ghost4->ghost_pos[1] = shared->ghost4_pos[1];
                
                shared->paused = -2;
                ghost1->paused = -2;
                ghost2->paused = -2;
                ghost3->paused = -2;
                ghost4->paused = -2;
            }
        }
    }
}

void resetGame(SharedMemory* shared)
{
    shared->pacman_pos[0] = 14;
    shared->pacman_pos[1] = 21;
    shared->poweredUp = false;
    shared->pacman_direction = 0;
    shared->lives = 3;

    shared->ghost1_pos[0] = 13;
    shared->ghost1_pos[1] = 14;
    shared->ghost1_direction = 0;
    shared->ghost2_pos[0] = 14;
    shared->ghost2_pos[1] = 14;
    shared->ghost2_direction = 0;
    shared->ghost3_pos[0] = 15;
    shared->ghost3_pos[1] = 14;
    shared->ghost3_direction = 0;
    shared->ghost4_pos[0] = 14;
    shared->ghost4_pos[1] = 15;
    shared->ghost4_direction = 0;

    shared->score = 0;
    shared->window_open = true;
    shared->paused = 0;
    shared->game_state = -1;
    shared->game_win = false;
    shared->loading = false;
}

void printGhosts(SharedMemory* shared, RenderWindow &window,
                 Sprite& ghost, Texture left, Texture right,
                 Texture weak1, Texture weak2, int ghost_num,
                 Clock& ghost_animation, int direction, int& state)
{
    if (shared->poweredUp)
    {
        float ghost_time = ghost_animation.getElapsedTime().asMilliseconds();

        if (ghost_time > 300)
        {
            state = 1 + (state % 2);
            ghost_animation.restart();
        }

        if (state == 1)
            ghost.setTexture(weak1);
        else
            ghost.setTexture(weak2);
    }
    else
    {
        ghost.setTexture(left);

        if (direction == 1)
            ghost.setTexture(left);
        else if (direction == 2)
            ghost.setTexture(right);
    }

    if (ghost_num == 1)
        ghost.setPosition(shared->ghost1_pos[0]*25, shared->ghost1_pos[1]*25 - 5);
    else if (ghost_num == 2)
        ghost.setPosition(shared->ghost2_pos[0]*25, shared->ghost2_pos[1]*25 - 5);
    else if (ghost_num == 3)
        ghost.setPosition(shared->ghost3_pos[0]*25, shared->ghost3_pos[1]*25 - 5);
    else if (ghost_num == 4)
        ghost.setPosition(shared->ghost4_pos[0]*25, shared->ghost4_pos[1]*25 - 5);

    
    window.draw(ghost);
}

void printPacMan(SharedMemory* shared, RenderWindow &window, 
                Sprite& pacman, 
                Texture pacman_tex1, Texture pacman_tex2, Texture pacman_tex3,
                Texture pacman_tex4, Texture pacman_tex5, Texture pacman_tex6,
                Texture pacman_tex7, Texture pacman_tex8, Texture pacman_tex9,
                Texture pacman_tex10, Texture pacman_tex11, Texture pacman_tex12,
                int& pac_state, Clock& pacman_animation)
{
    float pac_time = pacman_animation.getElapsedTime().asMilliseconds();

    if (pac_time > 100)
    {
        pac_state += 1;
        pac_state = 1 + (pac_state % 12);
        pacman_animation.restart();
    }

    if (pac_state == 1)
        pacman.setTexture(pacman_tex1);
    else if (pac_state == 2)
        pacman.setTexture(pacman_tex2);
    else if (pac_state == 3)
        pacman.setTexture(pacman_tex3);
    else if (pac_state == 4)
        pacman.setTexture(pacman_tex4);
    else if (pac_state == 5)
        pacman.setTexture(pacman_tex5);
    else if (pac_state == 6)
        pacman.setTexture(pacman_tex6);
    else if (pac_state == 7)
        pacman.setTexture(pacman_tex7);
    else if (pac_state == 8)
        pacman.setTexture(pacman_tex8);
    else if (pac_state == 9)
        pacman.setTexture(pacman_tex9);
    else if (pac_state == 10)
        pacman.setTexture(pacman_tex10);
    else if (pac_state == 11)
        pacman.setTexture(pacman_tex11);
    else if (pac_state == 12)
        pacman.setTexture(pacman_tex12);
        
    Vector2f offset;
    offset.x = shared->pacman_pos[0] - (pacman.getPosition().x - 2)/25;
    offset.y = shared->pacman_pos[1] - ((pacman.getPosition().y + 5)/25);


    //pacman.move(offset.x * 3, offset.y * 3);
    
    pacman.setPosition(shared->pacman_pos[0]*25, shared->pacman_pos[1]*25 - 5);
    window.draw(pacman);
}

void game_UI(SharedMemory* shared, RenderWindow &window)
{
    sf::Font font1, font2;
    font1.loadFromFile("fonts/font1.ttf");
    font2.loadFromFile("fonts/font2.ttf");

    sf::Text score_text;
    score_text.setFont(font2);
    score_text.setString("Score: " + to_string(shared->score));
    score_text.setPosition(800, 100);
    score_text.setCharacterSize(35);
    score_text.setFillColor(sf::Color::White);

    sf::Text score_num;
    score_num.setFont(font1);
    score_num.setString(to_string(shared->score));
    score_num.setPosition(800, 140);
    score_num.setCharacterSize(35);
    score_num.setFillColor(sf::Color::White);

    Sprite lives;
    Texture lives_tex;
    lives_tex.loadFromFile("images/lives.png");
    lives.setTexture(lives_tex);
    lives.setScale(0.1,0.1);

    Sprite paused;
    Texture pause_tex;
    pause_tex.loadFromFile("images/menu/pause.png");
    paused.setTexture(pause_tex);

    Sprite mazer;
    Texture mazer_tex;

    mazer_tex.loadFromFile("images/mazer1.png");
    mazer.setTexture(mazer_tex);

    Sprite key;
    Texture key_tex;
    Sprite permit;
    Texture permit_tex;
    Sprite ghost_wall;
    Texture ghost_wall_tex;
    Sprite portal;
    Texture portal_tex;
    Sprite grass;
    Texture grass_tex;
    Sprite wall;
    Texture wall_tex;
    Sprite pellet;
    Texture pellet_tex;
    Sprite PowerUp;
    Texture power_tex;
    Sprite background;
    Texture back_tex;

    Sprite pacman;
    Texture pacman_tex1, pacman_tex2, pacman_tex3;
    Texture pacman_tex4, pacman_tex5, pacman_tex6;
    Texture pacman_tex7, pacman_tex8, pacman_tex9;
    Texture pacman_tex10, pacman_tex11, pacman_tex12;

    Sprite ghost1, ghost2, ghost3, ghost4;
    Texture ghost1_righttex, ghost2_righttex, ghost3_righttex, ghost4_righttex;
    Texture ghost1_lefttex, ghost2_lefttex, ghost3_lefttex, ghost4_lefttex;
    Texture weak_ghost1, weak_ghost2;

    portal_tex.loadFromFile("images/portal.png");
    back_tex.loadFromFile("images/background.png");
    grass_tex.loadFromFile("images/grass.png");
    wall_tex.loadFromFile("images/brick3.png");
    pellet_tex.loadFromFile("images/pellet.png");
    power_tex.loadFromFile("images/bacon.png");
    ghost_wall_tex.loadFromFile("images/ghostwall.png");
    key_tex.loadFromFile("images/key.png");
    permit_tex.loadFromFile("images/permit.png");

    pacman_tex1.loadFromFile("images/pacman/1.png");
    pacman_tex2.loadFromFile("images/pacman/2.png");
    pacman_tex3.loadFromFile("images/pacman/3.png");
    pacman_tex4.loadFromFile("images/pacman/4.png");
    pacman_tex5.loadFromFile("images/pacman/5.png");
    pacman_tex6.loadFromFile("images/pacman/6.png");
    pacman_tex7.loadFromFile("images/pacman/7.png");
    pacman_tex8.loadFromFile("images/pacman/8.png");
    pacman_tex9.loadFromFile("images/pacman/9.png");
    pacman_tex10.loadFromFile("images/pacman/10.png");
    pacman_tex11.loadFromFile("images/pacman/11.png");
    pacman_tex12.loadFromFile("images/pacman/12.png");

    ghost1_righttex.loadFromFile("images/ghost/ghost1/ghost1right.png");
    ghost1_lefttex.loadFromFile("images/ghost/ghost1/ghost1left.png");

    ghost2_righttex.loadFromFile("images/ghost/ghost2/ghost2right.png");
    ghost2_lefttex.loadFromFile("images/ghost/ghost2/ghost2left.png");

    ghost3_righttex.loadFromFile("images/ghost/ghost3/ghost3right.png");
    ghost3_lefttex.loadFromFile("images/ghost/ghost3/ghost3left.png");

    ghost4_righttex.loadFromFile("images/ghost/ghost4/ghost4right.png");
    ghost4_lefttex.loadFromFile("images/ghost/ghost4/ghost4left.png");

    weak_ghost1.loadFromFile("images/ghost/weak1.png");
    weak_ghost2.loadFromFile("images/ghost/weak2.png");

    portal.setTexture(portal_tex);
    portal.setScale(0.5,0.5);
    grass.setTexture(grass_tex);
    grass.setScale(0.5,0.5);
    background.setTexture(back_tex);
    background.setScale(1,1);
    wall.setTexture(wall_tex);
    wall.setScale(0.5,0.5);
    pellet.setTexture(pellet_tex);
    pellet.setScale(0.5,0.5);
    PowerUp.setTexture(power_tex);
    PowerUp.setScale(0.5,0.5);
    ghost_wall.setTexture(ghost_wall_tex);
    ghost_wall.setScale(0.5,0.5);
    key.setTexture(key_tex);
    key.setScale(0.5,0.5);
    permit.setTexture(permit_tex);
    permit.setScale(0.5,0.5);

    pacman.setTexture(pacman_tex1);
    pacman.setScale(0.45,0.45);

    ghost1.setTexture(ghost1_lefttex);
    ghost1.setScale(0.5,0.5);
    ghost2.setTexture(ghost2_lefttex);
    ghost2.setScale(0.5,0.5);
    ghost3.setTexture(ghost3_lefttex);
    ghost3.setScale(0.5,0.5);
    ghost4.setTexture(ghost4_lefttex);
    ghost4.setScale(0.5,0.5);

    Clock pacman_animation;
    pacman_animation.restart();
    int pac_state = 1;

    Clock ghost1_animation;
    ghost1_animation.restart();
    int ghost1_state = 1;

    Clock ghost2_animation;
    ghost2_animation.restart();
    int ghost2_state = 1;

    Clock ghost3_animation;
    ghost3_animation.restart();
    int ghost3_state = 1;

    Clock ghost4_animation;
    ghost4_animation.restart();
    int ghost4_state = 1;

    pacman.setPosition(shared->pacman_pos[0]*25,shared->pacman_pos[1]*25 - 5);

    while (window.isOpen())
    {
        Event event;
        while (window.pollEvent(event))
        {
            // Request for closing the window
            if (event.type == sf::Event::Closed)
            {
                shared->window_open = false;
                window.close();
            }
        }

        window.draw(background);

        for (int i = 0; i < 31; ++i)
        {
            for (int j = 0; j < 28; ++j)
            {
                if (shared->level1[i][j] == 1)
                {
                    wall.setPosition(i*25,j*25);
                    window.draw(wall);
                }
                else if (shared->level1[i][j] == 0)
                {
                    pellet.setPosition(i*25, j*25);
                    window.draw(pellet);
                }
                else if (shared->level1[i][j] == 2)
                {
                    PowerUp.setPosition(i*25, j*25);
                    window.draw(PowerUp);
                }
                else if (shared->level1[i][j] == 3)
                {
                    ghost_wall.setPosition(i*25, j*25);
                    window.draw(ghost_wall);
                }
                else if (shared->level1[i][j] == 4)
                {
                    key.setPosition(i*25, j*25);
                    window.draw(key);
                }
                else if (shared->level1[i][j] == 5)
                {
                    permit.setPosition(i*25, j*25);
                    window.draw(permit);
                }
                else if (shared->level1[i][j] == -2)
                {
                    portal.setPosition(i*25,j*25);
                    window.draw(portal);
                }
                else if (shared->level1[i][j] == -1)
                {
                    grass.setPosition(i*25,j*25);
                    window.draw(grass);
                }
            }
        }

        window.draw(mazer);

        printPacMan(shared, window, pacman, 
                    pacman_tex1, pacman_tex2, pacman_tex3,
                    pacman_tex4, pacman_tex5, pacman_tex6,
                    pacman_tex7, pacman_tex8, pacman_tex9,
                    pacman_tex10, pacman_tex11, pacman_tex12,
                    pac_state, pacman_animation);

        printGhosts(shared, window, ghost1,
                     ghost1_lefttex, ghost1_righttex,
                     weak_ghost1, weak_ghost2, 1, ghost1_animation,
                     shared->ghost1_direction, ghost1_state);
        printGhosts(shared, window, ghost2,
                     ghost2_lefttex, ghost2_righttex,
                     weak_ghost1, weak_ghost2, 2, ghost2_animation,
                     shared->ghost2_direction, ghost2_state);
        printGhosts(shared, window, ghost3,
                     ghost3_lefttex, ghost3_righttex,
                     weak_ghost1, weak_ghost2, 3, ghost3_animation,
                     shared->ghost3_direction, ghost3_state);
        printGhosts(shared, window, ghost4,
                     ghost4_lefttex, ghost4_righttex,
                     weak_ghost1, weak_ghost2, 4, ghost4_animation,
                     shared->ghost4_direction, ghost4_state);

        score_text.setString("Score: \n");
        score_num.setString(to_string(shared->score));
        window.draw(score_text);
        window.draw(score_num);

        for (int i = 0; i < shared->lives; ++i)
        {
            lives.setPosition(800 + i * 20, 60);
            window.draw(lives);
        }

        if (shared->paused == 1)
        {
            window.draw(paused);
        }

        if (shared->game_state != 0)
            return;

        window.display();
    }
}

void menu_UI(SharedMemory* shared, RenderWindow &window)
{
    Sprite background;

    Texture menu_bg1, menu_bg2, menu_bg3, credits;

    menu_bg1.loadFromFile("images/menu/main_menu/menu11.png");
    menu_bg2.loadFromFile("images/menu/main_menu/menu22.png");
    menu_bg3.loadFromFile("images/menu/main_menu/menu33.png");
    credits.loadFromFile("images/menu/credits1.png");

    int menu_state = 0;
    background.setTexture(menu_bg1);

    while(window.isOpen())
    {
        Event event;
        while (window.pollEvent(event))
        {

            // Request for closing the window
            if (event.type == sf::Event::Closed)
            {
                shared->window_open = false;
                window.close();
            }
            else if (event.type == sf::Event::KeyReleased)
            {
                if (event.key.code == sf::Keyboard::Up || event.key.code == sf::Keyboard::Down)
                {
                    if (menu_state == 0)
                        menu_state = 1;
                    else if (menu_state == 1)
                        menu_state = 2;
                    else if (menu_state == 2)
                        menu_state = 1;
                }
                else if (event.key.code == sf::Keyboard::Enter)
                {
                    if (menu_state == 0)
                        menu_state = 1;
                    else if (menu_state == 1)
                    {
                        shared->game_state = 0;
                        window.clear();
                        return;
                    }
                    else if (menu_state == 2)
                        menu_state = 3;
                    else if (menu_state == 3)
                        menu_state = 2;
                }
            }

            if (menu_state == 0)
            {
                background.setTexture(menu_bg1);              
            }
            else if (menu_state == 1)
            {
                background.setTexture(menu_bg2); 
            }
            else if (menu_state == 2)
            {
                background.setTexture(menu_bg3);
            }
            else if (menu_state == 3)
            {
                background.setTexture(credits);
            }

            window.clear();
            window.draw(background);

            window.display();
        }
    }

}

void LoadingScreen(bool &state_loading, RenderWindow &window)
{
    state_loading = true;
    Clock timer;

    Sprite loading;

    Texture load1, load2, load3, load4, load5;

    load1.loadFromFile("images/menu/loading/load1.png");
    load2.loadFromFile("images/menu/loading/load2.png");
    load3.loadFromFile("images/menu/loading/load3.png");
    load4.loadFromFile("images/menu/loading/load4.png");
    load5.loadFromFile("images/menu/loading/load5.png");

    loading.setTexture(load1);

    Clock load_delay;
    load_delay.restart();
    int load_count = 1;

    timer.restart();
    do
    {
        float loader = load_delay.getElapsedTime().asMilliseconds();
        if (loader > 150)
        {
            if (load_count == 1)
                loading.setTexture(load1);
            else if (load_count == 2)
                loading.setTexture(load2);
            else if (load_count == 3)
                loading.setTexture(load3);
            else if (load_count == 4)
                loading.setTexture(load4);
            else if (load_count == 5)
                loading.setTexture(load5);
            else if (load_count == 6)
                loading.setTexture(load4);
            else if (load_count == 7)
                loading.setTexture(load3);
            else if (load_count == 8)
                loading.setTexture(load2);

            load_count += 1;
            load_count = 1 + load_count % 8;
            load_delay.restart();
        }
        window.draw(loading);

        window.display();
    } while (timer.getElapsedTime().asSeconds() < 1);

    state_loading = false;
}

void GameOver(SharedMemory* shared, RenderWindow &window)
{
    Sprite background;

    Texture win1, win2, loss1, loss2;

    sf::Font font1, font2;
    font1.loadFromFile("fonts/font1.ttf");
    font2.loadFromFile("fonts/font3.otf");

    sf::Text score_text;
    score_text.setFont(font2);
    score_text.setString("Score: " + to_string(shared->score));
    score_text.setPosition(100, 340);
    score_text.setCharacterSize(45);
    score_text.setFillColor(sf::Color::White);

    sf::Text score_num;
    score_num.setFont(font2);
    score_num.setString(to_string(shared->score));
    score_num.setPosition(150, 390);
    score_num.setCharacterSize(45);
    score_num.setFillColor(sf::Color::White);

    win1.loadFromFile("images/menu/win11.png");
    win2.loadFromFile("images/menu/win22.png");
    loss1.loadFromFile("images/menu/loss11.png");
    loss2.loadFromFile("images/menu/loss22.png");

    int menu_state = 1;
    if (shared->game_win)
        background.setTexture(win1);
    else
        background.setTexture(loss1);

    window.draw(background);

    while(window.isOpen())
    {
        Event event;
        while (window.pollEvent(event))
        {

            // Request for closing the window
            if (event.type == sf::Event::Closed)
            {
                shared->window_open = false;
                window.close();
            }
            else if (event.type == sf::Event::KeyReleased)
            {
                if (event.key.code == sf::Keyboard::Left || event.key.code == sf::Keyboard::Right)
                {
                    if (menu_state == 1)
                        menu_state = 2;
                    else if (menu_state == 2)
                        menu_state = 1;
                }
                else if (event.key.code == sf::Keyboard::Enter)
                {
                    if (menu_state == 1)
                    {
                        resetGame(shared);
                        window.clear();
                        shared->game_state = 0;
                        return;
                    }
                    else if (menu_state == 2)
                    {
                        shared->window_open = false;
                        window.close();
                        return;
                    }
                }
            }

            if (shared->game_win)
            {
                if (menu_state == 1)
                {
                    background.setTexture(win1); 
                }
                else if (menu_state == 2)
                {
                    background.setTexture(win2);
                }
            }
            else
            {
                if (menu_state == 1)
                {
                    background.setTexture(loss1); 
                }
                else if (menu_state == 2)
                {
                    background.setTexture(loss2);
                }
            }

            window.clear();
            window.draw(background);
            score_text.setString("Score: \n");
            score_num.setString(to_string(shared->score));
            window.draw(score_text);
            window.draw(score_num);

            window.display();
        }
    }
}

void SavePowerPositions(SharedMemory* shared)
{
    int index = 0;
    for (int i = 0; i < 31; ++i)
    {
        for (int j = 0; j < 28; ++j)
        {
            if (shared->level1[i][j] == 2)
            {
                shared->powerupPositions[index].positions[0] = i;
                shared->powerupPositions[index].positions[1] = j;
                index += 1;
            }
        }
    }
}

void* UI_Thread(void* arg)
{
    SharedMemory* shared = (SharedMemory*) arg;
    string title = "Game UI";
    pthread_t tid1;

    for (int i = 0; i < 2; ++i)
    {
        shared->permits[i].positions[0] = -1;
        shared->permits[i].positions[1] = -1;
        shared->keys[i].positions[0] = -1;
        shared->keys[i].positions[1] = -1;
    }

    setPermitsandKeys(shared);
    savePermitPositions(shared, shared->level1);
    SavePowerPositions(shared);

    SoundBuffer music_buff;
    Sound music_sound;
    music_buff.loadFromFile("audio/music.wav");
    music_sound.setBuffer(music_buff);
    music_sound.setVolume(40);

    music_sound.play();
    music_sound.setLoop(true);
    music_sound.setVolume(60);

    RenderWindow window(VideoMode(1000,800), title);

    while(window.isOpen())
    {
        Event event;
        while (window.pollEvent(event))
        {
            // Request for closing the window
            if (event.type == sf::Event::Closed)
            {
                shared->window_open = false;
                window.close();
            }

            if (shared->game_state == -1)
            {
                window.clear();
                menu_UI(shared, window);
            }
            if (shared->game_state == 0)
            {
                window.clear();
                LoadingScreen(shared->loading, window);
                pthread_create( &tid1, NULL, Engine_Thread, (void*)shared);
                game_UI(shared, window);
            }
            if (shared->game_state == 1 || 2)
            {
                window.clear();
                pthread_cancel(tid1);
                LoadingScreen(shared->loading, window);
                GameOver(shared, window);
            }
            window.display();
        }
    }
    
    pthread_exit(NULL);
}

int main()
{
    XInitThreads();
    sem_init(&key, 0, 1);
    sem_init(&key, 0, 1);

    SharedMemory* shared = new SharedMemory;

    pthread_t tid2;
    pthread_create( &tid2, NULL, UI_Thread, (void*)shared);
    
    while (shared->window_open)
    {}

    return 0;
}