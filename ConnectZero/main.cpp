#include <iostream>
#include <bitset>
#include <thread>
#include "Board.h"
#include "MonteCarloTreeSearch.h"

#include <SFML/Graphics.hpp>

#define SQUARE_SIZE 80

Board gameBoard;

void waitForComputer()
{
    std::this_thread::sleep_for(std::chrono::milliseconds(2000));
    stopComputer();
}

void playMove(int move)
{
    if (!gameBoard.GetStatus())
    {
        if (move >= 0 && move < 7)
        {
            gameBoard.Move(move);
        }
    }
}

void launchComputerThread()
{
    std::thread thinkingThread(findBestMoveCallback, gameBoard, playMove);
    thinkingThread.detach();

    std::thread timingThread(waitForComputer);
    timingThread.detach();
}

int main()
{
    sf::RenderWindow window(sf::VideoMode(7 * SQUARE_SIZE, 6 * SQUARE_SIZE), "ConnectZero", sf::Style::Titlebar | sf::Style::Close);
    sf::Event ev;

    //Game Loop
    while (window.isOpen())
    {
        // Events
        while (window.pollEvent(ev))
        {
            switch (ev.type)
            {
            case sf::Event::Closed:
                window.close();
                break;
            case sf::Event::KeyPressed:
                if (ev.key.code == sf::Keyboard::Escape)
                    window.close();
                else
                    std::cout << ev.key.code << std::endl;
                break;
            case sf::Event::MouseButtonReleased:
                if (ev.mouseButton.button == sf::Mouse::Left)
                {
                    int mouseX = sf::Mouse::getPosition().x - window.getPosition().x;
                    int file = mouseX / SQUARE_SIZE;

                    if (gameBoard.redsTurn)
                    {
                        char possibleMoves = gameBoard.PossibleMoves();
                        if (possibleMoves & (1 << file))
                        {
                            if (!gameBoard.GetStatus())
                            {
                                gameBoard.Move(file);

                                if (!gameBoard.GetStatus())
                                {
                                    launchComputerThread();
                                }
                            }
                        }
                    }
                }
                break;
            }
        }

        window.clear(sf::Color(200, 200, 200));

        sf::CircleShape circle(SQUARE_SIZE * 0.3f, 30);

        for (int j = 0; j < 6; j++)
        {
            for (int i = 0; i < 7; i++)
            {
                float offset = SQUARE_SIZE * 0.2f;
                circle.setPosition(i * SQUARE_SIZE + offset, j * SQUARE_SIZE + offset);

                U64 bit = 1ULL << (i + 8 * (5 - j));

                if (gameBoard.red & bit)
                {
                    circle.setFillColor(sf::Color(255, 50, 50));
                }
                else if (gameBoard.blue & bit)
                {
                    circle.setFillColor(sf::Color(50, 50, 255));
                }
                else
                {
                    circle.setFillColor(sf::Color(150, 150, 150));
                }

                window.draw(circle);
            }
        }

        window.display();
    }

    return 0;
}