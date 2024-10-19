#include "Menu.h"
#include <SFML/Graphics.hpp>
using namespace std;
using namespace sf;

//This function will load from file the font which will be used
Menu::Menu(float width, float height) {
	font.loadFromFile("./fonts/BACKTO1982.TTF");

	MainMenu[0].setFont(font);
	MainMenu[0].setFillColor(Color::Black);
	MainMenu[0].setString("Play");
	MainMenu[0].setCharacterSize(50);
	MainMenu[0].setPosition(Vector2f(285, 400));

	MainMenu[1].setFont(font);
	//MainMenu[1].setFillColor(Color::Black);
	MainMenu[1].setString("Tips");
	MainMenu[1].setCharacterSize(50);
	MainMenu[1].setPosition(Vector2f(285, 530));

	MainMenu[2].setFont(font);
	//MainMenu[2].setFillColor(Color::Black);
	MainMenu[2].setString("Options");
	MainMenu[2].setCharacterSize(50);
	MainMenu[2].setPosition(Vector2f(220, 660));


	MainMenu[3].setFont(font);
	//MainMenu[2].setFillColor(Color::Black);
	MainMenu[3].setString("Credits");
	MainMenu[3].setCharacterSize(50);
	MainMenu[3].setPosition(Vector2f(220, 810));

	MainMenu[4].setFont(font);
	//MainMenu[3].setFillColor(Color::Black);
	MainMenu[4].setString("Exit");
	MainMenu[4].setCharacterSize(50);
	MainMenu[4].setPosition(Vector2f(285, 950));

	ButtonsText[0].setFont(font);
	ButtonsText[0].setFillColor(Color::Black);
	ButtonsText[0].setString("Restart");
	ButtonsText[0].setCharacterSize(45);
	ButtonsText[0].setPosition(Vector2f(width / 2.4, height / (1.85)));

	ButtonsText[1].setFont(font);
	//ButtonsText[0].setFillColor(Color::Black);
	ButtonsText[1].setString("Quit");
	ButtonsText[1].setCharacterSize(45);
	ButtonsText[1].setPosition(Vector2f(width / 2.25, height / (1.42)));

	ExitText.setFont(font);
	ExitText.setFillColor(Color::Black);
	ExitText.setString("Exit");
	ExitText.setCharacterSize(45);
	ExitText.setPosition(Vector2f(width / 2.4 + 60, height / (1.85)));


	selected = 0;
	selectedButton = 0;
}



Menu::~Menu() {} //New destructor

void Menu::setSelected(int n) {
	selected = n;

}

void Menu::setSelectedButton(int n) {
	selectedButton = n;
}

void Menu::draw(RenderWindow& window) {
	for (int i = 0; i < 5; i++) {
		window.draw(MainMenu[i]); //To draw the text
	}
}

void Menu::drawButtons(RenderWindow& window)
{
	for (int i = 0; i < 2; ++i)
		window.draw(ButtonsText[i]);
}

void Menu::MoveDown() {
	if (selected + 1 <= 5) {//Set exit
		MainMenu[selected].setFillColor(Color{ 187,187,187 });
		selected++;
		if (selected == 5) {
			selected = 0;
		}
		MainMenu[selected].setFillColor(Color::Black);
	}
}


void Menu::MoveUp() {
	if (selected - 1 >= 0) {
		MainMenu[selected].setFillColor(Color{ 187,187,187 });
		selected--;
		MainMenu[selected].setFillColor(Color::Black);
	}
	else {
		MainMenu[selected].setFillColor(Color{ 187,187,187 });
		selected = 4;  // Wrap around to the last item when moving up from the first item
		MainMenu[selected].setFillColor(Color::Black);
	}


}

void Menu::MoveButtonDown()
{
	if (selectedButton + 1 <= 2) {//Set exit
		ButtonsText[selectedButton].setFillColor(Color{ 187,187,187 });
		selectedButton++;
		if (selectedButton == 2) {
			selectedButton = 0;
		}
		ButtonsText[selectedButton].setFillColor(Color::Black);
	}
}




void Menu::MoveButtonUp()
{
	if (selectedButton - 1 >= 0) {
		ButtonsText[selectedButton].setFillColor(Color{ 187,187,187 });
		selectedButton--;
		ButtonsText[selectedButton].setFillColor(Color::Black);
	}
	else {
		ButtonsText[selectedButton].setFillColor(Color{ 187,187,187 });
		selectedButton = 1;  // Wrap around to the last item when moving up from the first item
		ButtonsText[selectedButton].setFillColor(Color::Black);
	}
}