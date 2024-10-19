#pragma once
#include <SFML/Graphics.hpp>
#include <iostream>
using namespace std;
using namespace sf;

class Menu
{
public: //So it can be used anywhere
    Text MainMenu[5]; //Text array formed from 3 words to store menu items
    Text ButtonsText[2];
    Text ExitText;
    Menu(float width, float height); //Menu function constructor 
    void draw(RenderWindow& window); //Passing by reference to draw the menu
    void MoveUp();
    void MoveDown();
    void drawButtons(RenderWindow& window);
    void MoveButtonDown();
    void MoveButtonUp();
    void setSelected(int n);
    void setSelectedButton(int n);
    int pressed() {
        return selected;
    }
    int ButtonPressed()
    {
        return selectedButton;
    }
    void drawOptions(RenderWindow& window); //To draw options
    void MoveRight();
    void MoveLeft();
    int getVolume() {
        return volume;
    }
    void updateVolumeText(); //To update volume text


    ~Menu(); //When you enter a class it implements it (Destructor)

private:
    int selected, selectedButton; //Index of selected item
    Font font;
    Text volumeText;
    int volume;
    const int minVolume = 0;
    const int maxVolume = 100;
};