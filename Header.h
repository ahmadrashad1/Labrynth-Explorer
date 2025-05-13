#pragma once
#pragma once


#pragma once
#include "source.cpp"
#include <ctime>
#include<windows.h>
#include <SFML/Graphics.hpp>
//#include <SFML/Audio.hpp>
#include "player1.h"

#include <SFML/Graphics.hpp>



class Menu {
public:
    Font font;
    Text title;
    Text crawlText;
    Text options[4];
    Sprite leftWing;
    Sprite rightWing;
    Text levelOptions[4];

    bool isPaused;
    // SoundBuffer buffer;
     //Sound sound;
    Game g;


    //void display_menu();

    Menu() : isPaused(false)
    {




        font.loadFromFile("img/font22.otf");
        title.setFont(font);
        title.setString("DODGE EM");
        title.setCharacterSize(72);
        title.setFillColor(Color::White);
        title.setPosition(200, 50);

        font.loadFromFile("img/font22.otf");
        crawlText.setFont(font);
        crawlText.setString("");
        crawlText.setCharacterSize(24);
        crawlText.setFillColor(Color::Yellow);
        crawlText.setPosition(275, 400);

        /*options[0].setFont(font);
        options[0].setString("New Game");
        options[0].setCharacterSize(30);
        options[0].setFillColor(Color::White);
        options[0].setPosition(300, 150);*/
        int inc = 50;
        for (int i = 0; i < 4; i++) {

            levelOptions[i].setFont(font);
            levelOptions[i].setCharacterSize(30);
            levelOptions[i].setFillColor(Color::White);
            levelOptions[i].setPosition(295, 150 + inc); // Adjust positions accordingly
            levelOptions[i].setString("Level " + std::to_string(i + 1));
            inc += 50;
        }

        /* options[1].setFont(font);
         options[1].setString("Continue Game");
         options[1].setCharacterSize(30);
         options[1].setFillColor(Color::White);
         options[1].setPosition(250, 400);*/

        options[2].setFont(font);
        options[2].setString("LeaderBoard");
        options[2].setCharacterSize(30);
        options[2].setFillColor(Color::White);
        options[2].setPosition(295, 450);

        /* options[3].setFont(font);
         options[3].setString("Exit");
         options[3].setCharacterSize(30);
         options[3].setFillColor(Color::White);
         options[3].setPosition(363, 500);*/



         //Texture tex;
         //tex.loadFromFile("img/dragon.png");
         //leftWing.setTexture(tex);
         ////leftWing.setPosition(0, 400);

         //tex.loadFromFile("img/dragon1.png");
         //rightWing.setTexture(tex);
         //rightWing.setPosition(700, 400);

        // buffer.loadFromFile("assets/selectOption.wav");
         //sound.setBuffer(buffer);



    }

    void displayLeaderboard()
    {
        // Create a new window for the leaderboard
        RenderWindow leaderboardWindow(VideoMode(780, 780), "Leaderboard");

        Font font;
        font.loadFromFile("img/SIXTY.ttf");

        // Text object to display the scores
        Text leaderboardText;
        leaderboardText.setFont(font);
        leaderboardText.setCharacterSize(30);
        leaderboardText.setFillColor(Color::White);
        leaderboardText.setPosition(100, 100);

        while (leaderboardWindow.isOpen()) {
            Event event;
            while (leaderboardWindow.pollEvent(event)) {
                if (event.type == Event::Closed) {
                    leaderboardWindow.close();
                }
                // Check for the 'B' key to go back to the menu
                if (event.type == Event::KeyPressed && event.key.code == Keyboard::B) {
                    leaderboardWindow.close();
                }
            }

            // Display scores using the display_scores function
            std::string scoresText = display_scores();
            leaderboardText.setString(scoresText);

            leaderboardWindow.clear();
            leaderboardWindow.draw(leaderboardText);
            leaderboardWindow.display();
        }
    }



