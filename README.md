# Multithreaded Pacman Game

Welcome to the Multithreaded Pacman Game repository! This project is a classic Pacman game implemented in C++ with multithreading capabilities.

## Features

- **Multithreaded Gameplay:** Experience smooth and responsive gameplay with multithreading support.
- **Player-Controlled Pacman Character:** Guide Pacman through the maze to collect points and avoid ghosts.
- **AI-Controlled Ghosts:** Beware of the ghosts, they chase Pac-Man if you get spotted.
- **Score Tracking:** Keep track of your score as you collect dots.
- **Synchronisation:** Various synchronisation techniques ensure smooth gameplay and no issues between threads. 

## Installation Instructions
1. Clone the repository to your local machine.
2. Navigate to the project directory.
3. Compile the source code using your preferred C++ compiler.
     - Compilation command for Ubuntu: g++ main.cpp -o main -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio -pthread -lX11
     - Run: ./main
  
## Usage

- Use the arrow keys to move Pacman up, down, left, or right.
- Avoid the ghosts while collecting dots to increase your score.
- Each time you collide with a ghost, you lose a life.
- Eat PowerUps to be able to eat ghosts for extra points.
- Eat all dots to win the game.

## Credits

- Developed by Ali Usman(https://github.com/aliusman0926)
- Based on the classic Pacman game concept.
- Made using the SFML and pthread libraries for C++.

## Contact

If you have any questions or suggestions, feel free to contact me at ali.usman8704@gmail.com
