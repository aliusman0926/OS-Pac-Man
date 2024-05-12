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

using namespace std;
using namespace sf;

struct SharedMemory
{
public:
    int pacman_pos[2] = {23,14};
    int pacman_direction = 0;
    int score = 0;
    bool window_open = true;
    int paused = 0;
    int game_state = -1;
    bool game_win = false;
    bool loading = false;
    int level1[31][28] = {
                            {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
                            {1,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,1},
                            {1,0,1,1,1,1,0,1,1,1,1,1,0,1,1,0,1,1,1,1,1,0,1,1,1,1,0,1},
                            {1,0,1,-1,-1,1,0,1,-1,-1,-1,1,0,1,1,0,1,-1,-1,-1,1,0,1,-1,-1,1,0,1},
                            {1,0,1,1,1,1,0,1,1,1,1,1,0,1,1,0,1,1,1,1,1,0,1,1,1,1,0,1},
                            {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
                            {1,0,1,1,1,1,0,1,1,0,1,1,1,1,1,1,1,1,0,1,1,0,1,1,1,1,0,1},
                            {1,0,1,1,1,1,0,1,1,0,1,1,1,1,1,1,1,1,0,1,1,0,1,1,1,1,0,1},
                            {1,0,0,0,0,0,0,1,1,0,0,0,0,1,1,0,0,0,0,1,1,0,0,0,0,0,0,1},
                            {1,1,1,1,1,1,0,1,1,1,1,1,-1,1,1,-1,1,1,1,1,1,0,1,1,1,1,1,1},
                            {-1,-1,-1,-1,-1,1,0,1,1,1,1,1,-1,1,1,-1,1,1,1,1,1,0,1,-1,-1,-1,-1,-1},
                            {-1,-1,-1,-1,-1,1,0,1,1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,1,1,0,1,-1,-1,-1,-1,-1},
                            {-1,-1,-1,-1,-1,1,0,1,1,-1,1,1,1,1,1,1,1,1,-1,1,1,0,1,-1,-1,-1,-1,-1},
                            {1,1,1,1,1,1,0,1,1,-1,1,-1,-1,-1,-1,-1,-1,1,-1,1,1,0,1,1,1,1,1,1},
                            {-2,-1,-1,-1,-1,-1,0,-1,-1,-1,1,-1,-1,-1,-1,-1,-1,1,-1,-1,-1,0,-1,-1,-1,-1,-1,-2},
                            {1,1,1,1,1,1,0,1,1,-1,1,-1,-1,-1,-1,-1,-1,1,-1,1,1,0,1,1,1,1,1,1},
                            {-1,-1,-1,-1,-1,1,0,1,1,-1,1,1,1,1,1,1,1,1,-1,1,1,0,1,-1,-1,-1,-1,-1},
                            {-1,-1,-1,-1,-1,1,0,1,1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,1,1,0,1,-1,-1,-1,-1,-1},
                            {-1,-1,-1,-1,-1,1,0,1,1,-1,1,1,1,1,1,1,1,1,-1,1,1,0,1,-1,-1,-1,-1,-1},
                            {1,1,1,1,1,1,0,1,1,-1,1,1,1,1,1,1,1,1,-1,1,1,0,1,1,1,1,1,1},
                            {1,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,1},
                            {1,0,1,1,1,1,0,1,1,1,1,1,0,1,1,0,1,1,1,1,1,0,1,1,1,1,0,1},
                            {1,0,1,1,1,1,0,1,1,1,1,1,0,1,1,0,1,1,1,1,1,0,1,1,1,1,0,1},
                            {1,0,0,0,1,1,0,0,0,0,0,0,0,-1,-1,0,0,0,0,0,0,0,1,1,0,0,0,1},
                            {1,1,1,0,1,1,0,1,1,0,1,1,1,1,1,1,1,1,0,1,1,0,1,1,0,1,1,1},
                            {1,1,1,0,1,1,0,1,1,0,1,1,1,1,1,1,1,1,0,1,1,0,1,1,0,1,1,1},
                            {1,0,0,0,0,0,0,1,1,0,0,0,0,1,1,0,0,0,0,1,1,0,0,0,0,0,0,1},
                            {1,0,1,1,1,1,1,1,1,1,1,1,0,1,1,0,1,1,1,1,1,1,1,1,1,1,0,1},
                            {1,0,1,1,1,1,1,1,1,1,1,1,0,1,1,0,1,1,1,1,1,1,1,1,1,1,0,1},
                            {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
                            {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
                         };
};

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

void eatPellet(int* position, int board[31][28], int& score)
{
    if (board[position[0]][position[1]] == 0)
    {
        score += 1;
        board[position[0]][position[1]] = -1;
    }
}

bool checkWin(SharedMemory* shared)
{
    for (int i = 0; i < 31; ++i)
    {
        for (int j = 0; j < 28; ++j)
        {
            if (shared->level1[i][j] == 0 || shared->level1[i][j] == 2)
                return false;
        }
    }
    return true;
}

void PacManFunctionality(SharedMemory* shared, Clock& pacman_delay_time)
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
        if (pacman_delay > 200)
        {
            pacman_move(shared->pacman_direction, shared->level1, shared->pacman_pos);
            pacman_delay_time.restart();
        }
        eatPellet(shared->pacman_pos, shared->level1, shared->score);
    }
}

void* Engine_Thread(void* arg)
{
    SharedMemory* shared = (SharedMemory*) arg;
    
    Clock pacman_delay_time;
    pacman_delay_time.restart();

    while (shared->window_open)
    {
        if (shared->game_state == 0 && shared->loading == false)
        {
            PacManFunctionality(shared, pacman_delay_time);
        }

        if (checkWin(shared))
        {
            if (shared->game_win)
                shared->game_state = 1;
            else 
                shared->game_state = 2;
        }
    }
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

    Sprite portal;
    Texture portal_tex;
    Sprite wall;
    Texture wall_tex;
    Sprite pellet;
    Texture pellet_tex;
    Sprite background;
    Texture back_tex;
    Sprite pacman;
    Texture pacman_texRight1, pacman_texRight2, pacman_texRight3;
    Texture pacman_texLeft1, pacman_texLeft2, pacman_texLeft3;
    Texture pacman_texUp1, pacman_texUp2, pacman_texUp3;
    Texture pacman_texDown1, pacman_texDown2, pacman_texDown3;

    portal_tex.loadFromFile("images/Portal.png");
    back_tex.loadFromFile("images/background.jpg");
    wall_tex.loadFromFile("images/brick1.png");
    pellet_tex.loadFromFile("images/pellet.png");
    pacman_texRight1.loadFromFile("images/pacman/pacmanRight/pacman1.jpg");
    pacman_texRight2.loadFromFile("images/pacman/pacmanRight/pacman2.jpg");
    pacman_texRight3.loadFromFile("images/pacman/pacmanRight/pacman3.jpg");
    pacman_texLeft1.loadFromFile("images/pacman/pacmanLeft/pacman1.jpg");
    pacman_texLeft2.loadFromFile("images/pacman/pacmanLeft/pacman2.jpg");
    pacman_texLeft3.loadFromFile("images/pacman/pacmanLeft/pacman3.jpg");
    pacman_texUp1.loadFromFile("images/pacman/pacmanUp/pacman1.jpg");
    pacman_texUp2.loadFromFile("images/pacman/pacmanUp/pacman2.jpg");
    pacman_texUp3.loadFromFile("images/pacman/pacmanUp/pacman3.jpg");
    pacman_texDown1.loadFromFile("images/pacman/pacmanUp/pacman1.jpg");
    pacman_texDown2.loadFromFile("images/pacman/pacmanDown/pacman2.jpg");
    pacman_texDown3.loadFromFile("images/pacman/pacmanDown/pacman3.jpg");

    portal.setTexture(portal_tex);
    portal.setScale(0.5,0.5);
    background.setTexture(back_tex);
    background.setScale(1,1);
    wall.setTexture(wall_tex);
    wall.setScale(0.5,0.5);
    pellet.setTexture(pellet_tex);
    pellet.setScale(1,1);
    pacman.setTexture(pacman_texRight1);
    pacman.setScale(0.14,0.14);

    Clock pacman_animation;
    pacman_animation.restart();
    int pac_state = 1;

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

        float pac_time = pacman_animation.getElapsedTime().asMilliseconds();

        if (pac_time > 150)
        {
            pac_state += 1;
            pac_state = 1 + (pac_state%3);
            pacman_animation.restart();
        }

        if (pac_state == 1)
        {
            if (shared->pacman_direction == 1)
                pacman.setTexture(pacman_texRight1);
            else if (shared->pacman_direction == 2)
                pacman.setTexture(pacman_texLeft1);
            else if (shared->pacman_direction == 3)
                pacman.setTexture(pacman_texDown1);
            else if (shared->pacman_direction == 4)
                pacman.setTexture(pacman_texUp1);
        }
        else if (pac_state == 2)
        {
            if (shared->pacman_direction == 1)
                pacman.setTexture(pacman_texRight2);
            else if (shared->pacman_direction == 2)
                pacman.setTexture(pacman_texLeft2);
            else if (shared->pacman_direction == 3)
                pacman.setTexture(pacman_texDown2);
            else if (shared->pacman_direction == 4)
                pacman.setTexture(pacman_texUp2);
        }
        else if (pac_state == 3)
        {
            if (shared->pacman_direction == 1)
                pacman.setTexture(pacman_texRight3);
            else if (shared->pacman_direction == 2)
                pacman.setTexture(pacman_texLeft3);
            else if (shared->pacman_direction == 3)
                pacman.setTexture(pacman_texDown3);
            else if (shared->pacman_direction == 4)
                pacman.setTexture(pacman_texUp3);
        }

        pacman.setPosition(2 + shared->pacman_pos[0]*25, 2 + shared->pacman_pos[1]*25);

        window.draw(pacman);
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
                    pellet.setPosition(10 + i*25, 10 + j*25);
                    window.draw(pellet);
                }
                else if (shared->level1[i][j] == -2)
                {
                    portal.setPosition(i*25,j*25);
                    window.draw(portal);
                }
            }
        }
        score_text.setString("Score: \n");
        score_num.setString(to_string(shared->score));
        window.draw(score_text);
        window.draw(score_num);

        if (shared->game_state != 0)
            return;

        window.display();
    }
}

void menu_UI(SharedMemory* shared, RenderWindow &window)
{
    Sprite background;

    Texture menu_bg1, menu_bg2, menu_bg3, credits;

    menu_bg1.loadFromFile("images/menu/main_menu/menu1.png");
    menu_bg2.loadFromFile("images/menu/main_menu/menu2.png");
    menu_bg3.loadFromFile("images/menu/main_menu/menu3.png");
    credits.loadFromFile("images/menu/credits.png");

    int menu_state = 0;

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
    } while (timer.getElapsedTime().asSeconds() < 5);

    state_loading = false;
}

void* UI_Thread(void* arg)
{
    SharedMemory* shared = (SharedMemory*) arg;
    string title = "Game UI";

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
                game_UI(shared, window);
            }

            window.clear();
            window.display();
        }
    }
    
    pthread_exit(NULL);
}

int main()
{
    XInitThreads();

    SharedMemory* shared = new SharedMemory;

    pthread_t tid1;
    pthread_create( &tid1, NULL, Engine_Thread, (void*)shared);
    pthread_t tid2;
    pthread_create( &tid2, NULL, UI_Thread, (void*)shared);
    
    while (shared->window_open)
    {}

    return 0;
}