    void display_menu()
    {

        int selectedLevel = 0;

        RenderWindow menuWindow(VideoMode(780, 780), "DODGE EM");

        // Load the custom cursor image
        Image cursorImage;
        cursorImage.loadFromFile("img/dodgeEm.png");
        Cursor cursor;
        cursor.loadFromPixels(cursorImage.getPixelsPtr(), cursorImage.getSize(), { 0, 0 });
        menuWindow.setMouseCursor(cursor);

        // Print large "SPACE SHOOTER" text centered on screen
        FloatRect titleRect = title.getLocalBounds();
        title.setPosition(370 - titleRect.width / 2, 50);

        // Play crawl text
        crawlText.setString(R"(
 Navigate     the      Chaos     brought    by    


Enemies    Embrace       the       thrill    of        


 precision  and    Reflexes   in  a   High    stake     


  world    of    dodging    and        evasion

               
)");

        crawlText.setFillColor(sf::Color(255, 0, 51));
        crawlText.setCharacterSize(24);
        crawlText.setPosition(400, 700);
        crawlText.setOrigin(crawlText.getLocalBounds().width / 2, 0);

        float crawlSpeed = 0.25f;
        float crawlPosY = 800.0;


        while (crawlText.getPosition().y > -crawlText.getLocalBounds().height) {
            // Handle events
            Event event;
            while (menuWindow.pollEvent(event)) {
                if (event.type == Event::Closed) {
                    menuWindow.close();
                }
            }

            // Update crawl text position
            crawlPosY -= crawlSpeed;
            crawlText.setPosition(400, crawlPosY);

            // Clear window and draw objects
            menuWindow.clear();
            menuWindow.draw(crawlText);
            menuWindow.draw(title);
            menuWindow.display();

        }

        Clock clock;
        RectangleShape fade;
        fade.setPosition(Vector2f(0, 0));
        fade.setFillColor(Color(0, 0, 0, 0));
        fade.setSize(Vector2f(800, 600));
        for (int i = 0; i <= 255; i++) {
            fade.setFillColor(Color(0, 0, 0, i));
            menuWindow.draw(fade);
            menuWindow.display();
            sleep(seconds(0.01));
            clock.restart();
        }

        //sleep(seconds(0.2));

        for (int i = 255; i >= 0; i--) {
            fade.setFillColor(Color(0, 0, 0, i));
            menuWindow.draw(fade);
            menuWindow.display();
            //sleep(seconds(0.1));
            clock.restart();
        }

        // Undim screen and display menu options
        int selectedOption = 0;
        Text tempOptions[4];
        for (int i = 0; i < 4; i++) {
            tempOptions[i] = options[i];
        }
        tempOptions[selectedOption].setFillColor(sf::Color(255, 0, 51));

        std::string playerName;

        while (menuWindow.isOpen()) {


            // Handle events
            Event event;
            while (menuWindow.pollEvent(event)) {

                if (event.type == Event::Closed) {
                    menuWindow.close();
                }
                /* if (event.type == Event::KeyPressed && event.key.code == Keyboard::P) {
                     isPaused = true;
                 }*/
                if (event.type == Event::KeyPressed) {
                    if (event.key.code == Keyboard::Left) {
                        if (selectedLevel > 0) {
                            selectedLevel--;
                        }
                    }
                    if (event.key.code == Keyboard::Right) {
                        if (selectedLevel < 3) {  // Adjust for the number of levels
                            selectedLevel++;
                        }
                    }
                }
                if (event.type == Event::KeyPressed) {
                    if (event.key.code == Keyboard::Up) {
                        // sound.play();
                        if (selectedOption > 0) {
                            selectedOption--;
                        }
                    }
                    if (event.key.code == Keyboard::Down) {
                        // sound.play();
                        if (selectedOption < 3) {
                            selectedOption++;
                        }
                    }
                    if (event.key.code == Keyboard::Return) {
                        switch (selectedOption) {
                        case 0:
                            menuWindow.close();
                            playerName = Name();
                            g.start_game(playerName, selectedLevel + 1);
                            break;
                        case 1:
                            menuWindow.close();
                            playerName = Name();
                            g.start_game(playerName, selectedLevel + 1);

                            break;
                        case 2:
                            displayLeaderboard();
                            break;
                        case 3:
                            menuWindow.close();
                            playerName = Name();
                            g.start_game(playerName, selectedLevel + 1);
                            // window.close();
                            break;
                        }
                    }
                }
                if (event.type == Event::MouseButtonPressed) {
                    if (event.mouseButton.button == Mouse::Left) {
                        Vector2i mousePosition = Mouse::getPosition(menuWindow);
                        // Option 0
                        if (mousePosition.x >= options[0].getPosition().x && mousePosition.x <= options[0].getPosition().x + options[0].getLocalBounds().width &&
                            mousePosition.y >= options[0].getPosition().y && mousePosition.y <= options[0].getPosition().y + options[0].getLocalBounds().height) {
                            menuWindow.close();
                            playerName = Name();
                            g.start_game(playerName, selectedLevel + 1);
                        }

                        // Option 1
                        if (mousePosition.x >= options[1].getPosition().x && mousePosition.x <= options[1].getPosition().x + options[1].getLocalBounds().width &&
                            mousePosition.y >= options[1].getPosition().y && mousePosition.y <= options[1].getPosition().y + options[1].getLocalBounds().height) {
                            menuWindow.close();
                            playerName = Name();
                            g.start_game(playerName, selectedLevel + 1);
                        }

                        // Option 2
                        if (mousePosition.x >= options[2].getPosition().x && mousePosition.x <= options[2].getPosition().x + options[2].getLocalBounds().width &&
                            mousePosition.y >= options[2].getPosition().y && mousePosition.y <= options[2].getPosition().y + options[2].getLocalBounds().height) {
                            // Handle "Leader Board" option
                        }

                        // Option 3
                        if (mousePosition.x >= options[3].getPosition().x && mousePosition.x <= options[3].getPosition().x + options[3].getLocalBounds().width &&
                            mousePosition.y >= options[3].getPosition().y && mousePosition.y <= options[3].getPosition().y + options[3].getLocalBounds().height) {
                            menuWindow.close();
                            playerName = Name();
                            g.start_game(playerName, selectedLevel + 1);
                            // window.close();

                        }
                    }
                }
            }



            // Draw menu options with glowing effect around selected option
            menuWindow.clear();
            menuWindow.draw(title);
            for (int i = 0; i < 4; i++) {
                if (i == selectedOption) {
                    options[i].setFillColor(sf::Color(255, 0, 51));
                }
                else {
                    options[i].setFillColor(Color::White);
                }
                menuWindow.draw(options[i]);
            }

            for (int i = 0; i < 4; i++) {
                if (i == selectedLevel) {
                    levelOptions[i].setFillColor(sf::Color(255, 0, 51));
                }
                else {
                    levelOptions[i].setFillColor(Color::White);
                }
            }

            // Draw menu options with glowing effect around selected option
            menuWindow.clear();
            menuWindow.draw(title);
            for (int i = 0; i < 4; i++) {
                menuWindow.draw(options[i]);
            }
            for (int i = 0; i < 4; i++) {
                menuWindow.draw(levelOptions[i]);
            }


            // Update selected option's color
            for (int i = 0; i < 4; i++) {
                if (i == selectedOption) {
                    tempOptions[i].setFillColor(sf::Color(255, 0, 51));
                }
                else {
                    tempOptions[i].setFillColor(Color::White);
                }
            }

            // Move ship sprite with selected option
         /*   float shipPosY = options[selectedOption].getPosition().y - 50;
            float shipPosX = options[selectedOption].getPosition().x;
            leftWing.setPosition(shipPosX-50, shipPosY);
            rightWing.setPosition(shipPosX+250, shipPosY);
            leftWing.setScale(Vector2f(0.75f, 0.8f));
            rightWing.setScale(Vector2f(0.75f, 0.8f));
            window.draw(leftWing);
            window.draw(rightWing);*/

            menuWindow.display();




        }

        //display menu screen here

        // add functionality of all the menu options here

        //if Start game option is chosen 

       // g.start_game();

        g.start_game(playerName, selectedLevel + 1);




    }


};

