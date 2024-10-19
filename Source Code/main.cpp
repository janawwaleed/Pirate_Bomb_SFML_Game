#include<SFML/Graphics.hpp>
#include<SFML/Audio.hpp>
#include<cstdlib>
#include<iostream>
#include<vector>
#include<cmath>
#include"menu.h"
#include<map>
using namespace std;
using namespace sf;
#define pb push_back
RenderWindow window(VideoMode(1920, 1080), "Pirate Bomber", Style::Fullscreen);

//====================================================ALL GLOBAL=======================================================
bool isSwallowed[15];
bool gameEnded;
Sprite deathUI[4];
Sprite winUI[2];
float dt, delay = 3.f, fallingDelay = 50.f, bombAnimTime = 0.f, shootTimer = 0.f, bombDelay = 0.1f, BombChargingTimer = 0.f, exclamationTimer = 0.f;

const float g = 10.f, jumpVelocity = -475.f, resistanceForce = 15.f;

const int textNum = 8, bombsNum = 15, baldTextNum = 4;

const long double pi = acos(-1), initAngle = atan(1); //For physics

int bombFired = 0;
int pirateAnimIdx = 0, baldAnimIdx = 0, bombAnimIdx = 0, whaleAnimIdx = 0, bigGuyAnimIdx = 0, captainAnimIdx = 0, captainAttackIdx = 0, cannonAniIdx = 0, cannonExpoAniIdx = 0, cucumberAnimIdx = 0; // pirate , bald , bomb , whale , big guy , captain
int pirateTimer = 0, particleTimer = 0, baldTimer = 0, bigGuyTimer = 0, throwAnim = 0, whaleTimer = 0.f, captainTimer = 0, cannonTimer = 0, cannonExpoTimer = 0, cucumberTimer = 0;
int doorAnimIdx = 0; // for door
int particleAnimIdx = 0; // for particles
int chargingBarAnimIdx = 0; //For chargingBar
int whaleSwallowIdx = 0; //index for whale swallowing
int exclamationAnimIdx = 0; // for exclamation
bool UpKeyPressed = false;
bool downKeyPressed = false;
bool player_init = false;
bool levelFinished = false;

bool player_in_range = false;
int  bigGuyAttackIdx = 0; // index for big guy attack
bool oneTime_bigGuy_anim = false;
int  baldAttackIdx = 0; // index for bald guy attack

int playerHearts = 3;
int WhaleJumpCount = 0;

int u = 0;

float TimeToDetonate = 200.f;

bool isPressed[120];
RectangleShape platform[6];

map<int, bool> vis;
map<int, Vector2f> wadi;


////--------------------------------------------------------SOUNDS--------------------------------------------------\\
//
//


//        ======================= ALL STRUCTS ARE STILL UNDER CONSTRUCTION========================

struct Pirate
{
    int  player_in_Frame = 0, player_in_mxFrame = 9;// to start the game  
    Texture textures[10];
    Sprite shape;
    Sprite particle;
    float player_init_timer = 0;
    Vector2f velocity;
    RectangleShape hitbox;
    bool isJumping = false;
    bool isAlive = true;
    bool isCollided = false;
    bool isHit = false;
    bool stop_jumpParticles = false;
    bool doorInControl = false;
    Pirate()
    {
        textures[0].loadFromFile("./textures/pirate/idle.png");
        textures[1].loadFromFile("./textures/pirate/run.png");
        textures[2].loadFromFile("./textures/pirate/jump.png");
        textures[3].loadFromFile("./textures/pirate/fall.png");
        textures[4].loadFromFile("./textures/pirate/door out.png");
        textures[5].loadFromFile("./textures/pirate/door in.png");
        textures[6].loadFromFile("./textures/particles/RUN.png"); //particle run 
        textures[7].loadFromFile("./textures/particles/JUMP.png"); //particle jump
        textures[8].loadFromFile("./textures/pirate/hit.png");
        textures[9].loadFromFile("./textures/pirate/dead hit.png");
        velocity = { 0,0 };
        shape.setScale(2.f, 2.f);
        hitbox.setSize(Vector2f(50.f, 90.f));
        hitbox.setFillColor(Color::Transparent);
        hitbox.setOutlineColor(Color::Cyan);
        hitbox.setOutlineThickness(1.f);
        hitbox.setOrigin(hitbox.getSize().x / 2, hitbox.getSize().y / 2);

    }
    ~Pirate() {}
} pirate;


struct Bald_guy
{
    Sprite shape;
    Texture text[baldTextNum];
    Vector2f velocity;
    RectangleShape hitbox, range, bombRange;
    bool turn = false, pick_up = false, shoot = false, isAlive = true;
    Bald_guy()
    {
        velocity = { 0,0 };
        text[0].loadFromFile("./textures/bald pirate/idle.png");
        text[1].loadFromFile("./textures/bald pirate/run.png");
        text[2].loadFromFile("./textures/bald pirate/attack.png");
        text[3].loadFromFile("./textures/bald pirate/dead.png");

        hitbox.setSize(Vector2f(90.f, 50.f));
        hitbox.setFillColor(Color::Transparent);
        hitbox.setOutlineColor(Color::Cyan);
        hitbox.setOutlineThickness(1.f);
        hitbox.setOrigin(hitbox.getSize().x / 2, hitbox.getSize().y / 2);

        range.setSize(Vector2f(800, 50.f));
        range.setFillColor(Color::Transparent);
        range.setOutlineColor(Color::Green);
        range.setOutlineThickness(1.f);
        range.setOrigin(range.getSize().x / 2, range.getSize().y / 2);

        bombRange.setSize(Vector2f(400, 50.f));
        bombRange.setFillColor(Color::Transparent);
        bombRange.setOutlineColor(Color::Red);
        bombRange.setOutlineThickness(1.f);
        bombRange.setOrigin(bombRange.getSize().x / 2, bombRange.getSize().y / 2);

    }
    ~Bald_guy() {}
}bald, bald_lvl3;

struct Bomb
{
    Texture bombTex;
    Texture bombOffTex;
    Sprite shape;
    vector<Bomb>bombs;
    Vector2f velocity;
    bool isGrounded;
    bool isShot = false;
    int detonateTimer;
    RectangleShape hitbox, range;
    Bomb()
    {
        bombTex.loadFromFile("./textures/bomb/Bomb_off.png");
        bombOffTex.loadFromFile("./textures/bomb/explosion.png");
        shape.setScale(2.f, 2.f);
        velocity = { 0,0 };
        isGrounded = false;
        detonateTimer = 0.f;

        hitbox.setSize(Vector2f(60.f, 70.f));
        hitbox.setFillColor(Color::Transparent);
        hitbox.setOutlineColor(Color::Green);
        hitbox.setOutlineThickness(2.f);
        hitbox.setOrigin(hitbox.getSize().x / 2, hitbox.getSize().y / 2);
        shape.setOrigin(shape.getTextureRect().width / 2, shape.getTextureRect().height / 2);

        range.setSize(Vector2f(200.f, 100.f));
        range.setFillColor(Color::Transparent);
        range.setOutlineColor(Color::Green);
        range.setOutlineThickness(2.f);
        range.setOrigin(range.getSize().x / 2, range.getSize().y / 2);
    }

}bomb;

struct Big_guy
{
    Sprite shape;
    Texture text[7];
    Vector2f velocity;
    RectangleShape hitbox, range, RunToBombRange;

    bool turn = false, stop = false, pick_up = false, bomb_throw = false, isAlive = true;
    Big_guy()
    {
        velocity = { 0,0 };
        text[0].loadFromFile("./textures/Big guy/idle.png");
        text[1].loadFromFile("./textures/Big guy/run.png");
        text[2].loadFromFile("./textures/Big guy/attack.png");
        text[3].loadFromFile("./textures/Big guy/pick_bomb.png");
        text[4].loadFromFile("./textures/Big guy/throw.png");
        text[5].loadFromFile("./textures/Big guy/hit.png");
        text[6].loadFromFile("./textures/Big guy/dead.png");
        //range_attack
        hitbox.setSize(Vector2f(110.f, 135.f));
        hitbox.setFillColor(Color::Transparent);
        hitbox.setOutlineColor(Color::Green);
        hitbox.setOutlineThickness(1.f);
        hitbox.setOrigin(hitbox.getSize().x / 2, hitbox.getSize().y / 2);
        //Range
        range.setSize(Vector2f(600, 50.f));
        range.setFillColor(Color::Transparent);
        range.setOutlineColor(Color::Green);
        range.setOutlineThickness(1.f);
        range.setOrigin(range.getSize().x / 2, range.getSize().y / 2);
        // range for bomb pick up
        RunToBombRange.setSize(Vector2f(500.f, 100.f));
        RunToBombRange.setFillColor(Color::Transparent);
        RunToBombRange.setOutlineColor(Color::Red);
        RunToBombRange.setOutlineThickness(1.f);
        RunToBombRange.setOrigin(RunToBombRange.getSize().x / 2, RunToBombRange.getSize().y / 2);
    }
    ~Big_guy() {}

}big_guy, bigGuyLvl2, bigGuyLvl3;


struct Whale
{
    Texture textures[textNum];
    Sprite shape;
    Vector2f velocity;
    RectangleShape range, hitbox;
    float SwallowTimer = 100;
    bool isJumping = false, focus = false, isAlive = true, attack = false, isSwallowing = false, bombInRange = false;


    //hunger bar
    Texture HungerBarTXT;
    Sprite hungerBar;


    Whale()
    {
        velocity = { 0,0 };
        textures[0].loadFromFile("./textures/whale/idle.png");
        textures[1].loadFromFile("./textures/whale/run.png");
        textures[2].loadFromFile("./textures/whale/jump.png");
        textures[3].loadFromFile("./textures/whale/fall.png");
        textures[4].loadFromFile("./textures/whale/attack.png");
        textures[5].loadFromFile("./textures/whale/swallow.png");
        textures[6].loadFromFile("./textures/whale/dead.png");
        //Hitbox
        hitbox.setSize(Vector2f(90.f, 50.f));
        hitbox.setFillColor(Color::Transparent);
        hitbox.setOutlineColor(Color::Cyan);
        hitbox.setOutlineThickness(1.f);
        hitbox.setOrigin(hitbox.getSize().x / 2, hitbox.getSize().y / 2);
        //Range
        range.setSize(Vector2f(350, 50.f));
        range.setFillColor(Color::Transparent);
        range.setOutlineColor(Color::Green);
        range.setOutlineThickness(1.f);
        range.setOrigin(range.getSize().x / 2, range.getSize().y / 2);

        //HungerBar

        HungerBarTXT.loadFromFile("./textures/pirate/charge_bar1.png");
        hungerBar.setTexture(HungerBarTXT);
        hungerBar.setTextureRect(IntRect(0, 0, 39, 9));
        hungerBar.setScale(2.f, 2.f);
        hungerBar.setPosition(whale.shape.getPosition().x, whale.shape.getPosition().y - 60);
        hungerBar.setOrigin(hungerBar.getTextureRect().width / 2, hungerBar.getTextureRect().height / 2);

    }
}whale, whaleLvl3;


struct Door
{
    Texture door_tex[3];
    Sprite shape;
    int frame = 0;
    const int mxframe = 3;
    Door() {
        door_tex[0].loadFromFile("./textures/door/idle.png");
        door_tex[1].loadFromFile("./textures/door/opening_door.png");
        door_tex[2].loadFromFile("./textures/door/closing_door.png");
    }
    ~Door() {}
};

struct Exclamation
{
    Texture exclamation_txt;
    Sprite shape;
    bool playerInRange = false;
    int frame = 0;
    const int mxframe = 4;
    Exclamation()
    {
        exclamation_txt.loadFromFile("./textures/Exclamation.png");
    }
    ~Exclamation() {}
};
Exclamation exclamation[3] = { Exclamation(), Exclamation(), Exclamation() };


struct Captain
{
    Sprite shape;
    Texture text[4];
    Vector2f velocity;
    RectangleShape hitbox, range, RunFromBomb;
    bool escape = false;
    bool turn = false, stop = false, pick_up = false, bomb_throw = false, isAlive = true;
    Captain()
    {
        velocity = { 0,0 };
        text[0].loadFromFile("./textures/captain/run.png");
        text[1].loadFromFile("./textures/captain/attack.png");
        text[2].loadFromFile("./textures/captain/scare run.png");
        text[3].loadFromFile("./textures/captain/dead.png");
        //Hitbox
        hitbox.setSize(Vector2f(100.f, 100.f));
        hitbox.setFillColor(Color::Transparent);
        hitbox.setOutlineColor(Color::Green);
        hitbox.setOutlineThickness(1.f);
        hitbox.setOrigin(hitbox.getSize().x / 2, hitbox.getSize().y / 2);

        //Range
        range.setSize(Vector2f(600, 50.f));
        range.setFillColor(Color::Transparent);
        range.setOutlineColor(Color::Green);
        range.setOutlineThickness(1.f);
        range.setOrigin(range.getSize().x / 2, range.getSize().y / 2);
        // range for bomb pick up
        RunFromBomb.setSize(Vector2f(500.f, 100.f));
        RunFromBomb.setFillColor(Color::Transparent);
        RunFromBomb.setOutlineColor(Color::Red);
        RunFromBomb.setOutlineThickness(1.f);
        RunFromBomb.setOrigin(RunFromBomb.getSize().x / 2, RunFromBomb.getSize().y / 2);
    }
    ~Captain() {}
} captain;

struct Cannon
{
    Texture textures[3];
    Sprite shape;
    int bombCounter = 0;

    Cannon()
    {
        textures[0].loadFromFile("./textures/cannon/idle.png");
        textures[1].loadFromFile("./textures/cannon/attack.png");
        textures[2].loadFromFile("./textures/cannon/explosion.png");

    }
    ~Cannon() {}
}cannon;

struct CannonBall
{
    Texture cannonBallTex;
    Sprite shape;
    // vector<CannonBall>cannonBalls;
    Vector2f velocity;
    bool isGrounded = false;
    bool isShot = false;
    int detonateTimer;
    RectangleShape hitbox, range;
    CannonBall()
    {

        cannonBallTex.loadFromFile("./textures/cannon/ball.png");
        shape.setScale(2.f, 2.f);
        velocity = { 0,0 };
        isGrounded = false;
        detonateTimer = 0.f;

        hitbox.setSize(Vector2f(50.f, 55.f));
        hitbox.setFillColor(Color::Transparent);
        hitbox.setOutlineColor(Color::Green);
        hitbox.setOutlineThickness(2.f);
        hitbox.setOrigin(hitbox.getSize().x / 2, hitbox.getSize().y / 2);
        shape.setOrigin(shape.getTextureRect().width / 2, shape.getTextureRect().height / 2);

        range.setSize(Vector2f(200.f, 100.f));
        range.setFillColor(Color::Transparent);
        range.setOutlineColor(Color::Green);
        range.setOutlineThickness(2.f);
        range.setOrigin(range.getSize().x / 2, range.getSize().y / 2);


    }
    ~CannonBall() {}
}cannonBall[3];


struct Cucumber {
    Texture text[5];
    Sprite shape;
    int attackIdx = 0;
    bool isBlowing = false, playerInRange = false, stop = false, isAlive = true, turn = false;
    RectangleShape hitbox, range, RunToBombRange;
    Vector2f velocity;
    Cucumber()
    {
        text[0].loadFromFile("./textures/cucumber/idle.png");
        text[1].loadFromFile("./textures/cucumber/run.png");
        text[2].loadFromFile("./textures/cucumber/attack.png");
        text[3].loadFromFile("./textures/cucumber/blow.png");
        text[4].loadFromFile("./textures/cucumber/dead.png");
        //range_attack
        hitbox.setSize(Vector2f(60.f, 135.f));
        hitbox.setFillColor(Color::Transparent);
        hitbox.setOutlineColor(Color::Green);
        hitbox.setOutlineThickness(1.f);
        hitbox.setOrigin(hitbox.getSize().x / 2, hitbox.getSize().y / 2);
        //Range
        range.setSize(Vector2f(600, 50.f));
        range.setFillColor(Color::Transparent);
        range.setOutlineColor(Color::Green);
        range.setOutlineThickness(1.f);
        range.setOrigin(range.getSize().x / 2, range.getSize().y / 2);
        // range for bomb pick up
        RunToBombRange.setSize(Vector2f(500.f, 100.f));
        RunToBombRange.setFillColor(Color::Transparent);
        RunToBombRange.setOutlineColor(Color::Red);
        RunToBombRange.setOutlineThickness(1.f);
        RunToBombRange.setOrigin(RunToBombRange.getSize().x / 2, RunToBombRange.getSize().y / 2);

        velocity = { 0,0 };
    }
    ~Cucumber() {}
}cucumber;

//-------------------------------------------All sounds in a struct-----------------------------------------------------------\\

struct Sounds
{
    Sound menu_select, walking, jumpSFX, bombsound, hitSFX, press, doorsfx, gulpsound, kickSFX, angry, livesdec, gameover, backsfx, lvl1Sound, lvl2Sound, groundhit, cannonGone, cannonShoot , blow , lvl3Sound;

    SoundBuffer pressbuffer;
    SoundBuffer menubuffer;
    SoundBuffer jumpBuffer;
    SoundBuffer gulpBuffer;
    SoundBuffer hitBuffer;
    SoundBuffer kickBuffer;
    SoundBuffer angryBuffer;
    SoundBuffer decBuffer;
    SoundBuffer BombBuffer;
    SoundBuffer gameoverBuffer;
    SoundBuffer WalkBuffer;
    SoundBuffer doorBuffer;
    SoundBuffer backBuffer;
    SoundBuffer lvl1SoundBuffer;
    SoundBuffer lvl2SoundBuffer;
    SoundBuffer lvl3SoundBuffer;
    SoundBuffer groundhitBuffer;
    SoundBuffer cannonGoneBuffer;
    SoundBuffer cannonShootBuffer;
    SoundBuffer blowBuffer;
    Sounds()
    {
        groundhitBuffer.loadFromFile("./sound/groundhit.mp3");
        groundhit.setBuffer(groundhitBuffer);

        pressbuffer.loadFromFile("./sound/press4.mp3");
        press.setBuffer(pressbuffer);

        menubuffer.loadFromFile("./sound/Menu Selection Click.wav");
        menu_select.setBuffer(menubuffer);

        jumpBuffer.loadFromFile("./sound/jumpnew.mp3");
        jumpSFX.setBuffer(jumpBuffer);

        gulpBuffer.loadFromFile("./sound/gulp2.mp3");
        gulpsound.setBuffer(gulpBuffer);

        cannonGoneBuffer.loadFromFile("./sound/cannon gone.mp3");
        cannonGone.setBuffer(cannonGoneBuffer);

        cannonShootBuffer.loadFromFile("./sound/cannon shoot.mp3");
        cannonShoot.setBuffer(cannonShootBuffer);

        blowBuffer.loadFromFile("./sound/blow.mp3");
        blow.setBuffer(blowBuffer);

        hitBuffer.loadFromFile("./sound/punch2.mp3");
        hitSFX.setBuffer(hitBuffer);
        hitSFX.setVolume(100);

        kickBuffer.loadFromFile("./sound/kick.mp3");
        kickSFX.setBuffer(kickBuffer);
        kickSFX.setVolume(100);

        angryBuffer.loadFromFile("./sound/angry2.mp3");
        angry.setBuffer(angryBuffer);
        angry.setVolume(100);

        decBuffer.loadFromFile("./sound/lives decrement.mp3");
        livesdec.setBuffer(decBuffer);
        livesdec.setVolume(100);

        gameoverBuffer.loadFromFile("./sound/gameover.mp3");
        gameover.setBuffer(gameoverBuffer);
        gameover.setVolume(100);

        WalkBuffer.loadFromFile("./sound/walk2.mp3");
        walking.setBuffer(WalkBuffer);
        walking.setVolume(100);

        BombBuffer.loadFromFile("./sound/Explode sound effect.mp3");
        bombsound.setBuffer(BombBuffer);
        bombsound.setVolume(100);

        doorBuffer.loadFromFile("./sound/door2.mp3");
        doorsfx.setBuffer(doorBuffer);
        doorsfx.setLoop(false);

        backBuffer.loadFromFile("./sound/He's a Pirate.mp3");
        backsfx.setBuffer(backBuffer);
        backsfx.setLoop(true);
        backsfx.setVolume(5);

        lvl1SoundBuffer.loadFromFile("./sound/Sea Of Thieves OST - Arriving The Sea Of Thieves.mp3");
        lvl1Sound.setBuffer(lvl1SoundBuffer);
        lvl1Sound.setLoop(true);
        lvl1Sound.setVolume(5);

        lvl2SoundBuffer.loadFromFile("./sound/Assassin's Creed IV Black Flag - Assassin's Creed IV Black Flag Main Theme (Track 01).mp3");
        lvl2Sound.setBuffer(lvl2SoundBuffer);
        lvl2Sound.setLoop(true);
        lvl2Sound.setVolume(5);

        lvl3SoundBuffer.loadFromFile("./sound/Miguel Johnson - Pirates out for Blood (Epic Orchestral Cinematic Ethnic) [No Copyright Music].mp3");
        lvl3Sound.setBuffer(lvl3SoundBuffer);
        lvl3Sound.setLoop(true);
        lvl3Sound.setVolume(5);

    }
    ~Sounds() {}

}sound;

//==============================================

//Functions

void animate(Sprite& s, Texture& text, int& idx, int width, int height, int& timer, int mxframe, int dir, float scale, int mt = 1);
void dead_animation(Sprite& s, Texture& text, int& idx, int width, int height, int& timer, int mxframe, int dir, float scale, int limiter, int mt = 1);
bool CollisionDetection(RectangleShape& shape1, RectangleShape& shape2);
void HitBoxUpdate(Sprite& s, RectangleShape& hitbox, int add = 0);
void Pirate_move_code(float = 2.f);
void big_guy_code(Big_guy& bigGuyLvl2, int dir1, int dir2);
void bald_code(Bald_guy& bald, int dir1, int dir2, float scale);
void Whale_code(Whale& whale, int dir, RectangleShape& platform);
void captainCode(int dir1, int dir2);
void cannonCode(Cannon& cannon, RectangleShape object, Sprite explosion, Texture expoTex);
void cucumber_code(Cucumber& cucumber, int dir1, int dir2);
void exclamation_ani(int i);
void initialize();
void RepeatedKeyHandle(Event& event, bool isPressed[]);
float posX(int pos);
float posY(int pos);
void mainMenuInit(RenderWindow& window);
void Level_1(RenderWindow& window);
void Level_2(RenderWindow& window);
void Level_3(RenderWindow& window);
void TipsMenu(RenderWindow& window);
void lobby_1(RenderWindow& window);
void lobby_2(RenderWindow& window);
void lobby_3(RenderWindow& window);
void creditsMenu(RenderWindow& window);
void optionsMenu(RenderWindow& window);
//Sound

bool isJumping = false;


int phase = 0;

Menu mainMenu(1920, 1080);
Texture mainMenutext;
Sprite bg;
Sprite tips[6];

int main()
{
    window.setMouseCursorVisible(false);
    Texture tipsTxt[6];
    tipsTxt[0].loadFromFile("./tips/tip 1.png");
    tipsTxt[1].loadFromFile("./tips/tip 2.png");
    tipsTxt[2].loadFromFile("./tips/tip 3.png");
    tipsTxt[3].loadFromFile("./tips/tip 4.png");
    tipsTxt[4].loadFromFile("./tips/tip 5.png");
    tipsTxt[5].loadFromFile("./tips/tip 6.png");

    for (int i = 0; i < 6; i++)
    {
        tips[i].setTexture(tipsTxt[i]);
        tips[i].setScale(1.f, 1.f);
    }



    mainMenutext.loadFromFile("./UI/mainmenu_new.jpg");
    bg.setTexture(mainMenutext);
    while (phase == 0)
        mainMenuInit(window);
    while (phase == 1)
        lobby_1(window);
    while (phase == 2)
        Level_1(window);
    while (phase == 3)
        lobby_2(window);
    while (phase == 4)
        Level_2(window);
    while (phase == 5)
        lobby_3(window);
    while (phase == 6)
        Level_3(window);



    return 0;
}

void mainMenuInit(RenderWindow& window)
{
    bool showTips = false;
    sound.backsfx.play();
    while (true)
    {

        Event event;

        while (window.pollEvent(event))
        {

            if (event.type == Event::Closed || event.type == Event::KeyPressed && event.key.code == Keyboard::Escape)
            {
                phase = -1;
                window.close();
                return;
            }
            //cout << "x: " << Mouse::getPosition().x << "y: " << Mouse::getPosition().y << endl;
            if (event.type == Event::KeyReleased)
            {
                if (event.key.code == Keyboard::Up)
                {
                    sound.menu_select.play();
                    mainMenu.MoveUp();

                }
                if (event.key.code == Keyboard::Down)
                {
                    sound.menu_select.play();
                    mainMenu.MoveDown();

                }
                if (event.key.code == Keyboard::Return)
                {
                    if (mainMenu.pressed() == 0)
                    {
                        sound.press.play();
                        phase++;

                        return;
                    }
                    if (mainMenu.pressed() == 1)
                    {
                        TipsMenu(window);
                    }

                    if (mainMenu.pressed() == 2)
                    {
                        optionsMenu(window);
                    }


                    if (mainMenu.pressed() == 3)
                    {

                        creditsMenu(window);

                    }


                    if (mainMenu.pressed() == 4)
                    {
                        sound.press.play();
                        phase = -1;
                        return;
                    }


                }
            }
            else if (event.type == Event::MouseButtonPressed)
            {
                if (mainMenu.MainMenu[0].getGlobalBounds().contains(event.mouseButton.x, event.mouseButton.y))
                {
                    sound.press.play();
                    phase++;
                    return;
                }
                if (mainMenu.MainMenu[3].getGlobalBounds().contains(event.mouseButton.x, event.mouseButton.y))
                {
                    sound.press.play();
                    phase = -1;
                    return;
                }
            }
            window.clear(Color::Black);
            window.draw(bg);
            mainMenu.draw(window);
            window.display();

        }
    }
}

void TipsMenu(RenderWindow& window)
{
    int tipsIndex = 0;

    while (true)
    {
        Event event;

        while (window.pollEvent(event))
        {

            if (event.type == Event::Closed || event.type == Event::KeyPressed && event.key.code == Keyboard::Escape)
            {
                return;
            }


            if (event.type == Event::KeyReleased)
            {

                if (event.key.code == Keyboard::Right and tipsIndex < 5)
                {
                    tipsIndex++;
                }
                if (event.key.code == Keyboard::Left and tipsIndex > 0)
                {
                    tipsIndex--;
                }

            }
        }

        if (tipsIndex < 5) window.clear();
        window.draw(tips[tipsIndex]);
        window.display();
    }

}

void lobby_1(RenderWindow& window)
{
    window.setFramerateLimit(60); //set it according to your monitor's refresh rate
    window.setVerticalSyncEnabled(true); //to avoid screen tearing

    initialize();

    //game clock
    Clock clock;
    Texture lobbyTxt;
    lobbyTxt.loadFromFile("./textures/lobby.png");
    Sprite lobby;
    lobby.setTexture(lobbyTxt);
    lobby.setScale(1, 1);

    pirate.shape.setTextureRect(IntRect(0, 0, 58, 58));
    pirate.shape.setPosition(Vector2f(200.f, -50.f));
    pirate.shape.setOrigin(pirate.shape.getTextureRect().width / 2, pirate.shape.getTextureRect().height / 2);
    pirate.shape.setScale(2.f, 2.f);
    pirate.shape.setTexture(pirate.textures[0]);
    //Door init
    Door door;
    door.shape.setTexture(door.door_tex[1]); //idle
    door.shape.setTextureRect(IntRect(door.frame * 78, 0, 78, 98));
    door.shape.setScale(2.f, 2.f);
    door.shape.setPosition(880.f, 245.f);
    bool door_opened = false;
    double doorDelay = 1.f;
    Door door2;
    door2.shape.setTexture(door.door_tex[1]); //idle
    door2.shape.setTextureRect(IntRect(0, 0, 78, 98));
    door2.shape.setScale(2.f, 2.f);
    door2.shape.setPosition(1425.f, 767.5f);

    Door door3;
    door3.shape.setTexture(door.door_tex[1]); //idle
    door3.shape.setTextureRect(IntRect(0, 0, 78, 98));
    door3.shape.setScale(2.f, 2.f);
    door3.shape.setPosition(890.f, 767.5f);


    //Door Bar
    RectangleShape doorBar(Vector2f(10, 150));
    doorBar.setPosition(door.shape.getPosition().x + 110, door.shape.getPosition().y + 50);
    doorBar.setFillColor(Color::Blue);

    float doorTimer = 0, doorTimer2 = 0;

    //Objects
    RectangleShape objects[5];
    objects[0].setSize(Vector2f(1050, 215));
    objects[0].setPosition(95, 440);
    objects[0].setFillColor(Color::Red);

    objects[1].setSize(Vector2f(1240, 105));
    objects[1].setPosition(590, 965);
    objects[1].setFillColor(Color::Red);



    while (window.isOpen())
    {
        Event event;
        dt = clock.restart().asSeconds();
        while (window.pollEvent(event))
        {
            if (event.type == Event::Closed || event.type == Event::KeyPressed && event.key.code == Keyboard::Escape)
            {
                sound.press.play();
                phase = -1;
                window.close();
            }
        }
        if (pirate.shape.getScale().x > 0)pirate.hitbox.setPosition(pirate.shape.getPosition().x + 10, pirate.shape.getPosition().y + 5);
        else pirate.hitbox.setPosition(pirate.shape.getPosition().x - 10, pirate.shape.getPosition().y + 5);

        //apply gravity
        pirate.velocity.y += g * dt;




        if (door.frame < door.mxframe) {
            if (door.frame == 0) sound.doorsfx.play();
            doorTimer += dt;
            // cout << doorTimer << endl;
            if (doorTimer > 0.9f) {

                doorTimer = 0;
                door.frame++;
                door.shape.setTextureRect(IntRect(door.frame * 78, 0, 78, 98));
            }
        }
        else {
            door_opened = true;
            player_init = true;
        }

        //collision with objects for player (width is height :) )
        for (int i = 0; i < 2; i++)
        {
            //ground
            if (pirate.hitbox.getPosition().y + pirate.hitbox.getGlobalBounds().width / 2.f < objects[i].getPosition().y)//check if player is on top of the object
            {
                if (CollisionDetection(pirate.hitbox, objects[i]))
                {

                    pirate.velocity.y = 0;
                    pirate.shape.setPosition(pirate.shape.getPosition().x, objects[i].getPosition().y - pirate.hitbox.getGlobalBounds().height * 0.5f - 5);


                    if (pirate.isJumping)
                    {
                        sound.groundhit.play();
                        pirate.isJumping = false;
                        pirate.shape.setTexture(pirate.textures[0]);
                    }
                    // cout << "foo2\n";
                }
            }
            //top
            else if (pirate.hitbox.getPosition().y - pirate.hitbox.getGlobalBounds().width / 2.f > objects[i].getPosition().y + objects[i].getGlobalBounds().height)
            {
                if (CollisionDetection(pirate.hitbox, objects[i]) and pirate.velocity.y <= 0)
                {
                    pirate.velocity.y *= -80.f * dt;
                }
            }
        }


        Pirate_move_code();
        pirate.shape.move(pirate.velocity);

        if (doorBar.getGlobalBounds().intersects(pirate.hitbox.getGlobalBounds()))
        {
            pirate.doorInControl = true;
            pirate.velocity = { 0,0 };

            if (pirate.shape.getTexture() != &pirate.textures[5])
            {
                pirate.shape.setTexture(pirate.textures[5]);
                pirateAnimIdx = 0;
                pirate.shape.setTextureRect(IntRect(pirateAnimIdx * 58, 0, 58, 58));
            }
            if (pirateAnimIdx < 13)
            {
                if (pirateTimer <= 0)
                {
                    pirateAnimIdx++;
                    pirateAnimIdx %= 14;
                    pirate.shape.setTextureRect(IntRect(pirateAnimIdx * 58, 0, 58, 58));
                    pirateTimer = delay * 1.5;
                }
                else pirateTimer -= dt;
            }
            else
            {
                phase++;
                return;
            }

        }



        RepeatedKeyHandle(event, isPressed);

        window.clear();
        window.draw(lobby);
        window.draw(door.shape);
        window.draw(door2.shape);
        window.draw(door3.shape);
        //window.draw(doorBar);
        window.draw(pirate.shape);

        /* for (int i = 0; i < 2; i++)
         {
         window.draw(objects[i]);
         }*/

        window.display();

    }


}

void Level_1(RenderWindow& window)
{
    sound.backsfx.stop();
    sound.lvl1Sound.play();
    window.setFramerateLimit(60); //set it according to your monitor's refresh rate
    window.setVerticalSyncEnabled(true); //to avoid screen tearing
    initialize();
    //game clock
    Clock clock;


    //background
    Sprite back;
    Texture backText;
    backText.loadFromFile("./textures/maps/maplvl1.png");
    back.setTexture(backText);
    back.setPosition(0, 0);

    //============================================================================================================

    RectangleShape objects[10];

    {
        objects[0].setSize(Vector2f(320, 384)); // minus hitbox of char in x onlyy (50)
        objects[0].setFillColor(Color::Transparent);
        objects[0].setPosition(0, 384);

        objects[1].setSize(Vector2f(1920, 48));
        objects[1].setFillColor(Color::Transparent);
        objects[1].setPosition(0, 0);

        objects[3].setSize(Vector2f(1, 1));  //176 to 140
        objects[3].setFillColor(Color::Transparent);
        objects[3].setPosition(800, 0);

        objects[4].setSize(Vector2f(1000, 432)); // minus 50 for the hitbox 
        objects[4].setFillColor(Color::Transparent);
        objects[4].setPosition(960, 665);    // moving 50 in +ve x axis in position 

        objects[5].setSize(Vector2f(960, 80));
        objects[5].setFillColor(Color::Transparent);
        objects[5].setPosition(0, 1008);
    }


    {

        platform[0].setSize(Vector2f(250, 2));  // minus 50 for the hitbox 
        platform[0].setFillColor(Color::Transparent);
        platform[0].setPosition(325, 538);

        platform[1].setSize(Vector2f(285, 2));   // minus 5o for the hit box
        platform[1].setFillColor(Color::Transparent);
        platform[1].setPosition(660, 666);     // moving 50 in +ve x axis in position 

        platform[2].setSize(Vector2f(176, 2));
        platform[2].setFillColor(Color::Transparent);
        platform[2].setPosition(784, 840);

        platform[3].setSize(Vector2f(283, 2)); // minus 100 for the hitbox 
        platform[3].setFillColor(Color::Transparent);
        platform[3].setPosition(1320, 448);// (moving 50 in +ve x axis in position) (moving 16 in +ve y axis in position) 

        platform[4].setSize(Vector2f(420, 2));
        platform[4].setFillColor(Color::Transparent);
        platform[4].setPosition(670, 416);
    }

    Texture bombnum_tex[16];
    Sprite bombnum;
    int bombcounter = bombsNum;

    bombnum_tex[0].loadFromFile("./textures/bomb/BombCounter/0.png");
    bombnum_tex[1].loadFromFile("./textures/bomb/BombCounter/1.png");
    bombnum_tex[2].loadFromFile("./textures/bomb/BombCounter/2.png");
    bombnum_tex[3].loadFromFile("./textures/bomb/BombCounter/3.png");
    bombnum_tex[4].loadFromFile("./textures/bomb/BombCounter/4.png");
    bombnum_tex[5].loadFromFile("./textures/bomb/BombCounter/5.png");
    bombnum_tex[6].loadFromFile("./textures/bomb/BombCounter/6.png");
    bombnum_tex[7].loadFromFile("./textures/bomb/BombCounter/7.png");
    bombnum_tex[8].loadFromFile("./textures/bomb/BombCounter/8.png");
    bombnum_tex[9].loadFromFile("./textures/bomb/BombCounter/9.png");
    bombnum_tex[10].loadFromFile("./textures/bomb/BombCounter/10.png");
    bombnum_tex[11].loadFromFile("./textures/bomb/BombCounter/11.png");
    bombnum_tex[12].loadFromFile("./textures/bomb/BombCounter/12.png");
    bombnum_tex[13].loadFromFile("./textures/bomb/BombCounter/13.png");
    bombnum_tex[14].loadFromFile("./textures/bomb/BombCounter/14.png");
    bombnum_tex[15].loadFromFile("./textures/bomb/BombCounter/15.png");

    bombnum.setTexture(bombnum_tex[15]);
    bombnum.setScale(0.13, 0.13);
    bombnum.setPosition(1777, 100);

    //============================================================================================================


    //player init

    //pirate.shape.setTexture(pirate.textures[0]);

    pirate.shape.setTextureRect(IntRect(0, 0, 58, 58));
    pirate.shape.setPosition(Vector2f(150.f, 330.f));
    pirate.shape.setOrigin(pirate.shape.getTextureRect().width / 2, pirate.shape.getTextureRect().height / 2);


    //============================================================================================================


    //Bald_Pirate

    bald.shape.setTexture(bald.text[0]);
    bald.shape.setScale(2.f, 2.f); // 2X
    bald.shape.setTextureRect(IntRect(0, 0, 63, 67));
    bald.shape.setPosition(1400.f, 595.f);
    bald.shape.setOrigin(bald.shape.getTextureRect().width / 2, bald.shape.getTextureRect().height / 2);
    //============================================================================================================


    //whale
    whale.shape.setTexture(whale.textures[0]);
    whale.shape.setScale(1.5f, 1.5f);
    whale.shape.setTextureRect(IntRect(0, 0, 63, 67));
    whale.shape.setPosition(950.f, 395.f);
    whale.shape.setOrigin(whale.shape.getTextureRect().width / 2, whale.shape.getTextureRect().height / 2);

    //============================================================================================================


      //Big guy

    big_guy.shape.setTexture(big_guy.text[0]);
    big_guy.shape.setScale(2.f, 2.f); // 2xL*W
    big_guy.shape.setTextureRect(IntRect(0, 0, 77, 74));
    big_guy.shape.setPosition(100.f, 930.f);
    big_guy.shape.setOrigin(big_guy.shape.getTextureRect().width / 2, big_guy.shape.getTextureRect().height / 2);

    //============================================================================================================

    //Bomb init

    bomb.shape.setTexture(bomb.bombTex);
    bomb.shape.setTextureRect(IntRect(0, 0, 96, 108));
    bomb.shape.setPosition(pirate.shape.getPosition().x, pirate.shape.getPosition().y - 10);
    bomb.shape.setOrigin(bomb.shape.getTextureRect().width / 2.f, bomb.shape.getTextureRect().height / 2.f);
    bomb.range.setPosition(bomb.shape.getPosition());

    //============================================================================================================

    //Door init
    Door door;
    door.shape.setTexture(door.door_tex[1]); //idle
    door.shape.setTextureRect(IntRect(door.frame * 78, 0, 78, 98));
    door.shape.setScale(2.f, 2.f);
    door.shape.setPosition(83.f, 187.f);
    bool door_opened = false;
    double doorDelay = 1.f;

    Door door2;
    door2.shape.setTexture(door.door_tex[1]); //idle
    door2.shape.setTextureRect(IntRect(0, 0, 78, 98));
    door2.shape.setScale(2.f, 2.f);
    door2.shape.setPosition(83.f, 808.f);//83.f
    //Door Bar
    RectangleShape doorBar(Vector2f(10, 150));
    doorBar.setPosition(door2.shape.getPosition().x + 40, door2.shape.getPosition().y + 50);
    doorBar.setFillColor(Color::Transparent);
    float doorTimer = 0, doorTimer2 = 0;

    //============================================================================================================

    //HITBOX
    //Pirate
    pirate.hitbox.setPosition(pirate.shape.getPosition().x, pirate.shape.getPosition().y + 10);
    //Big guy
    big_guy.hitbox.setPosition(big_guy.shape.getPosition().x, big_guy.shape.getPosition().y + 10);
    big_guy.RunToBombRange.setPosition(big_guy.shape.getPosition().x, big_guy.shape.getPosition().y);
    //bald guy
    bald.hitbox.setPosition(bald.shape.getPosition().x, bald.shape.getPosition().y + 40);
    bald.range.setPosition(bald.shape.getPosition().x, bald.shape.getPosition().y + 10);
    bald.bombRange.setPosition(bald.shape.getPosition().x, bald.shape.getPosition().y + 40);
    //Whale
    whale.range.setPosition(whale.shape.getPosition().x, whale.shape.getPosition().y);
    whale.hitbox.setPosition(whale.shape.getPosition().x, whale.shape.getPosition().y);

    //============================================================================================================

    //Health
    Texture healthBar_text, heart_text;
    {
        healthBar_text.loadFromFile("./textures/Health/Health Bar.png");
        heart_text.loadFromFile("./textures/Health/Heart.png");
    }
    Sprite healthBar, heart[3];
    {

        healthBar.setTexture(healthBar_text);
        healthBar.setScale(1.8f, 1.8f);
        healthBar.setPosition(0, 0);

        heart[0].setTexture(heart_text);
        heart[1].setTexture(heart_text);
        heart[2].setTexture(heart_text);

        heart[0].setScale(1.8f, 1.8f);
        heart[1].setScale(1.8f, 1.8f);
        heart[2].setScale(1.8f, 1.8f);

        heart[0].setPosition(70, 40);
        heart[1].setPosition(120, 40);
        heart[2].setPosition(165, 40);
    }

    //=======================================================================================================


    exclamation[0].shape.setTexture(exclamation[0].exclamation_txt);
    exclamation[0].shape.setTextureRect(IntRect(0, 0, 20, 22));
    exclamation[0].shape.setScale(2.f, 2.f);
    exclamation[0].shape.setPosition(big_guy.shape.getPosition().x, big_guy.shape.getPosition().y - 50);

    exclamation[1].shape.setTexture(exclamation[1].exclamation_txt);
    exclamation[1].shape.setTextureRect(IntRect(0, 0, 20, 22));
    exclamation[1].shape.setScale(2.f, 2.f);
    exclamation[1].shape.setPosition(bald.shape.getPosition().x, bald.shape.getPosition().y - 50);

    exclamation[2].shape.setTexture(exclamation[2].exclamation_txt);
    exclamation[2].shape.setTextureRect(IntRect(0, 0, 20, 22));
    exclamation[2].shape.setScale(2.f, 2.f);
    exclamation[2].shape.setPosition(whale.shape.getPosition().x, whale.shape.getPosition().y - 50);

    Texture GameOver_Txt[4];
    GameOver_Txt[0].loadFromFile("./UI/gameover.png");
    GameOver_Txt[1].loadFromFile("./UI/background.png");
    GameOver_Txt[2].loadFromFile("./UI/buttons.png");
    GameOver_Txt[3].loadFromFile("./UI/noBombsLeft.png");

    // 0 ->>>> GameOver
    deathUI[0].setTexture(GameOver_Txt[0]);
    deathUI[0].setPosition(window.getSize().x / 2 - 50, window.getSize().y / 2);
    deathUI[0].setOrigin(deathUI[0].getTextureRect().width / 2, deathUI[0].getTextureRect().height / 2);
    deathUI[0].setScale(0.75f, 0.75f);
    // 1 ->>>> bg
    deathUI[1].setTexture(GameOver_Txt[1]);
    deathUI[1].setPosition(window.getSize().x / 2 - 50, window.getSize().y / 2);
    deathUI[1].setOrigin(deathUI[1].getTextureRect().width / 2, deathUI[1].getTextureRect().height / 2);
    deathUI[1].setScale(0.75f, 0.75f);

    // 2 ->>>>>buttons
    deathUI[2].setTexture(GameOver_Txt[2]);
    deathUI[2].setPosition(window.getSize().x / 2 - 50, window.getSize().y / 1.95);
    deathUI[2].setOrigin(deathUI[2].getTextureRect().width / 2, deathUI[2].getTextureRect().height / 2);
    deathUI[2].setScale(1.f, 1.f);

    // 3 -->>>>>>> No bombs left
    deathUI[3].setTexture(GameOver_Txt[3]);
    deathUI[3].setPosition(window.getSize().x / 2 - 50, window.getSize().y / 2);
    deathUI[3].setOrigin(deathUI[1].getTextureRect().width / 2, deathUI[1].getTextureRect().height / 2);
    deathUI[3].setScale(0.75f, 0.75f);


    while (window.isOpen())
    {
        //cout << pirate.hitbox.getPosition().y << endl; //967
        Event event;
        dt = clock.restart().asSeconds();


        if (!gameEnded)
        {
            //cout << "Game started\n";
            //Event event;
            while (window.pollEvent(event))
            {
                if (event.type == Event::Closed || event.type == Event::KeyPressed && event.key.code == Keyboard::Escape)
                {
                    sound.press.play();
                    phase = -1;
                    window.close();
                }
            }
            //Pirate Hitbox
            /*HitBoxUpdate(pirate.shape, pirate.hitbox, 10);*/
            if (pirate.shape.getScale().x > 0)
                pirate.hitbox.setPosition(pirate.shape.getPosition().x + 10, pirate.shape.getPosition().y + 10);
            else pirate.hitbox.setPosition(pirate.shape.getPosition().x - 10, pirate.shape.getPosition().y + 10);

            //Big guy Hitbox
            HitBoxUpdate(big_guy.shape, big_guy.hitbox, 10);
            HitBoxUpdate(big_guy.shape, big_guy.RunToBombRange, 45);
            exclamation[0].shape.setPosition(big_guy.shape.getPosition().x + 20, big_guy.shape.getPosition().y - 80);
            //Bald Hitbox
            HitBoxUpdate(bald.shape, bald.hitbox, 40);
            exclamation[1].shape.setPosition(bald.shape.getPosition().x + 20, bald.shape.getPosition().y - 80);

            //Range big guy update
            HitBoxUpdate(big_guy.shape, big_guy.range, -20);

            //Range bald pirate update
            bald.bombRange.setPosition(bald.shape.getPosition().x, bald.shape.getPosition().y + 40);
            HitBoxUpdate(bald.shape, bald.range, -20);
            HitBoxUpdate(bald.shape, bald.bombRange, 40);

            // bomb
            HitBoxUpdate(bomb.shape, bomb.range, 0);
            HitBoxUpdate(bomb.shape, bomb.hitbox, 30);

            //Whale
            HitBoxUpdate(whale.shape, whale.range, -10);
            HitBoxUpdate(whale.shape, whale.hitbox, -10);
            exclamation[2].shape.setPosition(whale.shape.getPosition().x + 10, whale.shape.getPosition().y - 60);
            whale.hungerBar.setPosition(whale.shape.getPosition().x - 5, whale.shape.getPosition().y - 50);
            whale.hungerBar.setTextureRect(IntRect(u * 39, 0, 39, 9));



            if (!door_opened) {
                if (door.frame == 0) sound.doorsfx.play();
                pirate.shape.setScale(0, 0);
                if (door.frame < door.mxframe) {
                    doorTimer += dt;
                    //cout << doorTimer << endl;
                    if (doorTimer > 0.8f) {

                        doorTimer = 0;
                        door.frame++;
                        door.shape.setTextureRect(IntRect(door.frame * 78, 0, 78, 98));
                    }
                }
                else {
                    door_opened = true;
                    pirate.shape.setScale(2.f, 2.f);
                }
            }

            if (door_opened) {
                if (pirate.player_in_Frame <= pirate.player_in_mxFrame)
                {
                    pirate.shape.setTexture(pirate.textures[4]);

                    pirate.shape.setTextureRect(IntRect(pirate.player_in_Frame * 58, 0, 58, 58));

                    if (pirate.player_init_timer <= 0)
                    {
                        pirate.player_in_Frame++;
                        pirate.player_in_Frame %= 11; // 0 -> 10
                        pirate.shape.setTextureRect(IntRect(pirate.player_in_Frame * 58, 0, 58, 58));
                        pirate.player_init_timer = 0.06;
                    }
                    else pirate.player_init_timer -= dt;
                }

                else if (pirate.player_in_Frame >= pirate.player_in_mxFrame)
                    player_init = true;
            }

            if (player_init) {
                //pirate.shape.setScale(2.f, 2.f);
                if (door.frame > 0) {
                    doorTimer += dt;
                    if (doorTimer > 0.6f) {
                        doorTimer = 0;
                        door.frame--;
                        door.shape.setTextureRect(IntRect(door.frame * 78, 0, 78, 98));
                    }
                }
            }



            //apply gravity
            pirate.velocity.y += g * dt;

            //collision with objects for player (width is height :) )
            for (int i = 0; i < 6; i++)
            {
                if (i == 2)continue;
                //ground
                if (pirate.hitbox.getPosition().y + pirate.hitbox.getGlobalBounds().width / 2.f < objects[i].getPosition().y)//check if player is on top of the object
                {
                    if (CollisionDetection(pirate.hitbox, objects[i]))
                    {
                        pirate.velocity.y = 0;
                        pirate.shape.setPosition(pirate.shape.getPosition().x, objects[i].getPosition().y - pirate.hitbox.getGlobalBounds().height * 0.5f - 5);


                        if (pirate.isJumping)
                        {
                            sound.groundhit.play();
                            pirate.isJumping = false;
                            pirate.shape.setTexture(pirate.textures[0]);
                        }
                        // cout << "foo2\n";
                    }
                }
                //top
                else if (pirate.hitbox.getPosition().y - pirate.hitbox.getGlobalBounds().width / 2.f > objects[i].getPosition().y + objects[i].getGlobalBounds().height)
                {
                    if (CollisionDetection(pirate.hitbox, objects[i]) and pirate.velocity.y <= 0)
                    {
                        pirate.velocity.y *= -80.f * dt;
                    }
                }
            }


            for (int i = 0; i < 6; ++i)
            {
                if (i == 2)continue;
                //right
                if (pirate.hitbox.getPosition().y + pirate.hitbox.getGlobalBounds().height / 2.f > objects[i].getPosition().y
                    and pirate.hitbox.getPosition().x - pirate.hitbox.getGlobalBounds().width / 2.f < objects[i].getPosition().x + objects[i].getGlobalBounds().width
                    and !(pirate.hitbox.getPosition().y - pirate.hitbox.getGlobalBounds().width / 2.f > objects[i].getPosition().y + objects[i].getGlobalBounds().height)
                    and !(pirate.hitbox.getPosition().y + pirate.hitbox.getGlobalBounds().width / 2.f < objects[i].getPosition().y)
                    and pirate.shape.getScale().x < 0
                    )
                {
                    if (CollisionDetection(pirate.hitbox, objects[i]))
                    {
                        //cout << "collide\n";
                        //right
                        pirate.velocity.x = 0.f;
                        pirate.shape.setPosition(objects[i].getPosition().x + objects[i].getGlobalBounds().width + 35, pirate.shape.getPosition().y);
                    }
                }
                //left
                else if (pirate.hitbox.getPosition().y + pirate.hitbox.getGlobalBounds().height / 2.f > objects[i].getPosition().y
                    and pirate.hitbox.getPosition().x + pirate.hitbox.getGlobalBounds().width / 2.f > objects[i].getPosition().x
                    and !(pirate.hitbox.getPosition().y - pirate.hitbox.getGlobalBounds().width / 2.f > objects[i].getPosition().y + objects[i].getGlobalBounds().height)
                    and !(pirate.hitbox.getPosition().y + pirate.hitbox.getGlobalBounds().width / 2.f < objects[i].getPosition().y)
                    and pirate.shape.getScale().x > 0
                    )
                {
                    if (CollisionDetection(pirate.hitbox, objects[i]))
                    {
                        //cout << "collide\n";
                        //right
                        pirate.velocity.x = 0.f;
                        pirate.shape.setPosition(objects[i].getPosition().x - 35, pirate.shape.getPosition().y);
                    }
                }
            }


            //collision with platforms for player
            for (int i = 0; i < 5; i++)
            {
                if (pirate.shape.getPosition().y + pirate.shape.getGlobalBounds().height / 4.f + 20 < platform[i].getPosition().y) // check on top ola l2
                {
                    if (pirate.hitbox.getGlobalBounds().intersects(platform[i].getGlobalBounds())) {
                        if (pirate.isJumping)
                        {
                            sound.groundhit.play();
                            pirate.isJumping = false;
                            pirate.shape.setTexture(pirate.textures[0]);
                        }
                        pirate.velocity.y = 0;
                    }

                }
            }

            //else character.pirate.isJumping = true;

        //collision OF PIRATE  with window
            if (pirate.shape.getPosition().x - pirate.shape.getGlobalBounds().width * 0.5f <= 0) // left
                pirate.shape.setPosition(pirate.shape.getGlobalBounds().width * 0.5f, pirate.shape.getPosition().y);

            if (pirate.shape.getPosition().x >= window.getSize().x - (pirate.shape.getGlobalBounds().width * 0.5f))// right
                pirate.shape.setPosition(window.getSize().x - pirate.shape.getGlobalBounds().width * 0.5f, pirate.shape.getPosition().y);

            if (pirate.shape.getPosition().y <= 0) // top
                pirate.shape.setPosition(pirate.shape.getPosition().x, 0.f);

            if (pirate.shape.getPosition().y >= window.getSize().y - pirate.shape.getGlobalBounds().height) // bottom
                pirate.shape.setPosition(pirate.shape.getPosition().x, window.getSize().y - pirate.shape.getGlobalBounds().height);

            //collision OF BOMB with window 
            for (int i = 0; i < bomb.bombs.size(); ++i)
            {
                if (bomb.bombs[i].shape.getPosition().x - bomb.bombs[i].shape.getGlobalBounds().width * 0.25f <= 0) // left
                    bomb.bombs[i].shape.setPosition(bomb.bombs[i].shape.getGlobalBounds().width * 0.25f, bomb.bombs[i].shape.getPosition().y);

                if (bomb.bombs[i].shape.getPosition().x >= window.getSize().x - (bomb.bombs[i].shape.getGlobalBounds().width * 0.25f))// right
                    bomb.bombs[i].shape.setPosition(window.getSize().x - bomb.bombs[i].shape.getGlobalBounds().width * 0.25f, bomb.bombs[i].shape.getPosition().y);
            }


            //Functions Call
            Pirate_move_code();

            big_guy_code(big_guy, 55, 850);

            bald_code(bald, 980, 1900, 2.f);

            Whale_code(whale, 1, platform[4]);


            // Bomb shoot 
            if (shootTimer < 12.5)
                shootTimer++;
            if (player_init and playerHearts > 0)
            {
                if (Mouse::isButtonPressed(Mouse::Left) and shootTimer >= 12.5 and bomb.bombs.size() < 5) {
                    //sound.bombsound.play();
                    bombcounter--;
                    bombnum.setTexture(bombnum_tex[bombcounter]);


                    bomb.shape.setPosition(pirate.shape.getPosition().x, pirate.shape.getPosition().y - 35.f);
                    bomb.velocity.x = (pirate.shape.getScale().x > 0.f) ? 10.f * cos(initAngle) : -10.f * cos(initAngle);
                    bomb.velocity.y = -10.f * sin(initAngle);
                    bomb.isShot = true;
                    bombFired++;

                    if (bombFired <= bombsNum and bomb.bombs.size() < 5)bomb.bombs.pb(bomb); // BOMB fire limiter
                    shootTimer = 0; //RESET timer
                }
            }


            //bomb animation
            if (bombAnimTime <= 0 and !bomb.bombs.empty())
            {
                bombAnimIdx++;
                bombAnimIdx %= 10;
                for (int i = 0; i < bomb.bombs.size(); i++)
                {
                    bomb.bombs[i].shape.setTextureRect(IntRect(bombAnimIdx * 96, 0, 96, 108));

                }
                bombAnimTime = bombDelay;
            }
            else bombAnimTime -= dt;



            //move bombs
            if (!bomb.bombs.empty())
            {
                for (int i = 0; i < bomb.bombs.size(); ++i)
                {
                    bomb.bombs[i].velocity.y += g * dt; //apply gravity

                    bomb.bombs[i].shape.move(bomb.bombs[i].velocity);
                    HitBoxUpdate(bomb.bombs[i].shape, bomb.bombs[i].hitbox, 30);
                    HitBoxUpdate(bomb.bombs[i].shape, bomb.bombs[i].range, 30);

                    //collision between bombs
                    for (int j = 0; j < bomb.bombs.size(); ++j)
                    {
                        if (i == j)continue;
                        if (bomb.bombs[i].hitbox.getGlobalBounds().intersects(bomb.bombs[j].hitbox.getGlobalBounds()))
                        {
                            if (bomb.bombs[i].hitbox.getPosition().x < bomb.bombs[j].hitbox.getPosition().x)
                                bomb.bombs[j].velocity.x += 2.5f;
                            else bomb.bombs[j].velocity.x -= 2.5f;
                        }
                    }
                    if (bomb.bombs[i].hitbox.getGlobalBounds().intersects(pirate.hitbox.getGlobalBounds()) and bomb.bombs[i].isGrounded)
                        bomb.bombs[i].velocity.x += (pirate.hitbox.getPosition().x > bomb.bombs[i].hitbox.getPosition().x) ? -0.8f : 0.8f;
                }
            }



            // collision with objects for bomb (up and down)
            if (!bomb.bombs.empty())
            {
                for (int i = 0; i < bomb.bombs.size(); ++i)
                {
                    for (int j = 1; j <= 3; ++j)//3 tops
                    {
                        //top collision
                        if (bomb.bombs[i].hitbox.getPosition().y - bomb.bombs[i].hitbox.getGlobalBounds().height * 0.25f - 30 < objects[j].getPosition().y + objects[j].getGlobalBounds().height
                            and !(bomb.bombs[i].hitbox.getPosition().x - bomb.bombs[i].hitbox.getGlobalBounds().width * 0.25f > objects[j].getPosition().x + objects[j].getGlobalBounds().width)//left
                            and !(bomb.bombs[i].hitbox.getPosition().x + bomb.bombs[i].hitbox.getGlobalBounds().width * 0.25f < objects[j].getPosition().x)//right
                            )
                        {
                            if (bomb.bombs[i].hitbox.getGlobalBounds().intersects(objects[j].getGlobalBounds()))
                                if (bomb.bombs[i].velocity.y < 0)bomb.bombs[i].velocity.y *= -0.8f;
                        }
                    }
                    for (int j = 4; j <= 5; ++j)//2 bottoms
                    {

                        if (bomb.bombs[i].hitbox.getPosition().y + bomb.bombs[i].hitbox.getGlobalBounds().height * 0.25f - 30 < objects[j].getPosition().y
                            and !(bomb.bombs[i].hitbox.getPosition().x - bomb.bombs[i].hitbox.getGlobalBounds().width * 0.25f > objects[j].getPosition().x + objects[j].getGlobalBounds().width)//left
                            and !(bomb.bombs[i].hitbox.getPosition().x + bomb.bombs[i].hitbox.getGlobalBounds().width * 0.25f < objects[j].getPosition().x)//right
                            )
                        {
                            if (bomb.bombs[i].hitbox.getGlobalBounds().intersects(objects[j].getGlobalBounds())) {
                                bomb.bombs[i].isGrounded = 1;
                                bomb.bombs[i].velocity.y = 0.f;
                                bomb.bombs[i].shape.setPosition(bomb.bombs[i].shape.getPosition().x, objects[j].getPosition().y - 60);
                                if (bomb.bombs[i].velocity.x > 0)
                                {
                                    bomb.bombs[i].velocity.x -= resistanceForce * dt;
                                    if (bomb.bombs[i].velocity.x <= 0)bomb.bombs[i].velocity.x = 0.f;
                                }

                                else if (bomb.bombs[i].velocity.x < 0)

                                {
                                    bomb.bombs[i].velocity.x += resistanceForce * dt;
                                    if (bomb.bombs[i].velocity.x >= 0)bomb.bombs[i].velocity.x = 0.f;
                                }
                                //  cout << "Ground\n";
                            }
                        }
                    }
                }
            }


            //collision with objects for bomb (left and right)
            if (!bomb.bombs.empty())
            {
                for (int i = 0; i < bomb.bombs.size(); ++i)
                {
                    for (int j = 0; j <= 8; j++)
                    {
                        //left direction
                        if (bomb.bombs[i].hitbox.getPosition().x - bomb.bombs[i].hitbox.getGlobalBounds().width * 0.25f > objects[j].getPosition().x + objects[j].getGlobalBounds().width
                            and !(bomb.bombs[i].hitbox.getPosition().y + bomb.bombs[i].hitbox.getGlobalBounds().height * 0.25f - 30 < objects[j].getPosition().y)
                            and !(bomb.bombs[i].hitbox.getPosition().y - bomb.bombs[i].hitbox.getGlobalBounds().height * 0.25f + 30 > objects[j].getPosition().y + objects[j].getGlobalBounds().height)
                            )
                        {
                            if (CollisionDetection(bomb.bombs[i].hitbox, objects[j]))
                            {

                                if (bomb.bombs[i].velocity.x < 0)bomb.bombs[i].velocity.x *= -0.8f;
                                //cout << "Left\n";
                            }
                        }
                        //right direction
                        if (bomb.bombs[i].hitbox.getPosition().x + bomb.bombs[i].hitbox.getGlobalBounds().width * 0.25f < objects[j].getPosition().x
                            and !(bomb.bombs[i].hitbox.getPosition().y + bomb.bombs[i].hitbox.getGlobalBounds().height * 0.25 - 30 < objects[j].getPosition().y)
                            and !(bomb.bombs[i].hitbox.getPosition().y - bomb.bombs[i].hitbox.getGlobalBounds().height * 0.25f + 30 > objects[j].getPosition().y + objects[j].getGlobalBounds().height)
                            )
                        {
                            if (CollisionDetection(bomb.bombs[i].hitbox, objects[j]))
                            {
                                if (bomb.bombs[i].velocity.x > 0)bomb.bombs[i].velocity.x *= -0.8f;
                                // cout << bomb.bombs[i].velocity.x << "\n";
                                 //cout << "Right\n";
                            }
                        }
                    }
                }
            }

            //special case for objects[0] -> ground / top
            if (!bomb.bombs.empty())
            {
                for (int i = 0; i < bomb.bombs.size(); ++i) {
                    //ground collision
                    if (bomb.bombs[i].hitbox.getPosition().y + bomb.bombs[i].hitbox.getGlobalBounds().height * 0.25 - 30 < objects[0].getPosition().y
                        and !(bomb.bombs[i].hitbox.getPosition().x + bomb.bombs[i].hitbox.getGlobalBounds().width * 0.25f < objects[0].getPosition().x)
                        and !(bomb.bombs[i].hitbox.getPosition().x - bomb.bombs[i].hitbox.getGlobalBounds().width * 0.25f > objects[0].getPosition().x + objects[0].getGlobalBounds().width)
                        )
                    {
                        if (bomb.bombs[i].hitbox.getGlobalBounds().intersects(objects[0].getGlobalBounds())) {
                            bomb.bombs[i].isGrounded = 1;
                            bomb.bombs[i].velocity.y = 0.f;
                            bomb.bombs[i].shape.setPosition(bomb.bombs[i].shape.getPosition().x, objects[0].getPosition().y - 60);
                            if (bomb.bombs[i].velocity.x > 0)
                            {
                                bomb.bombs[i].velocity.x -= resistanceForce * dt;
                                if (bomb.bombs[i].velocity.x <= 0)bomb.bombs[i].velocity.x = 0.f;
                            }

                            else if (bomb.bombs[i].velocity.x < 0)

                            {
                                bomb.bombs[i].velocity.x += resistanceForce * dt;
                                if (bomb.bombs[i].velocity.x >= 0)bomb.bombs[i].velocity.x = 0.f;
                            }
                            //cout << "Ground\n";
                        }
                    }
                    //top collision
                    if (bomb.bombs[i].hitbox.getPosition().y - bomb.bombs[i].hitbox.getGlobalBounds().height * 0.25f + 30 > objects[0].getPosition().y + objects[0].getGlobalBounds().height
                        and !(bomb.bombs[i].hitbox.getPosition().x + bomb.bombs[i].hitbox.getGlobalBounds().width * 0.25f < objects[0].getPosition().x)
                        and !(bomb.bombs[i].hitbox.getPosition().x - bomb.bombs[i].hitbox.getGlobalBounds().width * 0.25f > objects[0].getPosition().x + objects[0].getGlobalBounds().width)
                        )
                    {
                        if (bomb.bombs[i].hitbox.getGlobalBounds().intersects(objects[0].getGlobalBounds()))
                        {
                            bomb.bombs[i].velocity.y *= -0.8f;
                            //cout << "Top\n";
                        }
                    }
                }
            }

            //============================================================================================================
            //collision of bombs with platforms 

            if (!bomb.bombs.empty())
            {
                for (int i = 0; i < bomb.bombs.size(); ++i)
                {
                    for (int j = 0; j < 5; ++j)//ALL bottoms
                    {
                        if (bomb.bombs[i].hitbox.getPosition().y + bomb.bombs[i].hitbox.getGlobalBounds().height * 0.25f - 30 < platform[j].getPosition().y
                            and !(bomb.bombs[i].hitbox.getPosition().x - bomb.bombs[i].hitbox.getGlobalBounds().width * 0.25f > platform[j].getPosition().x + platform[j].getGlobalBounds().width)//left
                            and !(bomb.bombs[i].hitbox.getPosition().x + bomb.bombs[i].hitbox.getGlobalBounds().width * 0.25f < platform[j].getPosition().x)//right
                            )
                        {

                            if (bomb.bombs[i].hitbox.getGlobalBounds().intersects(platform[j].getGlobalBounds())) {

                                bomb.bombs[i].isGrounded = 1;
                                bomb.bombs[i].velocity.y = 0.f;
                                bomb.bombs[i].shape.setPosition(bomb.bombs[i].shape.getPosition().x, platform[j].getPosition().y - 60);
                                if (bomb.bombs[i].velocity.x > 0)
                                {
                                    bomb.bombs[i].velocity.x -= resistanceForce * dt;
                                    if (bomb.bombs[i].velocity.x <= 0)bomb.bombs[i].velocity.x = 0.f;
                                }

                                else if (bomb.bombs[i].velocity.x < 0)

                                {
                                    bomb.bombs[i].velocity.x += resistanceForce * dt;
                                    if (bomb.bombs[i].velocity.x >= 0)bomb.bombs[i].velocity.x = 0.f;
                                }

                                // cout << "Ground\n";
                            }
                        }
                    }
                }
            }

            //============================================================================================================


             //detonation
            for (int i = 0; i < bomb.bombs.size(); ++i)
            {
                if (bomb.bombs[i].isShot)
                {

                    //big guy stuff
                    if (big_guy.isAlive)
                        if (big_guy.RunToBombRange.getGlobalBounds().intersects(bomb.bombs[i].hitbox.getGlobalBounds()))
                        {
                            if (big_guy.hitbox.getPosition().x < bomb.bombs[i].hitbox.getPosition().x)// Bomb is on the right side
                            {
                                animate(big_guy.shape, big_guy.text[1], bigGuyAnimIdx, 77, 74, bigGuyTimer, 16, 1, 2.f);
                                big_guy.velocity.x = 90.f * dt;
                                if (big_guy.hitbox.getGlobalBounds().intersects(bomb.bombs[i].hitbox.getGlobalBounds()))
                                {
                                    big_guy.velocity.x = 0;
                                    if (big_guy.shape.getTexture() != &big_guy.text[3])
                                    {

                                        big_guy.shape.setTexture(big_guy.text[3]);
                                        //l = 0;
                                        big_guy.shape.setTextureRect(IntRect(bigGuyAttackIdx * 77, 0, 77, 74));
                                    }
                                    if (bigGuyTimer <= 0 and !big_guy.pick_up and !big_guy.bomb_throw)
                                    {
                                        bigGuyAttackIdx++;
                                        bigGuyAttackIdx %= 8;
                                        if (bigGuyAttackIdx == 5)
                                        {
                                            bomb.bombs[i].shape.setPosition(big_guy.hitbox.getPosition());
                                            big_guy.pick_up = false;
                                            big_guy.bomb_throw = true;
                                        }
                                        //cout << l << " ";
                                        big_guy.shape.setTextureRect(IntRect(bigGuyAttackIdx * 77, 0, 77, 74));
                                        bigGuyTimer = delay * 3;
                                    }
                                    else
                                        bigGuyTimer -= dt;

                                }
                                else bigGuyAttackIdx = 0;
                            }
                            else if (big_guy.hitbox.getPosition().x > bomb.bombs[i].hitbox.getPosition().x)// Bomb is on the left side
                            {
                                animate(big_guy.shape, big_guy.text[1], bigGuyAnimIdx, 77, 74, bigGuyTimer, 16, -1, 2.f);
                                big_guy.velocity.x = -90.f * dt;
                                if (big_guy.hitbox.getGlobalBounds().intersects(bomb.bombs[i].hitbox.getGlobalBounds()))
                                {
                                    big_guy.velocity.x = 0;
                                    if (big_guy.shape.getTexture() != &big_guy.text[3])
                                    {
                                        big_guy.shape.setTexture(big_guy.text[3]);
                                        //l = 0;
                                        big_guy.shape.setTextureRect(IntRect(bigGuyAttackIdx * 77, 0, 77, 74));
                                    }
                                    if (bigGuyTimer <= 0 and !big_guy.pick_up and !big_guy.bomb_throw)
                                    {
                                        bigGuyAttackIdx++;
                                        bigGuyAttackIdx %= 8;
                                        //cout << l << '\n';
                                        if (bigGuyAttackIdx == 5)
                                        {
                                            bomb.bombs[i].shape.setPosition(big_guy.hitbox.getPosition());
                                            big_guy.pick_up = false;
                                            big_guy.bomb_throw = true;
                                        }
                                        //cout << l << " ";
                                        big_guy.shape.setTextureRect(IntRect(bigGuyAttackIdx * 77, 0, 77, 74));
                                        bigGuyTimer = delay * 3;
                                    }
                                    else
                                        bigGuyTimer -= dt;
                                }
                                else bigGuyAttackIdx = 0;
                            }
                            if (!big_guy.pick_up and big_guy.bomb_throw and big_guy.hitbox.getGlobalBounds().intersects(bomb.bombs[i].hitbox.getGlobalBounds()))
                            {
                                if (big_guy.shape.getTexture() != &big_guy.text[4])
                                {
                                    if (!oneTime_bigGuy_anim)
                                    {
                                        bigGuyAttackIdx = 0;
                                        oneTime_bigGuy_anim = !oneTime_bigGuy_anim;
                                    }
                                    big_guy.shape.setTexture(big_guy.text[4]);
                                    big_guy.shape.setTextureRect(IntRect(bigGuyAttackIdx * 77, 0, 77, 74));
                                }
                                if (throwAnim <= 0 and big_guy.bomb_throw)
                                {
                                    bigGuyAttackIdx++;
                                    bigGuyAttackIdx %= 11;
                                    if (bigGuyAttackIdx == 4)
                                    {
                                        bomb.bombs[i].shape.setPosition(big_guy.hitbox.getPosition());
                                        bomb.bombs[i].velocity.y = -10 * sin(initAngle);
                                        if (pirate.shape.getPosition().x > big_guy.shape.getPosition().x)
                                            big_guy.shape.setScale(2.f, 2.f);
                                        else if (pirate.shape.getPosition().x < big_guy.shape.getPosition().x)
                                            big_guy.shape.setScale(-2.f, 2.f);
                                        bomb.bombs[i].velocity.x = (big_guy.shape.getScale().x > 0) ? 10 * cos(initAngle) : -10 * cos(initAngle);

                                    }
                                    if (bigGuyAttackIdx == 10)
                                    {
                                        big_guy.bomb_throw = false;
                                    }
                                    big_guy.shape.setTextureRect(IntRect(bigGuyAttackIdx * 77, 0, 77, 74));
                                    throwAnim = delay * 3;
                                }
                                else throwAnim -= dt;
                            }

                        }
                    //Bald guy kicks bomb
                    if (bald.isAlive)
                        if (bald.bombRange.getGlobalBounds().intersects(bomb.bombs[i].hitbox.getGlobalBounds()))
                        {
                            if (bald.hitbox.getPosition().x < bomb.bombs[i].hitbox.getPosition().x)// Bomb is on the right side
                            {
                                animate(bald.shape, bald.text[1], baldAnimIdx, 63, 67, baldTimer, 14, 1, 2.f);
                                bald.velocity.x = 90.f * dt;
                                if (bald.hitbox.getGlobalBounds().intersects(bomb.bombs[i].hitbox.getGlobalBounds()))
                                {
                                    sound.kickSFX.play();
                                    bald.velocity.x = 0;
                                    bomb.bombs[i].shape.move(0, bomb.bombs[i].velocity.y);
                                    if (bald.shape.getTexture() != &bald.text[2])
                                    {

                                        bald.shape.setTexture(bald.text[2]);
                                        //c = 0;
                                        bald.shape.setTextureRect(IntRect(baldAttackIdx * 63, 0, 63, 67));
                                    }
                                    if (baldTimer <= 0 and !bald.pick_up)
                                    {
                                        baldAttackIdx++;
                                        baldAttackIdx %= 12; // 0 -> 11
                                        if (baldAttackIdx == 6) bald.shoot = true;
                                        if (baldAttackIdx == 11) bald.pick_up = true;

                                        bald.shape.setTextureRect(IntRect(baldAttackIdx * 63, 0, 63, 67));
                                        baldTimer = delay * 3;
                                    }
                                    else
                                        baldTimer -= dt;

                                }
                                else baldAttackIdx = 0;
                            }
                            else if (bald.hitbox.getPosition().x > bomb.bombs[i].hitbox.getPosition().x)// Bomb is on the left side
                            {
                                animate(bald.shape, bald.text[1], baldAnimIdx, 63, 67, baldTimer, 14, -1, 2.f);
                                bald.velocity.x = -90.f * dt;
                                if (bald.hitbox.getGlobalBounds().intersects(bomb.bombs[i].hitbox.getGlobalBounds()))
                                {
                                    sound.kickSFX.play();
                                    bald.velocity.x = 0;
                                    if (bald.shape.getTexture() != &bald.text[2])
                                    {
                                        bald.shape.setTexture(bald.text[2]);
                                        //c = 0;
                                        bald.shape.setTextureRect(IntRect(baldAttackIdx * 63, 0, 63, 67));
                                    }
                                    if (baldTimer <= 0 and !bald.pick_up)
                                    {

                                        baldAttackIdx++;
                                        baldAttackIdx %= 12;
                                        if (baldAttackIdx == 6) bald.shoot = true;
                                        if (baldAttackIdx == 11)bald.pick_up = true;
                                        bald.shape.setTextureRect(IntRect(baldAttackIdx * 63, 0, 63, 67));
                                        baldTimer = delay * 3;
                                    }
                                    else
                                        baldTimer -= dt;
                                }
                                else baldAttackIdx = 0;
                            }

                        }
                    // bald shoot
                    // bald shoot
                    if (bald.shoot)
                    {
                        /* bomb.bombs[i].velocity.x = (bald.shape.getScale().x > 0.f) ? 10.f * cos(initAngle) : -10.f * cos(initAngle);
                         bomb.bombs[i].velocity.y = -10.f * sin(initAngle);*/
                         //bomb.bombs[i].shape.move(bomb.bombs[i].velocity);
                        bomb.bombs[i].velocity.y = -10.15f * sin(initAngle);
                        if (pirate.shape.getPosition().x > bald.shape.getPosition().x)
                            bald.shape.setScale(2.f, 2.f);
                        else if (pirate.shape.getPosition().x < bald.shape.getPosition().x)
                            bald.shape.setScale(-2.f, 2.f);
                        bomb.bombs[i].velocity.x = (bald.shape.getScale().x > 0) ? 10 * cos(initAngle) : -10 * cos(initAngle);
                        bald.shoot = false;

                    }

                    //whale stuff
                    if (whale.isAlive)
                    {
                        if (whale.range.getGlobalBounds().intersects(bomb.bombs[i].hitbox.getGlobalBounds()) and u < 10)
                        {

                            whale.bombInRange = true;
                            //left
                            if (bomb.bombs[i].hitbox.getPosition().x < whale.hitbox.getPosition().x and !whale.isJumping and whale.isAlive and !isSwallowed[i]
                                and !(bomb.bombs[i].hitbox.getPosition().y + bomb.bombs[i].hitbox.getGlobalBounds().height * 0.25f < whale.hitbox.getPosition().y - whale.hitbox.getGlobalBounds().height * 0.25f)
                                and whale.hitbox.getPosition().y > bomb.bombs[i].hitbox.getPosition().y
                                )
                            {
                                if (!whale.attack and whale.isAlive and !whale.isSwallowing)
                                {
                                    //whale_run_ani(1);
                                    animate(whale.shape, whale.textures[1], whaleAnimIdx, 68, 46, whaleTimer, 14, 1, 1.5f);
                                    whale.velocity.x = -80.f * dt;
                                }

                                if (bomb.bombs[i].hitbox.getGlobalBounds().intersects(whale.hitbox.getGlobalBounds()) and u < 10)
                                {
                                    whale.isSwallowing = true;
                                    bomb.bombs[i].velocity = { 0,0 };
                                    whale.velocity.x = 0;
                                    if (whale.SwallowTimer >= 60)
                                    {
                                        if (whale.shape.getTexture() != &whale.textures[5])
                                        {
                                            whaleSwallowIdx = 0;
                                            whale.shape.setTexture(whale.textures[5]);
                                            whale.shape.setTextureRect(IntRect(whaleSwallowIdx * 68, 0, 68, 46));
                                        }
                                        if (whaleTimer <= 0)
                                        {
                                            whaleSwallowIdx++;
                                            whaleSwallowIdx %= 10;
                                            if (whaleSwallowIdx == 8)
                                            {
                                                u += 5;
                                                sound.gulpsound.play();
                                                sound.bombsound.stop();
                                                bomb.bombs[i].shape.setPosition(whale.hitbox.getPosition());
                                                whale.isSwallowing = false;
                                                whale.bombInRange = false;
                                                bomb.bombs.erase(bomb.bombs.begin() + i);
                                                whale.SwallowTimer = -1;
                                                memset(isSwallowed, false, sizeof(isSwallowed));
                                                break;
                                            }
                                            whale.shape.setTextureRect(IntRect(whaleSwallowIdx * 68, 0, 68, 46));
                                            whaleTimer = delay;
                                        }
                                        else whaleTimer -= dt;
                                    }
                                }
                                else whaleSwallowIdx = 0;
                            }
                            else if (bomb.bombs[i].hitbox.getPosition().x > whale.hitbox.getPosition().x and !whale.isJumping and whale.isAlive and !isSwallowed[i]
                                and !(bomb.bombs[i].hitbox.getPosition().y + bomb.bombs[i].hitbox.getGlobalBounds().height * 0.25f < whale.hitbox.getPosition().y - whale.hitbox.getGlobalBounds().height * 0.25f)
                                and whale.hitbox.getPosition().y > bomb.bombs[i].hitbox.getPosition().y
                                )
                            {
                                if (!whale.attack and whale.isAlive and !whale.isSwallowing)
                                {
                                    animate(whale.shape, whale.textures[1], whaleAnimIdx, 68, 46, whaleTimer, 14, -1, 1.5f);
                                    whale.velocity.x = 80.f * dt;
                                }
                                if (bomb.bombs[i].hitbox.getGlobalBounds().intersects(whale.hitbox.getGlobalBounds()))
                                {
                                    whale.isSwallowing = true;
                                    bomb.bombs[i].velocity = { 0,0 };
                                    whale.velocity.x = 0;
                                    if (whale.SwallowTimer >= 60)
                                    {
                                        if (whale.shape.getTexture() != &whale.textures[5])
                                        {
                                            whaleSwallowIdx = 0;
                                            whale.shape.setTexture(whale.textures[5]);
                                            whale.shape.setTextureRect(IntRect(whaleSwallowIdx * 68, 0, 68, 46));
                                        }
                                        if (whaleTimer <= 0)
                                        {
                                            whaleSwallowIdx++;
                                            whaleSwallowIdx %= 10;
                                            if (whaleSwallowIdx == 8)
                                            {
                                                u += 5;
                                                sound.gulpsound.play();
                                                sound.bombsound.stop();
                                                bomb.bombs[i].shape.setPosition(whale.hitbox.getPosition());
                                                isSwallowed[i] = true;
                                                whale.isSwallowing = false;
                                                whale.bombInRange = false;
                                                bomb.bombs.erase(bomb.bombs.begin() + i);
                                                whale.SwallowTimer = -1;
                                                memset(isSwallowed, false, sizeof(isSwallowed));
                                                break;
                                            }
                                            whale.shape.setTextureRect(IntRect(whaleSwallowIdx * 68, 0, 68, 46));
                                            whaleTimer = delay;
                                        }
                                        else whaleTimer -= dt;
                                    }
                                }
                                else whaleSwallowIdx = 0;
                            }
                        }
                        else whale.bombInRange = false;
                    }



                    //bomb stuff
                    if (bomb.bombs[i].detonateTimer < TimeToDetonate)
                        bomb.bombs[i].detonateTimer++;
                    else
                    {
                        if (bomb.bombs[i].shape.getTexture() != &bomb.bombs[i].bombOffTex)
                        {
                            bombAnimIdx = 0;
                            bomb.bombs[i].shape.setTexture(bomb.bombs[i].bombOffTex);
                            bomb.bombs[i].shape.setTextureRect(IntRect(bombAnimIdx * 96, 0, 96, 108));
                        }
                        if (bombAnimTime <= 0)
                        {
                            bombAnimIdx++;
                            bombAnimIdx %= 9; // 0->8
                            bomb.bombs[i].shape.setTextureRect(IntRect(bombAnimIdx * 96, 0, 96, 108));
                            bombAnimTime = bombDelay;
                        }
                        else bombAnimTime -= dt;
                        if (bombAnimIdx == 1)
                        {
                            for (int j = 0; j < bomb.bombs.size(); ++j)
                            {
                                if (j == i)continue;
                                if (bomb.bombs[i].shape.getGlobalBounds().intersects(bomb.bombs[j].shape.getGlobalBounds()))
                                {
                                    if (bomb.bombs[i].hitbox.getPosition().x < bomb.bombs[j].hitbox.getPosition().x and bomb.bombs[i].hitbox.getPosition().y == bomb.bombs[j].hitbox.getPosition().y)
                                    {
                                        bomb.bombs[j].velocity.x += 5.f;
                                    }
                                    else if (bomb.bombs[i].hitbox.getPosition().x > bomb.bombs[j].hitbox.getPosition().x and bomb.bombs[i].hitbox.getPosition().y == bomb.bombs[j].hitbox.getPosition().y)
                                    {
                                        bomb.bombs[j].velocity.x -= 5.f;
                                    }
                                }

                            }
                            if (pirate.hitbox.getGlobalBounds().intersects(bomb.bombs[i].range.getGlobalBounds()) and !pirate.isHit)
                            {
                                sound.livesdec.play();
                                pirate.isHit = true;
                                playerHearts--;
                            }
                            if (big_guy.hitbox.getGlobalBounds().intersects(bomb.bombs[i].range.getGlobalBounds()))
                            {
                                big_guy.isAlive = false;
                            }
                            if (bald.hitbox.getGlobalBounds().intersects(bomb.bombs[i].range.getGlobalBounds()))
                            {
                                bald.isAlive = false;
                            }
                            if (whale.hitbox.getGlobalBounds().intersects(bomb.bombs[i].range.getGlobalBounds()) and !isSwallowed[i])
                            {
                                //cout << "dead\n";
                                whale.isAlive = false;
                            }

                        }
                        if (bombAnimIdx == 1)
                        {
                            sound.bombsound.play();
                        }
                        if (bombAnimIdx == 8) {
                            memset(isSwallowed, false, sizeof(isSwallowed));
                            big_guy.pick_up = false;
                            bald.pick_up = false;
                            whale.isSwallowing = false;
                            whale.bombInRange = false;
                            bomb.bombs.erase(bomb.bombs.begin() + i);
                            break;
                        }
                    }
                }
            }

            if (whale.SwallowTimer < 60)
            {
                whale.SwallowTimer++;
            }

            // cout << "X pos : " << Mouse::getPosition().x << " Y pos : " << Mouse::getPosition().y << endl;


             //death animation
            if (!big_guy.isAlive)
            {

                dead_animation(big_guy.shape, big_guy.text[6], bigGuyAnimIdx, 77, 74, bigGuyTimer, 6, (big_guy.shape.getScale().x > 0.f) ? 1 : -1, 2.f, 5, 2);
                big_guy.velocity = { 0,0 };
                big_guy.shape.setPosition(big_guy.shape.getPosition().x, 950.f);
            }


            if (!bald.isAlive)
            {
                dead_animation(bald.shape, bald.text[3], baldAnimIdx, 63, 67, baldTimer, 6, (bald.shape.getScale().x > 0.f) ? 1 : -1, 2.f, 5, 2);
                bald.velocity = { 0,0 };
                bald.shape.setPosition(bald.shape.getPosition().x, 620.f);
            }

            if (!whale.isAlive)
            {
                dead_animation(whale.shape, whale.textures[6], whaleAnimIdx, 68, 46, whaleTimer, 6, (whale.shape.getScale().x > 0.f) ? 1 : -1, 1.5f, 5, 2);
                whale.velocity = { 0,0 };
                whale.shape.setPosition(whale.shape.getPosition().x, 395.f);
            }

            if (pirate.isHit)
            {
                if (pirate.isHit and playerHearts > 0)
                {
                    animate(pirate.shape, pirate.textures[8], captainAnimIdx, 58, 58, pirateTimer, 8, (pirate.shape.getScale().x > 0.f) ? 1 : -1, 2.f);
                    if (captainAnimIdx == 7)
                        pirate.isHit = false;
                }
                else if (playerHearts <= 0)
                {
                    dead_animation(pirate.shape, pirate.textures[9], pirateAnimIdx, 58, 58, pirateTimer, 6, (pirate.shape.getScale().x > 0.f) ? 1 : -1, 2.f, 5);
                    if (pirateAnimIdx == 5)
                        pirateAnimIdx++;
                    //cout << pirateAnimIdx << '\n';
                    pirate.velocity = { 0,0 };
                }

            }

            //move character
           //moveCharacter(character);
            pirate.shape.move(pirate.velocity);
            big_guy.shape.move(big_guy.velocity);
            bald.shape.move(bald.velocity);
            whale.shape.move(whale.velocity);
            window.clear(Color::Black);



            //Kill switch to test door animation after all enemies are conquered

            if (Keyboard::isKeyPressed(Keyboard::Up))
            {
                // sound.livesdec.play();
                pirate.isAlive = 1;
                bald.isAlive = 0;
                whale.isAlive = 0;
                big_guy.isAlive = 0;
            }

            //door animation + player entering in animation after all enemies are conquered 
            if (pirate.isAlive and !bald.isAlive and !whale.isAlive and !big_guy.isAlive)
            {
                levelFinished = true;

                if (door2.frame <= door2.mxframe) {
                    if (door2.frame == 0) sound.doorsfx.play();
                    doorTimer2 += dt;
                    if (doorTimer2 > 0.8f) {
                        doorTimer2 = 0;
                        door2.frame++;
                        door2.shape.setTextureRect(IntRect(door2.frame * 78, 0, 78, 98));
                    }
                }
            }

            if (doorBar.getGlobalBounds().intersects(pirate.hitbox.getGlobalBounds()) and levelFinished)
            {
                pirate.doorInControl = true;
                pirate.velocity = { 0,0 };

                if (pirate.shape.getTexture() != &pirate.textures[5])
                {
                    pirate.shape.setTexture(pirate.textures[5]);
                    pirateAnimIdx = 0;
                    pirate.shape.setTextureRect(IntRect(pirateAnimIdx * 58, 0, 58, 58));
                }
                if (pirateAnimIdx < 13)
                {
                    if (pirateTimer <= 0)
                    {
                        pirateAnimIdx++;
                        pirateAnimIdx %= 14;
                        pirate.shape.setTextureRect(IntRect(pirateAnimIdx * 58, 0, 58, 58));
                        pirateTimer = delay * 1.5;
                    }
                    else pirateTimer -= dt;
                }
                else
                {
                    phase++;
                    return;
                }

            }


            if (Keyboard::isKeyPressed(Keyboard::Right) && !isPressed[Keyboard::Right])
            {
                //menuSelect.play();
                isPressed[Keyboard::Right] = 1;
            }

            RepeatedKeyHandle(event, isPressed);


            //==============================================Draw==============================================================
            //UI
            window.draw(back);
            window.draw(healthBar);

            if (pirate.isAlive)
                for (int i = 0; i < playerHearts; i++)
                {
                    window.draw(heart[i]);
                }

            if (bombcounter > 0)
            {
                window.draw(bombnum);
            }
            else
            {
                bombnum.setTexture(bombnum_tex[0]);
                window.draw(bombnum);
            }


            //platforms
            for (int i = 0; i < 5; i++)
            {
                window.draw(platform[i]);
            }
            for (int i = 0; i < 6; i++)
            {
                window.draw(objects[i]);
            }

            //Door
            window.draw(door.shape);
            window.draw(door2.shape);
            window.draw(doorBar);

            //===========================================================Characters=======================================================

                    //Bald
                    //if (bald.isAlive)
            {
                window.draw(bald.shape);
                // window.draw(bald.hitbox);
                // window.draw(bald.range);
                // window.draw(bald.bombRange);
                if (exclamation[1].playerInRange) window.draw(exclamation[1].shape);

            }

            //Big guy
            //if(big_guy.isAlive)
            {
                window.draw(big_guy.shape);
                // window.draw(big_guy.hitbox);
                 //window.draw(big_guy.range);
                 //window.draw(big_guy.RunToBombRange);
                if (exclamation[0].playerInRange) window.draw(exclamation[0].shape);
            }

            //whale
            window.draw(whale.shape);
            //window.draw(whale.hitbox);
            //window.draw(whale.range);
            if (exclamation[2].playerInRange) window.draw(exclamation[2].shape);
            if (u > 0 and whale.isAlive)
                window.draw(whale.hungerBar);


            for (int i = 0; i < bomb.bombs.size(); i++)
            {
                /*if (isSwallowed[i])continue;*/
                // window.draw(bomb.bombs[i].hitbox);
                if (playerHearts > 0)window.draw(bomb.bombs[i].shape);
                //window.draw(bomb.bombs[i].range);
            }

            //Pirate
          // window.draw(pirate.hitbox);
            if (door_opened)
                window.draw(pirate.shape);
            //window.draw(pirate.hitbox);
            if (!pirate.isJumping)
            {
                if (Keyboard::isKeyPressed(Keyboard::D) or Keyboard::isKeyPressed(Keyboard::A))
                    window.draw(pirate.particle);
            }
            else if (pirate.isJumping and !pirate.stop_jumpParticles)
                window.draw(pirate.particle);

        }
        //=========================================== End level UI========================================



        if (bombcounter <= 0 and !levelFinished)
        {
            gameEnded = true;
            while (window.pollEvent(event))
            {
                if (event.type == Event::Closed || event.type == Event::KeyPressed && event.key.code == Keyboard::Escape)
                {
                    window.close();
                }
                if (event.type == Event::KeyReleased)
                {
                    if (event.key.code == Keyboard::Up)
                    {
                        sound.menu_select.play();
                        mainMenu.MoveButtonUp();

                    }
                    if (event.key.code == Keyboard::Down)
                    {
                        //cout << "im here\n";
                        sound.menu_select.play();
                        mainMenu.MoveButtonDown();

                    }
                    if (event.key.code == Keyboard::Return)
                    {
                        if (mainMenu.ButtonPressed() == 0)
                        {
                            sound.press.play();
                            gameEnded = false;
                            pirate.shape.setScale(0, 0);
                            window.display();
                            return;
                        }
                        if (mainMenu.ButtonPressed() == 1)
                        {
                            sound.press.play();
                            phase = -1;
                            return;
                        }
                    }
                }
            }
            window.draw(deathUI[3]);
            window.draw(deathUI[2]);
            window.draw(mainMenu.ButtonsText[0]);
            window.draw(mainMenu.ButtonsText[1]);
            mainMenu.drawButtons(window);
        }




        //DEATH SCREEN
        if (playerHearts <= 0 and bomb.bombs.empty() and pirateAnimIdx == 6)
        {
            //cout << "Ended\n";
            /*if (!bomb.bombs.empty())
                for (auto& bomb : bomb.bombs)
                    bomb.shape.setScale(0, 0);*/
                    //bomb.bombs.erase(bomb.bombs.begin(), bomb.bombs.end());
            gameEnded = true;
            while (window.pollEvent(event))
            {
                if (event.type == Event::Closed || event.type == Event::KeyPressed && event.key.code == Keyboard::Escape)
                {
                    window.close();
                }
                if (event.type == Event::KeyReleased)
                {
                    if (event.key.code == Keyboard::Up)
                    {
                        sound.menu_select.play();
                        mainMenu.MoveButtonUp();

                    }
                    if (event.key.code == Keyboard::Down)
                    {
                        //cout << "im here\n";
                        sound.menu_select.play();
                        mainMenu.MoveButtonDown();

                    }
                    if (event.key.code == Keyboard::Return)
                    {
                        if (mainMenu.ButtonPressed() == 0)
                        {
                            sound.press.play();
                            gameEnded = false;
                            pirate.shape.setScale(0, 0);
                            window.display();
                            return;
                        }
                        if (mainMenu.ButtonPressed() == 1)
                        {
                            sound.press.play();
                            phase = -1;
                            return;
                        }
                    }
                }
            }
            window.draw(deathUI[0]);
            window.draw(deathUI[2]);
            window.draw(mainMenu.ButtonsText[0]);
            window.draw(mainMenu.ButtonsText[1]);
            mainMenu.drawButtons(window);
        }
        window.display();
    }

}

void lobby_2(RenderWindow& window)
{
    window.setFramerateLimit(60); //set it according to your monitor's refresh rate
    window.setVerticalSyncEnabled(true); //to avoid screen tearing

    initialize();

    //game clock
    Clock clock;
    Texture lobbyTxt;
    lobbyTxt.loadFromFile("./textures/lobby.png");
    Sprite lobby;
    lobby.setTexture(lobbyTxt);
    lobby.setScale(1, 1);

    //Door init
    Door door;
    door.shape.setTexture(door.door_tex[1]); //idle
    door.shape.setTextureRect(IntRect(door.frame * 78, 0, 78, 98));
    door.shape.setScale(2.f, 2.f);
    door.shape.setPosition(880.f, 245.f);
    bool door_opened = false;
    double doorDelay = 1.f;

    Door door2;
    door2.shape.setTexture(door.door_tex[1]); //idle
    door2.shape.setTextureRect(IntRect(0, 0, 78, 98));
    door2.shape.setScale(2.f, 2.f);
    door2.shape.setPosition(1425.f, 767.5f);
    //Door Bar
    RectangleShape doorBar(Vector2f(10, 150));
    doorBar.setPosition(door2.shape.getPosition().x + 110, door2.shape.getPosition().y + 45);
    doorBar.setFillColor(Color::Blue);

    Door door3;
    door3.shape.setTexture(door.door_tex[1]); //idle
    door3.shape.setTextureRect(IntRect(0, 0, 78, 98));
    door3.shape.setScale(2.f, 2.f);
    door3.shape.setPosition(890.f, 767.5f);

    pirate.shape.setTextureRect(IntRect(0, 0, 58, 58));
    pirate.shape.setPosition(Vector2f(door.shape.getPosition().x + 80, door.shape.getPosition().y + 30));
    pirate.shape.setOrigin(pirate.shape.getTextureRect().width / 2, pirate.shape.getTextureRect().height / 2);
    pirate.shape.setScale(2.f, 2.f);
    pirate.shape.setTexture(pirate.textures[0]);

    float doorTimer = 0, doorTimer2 = 0;

    //Objects
    RectangleShape objects[5];
    objects[0].setSize(Vector2f(1050, 215));
    objects[0].setPosition(95, 440);
    objects[0].setFillColor(Color::Red);

    objects[1].setSize(Vector2f(1240, 105));
    objects[1].setPosition(590, 965);
    objects[1].setFillColor(Color::Red);



    while (window.isOpen())
    {
        //cout << "X pos : " << Mouse::getPosition().x << " Y pos : " << Mouse::getPosition().y << endl;
        Event event;
        dt = clock.restart().asSeconds();
        while (window.pollEvent(event))
        {
            if (event.type == Event::Closed || event.type == Event::KeyPressed && event.key.code == Keyboard::Escape)
            {
                sound.press.play();
                phase = -1;
                window.close();
            }
        }
        if (pirate.shape.getScale().x > 0)pirate.hitbox.setPosition(pirate.shape.getPosition().x + 10, pirate.shape.getPosition().y + 5);
        else pirate.hitbox.setPosition(pirate.shape.getPosition().x - 10, pirate.shape.getPosition().y + 5);

        //apply gravity
        pirate.velocity.y += g * dt;



        if (!door_opened) {
            if (door.frame == 0) sound.doorsfx.play();
            pirate.shape.setScale(0, 0);
            if (door.frame < door.mxframe) {
                doorTimer += dt;
                //cout << doorTimer << endl;
                if (doorTimer > 0.8f) {

                    doorTimer = 0;
                    door.frame++;
                    door.shape.setTextureRect(IntRect(door.frame * 78, 0, 78, 98));
                }
            }
            else {
                door_opened = true;
                pirate.shape.setScale(2.f, 2.f);
            }
        }

        if (door_opened) {
            if (pirate.player_in_Frame <= pirate.player_in_mxFrame)
            {
                pirate.shape.setTexture(pirate.textures[4]);

                pirate.shape.setTextureRect(IntRect(pirate.player_in_Frame * 58, 0, 58, 58));

                if (pirate.player_init_timer <= 0)
                {
                    pirate.player_in_Frame++;
                    pirate.player_in_Frame %= 11; // 0 -> 10
                    pirate.shape.setTextureRect(IntRect(pirate.player_in_Frame * 58, 0, 58, 58));
                    pirate.player_init_timer = 0.06;
                }
                else pirate.player_init_timer -= dt;
            }

            else if (pirate.player_in_Frame >= pirate.player_in_mxFrame)
                player_init = true;
        }

        if (player_init) {
            //pirate.shape.setScale(2.f, 2.f);
            if (door.frame > 0) {
                doorTimer += dt;
                if (doorTimer > 0.6f) {
                    doorTimer = 0;
                    door.frame--;
                    door.shape.setTextureRect(IntRect(door.frame * 78, 0, 78, 98));
                }
            }
        }

        //collision with objects for player (width is height :) )
        for (int i = 0; i < 2; i++)
        {
            //ground
            if (pirate.hitbox.getPosition().y + pirate.hitbox.getGlobalBounds().width / 2.f < objects[i].getPosition().y)//check if player is on top of the object
            {
                if (CollisionDetection(pirate.hitbox, objects[i]))
                {

                    pirate.velocity.y = 0;
                    pirate.shape.setPosition(pirate.shape.getPosition().x, objects[i].getPosition().y - pirate.hitbox.getGlobalBounds().height * 0.5f - 5);


                    if (pirate.isJumping)
                    {
                        sound.groundhit.play();
                        pirate.isJumping = false;
                        pirate.shape.setTexture(pirate.textures[0]);
                    }
                    // cout << "foo2\n";
                }
            }
            //top
            else if (pirate.hitbox.getPosition().y - pirate.hitbox.getGlobalBounds().width / 2.f > objects[i].getPosition().y + objects[i].getGlobalBounds().height)
            {
                if (CollisionDetection(pirate.hitbox, objects[i]) and pirate.velocity.y <= 0)
                {
                    pirate.velocity.y *= -80.f * dt;
                }
            }
        }


        Pirate_move_code();
        pirate.shape.move(pirate.velocity);
        if (door_opened)
            if (door2.frame <= door2.mxframe) {
                sound.doorsfx.play();
                doorTimer2 += dt;
                if (doorTimer2 > 0.8f) {
                    doorTimer2 = 0;
                    door2.frame++;
                    door2.shape.setTextureRect(IntRect(door2.frame * 78, 0, 78, 98));
                }
            }


        if (doorBar.getGlobalBounds().intersects(pirate.hitbox.getGlobalBounds()))
        {
            pirate.doorInControl = true;
            pirate.velocity = { 0,0 };

            if (pirate.shape.getTexture() != &pirate.textures[5])
            {
                pirate.shape.setTexture(pirate.textures[5]);
                pirateAnimIdx = 0;
                pirate.shape.setTextureRect(IntRect(pirateAnimIdx * 58, 0, 58, 58));
            }
            if (pirateAnimIdx < 13)
            {
                if (pirateTimer <= 0)
                {
                    pirateAnimIdx++;
                    pirateAnimIdx %= 14;
                    pirate.shape.setTextureRect(IntRect(pirateAnimIdx * 58, 0, 58, 58));
                    pirateTimer = delay * 1.5;
                }
                else pirateTimer -= dt;
            }
            else
            {
                phase++;
                return;
            }

        }



        RepeatedKeyHandle(event, isPressed);

        window.clear();
        window.draw(lobby);
        window.draw(door.shape);
        window.draw(door2.shape);
        window.draw(door3.shape);
        //window.draw(doorBar);
        window.draw(pirate.shape);

        /* for (int i = 0; i < 2; i++)
         {
         window.draw(objects[i]);
         }*/

        window.display();

    }
}

void Level_2(RenderWindow& window)
{
    initialize();
    sound.lvl1Sound.stop();
    sound.lvl2Sound.play();
    window.setFramerateLimit(60); //set it according to your monitor's refresh rate
    window.setVerticalSyncEnabled(true); //to avoid screen tearing
    //game clock
    Clock clock;


    //background
    Sprite back;
    Texture backText;
    backText.loadFromFile("./textures/maps/maplvl2.jpg");
    back.setTexture(backText);
    back.setPosition(0, 0);



    //pirate.shape.setTexture(pirate.textures[0]);

    pirate.shape.setTextureRect(IntRect(0, 0, 58, 58));
    pirate.shape.setPosition(Vector2f(150.f, 330.f));
    pirate.shape.setOrigin(pirate.shape.getTextureRect().width / 2, pirate.shape.getTextureRect().height / 2);

    //Cannon
    cannon.shape.setTexture(cannon.textures[0]);
    cannon.shape.setScale(2.f, 2.f);
    cannon.shape.setPosition(1170, 495);
    cannon.shape.setOrigin(cannon.shape.getTextureRect().width / 2, cannon.shape.getTextureRect().height / 2);
    //CannonBalls init
    for (int i = 0; i < 3; i++)
    {
        cannonBall[i].shape.setTexture(cannonBall[i].cannonBallTex);
        cannonBall[i].shape.setPosition(cannon.shape.getPosition().x, cannon.shape.getPosition().y - 10);
        cannonBall[i].shape.setOrigin(cannonBall[i].shape.getTextureRect().width / 2.f, cannonBall[i].shape.getTextureRect().height / 2.f);
        cannonBall[i].range.setPosition(cannonBall[i].shape.getPosition());
    }
    Texture explosionTex;
    explosionTex.loadFromFile("./textures/cannon/explosion.png");
    Sprite explosion;
    explosion.setTexture(explosionTex);
    explosion.setOrigin(explosion.getTextureRect().width / 2.f, explosion.getTextureRect().height / 2.f);
    explosion.setPosition(1920, 450);
    explosion.setTextureRect(IntRect(0, 0, 96, 108));
    explosion.setScale(2, 2);

    //cucumber init
    cucumber.shape.setTexture(cucumber.text[0]);
    cucumber.shape.setScale(2.f, 2.f);
    cucumber.shape.setTextureRect(IntRect(0, 0, 63, 67));
    cucumber.shape.setPosition(350.f, 960.f);
    cucumber.shape.setOrigin(cucumber.shape.getTextureRect().width / 2, cucumber.shape.getTextureRect().height / 2);

    //Big guy

    bigGuyLvl2.shape.setTexture(bigGuyLvl2.text[0]);
    bigGuyLvl2.shape.setScale(2.f, 2.f); // 2xL*W
    bigGuyLvl2.shape.setTextureRect(IntRect(0, 0, 77, 74));
    bigGuyLvl2.shape.setPosition(1300, 955.f);
    bigGuyLvl2.shape.setOrigin(bigGuyLvl2.shape.getTextureRect().width / 2, bigGuyLvl2.shape.getTextureRect().height / 2);

    //Bomb init

    bomb.shape.setTexture(bomb.bombTex);
    bomb.shape.setTextureRect(IntRect(0, 0, 96, 108));
    bomb.shape.setPosition(pirate.shape.getPosition().x, pirate.shape.getPosition().y - 10);
    bomb.shape.setOrigin(bomb.shape.getTextureRect().width / 2.f, bomb.shape.getTextureRect().height / 2.f);
    bomb.range.setPosition(bomb.shape.getPosition());

    //Door init
    Door door;
    door.shape.setTexture(door.door_tex[1]); //idle
    door.shape.setTextureRect(IntRect(door.frame * 78, 0, 78, 98));
    door.shape.setScale(2.f, 2.f);
    door.shape.setPosition(96.f, 420.f);
    bool door_opened = false;
    double doorDelay = 1.f;

    Door door2;
    door2.shape.setTexture(door.door_tex[1]); //idle
    door2.shape.setTextureRect(IntRect(0, 0, 78, 98));
    door2.shape.setScale(1.75f, 1.75f);
    door2.shape.setPosition(1710.f, 32.f);//83.f
    //Door Bar
    RectangleShape doorBar(Vector2f(10, 150));
    doorBar.setPosition(door2.shape.getPosition().x + 40, door2.shape.getPosition().y + 50);
    doorBar.setFillColor(Color::Transparent);
    float doorTimer = 0, doorTimer2 = 0;


    RectangleShape platforms[10];

    {

        platforms[0].setSize(Vector2f(245, 14));
        platforms[0].setFillColor(Color::Transparent);
        platforms[0].setPosition(710, 705);

        platforms[1].setSize(Vector2f(125, 14));
        platforms[1].setFillColor(Color::Transparent);
        platforms[1].setPosition(990, 540);

        platforms[2].setSize(Vector2f(245, 14));
        platforms[2].setFillColor(Color::Transparent);
        platforms[2].setPosition(1650, 850);

        platforms[3].setSize(Vector2f(245, 14));
        platforms[3].setFillColor(Color::Transparent);
        platforms[3].setPosition(1450, 685);

        platforms[4].setSize(Vector2f(240, 14));
        platforms[4].setFillColor(Color::Transparent);
        platforms[4].setPosition(1654, 518);

        platforms[5].setSize(Vector2f(242, 14));
        platforms[5].setFillColor(Color::Transparent);
        platforms[5].setPosition(1453, 353);

        platforms[6].setSize(Vector2f(245, 14));
        platforms[6].setFillColor(Color::Transparent);
        platforms[6].setPosition(1653, 198);
    }

    RectangleShape objects[11];
    {
        objects[0].setSize(Vector2f(710, 108));
        objects[0].setFillColor(Color::Transparent);
        objects[0].setPosition(0, 610);

        objects[1].setSize(Vector2f(328, 199.5));
        objects[1].setFillColor(Color::Transparent);
        objects[1].setPosition(1120, 540);

        objects[2].setSize(Vector2f(990, 52));
        objects[2].setFillColor(Color::Transparent);
        objects[2].setPosition(0, 1030);

        objects[3].setSize(Vector2f(670, 52));
        objects[3].setFillColor(Color::Transparent);
        objects[3].setPosition(1252, 1032);

        objects[4].setSize(Vector2f(30, 1220));
        objects[4].setFillColor(Color::Transparent);
        objects[4].setPosition(1890, 0);

        objects[5].setSize(Vector2f(450, 30));
        objects[5].setFillColor(Color::Transparent);
        objects[5].setPosition(1440, 0);

        objects[6].setSize(Vector2f(275, 350));
        objects[6].setFillColor(Color::Transparent);
        objects[6].setPosition(1172, 0);

        objects[7].setSize(Vector2f(108, 192));
        objects[7].setFillColor(Color::Transparent);
        objects[7].setPosition(1342, 352);

        objects[8].setSize(Vector2f(1175, 140));
        objects[8].setFillColor(Color::Transparent);
        objects[8].setPosition(0, 0);

        objects[9].setSize(Vector2f(30, 1000));
        objects[9].setFillColor(Color::Transparent);
        objects[9].setPosition(0, 140);

        objects[10].setSize(Vector2f(250, 325));
        objects[10].setFillColor(Color::Transparent);
        objects[10].setPosition(0, 715);


    }

    wadi[0].x = -9.f * cos(initAngle);
    wadi[0].y = -9.f * sin(initAngle);

    wadi[1].x = -11.5f * cos(initAngle);
    wadi[1].y = -11.5f * sin(initAngle);

    wadi[2].x = -14.f * cos(initAngle * 1.4);
    wadi[2].y = -14.f * sin(initAngle * 1.4);


    //============================================================================================================

    //HITBOX
    //Pirate
    pirate.hitbox.setPosition(pirate.shape.getPosition().x, pirate.shape.getPosition().y + 10);
    //Big guy
    bigGuyLvl2.hitbox.setPosition(bigGuyLvl2.shape.getPosition().x, bigGuyLvl2.shape.getPosition().y + 10);
    bigGuyLvl2.RunToBombRange.setPosition(bigGuyLvl2.shape.getPosition().x, bigGuyLvl2.shape.getPosition().y);
    bigGuyLvl2.range.setPosition(bigGuyLvl2.shape.getPosition().x, bigGuyLvl2.shape.getPosition().y);

    //Health
    Texture healthBar_text, heart_text;
    {
        healthBar_text.loadFromFile("./textures/Health/Health Bar.png");
        heart_text.loadFromFile("./textures/Health/Heart.png");
    }
    Sprite healthBar, heart[3];
    {

        healthBar.setTexture(healthBar_text);
        healthBar.setScale(1.8f, 1.8f);
        healthBar.setPosition(0, 0);

        heart[0].setTexture(heart_text);
        heart[1].setTexture(heart_text);
        heart[2].setTexture(heart_text);

        heart[0].setScale(1.8f, 1.8f);
        heart[1].setScale(1.8f, 1.8f);
        heart[2].setScale(1.8f, 1.8f);

        heart[0].setPosition(70, 40);
        heart[1].setPosition(120, 40);
        heart[2].setPosition(165, 40);
    }


    Texture bombnum_tex[16];
    Sprite bombnum;
    int bombcounter = 16;

    {
        bombnum_tex[0].loadFromFile("./textures/bomb/BombCounter/0.png");
        bombnum_tex[1].loadFromFile("./textures/bomb/BombCounter/1.png");
        bombnum_tex[2].loadFromFile("./textures/bomb/BombCounter/2.png");
        bombnum_tex[3].loadFromFile("./textures/bomb/BombCounter/3.png");
        bombnum_tex[4].loadFromFile("./textures/bomb/BombCounter/4.png");
        bombnum_tex[5].loadFromFile("./textures/bomb/BombCounter/5.png");
        bombnum_tex[6].loadFromFile("./textures/bomb/BombCounter/6.png");
        bombnum_tex[7].loadFromFile("./textures/bomb/BombCounter/7.png");
        bombnum_tex[8].loadFromFile("./textures/bomb/BombCounter/8.png");
        bombnum_tex[9].loadFromFile("./textures/bomb/BombCounter/9.png");
        bombnum_tex[10].loadFromFile("./textures/bomb/BombCounter/10.png");
        bombnum_tex[11].loadFromFile("./textures/bomb/BombCounter/11.png");
        bombnum_tex[12].loadFromFile("./textures/bomb/BombCounter/12.png");
        bombnum_tex[13].loadFromFile("./textures/bomb/BombCounter/13.png");
        bombnum_tex[14].loadFromFile("./textures/bomb/BombCounter/14.png");
        bombnum_tex[15].loadFromFile("./textures/bomb/BombCounter/15.png");
    }

    bombnum.setTexture(bombnum_tex[15]);
    bombnum.setScale(0.13, 0.13);
    bombnum.setPosition(1777, 100);

    //UI
    Texture GameOver_Txt[4];
    GameOver_Txt[0].loadFromFile("./UI/gameover.png");
    GameOver_Txt[1].loadFromFile("./UI/background.png");
    GameOver_Txt[2].loadFromFile("./UI/buttons.png");
    GameOver_Txt[3].loadFromFile("./UI/noBombsLeft.png");

    // 0 ->>>> GameOver
    deathUI[0].setTexture(GameOver_Txt[0]);
    deathUI[0].setPosition(window.getSize().x / 2 - 50, window.getSize().y / 2);
    deathUI[0].setOrigin(deathUI[0].getTextureRect().width / 2, deathUI[0].getTextureRect().height / 2);
    deathUI[0].setScale(0.75f, 0.75f);
    // 1 ->>>> bg
    deathUI[1].setTexture(GameOver_Txt[1]);
    deathUI[1].setPosition(window.getSize().x / 2 - 50, window.getSize().y / 2);
    deathUI[1].setOrigin(deathUI[1].getTextureRect().width / 2, deathUI[1].getTextureRect().height / 2);
    deathUI[1].setScale(0.75f, 0.75f);

    // 2 ->>>>>buttons
    deathUI[2].setTexture(GameOver_Txt[2]);
    deathUI[2].setPosition(window.getSize().x / 2 - 50, window.getSize().y / 1.95);
    deathUI[2].setOrigin(deathUI[2].getTextureRect().width / 2, deathUI[2].getTextureRect().height / 2);
    deathUI[2].setScale(1.f, 1.f);

    // 3 -->>>>>>> No bombs left
    deathUI[3].setTexture(GameOver_Txt[3]);
    deathUI[3].setPosition(window.getSize().x / 2 - 50, window.getSize().y / 2);
    deathUI[3].setOrigin(deathUI[1].getTextureRect().width / 2, deathUI[1].getTextureRect().height / 2);
    deathUI[3].setScale(0.75f, 0.75f);

    Texture bombcntrText;
    bombcntrText.loadFromFile("./textures/bomb_overlay.png");
    Sprite bombcntr;
    bombcntr.setTexture(bombcntrText);

    bool flag = true; // for cannon explosion

    while (window.isOpen())
    {
        Event event;
        dt = clock.restart().asSeconds();
        if (Keyboard::isKeyPressed(Keyboard::M))
        {
            phase++;
            return;
        }

        if (!gameEnded)
        {
            while (window.pollEvent(event))
            {
                if (event.type == Event::Closed || event.type == Event::KeyPressed && event.key.code == Keyboard::Escape)
                {
                    sound.press.play();
                    phase = -1;
                    window.close();
                }
            }

            //Pirate Hitbox
            if (pirate.shape.getScale().x > 0)
                pirate.hitbox.setPosition(pirate.shape.getPosition().x + 10, pirate.shape.getPosition().y + 10);
            else pirate.hitbox.setPosition(pirate.shape.getPosition().x - 10, pirate.shape.getPosition().y + 10);

            //cucumber hitbox
            HitBoxUpdate(cucumber.shape, cucumber.hitbox, 10);
            HitBoxUpdate(cucumber.shape, cucumber.range, 45);
            HitBoxUpdate(cucumber.shape, cucumber.RunToBombRange, 15);

            // bomb hibox
            HitBoxUpdate(bomb.shape, bomb.range, 0);
            HitBoxUpdate(bomb.shape, bomb.hitbox, 30);

            //Big guy Hitbox
            HitBoxUpdate(bigGuyLvl2.shape, bigGuyLvl2.hitbox, 10);
            HitBoxUpdate(bigGuyLvl2.shape, bigGuyLvl2.RunToBombRange, 45);
            HitBoxUpdate(bigGuyLvl2.shape, bigGuyLvl2.range, 45);

            if (!door_opened) {
                if (door.frame == 0) sound.doorsfx.play();
                pirate.shape.setScale(0, 0);
                if (door.frame < door.mxframe) {
                    doorTimer += dt;
                    //cout << doorTimer << endl;
                    if (doorTimer > 0.8f) {

                        doorTimer = 0;
                        door.frame++;
                        door.shape.setTextureRect(IntRect(door.frame * 78, 0, 78, 98));
                    }
                }
                else {
                    door_opened = true;
                    pirate.shape.setScale(2.f, 2.f);
                }
            }

            if (door_opened) {
                if (pirate.player_in_Frame <= pirate.player_in_mxFrame)
                {
                    pirate.shape.setTexture(pirate.textures[4]);

                    pirate.shape.setTextureRect(IntRect(pirate.player_in_Frame * 58, 0, 58, 58));

                    if (pirate.player_init_timer <= 0)
                    {
                        pirate.player_in_Frame++;
                        pirate.player_in_Frame %= 11; // 0 -> 10
                        pirate.shape.setTextureRect(IntRect(pirate.player_in_Frame * 58, 0, 58, 58));
                        pirate.player_init_timer = 0.06;
                    }
                    else pirate.player_init_timer -= dt;
                }

                else if (pirate.player_in_Frame >= pirate.player_in_mxFrame)
                    player_init = true;
            }

            if (player_init) {
                //pirate.shape.setScale(2.f, 2.f);
                if (door.frame > 0) {
                    doorTimer += dt;
                    if (doorTimer > 0.6f) {
                        doorTimer = 0;
                        door.frame--;
                        door.shape.setTextureRect(IntRect(door.frame * 78, 0, 78, 98));
                    }
                }
            }




            //apply gravity
            pirate.velocity.y += g * dt;

            //collision with objects for player (width is height :) )
            for (int i = 0; i < 11; i++)
            {
                //ground
                if (pirate.hitbox.getPosition().y + pirate.hitbox.getGlobalBounds().width / 2.f < objects[i].getPosition().y)//check if player is on top of the object
                {
                    if (CollisionDetection(pirate.hitbox, objects[i]))
                    {
                        pirate.velocity.y = 0;
                        pirate.shape.setPosition(pirate.shape.getPosition().x, objects[i].getPosition().y - pirate.hitbox.getGlobalBounds().height * 0.5f - 5);

                        if (pirate.isJumping)
                        {
                            sound.groundhit.play();
                            pirate.isJumping = false;
                            //cout << "yess" << endl;
                            pirate.shape.setTexture(pirate.textures[0]);
                        }
                        // cout << "foo2\n";
                    }
                }
                //top
                else if (pirate.hitbox.getPosition().y - pirate.hitbox.getGlobalBounds().width / 2.f > objects[i].getPosition().y + objects[i].getGlobalBounds().height)
                {
                    if (CollisionDetection(pirate.hitbox, objects[i]) and pirate.velocity.y <= 0)
                    {
                        pirate.velocity.y *= -80.f * dt;
                    }
                }
            }
            //Collision
            {
                for (int i = 0; i < 11; ++i)
                {
                    //right
                    if (pirate.hitbox.getPosition().y + pirate.hitbox.getGlobalBounds().height / 2.f > objects[i].getPosition().y
                        and pirate.hitbox.getPosition().x - pirate.hitbox.getGlobalBounds().width / 2.f < objects[i].getPosition().x + objects[i].getGlobalBounds().width
                        and !(pirate.hitbox.getPosition().y - pirate.hitbox.getGlobalBounds().width / 2.f > objects[i].getPosition().y + objects[i].getGlobalBounds().height)
                        and !(pirate.hitbox.getPosition().y + pirate.hitbox.getGlobalBounds().width / 2.f < objects[i].getPosition().y)
                        and pirate.shape.getScale().x < 0
                        )
                    {
                        if (CollisionDetection(pirate.hitbox, objects[i]))
                        {
                            //cout << "collide\n";
                            //right
                            pirate.velocity.x = 0.f;
                            pirate.shape.setPosition(objects[i].getPosition().x + objects[i].getGlobalBounds().width + 35, pirate.shape.getPosition().y);
                        }
                    }
                    //left
                    else if (pirate.hitbox.getPosition().y + pirate.hitbox.getGlobalBounds().height / 2.f > objects[i].getPosition().y
                        and pirate.hitbox.getPosition().x + pirate.hitbox.getGlobalBounds().width / 2.f > objects[i].getPosition().x
                        and !(pirate.hitbox.getPosition().y - pirate.hitbox.getGlobalBounds().width / 2.f > objects[i].getPosition().y + objects[i].getGlobalBounds().height)
                        and !(pirate.hitbox.getPosition().y + pirate.hitbox.getGlobalBounds().width / 2.f < objects[i].getPosition().y)
                        and pirate.shape.getScale().x > 0
                        )
                    {
                        if (CollisionDetection(pirate.hitbox, objects[i]))
                        {
                            //cout << "collide\n";
                            //right
                            pirate.velocity.x = 0.f;
                            pirate.shape.setPosition(objects[i].getPosition().x - 35, pirate.shape.getPosition().y);
                        }
                    }
                }
            }

            //collision with platforms for player
            for (int i = 0; i < 7; i++)
            {
                if (pirate.shape.getPosition().y + pirate.shape.getGlobalBounds().height / 4.f + 20 < platforms[i].getPosition().y) // check on top ola l2
                {
                    if (pirate.hitbox.getGlobalBounds().intersects(platforms[i].getGlobalBounds())) {
                        if (pirate.isJumping)
                        {
                            sound.groundhit.play();
                            pirate.isJumping = false;
                            pirate.shape.setTexture(pirate.textures[0]);
                        }
                        pirate.velocity.y = 0;
                    }

                }
            }


            //CannonBall damage
            for (int i = 0; i < 3; i++)
            {
                if (CollisionDetection(pirate.hitbox, cannonBall[i].hitbox) and cannonBall[i].isGrounded)
                {
                    (pirate.hitbox.getPosition().x > cannonBall[i].shape.getPosition().x) ? pirate.velocity.x = 5 : pirate.velocity.x = -5;
                    if (pirate.hitbox.getPosition().y + pirate.hitbox.getGlobalBounds().width / 2.f + 5 < cannonBall[i].shape.getPosition().y)
                        pirate.velocity.y = -7;
                    sound.hitSFX.play();
                    pirate.isHit = true;
                    playerHearts--;

                }
            }

            //  cout << "x: " << Mouse::getPosition().x << "Y: " << Mouse::getPosition().y << endl;

            Pirate_move_code();
            big_guy_code(bigGuyLvl2, 1255, 1850);
            cucumber_code(cucumber, 256, 976);
            if (!vis[2])cannonCode(cannon, objects[0], explosion, explosionTex);
            else if (flag)
            {
                animate(explosion, explosionTex, cannonExpoAniIdx, 96, 108, cannonExpoTimer, 9, 1, 2.f);
                if (cannonExpoAniIdx == 5)
                    sound.cannonGone.play();
                if (cannonExpoAniIdx == 8)
                    flag = false;
            }
            RepeatedKeyHandle(event, isPressed);

            if (pirate.isHit)
            {
                if (pirate.isHit and playerHearts > 0)
                {
                    animate(pirate.shape, pirate.textures[8], captainAnimIdx, 58, 58, pirateTimer, 8, (pirate.shape.getScale().x > 0.f) ? 1 : -1, 2.f);
                    if (captainAnimIdx == 7)
                        pirate.isHit = false;
                }
                else if (playerHearts <= 0)
                {
                    dead_animation(pirate.shape, pirate.textures[9], pirateAnimIdx, 58, 58, pirateTimer, 6, (pirate.shape.getScale().x > 0.f) ? 1 : -1, 2.f, 5);
                    if (pirateAnimIdx == 5)
                        pirateAnimIdx++;
                    //cout << pirateAnimIdx << '\n';
                    pirate.velocity = { 0,0 };
                }

            }

            // Bomb shoot 
            if (shootTimer < 12.5)
                shootTimer++;
            if (player_init and playerHearts > 0)
            {
                if (Mouse::isButtonPressed(Mouse::Left) and shootTimer >= 12.5 and bomb.bombs.size() < 5) {
                    //sound.bombsound.play();

                    bombcounter--;
                    bombnum.setTexture(bombnum_tex[bombcounter - 1]);

                    bomb.shape.setPosition(pirate.shape.getPosition().x, pirate.shape.getPosition().y - 35.f);
                    bomb.velocity.x = (pirate.shape.getScale().x > 0.f) ? 10.f * cos(initAngle) : -10.f * cos(initAngle);
                    bomb.velocity.y = -10.f * sin(initAngle);
                    bomb.isShot = true;
                    bombFired++;


                    if (bombFired <= bombsNum)bomb.bombs.pb(bomb); // BOMB fire limiter
                    shootTimer = 0; //RESET timer
                }
            }


            //bomb animation
            if (bombAnimTime <= 0 and !bomb.bombs.empty())
            {
                bombAnimIdx++;
                bombAnimIdx %= 10;
                for (int i = 0; i < bomb.bombs.size(); i++)
                {
                    bomb.bombs[i].shape.setTextureRect(IntRect(bombAnimIdx * 96, 0, 96, 108));

                }
                bombAnimTime = bombDelay;
            }
            else bombAnimTime -= dt;



            //move bombs
            if (!bomb.bombs.empty())
            {
                for (int i = 0; i < bomb.bombs.size(); ++i)
                {
                    bomb.bombs[i].velocity.y += g * dt; //apply gravity

                    bomb.bombs[i].shape.move(bomb.bombs[i].velocity);
                    HitBoxUpdate(bomb.bombs[i].shape, bomb.bombs[i].hitbox, 30);
                    HitBoxUpdate(bomb.bombs[i].shape, bomb.bombs[i].range, 30);

                    //collision between bombs
                    for (int j = 0; j < bomb.bombs.size(); ++j)
                    {
                        if (i == j)continue;
                        if (bomb.bombs[i].hitbox.getGlobalBounds().intersects(bomb.bombs[j].hitbox.getGlobalBounds()))
                        {
                            if (bomb.bombs[i].hitbox.getPosition().x < bomb.bombs[j].hitbox.getPosition().x)
                                bomb.bombs[j].velocity.x += 2.5f;
                            else bomb.bombs[j].velocity.x -= 2.5f;
                        }
                    }
                    //collision between player and bombs
                    if (bomb.bombs[i].hitbox.getGlobalBounds().intersects(pirate.hitbox.getGlobalBounds()) and bomb.bombs[i].isGrounded)
                        bomb.bombs[i].velocity.x += (pirate.hitbox.getPosition().x > bomb.bombs[i].hitbox.getPosition().x) ? -0.8f : 0.8f;
                }
            }

            //collision of bombs with platforms
            if (!bomb.bombs.empty())
            {
                for (int i = 0; i < bomb.bombs.size(); ++i)
                {
                    for (int j = 0; j < 11; ++j)//ALL bottoms
                    {
                        if (bomb.bombs[i].hitbox.getPosition().y + bomb.bombs[i].hitbox.getGlobalBounds().height * 0.25f - 30 < platforms[j].getPosition().y
                            and !(bomb.bombs[i].hitbox.getPosition().x - bomb.bombs[i].hitbox.getGlobalBounds().width * 0.25f > platforms[j].getPosition().x + platforms[j].getGlobalBounds().width)//left
                            and !(bomb.bombs[i].hitbox.getPosition().x + bomb.bombs[i].hitbox.getGlobalBounds().width * 0.25f < platforms[j].getPosition().x)//right
                            )
                        {

                            if (bomb.bombs[i].hitbox.getGlobalBounds().intersects(platforms[j].getGlobalBounds())) {

                                bomb.bombs[i].isGrounded = 1;
                                bomb.bombs[i].velocity.y = 0.f;
                                bomb.bombs[i].shape.setPosition(bomb.bombs[i].shape.getPosition().x, platforms[j].getPosition().y - 60);
                                if (bomb.bombs[i].velocity.x > 0)
                                {
                                    bomb.bombs[i].velocity.x -= resistanceForce * dt;
                                    if (bomb.bombs[i].velocity.x <= 0)bomb.bombs[i].velocity.x = 0.f;
                                }

                                else if (bomb.bombs[i].velocity.x < 0)

                                {
                                    bomb.bombs[i].velocity.x += resistanceForce * dt;
                                    if (bomb.bombs[i].velocity.x >= 0)bomb.bombs[i].velocity.x = 0.f;
                                }

                                // cout << "Ground\n";
                            }
                        }
                    }
                }
            }

            //=========================================================================================================================




            // collision with objects for bomb (up and down)
            if (!bomb.bombs.empty())
            {
                for (int i = 0; i < bomb.bombs.size(); ++i)
                {
                    for (int j = 0; j < 11; ++j)//3 tops
                    {
                        //top collision
                        if (bomb.bombs[i].hitbox.getPosition().y - bomb.bombs[i].hitbox.getGlobalBounds().height * 0.25f - 30 < objects[j].getPosition().y + objects[j].getGlobalBounds().height
                            and !(bomb.bombs[i].hitbox.getPosition().x - bomb.bombs[i].hitbox.getGlobalBounds().width * 0.25f > objects[j].getPosition().x + objects[j].getGlobalBounds().width)//left
                            and !(bomb.bombs[i].hitbox.getPosition().x + bomb.bombs[i].hitbox.getGlobalBounds().width * 0.25f < objects[j].getPosition().x)//right
                            )
                        {
                            if (bomb.bombs[i].hitbox.getGlobalBounds().intersects(objects[j].getGlobalBounds()))
                                if (bomb.bombs[i].velocity.y < 0)bomb.bombs[i].velocity.y *= -0.8f;
                        }
                    }
                    for (int j = 0; j < 9; ++j)//2 bottoms
                    {
                        if (bomb.bombs[i].hitbox.getPosition().y + bomb.bombs[i].hitbox.getGlobalBounds().height * 0.25f - 30 < objects[j].getPosition().y
                            and !(bomb.bombs[i].hitbox.getPosition().x - bomb.bombs[i].hitbox.getGlobalBounds().width * 0.25f > objects[j].getPosition().x + objects[j].getGlobalBounds().width)//left
                            and !(bomb.bombs[i].hitbox.getPosition().x + bomb.bombs[i].hitbox.getGlobalBounds().width * 0.25f < objects[j].getPosition().x)//right
                            )
                        {
                            if (bomb.bombs[i].hitbox.getGlobalBounds().intersects(objects[j].getGlobalBounds())) {
                                bomb.bombs[i].isGrounded = 1;
                                bomb.bombs[i].velocity.y = 0.f;
                                bomb.bombs[i].shape.setPosition(bomb.bombs[i].shape.getPosition().x, objects[j].getPosition().y - 60);
                                if (bomb.bombs[i].velocity.x > 0)
                                {
                                    bomb.bombs[i].velocity.x -= resistanceForce * dt;
                                    if (bomb.bombs[i].velocity.x <= 0)bomb.bombs[i].velocity.x = 0.f;
                                }

                                else if (bomb.bombs[i].velocity.x < 0)

                                {
                                    bomb.bombs[i].velocity.x += resistanceForce * dt;
                                    if (bomb.bombs[i].velocity.x >= 0)bomb.bombs[i].velocity.x = 0.f;
                                }
                                //  cout << "Ground\n";
                            }
                        }
                    }
                }
            }


            //collision with objects for bomb (left and right)
            if (!bomb.bombs.empty())
            {
                for (int i = 0; i < bomb.bombs.size(); ++i)
                {
                    for (int j = 0; j < 11; j++)
                    {
                        //left direction
                        if (bomb.bombs[i].hitbox.getPosition().x - bomb.bombs[i].hitbox.getGlobalBounds().width * 0.25f > objects[j].getPosition().x + objects[j].getGlobalBounds().width
                            and !(bomb.bombs[i].hitbox.getPosition().y + bomb.bombs[i].hitbox.getGlobalBounds().height * 0.25f - 30 < objects[j].getPosition().y)
                            and !(bomb.bombs[i].hitbox.getPosition().y - bomb.bombs[i].hitbox.getGlobalBounds().height * 0.25f + 30 > objects[j].getPosition().y + objects[j].getGlobalBounds().height)
                            )
                        {
                            if (CollisionDetection(bomb.bombs[i].hitbox, objects[j]))
                            {
                                if (bomb.bombs[i].velocity.x < 0)bomb.bombs[i].velocity.x *= -0.8f;
                                //cout << "Left\n";
                            }
                        }
                        //right direction
                        if (bomb.bombs[i].hitbox.getPosition().x + bomb.bombs[i].hitbox.getGlobalBounds().width * 0.25f < objects[j].getPosition().x
                            and !(bomb.bombs[i].hitbox.getPosition().y + bomb.bombs[i].hitbox.getGlobalBounds().height * 0.25 - 30 < objects[j].getPosition().y)
                            and !(bomb.bombs[i].hitbox.getPosition().y - bomb.bombs[i].hitbox.getGlobalBounds().height * 0.25f + 30 > objects[j].getPosition().y + objects[j].getGlobalBounds().height)
                            )
                        {
                            if (CollisionDetection(bomb.bombs[i].hitbox, objects[j]))
                            {
                                if (bomb.bombs[i].velocity.x > 0)bomb.bombs[i].velocity.x *= -0.8f;
                                // cout << bomb.bombs[i].velocity.x << "\n";
                                 //cout << "Right\n";
                            }
                        }
                    }
                }
            }

            //detonation
            for (int i = 0; i < bomb.bombs.size(); ++i)
            {
                if (bomb.bombs[i].isShot)
                {

                    //big guy stuff
                    if (bigGuyLvl2.isAlive)
                        if (bigGuyLvl2.RunToBombRange.getGlobalBounds().intersects(bomb.bombs[i].hitbox.getGlobalBounds()))
                        {
                            if (bigGuyLvl2.hitbox.getPosition().x < bomb.bombs[i].hitbox.getPosition().x)// Bomb is on the right side
                            {
                                animate(bigGuyLvl2.shape, bigGuyLvl2.text[1], bigGuyAnimIdx, 77, 74, bigGuyTimer, 16, 1, 2.f);
                                bigGuyLvl2.velocity.x = 90.f * dt;
                                if (bigGuyLvl2.hitbox.getGlobalBounds().intersects(bomb.bombs[i].hitbox.getGlobalBounds()))
                                {
                                    bigGuyLvl2.velocity.x = 0;
                                    if (bigGuyLvl2.shape.getTexture() != &bigGuyLvl2.text[3])
                                    {

                                        bigGuyLvl2.shape.setTexture(bigGuyLvl2.text[3]);
                                        //l = 0;
                                        bigGuyLvl2.shape.setTextureRect(IntRect(bigGuyAttackIdx * 77, 0, 77, 74));
                                    }
                                    if (bigGuyTimer <= 0 and !bigGuyLvl2.pick_up and !bigGuyLvl2.bomb_throw)
                                    {
                                        bigGuyAttackIdx++;
                                        bigGuyAttackIdx %= 8;
                                        if (bigGuyAttackIdx == 5)
                                        {
                                            bomb.bombs[i].shape.setPosition(bigGuyLvl2.hitbox.getPosition());
                                            bigGuyLvl2.pick_up = false;
                                            bigGuyLvl2.bomb_throw = true;
                                        }
                                        //cout << l << " ";
                                        bigGuyLvl2.shape.setTextureRect(IntRect(bigGuyAttackIdx * 77, 0, 77, 74));
                                        bigGuyTimer = delay * 3;
                                    }
                                    else
                                        bigGuyTimer -= dt;

                                }
                                else bigGuyAttackIdx = 0;
                            }
                            else if (bigGuyLvl2.hitbox.getPosition().x > bomb.bombs[i].hitbox.getPosition().x)// Bomb is on the left side
                            {
                                animate(bigGuyLvl2.shape, bigGuyLvl2.text[1], bigGuyAnimIdx, 77, 74, bigGuyTimer, 16, -1, 2.f);
                                bigGuyLvl2.velocity.x = -90.f * dt;
                                if (bigGuyLvl2.hitbox.getGlobalBounds().intersects(bomb.bombs[i].hitbox.getGlobalBounds()))
                                {
                                    bigGuyLvl2.velocity.x = 0;
                                    if (bigGuyLvl2.shape.getTexture() != &bigGuyLvl2.text[3])
                                    {
                                        bigGuyLvl2.shape.setTexture(bigGuyLvl2.text[3]);
                                        //l = 0;
                                        bigGuyLvl2.shape.setTextureRect(IntRect(bigGuyAttackIdx * 77, 0, 77, 74));
                                    }
                                    if (bigGuyTimer <= 0 and !bigGuyLvl2.pick_up and !bigGuyLvl2.bomb_throw)
                                    {
                                        bigGuyAttackIdx++;
                                        bigGuyAttackIdx %= 8;
                                        //cout << l << '\n';
                                        if (bigGuyAttackIdx == 5)
                                        {
                                            bomb.bombs[i].shape.setPosition(bigGuyLvl2.hitbox.getPosition());
                                            bigGuyLvl2.pick_up = false;
                                            bigGuyLvl2.bomb_throw = true;
                                        }
                                        //cout << l << " ";
                                        bigGuyLvl2.shape.setTextureRect(IntRect(bigGuyAttackIdx * 77, 0, 77, 74));
                                        bigGuyTimer = delay * 3;
                                    }
                                    else
                                        bigGuyTimer -= dt;
                                }
                                else bigGuyAttackIdx = 0;
                            }
                            if (!bigGuyLvl2.pick_up and bigGuyLvl2.bomb_throw and bigGuyLvl2.hitbox.getGlobalBounds().intersects(bomb.bombs[i].hitbox.getGlobalBounds()))
                            {
                                if (bigGuyLvl2.shape.getTexture() != &bigGuyLvl2.text[4])
                                {
                                    if (!oneTime_bigGuy_anim)
                                    {
                                        bigGuyAttackIdx = 0;
                                        oneTime_bigGuy_anim = !oneTime_bigGuy_anim;
                                    }
                                    bigGuyLvl2.shape.setTexture(bigGuyLvl2.text[4]);
                                    bigGuyLvl2.shape.setTextureRect(IntRect(bigGuyAttackIdx * 77, 0, 77, 74));
                                }
                                if (throwAnim <= 0 and bigGuyLvl2.bomb_throw)
                                {
                                    bigGuyAttackIdx++;
                                    bigGuyAttackIdx %= 11;
                                    if (bigGuyAttackIdx == 4)
                                    {
                                        bomb.bombs[i].shape.setPosition(bigGuyLvl2.hitbox.getPosition());
                                        bomb.bombs[i].velocity.y = -10 * sin(initAngle);
                                        if (pirate.shape.getPosition().x > bigGuyLvl2.shape.getPosition().x)
                                            bigGuyLvl2.shape.setScale(2.f, 2.f);
                                        else if (pirate.shape.getPosition().x < bigGuyLvl2.shape.getPosition().x)
                                            bigGuyLvl2.shape.setScale(-2.f, 2.f);
                                        bomb.bombs[i].velocity.x = (bigGuyLvl2.shape.getScale().x > 0) ? 10 * cos(initAngle) : -10 * cos(initAngle);

                                    }
                                    if (bigGuyAttackIdx == 10)
                                    {
                                        bigGuyLvl2.bomb_throw = false;
                                    }
                                    bigGuyLvl2.shape.setTextureRect(IntRect(bigGuyAttackIdx * 77, 0, 77, 74));
                                    throwAnim = delay * 3;
                                }
                                else throwAnim -= dt;
                            }

                        }

                    //cucumber stuff
                    if (cucumber.isAlive)
                        if (cucumber.RunToBombRange.getGlobalBounds().intersects(bomb.bombs[i].hitbox.getGlobalBounds()))
                        {
                            if (cucumber.hitbox.getPosition().x < bomb.bombs[i].hitbox.getPosition().x)// Bomb is on the right side
                            {
                                animate(cucumber.shape, cucumber.text[1], cucumberAnimIdx, 64, 68, cucumberTimer, 12, -1, 2.f);
                                cucumber.velocity.x = 90.f * dt;
                                if (cucumber.hitbox.getGlobalBounds().intersects(bomb.bombs[i].hitbox.getGlobalBounds()) and bomb.bombs[i].hitbox.getPosition().y + bomb.bombs[i].hitbox.getGlobalBounds().height / 2 > cucumber.hitbox.getPosition().y)
                                {
                                    cucumber.velocity.x = 0;
                                    bomb.bombs[i].velocity.x = 0.f;
                                    if (cucumber.shape.getTexture() != &cucumber.text[3])
                                    {

                                        cucumber.shape.setTexture(cucumber.text[3]);
                                        //l = 0;
                                        cucumber.shape.setTextureRect(IntRect(cucumber.attackIdx * 64, 0, 64, 68));
                                    }
                                    if (cucumberTimer <= 0 and !cucumber.isBlowing)
                                    {
                                        cucumber.attackIdx++;
                                        cucumber.attackIdx %= 11;
                                        if (cucumber.attackIdx == 6)
                                        {
                                            sound.blow.play();
                                        }
                                        if (cucumber.attackIdx == 9)
                                        {
                                            bomb.bombs.erase(bomb.bombs.begin() + i);
                                            cucumber.isBlowing = false;
                                            break;
                                        }
                                        //cout << l << " ";
                                        cucumber.shape.setTextureRect(IntRect(cucumber.attackIdx * 64, 0, 64, 68));
                                        cucumberTimer = delay * 3;
                                    }
                                    else
                                        cucumberTimer -= dt;

                                }
                                else cucumber.attackIdx = 0;
                            }
                            else if (cucumber.hitbox.getPosition().x > bomb.bombs[i].hitbox.getPosition().x)// Bomb is on the left side
                            {
                                animate(cucumber.shape, cucumber.text[1], cucumberAnimIdx, 64, 68, cucumberTimer, 12, 1, 2.f);
                                cucumber.velocity.x = -90.f * dt;
                                if (cucumber.hitbox.getGlobalBounds().intersects(bomb.bombs[i].hitbox.getGlobalBounds()) and bomb.bombs[i].hitbox.getPosition().y + bomb.bombs[i].hitbox.getGlobalBounds().height / 2 > cucumber.hitbox.getPosition().y)
                                {
                                    cucumber.velocity.x = 0;
                                    bomb.bombs[i].velocity.x = 0;
                                    if (cucumber.shape.getTexture() != &cucumber.text[3])
                                    {
                                        cucumber.shape.setTexture(cucumber.text[3]);
                                        //l = 0;
                                        cucumber.shape.setTextureRect(IntRect(cucumber.attackIdx * 64, 0, 64, 68));
                                    }
                                    if (cucumberTimer <= 0 and !cucumber.isBlowing)
                                    {
                                        cucumber.attackIdx++;
                                        cucumber.attackIdx %= 11;
                                        //cout << l << '\n';
                                        if (cucumber.attackIdx == 6)
                                        {
                                            sound.blow.play();
                                        }
                                        if (cucumber.attackIdx == 9)
                                        {
                                            bomb.bombs.erase(bomb.bombs.begin() + i);
                                            cucumber.isBlowing = false;
                                            break;
                                        }
                                        //cout << l << " ";
                                        cucumber.shape.setTextureRect(IntRect(cucumber.attackIdx * 64, 0, 64, 68));
                                        cucumberTimer = delay * 3;
                                    }
                                    else
                                        cucumberTimer -= dt;
                                }
                                else cucumber.attackIdx = 0;
                            }
                        }
                    //bomb stuff
                    if (bomb.bombs[i].detonateTimer < TimeToDetonate)
                        bomb.bombs[i].detonateTimer++;
                    else
                    {
                        if (bomb.bombs[i].shape.getTexture() != &bomb.bombs[i].bombOffTex)
                        {
                            bombAnimIdx = 0;
                            bomb.bombs[i].shape.setTexture(bomb.bombs[i].bombOffTex);
                            bomb.bombs[i].shape.setTextureRect(IntRect(bombAnimIdx * 96, 0, 96, 108));
                        }
                        if (bombAnimTime <= 0)
                        {
                            bombAnimIdx++;
                            bombAnimIdx %= 9; // 0->8
                            bomb.bombs[i].shape.setTextureRect(IntRect(bombAnimIdx * 96, 0, 96, 108));
                            bombAnimTime = bombDelay;
                        }
                        else bombAnimTime -= dt;
                        if (bombAnimIdx == 1)
                        {
                            for (int j = 0; j < bomb.bombs.size(); ++j)
                            {
                                if (j == i)continue;
                                if (bomb.bombs[i].shape.getGlobalBounds().intersects(bomb.bombs[j].shape.getGlobalBounds()))
                                {
                                    if (bomb.bombs[i].hitbox.getPosition().x < bomb.bombs[j].hitbox.getPosition().x and bomb.bombs[i].hitbox.getPosition().y == bomb.bombs[j].hitbox.getPosition().y)
                                    {
                                        bomb.bombs[j].velocity.x += 5.f;
                                    }
                                    else if (bomb.bombs[i].hitbox.getPosition().x > bomb.bombs[j].hitbox.getPosition().x and bomb.bombs[i].hitbox.getPosition().y == bomb.bombs[j].hitbox.getPosition().y)
                                    {
                                        bomb.bombs[j].velocity.x -= 5.f;
                                    }
                                }

                            }
                            if (pirate.hitbox.getGlobalBounds().intersects(bomb.bombs[i].range.getGlobalBounds()) and !pirate.isHit)
                            {
                                sound.livesdec.play();
                                pirate.isHit = true;
                                playerHearts--;
                            }
                            if (bigGuyLvl2.hitbox.getGlobalBounds().intersects(bomb.bombs[i].range.getGlobalBounds()))
                            {
                                bigGuyLvl2.isAlive = false;
                            }
                            if (cucumber.hitbox.getGlobalBounds().intersects(bomb.bombs[i].range.getGlobalBounds()))
                            {
                                cucumber.isAlive = false;
                            }

                        }
                        if (bombAnimIdx == 1)
                        {
                            sound.bombsound.play();
                        }
                        if (bombAnimIdx == 8) {
                            bigGuyLvl2.pick_up = false;
                            cucumber.isBlowing = false;
                            bomb.bombs.erase(bomb.bombs.begin() + i);
                            break;
                        }
                    }
                }
            }

            //death animation
            if (!bigGuyLvl2.isAlive)
            {

                dead_animation(bigGuyLvl2.shape, bigGuyLvl2.text[6], bigGuyAnimIdx, 77, 74, bigGuyTimer, 6, (bigGuyLvl2.shape.getScale().x > 0.f) ? 1 : -1, 2.f, 5, 2);
                bigGuyLvl2.velocity = { 0,0 };
                bigGuyLvl2.shape.setPosition(bigGuyLvl2.shape.getPosition().x, 980.f);
            }
            if (!cucumber.isAlive)
            {

                dead_animation(cucumber.shape, cucumber.text[4], cucumberAnimIdx, 64, 68, cucumberTimer, 6, (cucumber.shape.getScale().x > 0.f) ? 1 : -1, 2.f, 5, 2);
                cucumber.velocity = { 0,0 };
                cucumber.shape.setPosition(cucumber.shape.getPosition().x, 985.f);
            }

            if (pirate.isHit)
            {
                if (pirate.isHit and playerHearts > 0)
                {
                    animate(pirate.shape, pirate.textures[8], captainAnimIdx, 58, 58, pirateTimer, 8, (pirate.shape.getScale().x > 0.f) ? 1 : -1, 2.f);
                    if (captainAnimIdx == 7)
                        pirate.isHit = false;
                }
                else if (playerHearts <= 0)
                {
                    dead_animation(pirate.shape, pirate.textures[9], pirateAnimIdx, 58, 58, pirateTimer, 6, (pirate.shape.getScale().x > 0.f) ? 1 : -1, 2.f, 5);
                    if (pirateAnimIdx == 5)
                        pirateAnimIdx++;
                    //cout << pirateAnimIdx << '\n';
                    pirate.velocity = { 0,0 };
                }

            }

            pirate.shape.move(pirate.velocity);
            cucumber.shape.move(cucumber.velocity);
            bigGuyLvl2.shape.move(bigGuyLvl2.velocity);

            if (pirate.shape.getPosition().y >= 1150) {
                pirate.isAlive = false;
            }

            // door animation + player entering in animation after all enemies are conquered
            if (pirate.isAlive and !bigGuyLvl2.isAlive and !cucumber.isAlive)
            {
                levelFinished = true;

                if (door2.frame <= door2.mxframe) {
                    if (door2.frame == 0) sound.doorsfx.play();
                    doorTimer2 += dt;
                    if (doorTimer2 > 0.8f) {
                        doorTimer2 = 0;
                        door2.frame++;
                        door2.shape.setTextureRect(IntRect(door2.frame * 78, 0, 78, 98));
                    }
                }
            }

            if (doorBar.getGlobalBounds().intersects(pirate.hitbox.getGlobalBounds()) and levelFinished)
            {
                pirate.doorInControl = true;
                pirate.velocity = { 0,0 };

                if (pirate.shape.getTexture() != &pirate.textures[5])
                {
                    pirate.shape.setTexture(pirate.textures[5]);
                    pirateAnimIdx = 0;
                    pirate.shape.setTextureRect(IntRect(pirateAnimIdx * 58, 0, 58, 58));
                }
                if (pirateAnimIdx < 13)
                {
                    if (pirateTimer <= 0)
                    {
                        pirateAnimIdx++;
                        pirateAnimIdx %= 14;
                        pirate.shape.setTextureRect(IntRect(pirateAnimIdx * 58, 0, 58, 58));
                        pirateTimer = delay * 1.5;
                    }
                    else pirateTimer -= dt;
                }
                else
                {
                    phase++;
                    return;
                }

            }


        }
        window.clear(Color::Black);

        window.draw(back);
        window.draw(door2.shape);
        window.draw(bombcntr);
        window.draw(bombnum);
        window.draw(healthBar);

        if (pirate.isAlive)
            for (int i = 0; i < playerHearts; i++)
            {
                window.draw(heart[i]);
            }
        window.draw(door.shape);

        window.draw(pirate.shape);
        if (!vis[2]/*true*/)window.draw(cannon.shape);//want it ?
        window.draw(cucumber.shape);
        // window.draw(cucumber.RunToBombRange);
        window.draw(bigGuyLvl2.shape);
        if (vis[2] and flag)
            window.draw(explosion);
        for (int i = 0; i < 3; i++)
        {
            if (cannonBall[i].velocity.x < 0 or cannonBall[i].isGrounded)window.draw(cannonBall[i].shape);
            //window.draw(cannonBall[i].hitbox);
        }

        for (int i = 0; i < bomb.bombs.size(); ++i)
            window.draw(bomb.bombs[i].shape);

        for (int i = 0; i < 7; i++)
        {
            window.draw(platforms[i]);
        }
        for (int i = 0; i < 11; i++)
        {
            window.draw(objects[i]);
        }

        //=========================================== End level UI========================================



        if (bombcounter <= 0 and !levelFinished)
        {
            gameEnded = true;
            while (window.pollEvent(event))
            {
                if (event.type == Event::Closed || event.type == Event::KeyPressed && event.key.code == Keyboard::Escape)
                {
                    window.close();
                }
                if (event.type == Event::KeyReleased)
                {
                    if (event.key.code == Keyboard::Up)
                    {
                        sound.menu_select.play();
                        mainMenu.MoveButtonUp();

                    }
                    if (event.key.code == Keyboard::Down)
                    {
                        //cout << "im here\n";
                        sound.menu_select.play();
                        mainMenu.MoveButtonDown();

                    }
                    if (event.key.code == Keyboard::Return)
                    {
                        if (mainMenu.ButtonPressed() == 0)
                        {
                            sound.press.play();
                            gameEnded = false;
                            pirate.shape.setScale(0, 0);
                            window.display();
                            return;
                        }
                        if (mainMenu.ButtonPressed() == 1)
                        {
                            sound.press.play();
                            phase = -1;
                            return;
                        }
                    }
                }
            }
            window.draw(deathUI[3]);
            window.draw(deathUI[2]);
            window.draw(mainMenu.ButtonsText[0]);
            window.draw(mainMenu.ButtonsText[1]);
            mainMenu.drawButtons(window);
        }

        //DEATH SCREEN
        if (playerHearts <= 0 and bomb.bombs.empty() and pirateAnimIdx == 6)
        {
            gameEnded = true;
            while (window.pollEvent(event))
            {
                if (event.type == Event::Closed || event.type == Event::KeyPressed && event.key.code == Keyboard::Escape)
                {
                    window.close();
                }
                if (event.type == Event::KeyReleased)
                {
                    if (event.key.code == Keyboard::Up)
                    {
                        sound.menu_select.play();
                        mainMenu.MoveButtonUp();

                    }
                    if (event.key.code == Keyboard::Down)
                    {
                        //cout << "im here\n";
                        sound.menu_select.play();
                        mainMenu.MoveButtonDown();

                    }
                    if (event.key.code == Keyboard::Return)
                    {
                        if (mainMenu.ButtonPressed() == 0)
                        {
                            sound.press.play();
                            gameEnded = false;
                            pirate.shape.setScale(0, 0);
                            window.display();
                            return;
                        }
                        if (mainMenu.ButtonPressed() == 1)
                        {
                            sound.press.play();
                            phase = -1;
                            return;
                        }
                    }
                }
            }
            window.draw(deathUI[0]);
            window.draw(deathUI[2]);
            window.draw(mainMenu.ButtonsText[0]);
            window.draw(mainMenu.ButtonsText[1]);
            mainMenu.drawButtons(window);
        }

        window.display();


    }
}

void lobby_3(RenderWindow& window)
{
    window.setFramerateLimit(60); //set it according to your monitor's refresh rate
    window.setVerticalSyncEnabled(true); //to avoid screen tearing

    initialize();

    //game clock
    Clock clock;
    Texture lobbyTxt;
    lobbyTxt.loadFromFile("./textures/lobby.png");
    Sprite lobby;
    lobby.setTexture(lobbyTxt);
    lobby.setScale(1, 1);

    //Door init
    Door door;
    door.shape.setTexture(door.door_tex[1]); //idle
    door.shape.setTextureRect(IntRect(door.frame * 78, 0, 78, 98));
    door.shape.setScale(2.f, 2.f);
    door.shape.setPosition(880.f, 245.f);
    bool door_opened = false;
    double doorDelay = 1.f;

    Door door2;
    door2.shape.setTexture(door.door_tex[1]); //idle
    door2.shape.setTextureRect(IntRect(0, 0, 78, 98));
    door2.shape.setScale(2.f, 2.f);
    door2.shape.setPosition(1425.f, 767.5f);

    Door door3;
    door3.shape.setTexture(door.door_tex[1]); //idle
    door3.shape.setTextureRect(IntRect(0, 0, 78, 98));
    door3.shape.setScale(2.f, 2.f);
    door3.shape.setPosition(890.f, 767.5f);

    //Door Bar
    RectangleShape doorBar(Vector2f(10, 150));
    doorBar.setPosition(door3.shape.getPosition().x + 50, door3.shape.getPosition().y + 45);
    doorBar.setFillColor(Color::Blue);

    pirate.shape.setTextureRect(IntRect(0, 0, 58, 58));
    pirate.shape.setPosition(Vector2f(door2.shape.getPosition().x + 80, door2.shape.getPosition().y + 30));
    pirate.shape.setOrigin(pirate.shape.getTextureRect().width / 2, pirate.shape.getTextureRect().height / 2);
    pirate.shape.setScale(2.f, 2.f);
    pirate.shape.setTexture(pirate.textures[0]);

    float doorTimer = 0, doorTimer2 = 0;

    //Objects
    RectangleShape objects[5];
    objects[0].setSize(Vector2f(1050, 215));
    objects[0].setPosition(95, 440);
    objects[0].setFillColor(Color::Red);

    objects[1].setSize(Vector2f(1240, 105));
    objects[1].setPosition(590, 965);
    objects[1].setFillColor(Color::Red);



    while (window.isOpen())
    {
        //cout << "X pos : " << Mouse::getPosition().x << " Y pos : " << Mouse::getPosition().y << endl;
        Event event;
        dt = clock.restart().asSeconds();
        while (window.pollEvent(event))
        {
            if (event.type == Event::Closed || event.type == Event::KeyPressed && event.key.code == Keyboard::Escape)
            {
                sound.press.play();
                phase = -1;
                window.close();
            }
        }
        if (pirate.shape.getScale().x > 0)pirate.hitbox.setPosition(pirate.shape.getPosition().x + 10, pirate.shape.getPosition().y + 5);
        else pirate.hitbox.setPosition(pirate.shape.getPosition().x - 10, pirate.shape.getPosition().y + 5);

        //apply gravity
        pirate.velocity.y += g * dt;



        if (!door_opened) {
            if (door2.frame == 0) sound.doorsfx.play();
            pirate.shape.setScale(0, 0);
            if (door2.frame < door2.mxframe) {
                doorTimer += dt;
                //cout << doorTimer << endl;
                if (doorTimer > 0.8f) {

                    doorTimer = 0;
                    door2.frame++;
                    door2.shape.setTextureRect(IntRect(door2.frame * 78, 0, 78, 98));
                }
            }
            else {
                door_opened = true;
                pirate.shape.setScale(2.f, 2.f);
            }
        }

        if (door_opened) {
            if (pirate.player_in_Frame <= pirate.player_in_mxFrame)
            {
                pirate.shape.setTexture(pirate.textures[4]);

                pirate.shape.setTextureRect(IntRect(pirate.player_in_Frame * 58, 0, 58, 58));

                if (pirate.player_init_timer <= 0)
                {
                    pirate.player_in_Frame++;
                    pirate.player_in_Frame %= 11; // 0 -> 10
                    pirate.shape.setTextureRect(IntRect(pirate.player_in_Frame * 58, 0, 58, 58));
                    pirate.player_init_timer = 0.06;
                }
                else pirate.player_init_timer -= dt;
            }

            else if (pirate.player_in_Frame >= pirate.player_in_mxFrame)
                player_init = true;
        }

        if (player_init) {
            //pirate.shape.setScale(2.f, 2.f);
            if (door2.frame > 0) {
                doorTimer += dt;
                if (doorTimer > 0.6f) {
                    doorTimer = 0;
                    door2.frame--;
                    door2.shape.setTextureRect(IntRect(door2.frame * 78, 0, 78, 98));
                }
            }
        }

        //collision with objects for player (width is height :) )
        for (int i = 0; i < 2; i++)
        {
            //ground
            if (pirate.hitbox.getPosition().y + pirate.hitbox.getGlobalBounds().width / 2.f < objects[i].getPosition().y)//check if player is on top of the object
            {
                if (CollisionDetection(pirate.hitbox, objects[i]))
                {

                    pirate.velocity.y = 0;
                    pirate.shape.setPosition(pirate.shape.getPosition().x, objects[i].getPosition().y - pirate.hitbox.getGlobalBounds().height * 0.5f - 5);


                    if (pirate.isJumping)
                    {
                        sound.groundhit.play();
                        pirate.isJumping = false;
                        pirate.shape.setTexture(pirate.textures[0]);
                    }
                    // cout << "foo2\n";
                }
            }
            //top
            else if (pirate.hitbox.getPosition().y - pirate.hitbox.getGlobalBounds().width / 2.f > objects[i].getPosition().y + objects[i].getGlobalBounds().height)
            {
                if (CollisionDetection(pirate.hitbox, objects[i]) and pirate.velocity.y <= 0)
                {
                    pirate.velocity.y *= -80.f * dt;
                }
            }
        }


        Pirate_move_code();
        pirate.shape.move(pirate.velocity);
        if (door_opened)
            if (door3.frame <= door3.mxframe) {
                sound.doorsfx.play();
                doorTimer2 += dt;
                if (doorTimer2 > 0.8f) {
                    doorTimer2 = 0;
                    door3.frame++;
                    door3.shape.setTextureRect(IntRect(door3.frame * 78, 0, 78, 98));
                }
            }


        if (doorBar.getGlobalBounds().intersects(pirate.hitbox.getGlobalBounds()))
        {
            pirate.doorInControl = true;
            pirate.velocity = { 0,0 };

            if (pirate.shape.getTexture() != &pirate.textures[5])
            {
                pirate.shape.setTexture(pirate.textures[5]);
                pirateAnimIdx = 0;
                pirate.shape.setTextureRect(IntRect(pirateAnimIdx * 58, 0, 58, 58));
            }
            if (pirateAnimIdx < 13)
            {
                if (pirateTimer <= 0)
                {
                    pirateAnimIdx++;
                    pirateAnimIdx %= 14;
                    pirate.shape.setTextureRect(IntRect(pirateAnimIdx * 58, 0, 58, 58));
                    pirateTimer = delay * 1.5;
                }
                else pirateTimer -= dt;
            }
            else
            {
                phase++;
                return;
            }

        }



        RepeatedKeyHandle(event, isPressed);

        window.clear();
        window.draw(lobby);
        window.draw(door.shape);
        window.draw(door2.shape);
        window.draw(door3.shape);
        //   window.draw(doorBar);
        window.draw(pirate.shape);



        window.display();

    }
}

void Level_3(RenderWindow& window)
{
    sound.lvl2Sound.stop();
    sound.lvl3Sound.play();
    window.setFramerateLimit(60); //set it according to your monitor's refresh rate
    window.setVerticalSyncEnabled(true); //to avoid screen tearing

    initialize();

    //game clock
    Clock clock;


    //background
    Sprite back;
    Texture backText;
    backText.loadFromFile("./textures/maps/maplvl3.png");
    back.setTexture(backText);
    back.setPosition(0, 0);

    Door door;
    door.shape.setTexture(door.door_tex[1]); //idle
    door.shape.setTextureRect(IntRect(door.frame * 78, 0, 78, 98));
    door.shape.setScale(1.5f, 1.5f);
    door.shape.setPosition(620.f, 249.f);
    bool door_opened = false;
    double doorDelay = 1.f;


    Door door2;
    door2.shape.setTexture(door.door_tex[1]); //idle
    door2.shape.setTextureRect(IntRect(0, 0, 78, 98));
    door2.shape.setScale(1.5f, 1.5f);
    door2.shape.setPosition(1356.f, 809.f);
    //Door Bar
    RectangleShape doorBar(Vector2f(10, 150));
    doorBar.setPosition(door2.shape.getPosition().x + 60, door2.shape.getPosition().y);
    doorBar.setFillColor(Color::Blue);
    float doorTimer = 0, doorTimer2 = 0;


    //pirate.shape.setTexture(pirate.textures[0]);
    pirate.shape.setTextureRect(IntRect(0, 0, 58, 58));
    pirate.shape.setPosition(Vector2f(680.f, 350.f));
    pirate.shape.setOrigin(pirate.shape.getTextureRect().width / 2, pirate.shape.getTextureRect().height / 2);
    //pirate.shape.setScale(1.8f, 1.8f);


    bald_lvl3.shape.setTexture(bald_lvl3.text[0]);
    bald_lvl3.shape.setScale(-1.75f, 1.75f); // 2X
    bald_lvl3.shape.setTextureRect(IntRect(0, 0, 63, 67));
    bald_lvl3.shape.setPosition(1121.f, 340.f);
    bald_lvl3.shape.setOrigin(bald_lvl3.shape.getTextureRect().width / 2, bald_lvl3.shape.getTextureRect().height / 2);
    bald_lvl3.range.setSize(Vector2f(250, 50));
    bald_lvl3.range.setOrigin(bald_lvl3.range.getSize().x / 2, bald_lvl3.range.getSize().y / 2);

    bigGuyLvl3.shape.setTexture(bigGuyLvl3.text[0]);
    bigGuyLvl3.shape.setScale(1.8f, 1.8f); // 2xL*W
    bigGuyLvl3.shape.setTextureRect(IntRect(0, 0, 77, 74));
    bigGuyLvl3.shape.setPosition(190, 810);
    bigGuyLvl3.shape.setOrigin(bigGuyLvl3.shape.getTextureRect().width / 2, bigGuyLvl3.shape.getTextureRect().height / 2);
    bigGuyLvl3.range.setSize(Vector2f(550, 50.f));


    whaleLvl3.shape.setTexture(whaleLvl3.textures[0]);
    whaleLvl3.shape.setScale(1.5f, 1.5f);
    whaleLvl3.shape.setTextureRect(IntRect(0, 0, 63, 67));
    whaleLvl3.shape.setPosition(430.f, 300.f);
    whaleLvl3.shape.setOrigin(whaleLvl3.shape.getTextureRect().width / 2, whaleLvl3.shape.getTextureRect().height / 2);

    //Captain
    captain.shape.setTexture(captain.text[0]);
    captain.shape.setScale(1.8f, 1.8f);
    captain.shape.setTextureRect(IntRect(0, 0, 80, 72));
    captain.shape.setPosition(586.f, 885.f);
    captain.shape.setOrigin(captain.shape.getTextureRect().width / 2, captain.shape.getTextureRect().height / 2);


    captain.hitbox.setPosition(captain.shape.getPosition().x, captain.shape.getPosition().y);
    captain.range.setPosition(captain.shape.getPosition().x, captain.shape.getPosition().y + 30);

    //Bomb init
    //Bomb bomb(50);
    bomb.shape.setTexture(bomb.bombTex);
    bomb.shape.setScale(1.8f, 1.8f);
    bomb.shape.setTextureRect(IntRect(0, 0, 96, 108));
    bomb.shape.setPosition(pirate.shape.getPosition().x, pirate.shape.getPosition().y - 10);
    bomb.shape.setOrigin(bomb.shape.getTextureRect().width / 2.f, bomb.shape.getTextureRect().height / 2.f);
    bomb.range.setPosition(bomb.shape.getPosition());
    Texture GameOver_Txt[4];
    GameOver_Txt[0].loadFromFile("./UI/gameover.png");
    GameOver_Txt[1].loadFromFile("./UI/background.png");
    GameOver_Txt[2].loadFromFile("./UI/buttons.png");
    GameOver_Txt[3].loadFromFile("./UI/noBombsLeft.png");

    Texture WinScreen_Txt[2];
    WinScreen_Txt[0].loadFromFile("./UI/gamewon.png");
    WinScreen_Txt[1].loadFromFile("./UI/gamewon_button.png");

    // 0 ->>>> GameWon
    winUI[0].setTexture(WinScreen_Txt[0]);
    winUI[0].setPosition(window.getSize().x / 2 - 50, window.getSize().y / 2);
    winUI[0].setOrigin(winUI[0].getTextureRect().width / 2, winUI[0].getTextureRect().height / 2);
    winUI[0].setScale(0.75f, 0.75f);

    // 1 ->>>> Exit button (Game won)
    winUI[1].setTexture(WinScreen_Txt[1]);
    winUI[1].setPosition(window.getSize().x / 2 - 50, window.getSize().y / 1.95);
    winUI[1].setOrigin(winUI[1].getTextureRect().width / 2, winUI[1].getTextureRect().height / 2);
    winUI[1].setScale(1.f, 1.f);

    // 0 ->>>> GameOver
    deathUI[0].setTexture(GameOver_Txt[0]);
    deathUI[0].setPosition(window.getSize().x / 2 - 50, window.getSize().y / 2);
    deathUI[0].setOrigin(deathUI[0].getTextureRect().width / 2, deathUI[0].getTextureRect().height / 2);
    deathUI[0].setScale(0.75f, 0.75f);
    // 1 ->>>> bg
    deathUI[1].setTexture(GameOver_Txt[1]);
    deathUI[1].setPosition(window.getSize().x / 2 - 50, window.getSize().y / 2);
    deathUI[1].setOrigin(deathUI[1].getTextureRect().width / 2, deathUI[1].getTextureRect().height / 2);
    deathUI[1].setScale(0.75f, 0.75f);

    // 2 ->>>>>buttons
    deathUI[2].setTexture(GameOver_Txt[2]);
    deathUI[2].setPosition(window.getSize().x / 2 - 50, window.getSize().y / 1.95);
    deathUI[2].setOrigin(deathUI[2].getTextureRect().width / 2, deathUI[2].getTextureRect().height / 2);
    deathUI[2].setScale(1.f, 1.f);

    // 3 -->>>>>>> No bombs left
    deathUI[3].setTexture(GameOver_Txt[3]);
    deathUI[3].setPosition(window.getSize().x / 2 - 50, window.getSize().y / 2);
    deathUI[3].setOrigin(deathUI[1].getTextureRect().width / 2, deathUI[1].getTextureRect().height / 2);
    deathUI[3].setScale(0.75f, 0.75f);


    RectangleShape objects[10];

    {
        objects[0].setSize(Vector2f(posX(345), posY(255))); // minus hitbox of char in x onlyy (50)
        objects[0].setFillColor(Color::Transparent);
        objects[0].setPosition(posX(263), posY(320));

        objects[1].setSize(Vector2f(posX(284), posY(282)));
        objects[1].setFillColor(Color::Transparent);
        objects[1].setPosition(posX(611), posY(400));

        objects[2].setSize(Vector2f(posX(285), posY(282))); //240 to 230
        objects[2].setFillColor(Color::Transparent);
        objects[2].setPosition(posX(1022), posY(400));

        objects[3].setSize(Vector2f(posX(345), posY(255)));  //176 to 140
        objects[3].setFillColor(Color::Transparent);
        objects[3].setPosition(posX(1311), posY(316));

        objects[4].setSize(Vector2f(posX(385), (76))); // minus 50 for the hitbox 
        objects[4].setFillColor(Color::Transparent);
        objects[4].setPosition(posX(56), posY(878));    // moving 50 in +ve x axis in position 

        objects[5].setSize(Vector2f(posX(385), posY(76)));
        objects[5].setFillColor(Color::Transparent);
        objects[5].setPosition(posX(1478), posY(881));

        objects[6].setSize(Vector2f(posX(1032), posY(200)));
        objects[6].setFillColor(Color::Transparent);
        objects[6].setPosition(posX(445), posY(959));

        objects[7].setSize(Vector2f(posX(56), posY(880)));
        objects[7].setFillColor(Color::Transparent);
        objects[7].setPosition(posX(0), posY(0));

        objects[8].setSize(Vector2f(posX(56), posY(819)));
        objects[8].setFillColor(Color::Transparent);
        objects[8].setPosition(posX(1860), posY(58));

        objects[9].setSize(Vector2f(posX(1920), posY(54)));
        objects[9].setFillColor(Color::Transparent);
        objects[9].setPosition(posX(0), posY(0));
    }


    RectangleShape platforms[8];

    {
        platforms[0].setSize(Vector2f(posX(129), posY(2)));
        platforms[0].setFillColor(Color::Transparent);
        platforms[0].setPosition(posX(56), posY(415));

        platforms[1].setSize(Vector2f(posX(129), posY(2)));
        platforms[1].setFillColor(Color::Transparent);
        platforms[1].setPosition(posX(131), posY(570));

        platforms[2].setSize(Vector2f(posX(129), posY(2)));
        platforms[2].setFillColor(Color::Transparent);
        platforms[2].setPosition(posX(55), posY(722));

        platforms[3].setSize(Vector2f(posX(129), posY(2)));
        platforms[3].setFillColor(Color::Transparent);
        platforms[3].setPosition(posX(1734), posY(410));

        platforms[4].setSize(Vector2f(posX(129), posY(2)));
        platforms[4].setFillColor(Color::Transparent);
        platforms[4].setPosition(posX(1657), posY(564));

        platforms[5].setSize(Vector2f(posX(129), posY(2)));
        platforms[5].setFillColor(Color::Transparent);
        platforms[5].setPosition(posX(1735), posY(721));

    }

    Texture bombnum_tex[16];
    Sprite bombnum;
    int bombcounter = 16;

    {
        bombnum_tex[0].loadFromFile("./textures/bomb/BombCounter/0.png");
        bombnum_tex[1].loadFromFile("./textures/bomb/BombCounter/1.png");
        bombnum_tex[2].loadFromFile("./textures/bomb/BombCounter/2.png");
        bombnum_tex[3].loadFromFile("./textures/bomb/BombCounter/3.png");
        bombnum_tex[4].loadFromFile("./textures/bomb/BombCounter/4.png");
        bombnum_tex[5].loadFromFile("./textures/bomb/BombCounter/5.png");
        bombnum_tex[6].loadFromFile("./textures/bomb/BombCounter/6.png");
        bombnum_tex[7].loadFromFile("./textures/bomb/BombCounter/7.png");
        bombnum_tex[8].loadFromFile("./textures/bomb/BombCounter/8.png");
        bombnum_tex[9].loadFromFile("./textures/bomb/BombCounter/9.png");
        bombnum_tex[10].loadFromFile("./textures/bomb/BombCounter/10.png");
        bombnum_tex[11].loadFromFile("./textures/bomb/BombCounter/11.png");
        bombnum_tex[12].loadFromFile("./textures/bomb/BombCounter/12.png");
        bombnum_tex[13].loadFromFile("./textures/bomb/BombCounter/13.png");
        bombnum_tex[14].loadFromFile("./textures/bomb/BombCounter/14.png");
        bombnum_tex[15].loadFromFile("./textures/bomb/BombCounter/15.png");
    }

    bombnum.setTexture(bombnum_tex[15]);
    bombnum.setScale(0.13, 0.13);
    bombnum.setPosition(1777, 100);

    //Health
    Texture healthBar_text, heart_text;
    {
        healthBar_text.loadFromFile("./textures/Health/Health Bar.png");
        heart_text.loadFromFile("./textures/Health/Heart.png");
    }
    Sprite healthBar, heart[3];
    {

        healthBar.setTexture(healthBar_text);
        healthBar.setScale(1.8f, 1.8f);
        healthBar.setPosition(0, 0);

        heart[0].setTexture(heart_text);
        heart[1].setTexture(heart_text);
        heart[2].setTexture(heart_text);

        heart[0].setScale(1.8f, 1.8f);
        heart[1].setScale(1.8f, 1.8f);
        heart[2].setScale(1.8f, 1.8f);

        heart[0].setPosition(70, 40);
        heart[1].setPosition(120, 40);
        heart[2].setPosition(165, 40);
    }

    bool gameWon = false;

    while (window.isOpen())
    {
        //cout << "X pos : " << Mouse::getPosition().x << " Y pos : " << Mouse::getPosition().y << endl;
        //cout << pirate.hitbox.getPosition().y << endl;
        dt = clock.restart().asSeconds();
        Event event;

        if (!gameEnded and !gameWon)
        {
            while (window.pollEvent(event))
            {
                if (event.type == Event::Closed || event.type == Event::KeyPressed && event.key.code == Keyboard::Escape) {
                    phase = -1;
                    window.close();
                    return;
                }

            }



            //Pirate Hitbox
            //HitBoxUpdate(pirate.shape, pirate.hitbox, 10);
            if (pirate.shape.getScale().x > 0)pirate.hitbox.setPosition(pirate.shape.getPosition().x + 10, pirate.shape.getPosition().y + 5);
            else pirate.hitbox.setPosition(pirate.shape.getPosition().x - 10, pirate.shape.getPosition().y + 5);

            //Big guy Hitbox
            HitBoxUpdate(bigGuyLvl3.shape, bigGuyLvl3.hitbox, 10);
            HitBoxUpdate(bigGuyLvl3.shape, bigGuyLvl3.RunToBombRange, 45);
            exclamation[0].shape.setPosition(bigGuyLvl3.shape.getPosition().x + 20, bigGuyLvl3.shape.getPosition().y - 80);
            //Bald Hitbox
            HitBoxUpdate(bald_lvl3.shape, bald_lvl3.hitbox, 40);
            exclamation[1].shape.setPosition(bald_lvl3.shape.getPosition().x + 20, bald_lvl3.shape.getPosition().y - 80);

            //Range big guy update
            HitBoxUpdate(bigGuyLvl3.shape, bigGuyLvl3.range, -20);

            //Range bald pirate update
            //.bombRange.setPosition(bald_lvl2.shape.getPosition().x, bald_lvl2.shape.getPosition().y + 40);
            HitBoxUpdate(bald_lvl3.shape, bald_lvl3.range, 20);
            HitBoxUpdate(bald_lvl3.shape, bald_lvl3.bombRange, 40);

            // bomb
            HitBoxUpdate(bomb.shape, bomb.range, 0);
            HitBoxUpdate(bomb.shape, bomb.hitbox, 30);

            //Whale
            HitBoxUpdate(whaleLvl3.shape, whaleLvl3.range, -10);
            HitBoxUpdate(whaleLvl3.shape, whaleLvl3.hitbox, -10);
            exclamation[2].shape.setPosition(whaleLvl3.shape.getPosition().x + 10, whaleLvl3.shape.getPosition().y - 60);
            whaleLvl3.hungerBar.setPosition(whaleLvl3.shape.getPosition().x - 5, whaleLvl3.shape.getPosition().y - 50);
            whaleLvl3.hungerBar.setTextureRect(IntRect(u * 39, 0, 39, 9));

            //Captain
            HitBoxUpdate(captain.shape, captain.range, 30);
            HitBoxUpdate(captain.shape, captain.hitbox);

            if (!door_opened) {
                if (door.frame == 0) sound.doorsfx.play();
                pirate.shape.setScale(0, 0);
                if (door.frame < door.mxframe) {
                    doorTimer += dt;
                    if (doorTimer > 0.8f) {
                        doorTimer = 0;
                        door.frame++;
                        door.shape.setTextureRect(IntRect(door.frame * 78, 0, 78, 98));
                    }
                }
                else {
                    pirate.shape.setScale(1.8f, 1.8f);
                    door_opened = true;
                }
            }


            if (door_opened) {
                if (pirate.player_in_Frame <= pirate.player_in_mxFrame)
                {
                    pirate.shape.setTexture(pirate.textures[4]);

                    pirate.shape.setTextureRect(IntRect(pirate.player_in_Frame * 58, 0, 58, 58));

                    if (pirate.player_init_timer <= 0)
                    {
                        pirate.player_in_Frame++;
                        pirate.player_in_Frame %= 11; // 0 -> 15
                        pirate.shape.setTextureRect(IntRect(pirate.player_in_Frame * 58, 0, 58, 58));
                        pirate.player_init_timer = 0.06;
                    }
                    else pirate.player_init_timer -= dt;
                }

                else if (pirate.player_in_Frame >= pirate.player_in_mxFrame)
                    player_init = true;
            }

            if (player_init) {
                if (door.frame > 0) {
                    doorTimer += dt;
                    if (doorTimer > 0.6f) {
                        doorTimer = 0;
                        door.frame--;
                        door.shape.setTextureRect(IntRect(door.frame * 78, 0, 78, 98));
                    }




                }

            }


            pirate.velocity.y += g * dt; // apply gravity


            //collision with objects for player (width is height :) )
            for (int i = 0; i < 10; i++)
            {
                //ground
                if (pirate.hitbox.getPosition().y + pirate.hitbox.getGlobalBounds().width / 2.f < objects[i].getPosition().y)//check if player is on top of the object
                {
                    if (CollisionDetection(pirate.hitbox, objects[i]))
                    {
                        pirate.velocity.y = 0;
                        pirate.shape.setPosition(pirate.shape.getPosition().x, objects[i].getPosition().y - pirate.hitbox.getGlobalBounds().height * 0.5f);


                        if (pirate.isJumping)
                        {
                            sound.groundhit.play();
                            pirate.isJumping = false;
                            pirate.shape.setTexture(pirate.textures[0]);
                        }
                        // cout << "foo2\n";
                    }
                }
                //top
                else if (pirate.hitbox.getPosition().y - pirate.hitbox.getGlobalBounds().width / 2.f > objects[i].getPosition().y + objects[i].getGlobalBounds().height)
                {
                    if (CollisionDetection(pirate.hitbox, objects[i]) and pirate.velocity.y <= 0)
                    {
                        pirate.velocity.y *= -50.f * dt;
                    }
                }
            }


            for (int i = 0; i < 9; ++i)
            {
                //right
                if (pirate.hitbox.getPosition().y + pirate.hitbox.getGlobalBounds().height / 2.f > objects[i].getPosition().y
                    and pirate.hitbox.getPosition().x - pirate.hitbox.getGlobalBounds().width / 2.f < objects[i].getPosition().x + objects[i].getGlobalBounds().width
                    and !(pirate.hitbox.getPosition().y - pirate.hitbox.getGlobalBounds().width / 2.f > objects[i].getPosition().y + objects[i].getGlobalBounds().height)
                    and !(pirate.hitbox.getPosition().y + pirate.hitbox.getGlobalBounds().width / 2.f < objects[i].getPosition().y)
                    and pirate.shape.getScale().x < 0
                    )
                {
                    if (CollisionDetection(pirate.hitbox, objects[i]))
                    {
                        //cout << "collide\n";
                        //right
                        pirate.velocity.x = 0.f;
                        pirate.shape.setPosition(objects[i].getPosition().x + objects[i].getGlobalBounds().width + 35, pirate.shape.getPosition().y);
                    }
                }
                //left
                else if (pirate.hitbox.getPosition().y + pirate.hitbox.getGlobalBounds().height / 2.f > objects[i].getPosition().y
                    and pirate.hitbox.getPosition().x + pirate.hitbox.getGlobalBounds().width / 2.f > objects[i].getPosition().x
                    and !(pirate.hitbox.getPosition().y - pirate.hitbox.getGlobalBounds().width / 2.f > objects[i].getPosition().y + objects[i].getGlobalBounds().height)
                    and !(pirate.hitbox.getPosition().y + pirate.hitbox.getGlobalBounds().width / 2.f < objects[i].getPosition().y)
                    and pirate.shape.getScale().x > 0
                    )
                {
                    if (CollisionDetection(pirate.hitbox, objects[i]))
                    {
                        //cout << "collide\n";
                        //right
                        pirate.velocity.x = 0.f;
                        pirate.shape.setPosition(objects[i].getPosition().x - 35, pirate.shape.getPosition().y);
                    }
                }
            }


            // collision with platforms for player
            for (int i = 0; i < 10; i++)
            {
                if (pirate.shape.getPosition().y + pirate.shape.getGlobalBounds().height / 4.f + 20 < platforms[i].getPosition().y) // check on top ola l2
                {
                    if (pirate.hitbox.getGlobalBounds().intersects(platforms[i].getGlobalBounds())) {
                        if (pirate.isJumping)
                        {
                            sound.groundhit.play();
                            pirate.isJumping = false;
                            pirate.shape.setTexture(pirate.textures[0]);
                        }
                        pirate.velocity.y = 0;
                    }

                }
            }

            RepeatedKeyHandle(event, isPressed);

            Pirate_move_code(1.8f);
            bald_code(bald_lvl3, 1030, 1290, 1.7f);
            captainCode(500, 1450);
            big_guy_code(bigGuyLvl3, 75, 420);
            Whale_code(whaleLvl3, 1, objects[0]);
            // Bomb shoot 
            if (shootTimer < 12.5)
                shootTimer++;
            if (player_init and playerHearts > 0)
            {
                if (Mouse::isButtonPressed(Mouse::Left) and shootTimer >= 12.5 and bomb.bombs.size() < 5) {
                    //sound.bombsound.play();

                    bombcounter--;
                    bombnum.setTexture(bombnum_tex[bombcounter - 1]);

                    bomb.shape.setPosition(pirate.shape.getPosition().x, pirate.shape.getPosition().y - 35.f);
                    bomb.velocity.x = (pirate.shape.getScale().x > 0.f) ? 10.f * cos(initAngle) : -10.f * cos(initAngle);
                    bomb.velocity.y = -10.f * sin(initAngle);
                    bomb.isShot = true;
                    bombFired++;


                    if (bombFired <= bombsNum)bomb.bombs.pb(bomb); // BOMB fire limiter
                    shootTimer = 0; //RESET timer
                }
            }


            //bomb animation
            if (bombAnimTime <= 0 and !bomb.bombs.empty())
            {
                bombAnimIdx++;
                bombAnimIdx %= 10;
                for (int i = 0; i < bomb.bombs.size(); i++)
                {
                    bomb.bombs[i].shape.setTextureRect(IntRect(bombAnimIdx * 96, 0, 96, 108));

                }
                bombAnimTime = bombDelay;
            }
            else bombAnimTime -= dt;



            //move bombs
            if (!bomb.bombs.empty())
            {
                for (int i = 0; i < bomb.bombs.size(); ++i)
                {
                    bomb.bombs[i].velocity.y += g * dt; //apply gravity

                    bomb.bombs[i].shape.move(bomb.bombs[i].velocity);
                    HitBoxUpdate(bomb.bombs[i].shape, bomb.bombs[i].hitbox, 30);
                    HitBoxUpdate(bomb.bombs[i].shape, bomb.bombs[i].range, 30);

                    //collision between bombs
                    for (int j = 0; j < bomb.bombs.size(); ++j)
                    {
                        if (i == j)continue;
                        if (bomb.bombs[i].hitbox.getGlobalBounds().intersects(bomb.bombs[j].hitbox.getGlobalBounds()))
                        {
                            if (bomb.bombs[i].hitbox.getPosition().x < bomb.bombs[j].hitbox.getPosition().x)
                                bomb.bombs[j].velocity.x += 2.5f;
                            else bomb.bombs[j].velocity.x -= 2.5f;
                        }
                    }
                    //collision between player and bombs
                    if (bomb.bombs[i].hitbox.getGlobalBounds().intersects(pirate.hitbox.getGlobalBounds()) and bomb.bombs[i].isGrounded)
                        bomb.bombs[i].velocity.x += (pirate.hitbox.getPosition().x > bomb.bombs[i].hitbox.getPosition().x) ? -0.8f : 0.8f;
                }
            }

            //collision of bombs with platforms
            if (!bomb.bombs.empty())
            {
                for (int i = 0; i < bomb.bombs.size(); ++i)
                {
                    for (int j = 0; j < 9; ++j)//ALL bottoms
                    {
                        if (bomb.bombs[i].hitbox.getPosition().y + bomb.bombs[i].hitbox.getGlobalBounds().height * 0.25f - 30 < platforms[j].getPosition().y
                            and !(bomb.bombs[i].hitbox.getPosition().x - bomb.bombs[i].hitbox.getGlobalBounds().width * 0.25f > platforms[j].getPosition().x + platforms[j].getGlobalBounds().width)//left
                            and !(bomb.bombs[i].hitbox.getPosition().x + bomb.bombs[i].hitbox.getGlobalBounds().width * 0.25f < platforms[j].getPosition().x)//right
                            )
                        {

                            if (bomb.bombs[i].hitbox.getGlobalBounds().intersects(platforms[j].getGlobalBounds())) {

                                bomb.bombs[i].isGrounded = 1;
                                bomb.bombs[i].velocity.y = 0.f;
                                bomb.bombs[i].shape.setPosition(bomb.bombs[i].shape.getPosition().x, platforms[j].getPosition().y - 60);
                                if (bomb.bombs[i].velocity.x > 0)
                                {
                                    bomb.bombs[i].velocity.x -= resistanceForce * dt;
                                    if (bomb.bombs[i].velocity.x <= 0)bomb.bombs[i].velocity.x = 0.f;
                                }

                                else if (bomb.bombs[i].velocity.x < 0)

                                {
                                    bomb.bombs[i].velocity.x += resistanceForce * dt;
                                    if (bomb.bombs[i].velocity.x >= 0)bomb.bombs[i].velocity.x = 0.f;
                                }

                                // cout << "Ground\n";
                            }
                        }
                    }
                }
            }

            //=========================================================================================================================




            // collision with objects for bomb (up and down)
            if (!bomb.bombs.empty())
            {
                for (int i = 0; i < bomb.bombs.size(); ++i)
                {
                    for (int j = 0; j < 10; ++j)//3 tops
                    {
                        //top collision
                        if (bomb.bombs[i].hitbox.getPosition().y - bomb.bombs[i].hitbox.getGlobalBounds().height * 0.25f - 30 < objects[j].getPosition().y + objects[j].getGlobalBounds().height
                            and !(bomb.bombs[i].hitbox.getPosition().x - bomb.bombs[i].hitbox.getGlobalBounds().width * 0.25f > objects[j].getPosition().x + objects[j].getGlobalBounds().width)//left
                            and !(bomb.bombs[i].hitbox.getPosition().x + bomb.bombs[i].hitbox.getGlobalBounds().width * 0.25f < objects[j].getPosition().x)//right
                            )
                        {
                            if (bomb.bombs[i].hitbox.getGlobalBounds().intersects(objects[j].getGlobalBounds()))
                                if (bomb.bombs[i].velocity.y < 0)bomb.bombs[i].velocity.y *= -0.8f;
                        }
                    }
                    for (int j = 0; j < 9; ++j)//2 bottoms
                    {
                        if (bomb.bombs[i].hitbox.getPosition().y + bomb.bombs[i].hitbox.getGlobalBounds().height * 0.25f - 30 < objects[j].getPosition().y
                            and !(bomb.bombs[i].hitbox.getPosition().x - bomb.bombs[i].hitbox.getGlobalBounds().width * 0.25f > objects[j].getPosition().x + objects[j].getGlobalBounds().width)//left
                            and !(bomb.bombs[i].hitbox.getPosition().x + bomb.bombs[i].hitbox.getGlobalBounds().width * 0.25f < objects[j].getPosition().x)//right
                            )
                        {
                            if (bomb.bombs[i].hitbox.getGlobalBounds().intersects(objects[j].getGlobalBounds())) {
                                bomb.bombs[i].isGrounded = 1;
                                bomb.bombs[i].velocity.y = 0.f;
                                bomb.bombs[i].shape.setPosition(bomb.bombs[i].shape.getPosition().x, objects[j].getPosition().y - 60);
                                if (bomb.bombs[i].velocity.x > 0)
                                {
                                    bomb.bombs[i].velocity.x -= resistanceForce * dt;
                                    if (bomb.bombs[i].velocity.x <= 0)bomb.bombs[i].velocity.x = 0.f;
                                }

                                else if (bomb.bombs[i].velocity.x < 0)

                                {
                                    bomb.bombs[i].velocity.x += resistanceForce * dt;
                                    if (bomb.bombs[i].velocity.x >= 0)bomb.bombs[i].velocity.x = 0.f;
                                }
                                //  cout << "Ground\n";
                            }
                        }
                    }
                }
            }


            //collision with objects for bomb (left and right)
            if (!bomb.bombs.empty())
            {
                for (int i = 0; i < bomb.bombs.size(); ++i)
                {
                    for (int j = 0; j < 9; j++)
                    {
                        //left direction
                        if (bomb.bombs[i].hitbox.getPosition().x - bomb.bombs[i].hitbox.getGlobalBounds().width * 0.25f > objects[j].getPosition().x + objects[j].getGlobalBounds().width
                            and !(bomb.bombs[i].hitbox.getPosition().y + bomb.bombs[i].hitbox.getGlobalBounds().height * 0.25f - 30 < objects[j].getPosition().y)
                            and !(bomb.bombs[i].hitbox.getPosition().y - bomb.bombs[i].hitbox.getGlobalBounds().height * 0.25f + 30 > objects[j].getPosition().y + objects[j].getGlobalBounds().height)
                            )
                        {
                            if (CollisionDetection(bomb.bombs[i].hitbox, objects[j]))
                            {
                                if (bomb.bombs[i].velocity.x < 0)bomb.bombs[i].velocity.x *= -0.8f;
                                //cout << "Left\n";
                            }
                        }
                        //right direction
                        if (bomb.bombs[i].hitbox.getPosition().x + bomb.bombs[i].hitbox.getGlobalBounds().width * 0.25f < objects[j].getPosition().x
                            and !(bomb.bombs[i].hitbox.getPosition().y + bomb.bombs[i].hitbox.getGlobalBounds().height * 0.25 - 30 < objects[j].getPosition().y)
                            and !(bomb.bombs[i].hitbox.getPosition().y - bomb.bombs[i].hitbox.getGlobalBounds().height * 0.25f + 30 > objects[j].getPosition().y + objects[j].getGlobalBounds().height)
                            )
                        {
                            if (CollisionDetection(bomb.bombs[i].hitbox, objects[j]))
                            {
                                if (bomb.bombs[i].velocity.x > 0)bomb.bombs[i].velocity.x *= -0.8f;
                                // cout << bomb.bombs[i].velocity.x << "\n";
                                 //cout << "Right\n";
                            }
                        }
                    }
                }
            }



            //detonation
            if (bomb.bombs.size())
                for (int i = 0; i < bomb.bombs.size(); ++i)
                {
                    if (bomb.bombs[i].isShot)
                    {

                        //big guy stuff
                        if (bigGuyLvl3.isAlive)
                            if (bigGuyLvl3.RunToBombRange.getGlobalBounds().intersects(bomb.bombs[i].hitbox.getGlobalBounds()))
                            {
                                if (bigGuyLvl3.hitbox.getPosition().x < bomb.bombs[i].hitbox.getPosition().x)// Bomb is on the right side
                                {
                                    animate(bigGuyLvl3.shape, bigGuyLvl3.text[1], bigGuyAnimIdx, 77, 74, bigGuyTimer, 16, 1, 2.f);
                                    bigGuyLvl3.velocity.x = 90.f * dt;
                                    if (bigGuyLvl3.hitbox.getGlobalBounds().intersects(bomb.bombs[i].hitbox.getGlobalBounds()))
                                    {
                                        bigGuyLvl3.velocity.x = 0;
                                        if (bigGuyLvl3.shape.getTexture() != &big_guy.text[3])
                                        {

                                            bigGuyLvl3.shape.setTexture(bigGuyLvl3.text[3]);
                                            //l = 0;
                                            bigGuyLvl3.shape.setTextureRect(IntRect(bigGuyAttackIdx * 77, 0, 77, 74));
                                        }
                                        if (bigGuyTimer <= 0 and !bigGuyLvl3.pick_up and !bigGuyLvl3.bomb_throw)
                                        {
                                            bigGuyAttackIdx++;
                                            bigGuyAttackIdx %= 8;
                                            if (bigGuyAttackIdx == 5)
                                            {
                                                bomb.bombs[i].shape.setPosition(bigGuyLvl3.hitbox.getPosition());
                                                bigGuyLvl3.pick_up = false;
                                                bigGuyLvl3.bomb_throw = true;
                                            }
                                            //cout << l << " ";
                                            bigGuyLvl3.shape.setTextureRect(IntRect(bigGuyAttackIdx * 77, 0, 77, 74));
                                            bigGuyTimer = delay * 3;
                                        }
                                        else
                                            bigGuyTimer -= dt;

                                    }
                                    else bigGuyAttackIdx = 0;
                                }
                                else if (bigGuyLvl3.hitbox.getPosition().x > bomb.bombs[i].hitbox.getPosition().x)// Bomb is on the left side
                                {
                                    animate(bigGuyLvl3.shape, bigGuyLvl3.text[1], bigGuyAnimIdx, 77, 74, bigGuyTimer, 16, -1, 2.f);
                                    bigGuyLvl3.velocity.x = -90.f * dt;
                                    if (bigGuyLvl3.hitbox.getGlobalBounds().intersects(bomb.bombs[i].hitbox.getGlobalBounds()))
                                    {
                                        bigGuyLvl3.velocity.x = 0;
                                        if (bigGuyLvl3.shape.getTexture() != &bigGuyLvl3.text[3])
                                        {
                                            bigGuyLvl3.shape.setTexture(bigGuyLvl3.text[3]);
                                            //l = 0;
                                            bigGuyLvl3.shape.setTextureRect(IntRect(bigGuyAttackIdx * 77, 0, 77, 74));
                                        }
                                        if (bigGuyTimer <= 0 and !bigGuyLvl3.pick_up and !bigGuyLvl3.bomb_throw)
                                        {
                                            bigGuyAttackIdx++;
                                            bigGuyAttackIdx %= 8;
                                            //cout << l << '\n';
                                            if (bigGuyAttackIdx == 5)
                                            {
                                                bomb.bombs[i].shape.setPosition(bigGuyLvl3.hitbox.getPosition());
                                                bigGuyLvl3.pick_up = false;
                                                bigGuyLvl3.bomb_throw = true;
                                            }
                                            //cout << l << " ";
                                            bigGuyLvl3.shape.setTextureRect(IntRect(bigGuyAttackIdx * 77, 0, 77, 74));
                                            bigGuyTimer = delay * 3;
                                        }
                                        else
                                            bigGuyTimer -= dt;
                                    }
                                    else bigGuyAttackIdx = 0;
                                }
                                if (!bigGuyLvl3.pick_up and bigGuyLvl3.bomb_throw and bigGuyLvl3.hitbox.getGlobalBounds().intersects(bomb.bombs[i].hitbox.getGlobalBounds()))
                                {
                                    if (bigGuyLvl3.shape.getTexture() != &bigGuyLvl3.text[4])
                                    {
                                        if (!oneTime_bigGuy_anim)
                                        {
                                            bigGuyAttackIdx = 0;
                                            oneTime_bigGuy_anim = !oneTime_bigGuy_anim;
                                        }
                                        bigGuyLvl3.shape.setTexture(bigGuyLvl3.text[4]);
                                        bigGuyLvl3.shape.setTextureRect(IntRect(bigGuyAttackIdx * 77, 0, 77, 74));
                                    }
                                    if (throwAnim <= 0 and bigGuyLvl3.bomb_throw)
                                    {
                                        bigGuyAttackIdx++;
                                        bigGuyAttackIdx %= 11;
                                        if (bigGuyAttackIdx == 4)
                                        {
                                            bomb.bombs[i].shape.setPosition(bigGuyLvl3.hitbox.getPosition());
                                            bomb.bombs[i].velocity.y = -13 * sin(initAngle);
                                            //cout << bomb.bombs[i].velocity.y << '\n';
                                            if (pirate.shape.getPosition().x > bigGuyLvl3.shape.getPosition().x)
                                                bigGuyLvl3.shape.setScale(2.f, 2.f);
                                            else if (pirate.shape.getPosition().x < bigGuyLvl3.shape.getPosition().x)
                                                bigGuyLvl3.shape.setScale(-2.f, 2.f);
                                            bomb.bombs[i].velocity.x = (bigGuyLvl3.shape.getScale().x > 0) ? 10 * cos(initAngle) : -10 * cos(initAngle);

                                        }
                                        if (bigGuyAttackIdx == 10)
                                        {
                                            bigGuyLvl3.bomb_throw = false;
                                        }
                                        bigGuyLvl3.shape.setTextureRect(IntRect(bigGuyAttackIdx * 77, 0, 77, 74));
                                        throwAnim = delay * 3;
                                    }
                                    else throwAnim -= dt;
                                }

                            }
                        //Bald guy kicks bomb
                        if (bald_lvl3.isAlive)
                            if (bald_lvl3.bombRange.getGlobalBounds().intersects(bomb.bombs[i].hitbox.getGlobalBounds()))
                            {
                                if (bald_lvl3.hitbox.getPosition().x < bomb.bombs[i].hitbox.getPosition().x)// Bomb is on the right side
                                {
                                    animate(bald_lvl3.shape, bald_lvl3.text[1], baldAnimIdx, 63, 67, baldTimer, 14, 1, 1.75f);
                                    bald_lvl3.velocity.x = 90.f * dt;
                                    if (bald_lvl3.hitbox.getGlobalBounds().intersects(bomb.bombs[i].hitbox.getGlobalBounds()))
                                    {
                                        sound.kickSFX.play();
                                        bald_lvl3.velocity.x = 0;
                                        bomb.bombs[i].shape.move(0, bomb.bombs[i].velocity.y);
                                        if (bald_lvl3.shape.getTexture() != &bald_lvl3.text[2])
                                        {

                                            bald_lvl3.shape.setTexture(bald_lvl3.text[2]);
                                            //c = 0;
                                            bald_lvl3.shape.setTextureRect(IntRect(baldAttackIdx * 63, 0, 63, 67));
                                        }
                                        if (baldTimer <= 0 and !bald_lvl3.pick_up)
                                        {
                                            baldAttackIdx++;
                                            baldAttackIdx %= 12; // 0 -> 11
                                            if (baldAttackIdx == 6) bald_lvl3.shoot = true;
                                            if (baldAttackIdx == 11) bald_lvl3.pick_up = true;

                                            bald_lvl3.shape.setTextureRect(IntRect(baldAttackIdx * 63, 0, 63, 67));
                                            baldTimer = delay * 3;
                                        }
                                        else
                                            baldTimer -= dt;

                                    }
                                    else baldAttackIdx = 0;
                                }
                                else if (bald_lvl3.hitbox.getPosition().x > bomb.bombs[i].hitbox.getPosition().x)// Bomb is on the left side
                                {
                                    animate(bald_lvl3.shape, bald_lvl3.text[1], baldAnimIdx, 63, 67, baldTimer, 14, -1, 1.75f);
                                    //cout << baldAnimIdx << "\n";
                                    bald_lvl3.velocity.x = -90.f * dt;
                                    if (bald_lvl3.hitbox.getGlobalBounds().intersects(bomb.bombs[i].hitbox.getGlobalBounds()))
                                    {
                                        sound.kickSFX.play();
                                        bald_lvl3.velocity.x = 0;
                                        if (bald_lvl3.shape.getTexture() != &bald_lvl3.text[2])
                                        {
                                            bald_lvl3.shape.setTexture(bald_lvl3.text[2]);
                                            //c = 0;
                                            bald_lvl3.shape.setTextureRect(IntRect(baldAttackIdx * 63, 0, 63, 67));
                                        }
                                        if (baldTimer <= 0 and !bald_lvl3.pick_up)
                                        {

                                            baldAttackIdx++;
                                            baldAttackIdx %= 12;
                                            //cout << baldAttackIdx << '\n';
                                            if (baldAttackIdx == 6) bald_lvl3.shoot = true;
                                            if (baldAttackIdx == 11)bald_lvl3.pick_up = true;
                                            bald_lvl3.shape.setTextureRect(IntRect(baldAttackIdx * 63, 0, 63, 67));
                                            baldTimer = delay * 3;
                                        }
                                        else
                                            baldTimer -= dt;
                                    }
                                    else baldAttackIdx = 0;
                                }

                            }
                        // bald shoot
                        if (bald_lvl3.shoot)
                        {
                            bomb.bombs[i].velocity.y = -10.15f * sin(initAngle);
                            if (pirate.shape.getPosition().x > bald_lvl3.shape.getPosition().x)
                                bald_lvl3.shape.setScale(2.f, 2.f);
                            else if (pirate.shape.getPosition().x < bald_lvl3.shape.getPosition().x)
                                bald_lvl3.shape.setScale(-2.f, 2.f);
                            bomb.bombs[i].velocity.x = (bald_lvl3.shape.getScale().x > 0) ? 10 * cos(initAngle) : -10 * cos(initAngle);
                            bald_lvl3.shoot = false;

                        }


                        if (whaleLvl3.isAlive)
                        {
                            if (whaleLvl3.range.getGlobalBounds().intersects(bomb.bombs[i].hitbox.getGlobalBounds()) and u < 10)
                            {

                                whaleLvl3.bombInRange = true;
                                //left
                                if (bomb.bombs[i].hitbox.getPosition().x < whaleLvl3.hitbox.getPosition().x and !whaleLvl3.isJumping and whaleLvl3.isAlive and !isSwallowed[i]
                                    and !(bomb.bombs[i].hitbox.getPosition().y + bomb.bombs[i].hitbox.getGlobalBounds().height * 0.25f < whaleLvl3.hitbox.getPosition().y - whaleLvl3.hitbox.getGlobalBounds().height * 0.25f)
                                    and whaleLvl3.hitbox.getPosition().y > bomb.bombs[i].hitbox.getPosition().y
                                    )
                                {
                                    if (!whaleLvl3.attack and whaleLvl3.isAlive and !whaleLvl3.isSwallowing)
                                    {
                                        animate(whaleLvl3.shape, whaleLvl3.textures[1], whaleAnimIdx, 68, 46, whaleTimer, 14, 1, 1.5f);
                                        cout << whaleAnimIdx << endl;
                                        whaleLvl3.velocity.x = -80.f * dt;
                                    }

                                    if (bomb.bombs[i].hitbox.getGlobalBounds().intersects(whaleLvl3.hitbox.getGlobalBounds()) and u < 10)
                                    {
                                        whaleLvl3.isSwallowing = true;
                                        bomb.bombs[i].velocity = { 0,0 };
                                        whaleLvl3.velocity.x = 0;
                                        if (whaleLvl3.SwallowTimer >= 60)
                                        {
                                            if (whaleLvl3.shape.getTexture() != &whaleLvl3.textures[5])
                                            {
                                                whaleSwallowIdx = 0;
                                                whaleLvl3.shape.setTexture(whaleLvl3.textures[5]);
                                                whaleLvl3.shape.setTextureRect(IntRect(whaleSwallowIdx * 68, 0, 68, 46));
                                            }
                                            if (whaleTimer <= 0)
                                            {
                                                whaleSwallowIdx++;
                                                whaleSwallowIdx %= 10;
                                                if (whaleSwallowIdx == 8)
                                                {
                                                    u += 5;
                                                    sound.gulpsound.play();
                                                    sound.bombsound.stop();
                                                    bomb.bombs[i].shape.setPosition(whaleLvl3.hitbox.getPosition());
                                                    whaleLvl3.isSwallowing = false;
                                                    whaleLvl3.bombInRange = false;
                                                    bomb.bombs.erase(bomb.bombs.begin() + i);
                                                    whaleLvl3.SwallowTimer = -1;
                                                    memset(isSwallowed, false, sizeof(isSwallowed));
                                                    break;
                                                }
                                                whaleLvl3.shape.setTextureRect(IntRect(whaleSwallowIdx * 68, 0, 68, 46));
                                                whaleTimer = delay;
                                            }
                                            else whaleTimer -= dt;
                                        }
                                    }
                                    else whaleSwallowIdx = 0;
                                }
                                else if (bomb.bombs[i].hitbox.getPosition().x > whaleLvl3.hitbox.getPosition().x and !whaleLvl3.isJumping and whaleLvl3.isAlive and !isSwallowed[i]
                                    and !(bomb.bombs[i].hitbox.getPosition().y + bomb.bombs[i].hitbox.getGlobalBounds().height * 0.25f < whaleLvl3.hitbox.getPosition().y - whaleLvl3.hitbox.getGlobalBounds().height * 0.25f)
                                    and whaleLvl3.hitbox.getPosition().y > bomb.bombs[i].hitbox.getPosition().y
                                    )
                                {
                                    if (!whaleLvl3.attack and whaleLvl3.isAlive and !whaleLvl3.isSwallowing)
                                    {
                                        animate(whaleLvl3.shape, whaleLvl3.textures[1], whaleAnimIdx, 68, 46, whaleTimer, 14, -1, 1.5f);
                                        whaleLvl3.velocity.x = 80.f * dt;
                                    }
                                    if (bomb.bombs[i].hitbox.getGlobalBounds().intersects(whaleLvl3.hitbox.getGlobalBounds()))
                                    {
                                        whaleLvl3.isSwallowing = true;
                                        bomb.bombs[i].velocity = { 0,0 };
                                        whaleLvl3.velocity.x = 0;
                                        if (whaleLvl3.SwallowTimer >= 60)
                                        {
                                            if (whaleLvl3.shape.getTexture() != &whaleLvl3.textures[5])
                                            {
                                                whaleSwallowIdx = 0;
                                                whaleLvl3.shape.setTexture(whaleLvl3.textures[5]);
                                                whaleLvl3.shape.setTextureRect(IntRect(whaleSwallowIdx * 68, 0, 68, 46));
                                            }
                                            if (whaleTimer <= 0)
                                            {
                                                whaleSwallowIdx++;
                                                whaleSwallowIdx %= 10;
                                                if (whaleSwallowIdx == 8)
                                                {
                                                    u += 5;
                                                    sound.gulpsound.play();
                                                    sound.bombsound.stop();
                                                    bomb.bombs[i].shape.setPosition(whaleLvl3.hitbox.getPosition());
                                                    isSwallowed[i] = true;
                                                    whaleLvl3.isSwallowing = false;
                                                    whaleLvl3.bombInRange = false;
                                                    bomb.bombs.erase(bomb.bombs.begin() + i);
                                                    whaleLvl3.SwallowTimer = -1;
                                                    memset(isSwallowed, false, sizeof(isSwallowed));
                                                    break;
                                                }
                                                whaleLvl3.shape.setTextureRect(IntRect(whaleSwallowIdx * 68, 0, 68, 46));
                                                whaleTimer = delay;
                                            }
                                            else whaleTimer -= dt;
                                        }
                                    }
                                    else whaleSwallowIdx = 0;
                                }
                            }
                            else whaleLvl3.bombInRange = false;
                        }


                        //bomb stuff
                        if (bomb.bombs[i].detonateTimer < TimeToDetonate)
                            bomb.bombs[i].detonateTimer++;
                        else
                        {
                            if (bomb.bombs[i].shape.getTexture() != &bomb.bombs[i].bombOffTex)
                            {
                                bombAnimIdx = 0;
                                bomb.bombs[i].shape.setTexture(bomb.bombs[i].bombOffTex);
                                bomb.bombs[i].shape.setTextureRect(IntRect(bombAnimIdx * 96, 0, 96, 108));
                            }
                            if (bombAnimTime <= 0)
                            {
                                bombAnimIdx++;
                                bombAnimIdx %= 9; // 0->8
                                bomb.bombs[i].shape.setTextureRect(IntRect(bombAnimIdx * 96, 0, 96, 108));
                                bombAnimTime = bombDelay;
                            }
                            else bombAnimTime -= dt;
                            if (bombAnimIdx == 1)
                            {
                                for (int j = 0; j < bomb.bombs.size(); ++j)
                                {
                                    if (j == i)continue;
                                    if (bomb.bombs[i].shape.getGlobalBounds().intersects(bomb.bombs[j].shape.getGlobalBounds()))
                                    {
                                        if (bomb.bombs[i].hitbox.getPosition().x < bomb.bombs[j].hitbox.getPosition().x and bomb.bombs[i].hitbox.getPosition().y == bomb.bombs[j].hitbox.getPosition().y)
                                        {
                                            bomb.bombs[j].velocity.x += 5.f;
                                        }
                                        else if (bomb.bombs[i].hitbox.getPosition().x > bomb.bombs[j].hitbox.getPosition().x and bomb.bombs[i].hitbox.getPosition().y == bomb.bombs[j].hitbox.getPosition().y)
                                        {
                                            bomb.bombs[j].velocity.x -= 5.f;
                                        }
                                    }

                                }
                                if (pirate.hitbox.getGlobalBounds().intersects(bomb.bombs[i].range.getGlobalBounds()) and !pirate.isHit)
                                {
                                    sound.livesdec.play();
                                    pirate.isHit = true;
                                    playerHearts--;
                                }
                                if (bigGuyLvl3.hitbox.getGlobalBounds().intersects(bomb.bombs[i].range.getGlobalBounds()))
                                {
                                    bigGuyLvl3.isAlive = false;
                                }
                                if (bald_lvl3.hitbox.getGlobalBounds().intersects(bomb.bombs[i].range.getGlobalBounds()))
                                {
                                    bald_lvl3.isAlive = false;
                                }
                                if (whaleLvl3.hitbox.getGlobalBounds().intersects(bomb.bombs[i].range.getGlobalBounds()) and !isSwallowed[i])
                                {
                                    //cout << "dead\n";
                                    whaleLvl3.isAlive = false;
                                }
                                if (captain.hitbox.getGlobalBounds().intersects(bomb.bombs[i].range.getGlobalBounds()))
                                {
                                    captain.isAlive = false;
                                }

                            }
                            if (bombAnimIdx == 1)
                            {
                                sound.bombsound.play();
                            }
                            if (bombAnimIdx == 8) {
                                if (CollisionDetection(bomb.bombs[i].hitbox, captain.range))
                                    captain.escape = false;
                                memset(isSwallowed, false, sizeof(isSwallowed));
                                bigGuyLvl3.pick_up = false;
                                bald_lvl3.pick_up = false;
                                whaleLvl3.isSwallowing = false;
                                whaleLvl3.bombInRange = false;
                                bomb.bombs.erase(bomb.bombs.begin() + i);
                                break;
                            }
                        }
                    }
                }
            if (whaleLvl3.SwallowTimer < 60)
            {
                whaleLvl3.SwallowTimer++;
            }

            //death animation
            if (!bigGuyLvl3.isAlive)
            {

                dead_animation(bigGuyLvl3.shape, bigGuyLvl3.text[6], bigGuyAnimIdx, 77, 74, bigGuyTimer, 6, (bigGuyLvl3.shape.getScale().x > 0.f) ? 1 : -1, 2.f, 5, 2);
                bigGuyLvl3.velocity = { 0,0 };
                bigGuyLvl3.shape.setPosition(bigGuyLvl3.shape.getPosition().x, 830.f);
            }


            if (!bald_lvl3.isAlive)
            {
                dead_animation(bald_lvl3.shape, bald_lvl3.text[3], baldAnimIdx, 63, 67, baldTimer, 6, (bald_lvl3.shape.getScale().x > 0.f) ? 1 : -1, 1.5f, 5, 2);
                bald_lvl3.velocity = { 0,0 };
                bald_lvl3.shape.setPosition(bald_lvl3.shape.getPosition().x, 360);
            }

            if (!whaleLvl3.isAlive)
            {
                dead_animation(whaleLvl3.shape, whaleLvl3.textures[6], whaleAnimIdx, 68, 46, whaleTimer, 6, (whaleLvl3.shape.getScale().x > 0.f) ? 1 : -1, 1.5f, 5, 2);
                whaleLvl3.velocity = { 0,0 };
                whaleLvl3.shape.setPosition(whaleLvl3.shape.getPosition().x, whaleLvl3.shape.getPosition().y);
            }

            if (!captain.isAlive)
            {
                dead_animation(captain.shape, captain.text[3], captainAnimIdx, 80, 72, captainTimer, 6, (captain.shape.getScale().x > 0) ? 1 : -1, 1.8f, 5, 2);
                captain.velocity = { 0,0 };
                captain.shape.setPosition(captain.shape.getPosition().x, 900.f);
            }

            if (pirate.isHit)
            {
                if (pirate.isHit and playerHearts > 0)
                {
                    animate(pirate.shape, pirate.textures[8], captainAnimIdx, 58, 58, pirateTimer, 8, (pirate.shape.getScale().x > 0.f) ? 1 : -1, 2.f);
                    /*if (captainAnimIdx == 7)
                        pirate.isHit = false;*/
                }
                else if (playerHearts <= 0)
                {
                    dead_animation(pirate.shape, pirate.textures[9], pirateAnimIdx, 58, 58, pirateTimer, 6, (pirate.shape.getScale().x > 0.f) ? 1 : -1, 2.f, 5);
                    if (pirateAnimIdx == 5)
                        pirateAnimIdx++;
                    pirate.velocity = { 0,0 };
                }

            }


            //===========================================================================================

            if (pirate.isHit)
            {
                if (pirate.isHit and playerHearts > 0)
                {
                    animate(pirate.shape, pirate.textures[8], captainAnimIdx, 58, 58, pirateTimer, 8, (pirate.shape.getScale().x > 0.f) ? 1 : -1, 1.8f);
                    if (captainAnimIdx == 7)
                        pirate.isHit = false;
                }
                else if (playerHearts <= 0)
                {
                    dead_animation(pirate.shape, pirate.textures[9], pirateAnimIdx, 58, 58, pirateTimer, 6, (pirate.shape.getScale().x > 0.f) ? 1 : -1, 1.8f, 5);
                    pirate.velocity = { 0,0 };
                }

            }

            //Kill switch to test door animation after all enemies are conquered

            if (Keyboard::isKeyPressed(Keyboard::Up))
            {
                // sound.livesdec.play();
                pirate.isAlive = 1;
                bald_lvl3.isAlive = 0;
                whaleLvl3.isAlive = 0;
                bigGuyLvl3.isAlive = 0;
                captain.isAlive = 0;
            }

            //door animation + player entering in animation after all enemies are conquered
            if (pirate.isAlive and !bald_lvl3.isAlive and !whaleLvl3.isAlive and !bigGuyLvl3.isAlive and !captain.isAlive)
            {
                levelFinished = true;

                if (door2.frame <= door2.mxframe) {
                    if (door2.frame == 0) sound.doorsfx.play();
                    doorTimer2 += dt;
                    if (doorTimer2 > 0.8f) {
                        doorTimer2 = 0;
                        door2.frame++;
                        door2.shape.setTextureRect(IntRect(door2.frame * 78, 0, 78, 98));
                    }
                }
            }

            if (doorBar.getGlobalBounds().intersects(pirate.hitbox.getGlobalBounds()) and levelFinished)
            {
                pirate.doorInControl = true;
                pirate.velocity = { 0,0 };

                if (pirate.shape.getTexture() != &pirate.textures[5])
                {
                    pirate.shape.setTexture(pirate.textures[5]);
                    pirateAnimIdx = 0;
                    pirate.shape.setTextureRect(IntRect(pirateAnimIdx * 58, 0, 58, 58));
                }
                if (pirateAnimIdx < 13)
                {
                    if (pirateTimer <= 0)
                    {
                        pirateAnimIdx++;
                        pirateAnimIdx %= 14;
                        pirate.shape.setTextureRect(IntRect(pirateAnimIdx * 58, 0, 58, 58));
                        pirateTimer = delay * 1.5;
                    }
                    else pirateTimer -= dt;
                }
                else
                    gameWon = true;
            }






            pirate.shape.move(pirate.velocity);
            bald_lvl3.shape.move(bald_lvl3.velocity);
            captain.shape.move(captain.velocity);
            bigGuyLvl3.shape.move(bigGuyLvl3.velocity);
            whaleLvl3.shape.move(whaleLvl3.velocity);

            //======================================UI===================================


            window.clear();
            window.draw(back);
            window.draw(healthBar);

            if (bombcounter > 0)
            {
                window.draw(bombnum);
            }
            else
            {
                bombnum.setTexture(bombnum_tex[0]);
                window.draw(bombnum);
            }

            if (pirate.isAlive)
                for (int i = 0; i < playerHearts; i++)
                {
                    window.draw(heart[i]);
                }
            window.draw(door.shape);
            window.draw(door2.shape);
            // window.draw(doorBar);
            window.draw(bigGuyLvl3.shape);
            // window.draw(bigGuyLvl2.hitbox);
            // window.draw(bigGuyLvl2.range);


            window.draw(bald_lvl3.shape);
            //window.draw(bald_lvl2.range);
            //window.draw(bald_lvl2.hitbox);

            window.draw(whaleLvl3.shape);
            if (u > 0 and whaleLvl3.isAlive)
                window.draw(whaleLvl3.hungerBar);


            window.draw(captain.shape);
            // window.draw(captain.range);
             //window.draw(captain.hitbox);

            if (door_opened)
                window.draw(pirate.shape);
            //window.draw(pirate.hitbox);
            if (!pirate.isJumping)
            {
                if (Keyboard::isKeyPressed(Keyboard::D) or Keyboard::isKeyPressed(Keyboard::A))
                    window.draw(pirate.particle);
            }
            else if (pirate.isJumping and !pirate.stop_jumpParticles)
                window.draw(pirate.particle);



            for (int i = 0; i < bomb.bombs.size(); i++)
            {
                /*if (isSwallowed[i])continue;*/
                // window.draw(bomb.bombs[i].hitbox);
                window.draw(bomb.bombs[i].shape);
                //window.draw(bomb.bombs[i].range);
            }



            for (int i = 0; i < 10; i++)
            {
                window.draw(objects[i]);

            }

            for (int i = 0; i < 8; i++)
            {
                window.draw(platforms[i]);

            }
        }


        //=========================================== End level UI========================================

        if (bombcounter <= 0 and !gameWon)
        {
            gameEnded = true;
            while (window.pollEvent(event))
            {
                if (event.type == Event::Closed || event.type == Event::KeyPressed && event.key.code == Keyboard::Escape)
                {
                    window.close();
                }
                if (event.type == Event::KeyReleased)
                {
                    if (event.key.code == Keyboard::Up)
                    {
                        sound.menu_select.play();
                        mainMenu.MoveButtonUp();

                    }
                    if (event.key.code == Keyboard::Down)
                    {
                        //cout << "im here\n";
                        sound.menu_select.play();
                        mainMenu.MoveButtonDown();

                    }
                    if (event.key.code == Keyboard::Return)
                    {
                        if (mainMenu.ButtonPressed() == 0)
                        {
                            sound.press.play();
                            gameEnded = false;
                            pirate.shape.setScale(0, 0);
                            window.display();
                            return;
                        }
                        if (mainMenu.ButtonPressed() == 1)
                        {
                            sound.press.play();
                            phase = -1;
                            return;
                        }
                    }
                }
            }
            window.draw(deathUI[3]);
            window.draw(deathUI[2]);
            window.draw(mainMenu.ButtonsText[0]);
            window.draw(mainMenu.ButtonsText[1]);
            mainMenu.drawButtons(window);
        }

        //DEATH SCREEN
        if (playerHearts <= 0 and bomb.bombs.empty() and pirateAnimIdx == 6)
        {
            gameEnded = true;
            while (window.pollEvent(event))
            {
                if (event.type == Event::Closed || event.type == Event::KeyPressed && event.key.code == Keyboard::Escape)
                {
                    window.close();
                }
                if (event.type == Event::KeyReleased)
                {
                    if (event.key.code == Keyboard::Up)
                    {
                        sound.menu_select.play();
                        mainMenu.MoveButtonUp();

                    }
                    if (event.key.code == Keyboard::Down)
                    {
                        //cout << "im here\n";
                        sound.menu_select.play();
                        mainMenu.MoveButtonDown();

                    }
                    if (event.key.code == Keyboard::Return)
                    {
                        if (mainMenu.ButtonPressed() == 0)
                        {
                            sound.press.play();
                            gameEnded = false;
                            pirate.shape.setScale(0, 0);
                            window.display();
                            return;
                        }
                        if (mainMenu.ButtonPressed() == 1)
                        {
                            sound.press.play();
                            phase = -1;
                            return;
                        }
                    }
                }
            }
            window.draw(deathUI[0]);
            window.draw(deathUI[2]);
            window.draw(mainMenu.ButtonsText[0]);
            window.draw(mainMenu.ButtonsText[1]);
            mainMenu.drawButtons(window);
        }
        //GameWon
        if (gameWon)
        {
            while (window.pollEvent(event))
            {
                if (event.type == Event::Closed || event.type == Event::KeyPressed && event.key.code == Keyboard::Escape)
                {
                    window.close();
                }
                if (event.type == Event::KeyReleased)
                {

                    if (event.key.code == Keyboard::Return)
                    {
                        phase = 0;
                        window.close();
                        return;
                    }
                }
            }
            window.draw(winUI[0]);
            window.draw(winUI[1]);
            window.draw(mainMenu.ExitText);

        }




        window.display();
    }
}

void creditsMenu(RenderWindow& window)
{
    Texture CreditsBGTxt, CreditsTxt;
    CreditsBGTxt.loadFromFile("./UI/credits_bg_plat.png");
    CreditsTxt.loadFromFile("./UI/credits_names.png");
    Sprite creditsBG, credits;

    creditsBG.setTexture(CreditsBGTxt);
    credits.setTexture(CreditsTxt);
    credits.scale(1.f, 1.f);
    credits.setPosition(0, 850);

    Whale whaleCredits;
    whaleCredits.shape.setTexture(whaleCredits.textures[0]);
    whaleCredits.shape.setScale(2.5f, 2.5f);
    whaleCredits.shape.setTextureRect(IntRect(0, 0, 63, 67));
    whaleCredits.shape.setPosition(1600, 520);
    whaleCredits.shape.setOrigin(whaleCredits.shape.getTextureRect().width / 2, whaleCredits.shape.getTextureRect().height / 2);

    Captain captainCredits;
    captainCredits.shape.setTexture(captainCredits.text[2]);
    captainCredits.shape.setScale(2.5f, 2.5f);
    captainCredits.shape.setTextureRect(IntRect(0, 0, 80, 72));
    captainCredits.shape.setPosition(150, 707.5f);
    captainCredits.shape.setOrigin(captainCredits.shape.getTextureRect().width / 2, captainCredits.shape.getTextureRect().height / 2);

    Clock clock;

    while (true)
    {
        dt = clock.restart().asSeconds();
        Event event;


        window.setFramerateLimit(60); //set it according to your monitor's refresh rate
        window.setVerticalSyncEnabled(true); //to avoid screen tearing

        while (window.pollEvent(event))
        {

            if (event.type == Event::Closed || event.type == Event::KeyPressed && event.key.code == Keyboard::Escape)
            {
                return;
            }

        }

        if (credits.getPosition().y > 0)
            credits.move(0, -2.5f);

        animate(whaleCredits.shape, whaleCredits.textures[0], whaleAnimIdx, 68, 46, whaleTimer, 44, 1, 2.5);


        if (captainCredits.shape.getPosition().x <= 550 and captainCredits.turn == false)
        {
            animate(captainCredits.shape, captainCredits.text[2], captainAttackIdx, 80, 74, captainTimer, 12, 1, 2.5f);
            captainCredits.velocity.x = 110 * dt;

        }
        else
        {

            captainCredits.velocity.x = -110 * dt;
            captainCredits.turn = true;
        }

        if (captainCredits.shape.getPosition().x >= 145 and captainCredits.turn == true)
        {
            animate(captainCredits.shape, captainCredits.text[2], captainAttackIdx, 80, 74, captainTimer, 12, -1, 2.5f);
            captainCredits.velocity.x = -110 * dt;

        }
        else
        {
            captainCredits.velocity.x = 110 * dt;
            captainCredits.turn = false;
        }

        captainCredits.shape.move(captainCredits.velocity);


        window.clear();
        window.draw(creditsBG);
        window.draw(whaleCredits.shape);
        window.draw(captainCredits.shape);
        window.draw(credits);
        window.display();
    }
}

void optionsMenu(RenderWindow& window)
{
    Texture optionsBack, onSfxTex, offSfxTex, onMusicTex, offMusicTex;
    optionsBack.loadFromFile("./UI/options.jpg");
    onSfxTex.loadFromFile("./UI/sfx_on.png");
    offSfxTex.loadFromFile("./UI/sfx_off.png");
    onMusicTex.loadFromFile("./UI/music_on.png");
    offMusicTex.loadFromFile("./UI/music_off.png");
    Sprite back, onSfx, offSfx, onMusic, offMusic;
    back.setTexture(optionsBack);
    onSfx.setTexture(onSfxTex);
    offSfx.setTexture(offSfxTex);
    onMusic.setTexture(onMusicTex);
    offMusic.setTexture(offMusicTex);
    Clock clock;
    while (true)
    {
        dt = clock.restart().asSeconds();
        Event event;


        window.setFramerateLimit(60); //set it according to your monitor's refresh rate
        window.setVerticalSyncEnabled(true); //to avoid screen tearing

        while (window.pollEvent(event))
        {

            if (event.type == Event::Closed || event.type == Event::KeyPressed && event.key.code == Keyboard::Escape)
            {
                return;
            }

        }

        //  cout << "x: " << Mouse::getPosition().x << "y: " << Mouse::getPosition().y << endl;


        window.clear();
        window.draw(back);
        window.draw(onSfx);
        window.draw(offSfx);
        window.draw(onMusic);
        window.draw(offMusic);
        window.display();
    }
}

//============================Functions============================

//Pirate


void Pirate_move_code(float scale)
{
    //-------------------------------------Pirate----------------------------------------------\\
    
    if (!gameEnded)
        if (!pirate.isHit and !pirate.doorInControl and playerHearts > 0)
        {
            //right movement and animation
            if (player_init) {
                if (Keyboard::isKeyPressed(Keyboard::D) or Keyboard::isKeyPressed(Keyboard::Right))
                {
                    /*sound.walking.setLoop(true);
                    sound.walking.play();*/
                    pirate.shape.setScale(scale, scale);
                    if (!pirate.isJumping)animate(pirate.shape, pirate.textures[1], pirateAnimIdx, 58, 58, pirateTimer, 14, 1, scale);
                    pirate.velocity.x = 150 * dt;

                    if (!pirate.isJumping) {
                        // pirate.particle.setScale(2.f, 2.f);
                        pirate.particle.setPosition(pirate.shape.getPosition().x - 30, pirate.shape.getPosition().y + 45);
                        animate(pirate.particle, pirate.textures[6], particleAnimIdx, 12, 10, particleTimer, 6, 1, scale);
                    }
                }
                //left movement and animation
                if (Keyboard::isKeyPressed(Keyboard::A) or Keyboard::isKeyPressed(Keyboard::Left))
                {
                    pirate.shape.setScale(-scale, scale);
                    if (!pirate.isJumping)
                        animate(pirate.shape, pirate.textures[1], pirateAnimIdx, 58, 58, pirateTimer, 14, -1, scale);
                    pirate.velocity.x = -150 * dt;

                    if (!pirate.isJumping) {
                        // pirate.particle.setScale(-2.f, 2.f);
                        pirate.particle.setPosition(pirate.shape.getPosition().x + 30, pirate.shape.getPosition().y + 45);
                        animate(pirate.particle, pirate.textures[6], particleAnimIdx, 12, 10, particleTimer, 6, -1, scale);
                    }
                }

                if (!pirate.isJumping)pirate.stop_jumpParticles = false;
                //jumping and animation
                if (Keyboard::isKeyPressed(Keyboard::Space) && !isPressed[(Keyboard::Space)])
                {
                    isPressed[(Keyboard::Space)] = 1;
                    sound.jumpSFX.play();
                    if (!pirate.isJumping)
                    {
                        pirate.isJumping = true;
                        // cout << "jumpp";
                        pirate.velocity.y = jumpVelocity * dt;
                        pirate.shape.setTexture(pirate.textures[2]);
                        pirateAnimIdx = 0;
                        pirate.shape.setTextureRect(IntRect(pirateAnimIdx * 58, 0, 58, 58));
                    }
                    if (pirateTimer <= 0 and pirate.velocity.y < 0)
                    {
                        pirateAnimIdx++;
                        pirateAnimIdx %= 4;
                        pirate.shape.setTextureRect(IntRect(pirateAnimIdx * 58, 0, 58, 58));
                        pirateTimer = delay;
                    }
                    else if (pirate.velocity.y < 0) pirateTimer -= dt;
                }
                else
                {
                    isJumping = false;
                }
                //jump particles
                if (pirate.isJumping)
                {
                    pirate.particle.setScale(2.f, 2.f);
                    pirate.particle.setPosition(pirate.shape.getPosition().x - 20, pirate.shape.getPosition().y + 20);
                    animate(pirate.particle, pirate.textures[7], particleAnimIdx, 40, 28, particleTimer, 6, 1, 2.f);
                    if (particleAnimIdx == 5)
                        pirate.stop_jumpParticles = true;

                }

                //falling and animation
                if (pirate.velocity.y >= 0 and pirate.isJumping)
                {
                    animate(pirate.shape, pirate.textures[3], pirateAnimIdx, 58, 58, pirateTimer, 2, (pirate.shape.getScale().x > 0) ? 1 : -1, scale, 50 / 3.f);
                }

                //Pirate idle animation
                if (!Keyboard::isKeyPressed(Keyboard::D) and !Keyboard::isKeyPressed(Keyboard::A) and !Keyboard::isKeyPressed(Keyboard::Space) and !Keyboard::isKeyPressed(Keyboard::Left) and !Keyboard::isKeyPressed(Keyboard::Right) and !pirate.isJumping)
                {
                    pirate.velocity.x = 0;
                    animate(pirate.shape, pirate.textures[0], pirateAnimIdx, 58, 58, pirateTimer, 24, (pirate.shape.getScale().x > 0) ? 1 : -1, scale);
                }

            }
        }
}


//Big guy
void big_guy_code(Big_guy& big_guy, int dir1, int dir2)
{
    if (!gameEnded)
        if (big_guy.isAlive)
        {
            // Attack big guy
            if (pirate.hitbox.getGlobalBounds().intersects(big_guy.hitbox.getGlobalBounds()) and pirate.hitbox.getGlobalBounds().intersects(big_guy.range.getGlobalBounds()) and pirate.isAlive)
            {

                exclamation[0].playerInRange = false;
                if (pirate.hitbox.getPosition().x > big_guy.hitbox.getPosition().x)
                {
                    animate(big_guy.shape, big_guy.text[2], bigGuyAnimIdx, 77, 74, bigGuyTimer, 11, 1, 2.f);
                    if (bigGuyTimer <= 0)
                    {
                        if (bigGuyAnimIdx == 4 and pirate.hitbox.getGlobalBounds().intersects(big_guy.hitbox.getGlobalBounds()))
                        {
                            sound.hitSFX.play();
                            sound.livesdec.play();
                            pirate.isHit = true;
                            playerHearts--;
                        }
                    }

                    big_guy.velocity.x = 0;

                }

                else if (pirate.hitbox.getPosition().x < big_guy.hitbox.getPosition().x)
                {
                    animate(big_guy.shape, big_guy.text[2], bigGuyAnimIdx, 77, 74, bigGuyTimer, 11, -1, 2.f);
                    if (bigGuyTimer <= 0)
                    {
                        if (bigGuyAnimIdx == 4 and pirate.hitbox.getGlobalBounds().intersects(big_guy.hitbox.getGlobalBounds()))
                        {
                            sound.hitSFX.play();
                            sound.livesdec.play();
                            pirate.isHit = true;
                            playerHearts--;
                        }
                    }

                    big_guy.velocity.x = 0;

                }
                //hitSFX.play();
            }
            //Walking big guy
            else if (!pirate.hitbox.getGlobalBounds().intersects(big_guy.range.getGlobalBounds()))
            {
                exclamation[0].playerInRange = false;
                if (big_guy.shape.getPosition().x < dir2 and big_guy.turn == false)
                {

                    animate(big_guy.shape, big_guy.text[1], bigGuyAnimIdx, 77, 74, bigGuyTimer, 16, 1, 2.f);
                    big_guy.velocity.x = 60 * dt;
                }
                else
                {
                    big_guy.velocity.x = -60 * dt;
                    big_guy.turn = true;
                }

                if (big_guy.shape.getPosition().x >= dir1 and big_guy.turn == true)
                {
                    animate(big_guy.shape, big_guy.text[1], bigGuyAnimIdx, 77, 74, bigGuyTimer, 16, -1, 2.f);

                    big_guy.velocity.x = -60 * dt;
                }
                else
                {
                    big_guy.velocity.x = 60 * dt;
                    big_guy.turn = false;
                }
            }
            //  Big guy follow player
            else if (pirate.isAlive)
            {
                exclamation[0].playerInRange = true;
                if (pirate.hitbox.getPosition().x > big_guy.hitbox.getPosition().x)
                {
                    animate(big_guy.shape, big_guy.text[1], bigGuyAnimIdx, 77, 74, bigGuyTimer, 16, 1, 2.f);
                    exclamation_ani(0);
                    big_guy.velocity.x = 60 * dt;

                }

                if (pirate.hitbox.getPosition().x < big_guy.hitbox.getPosition().x)
                {
                    animate(big_guy.shape, big_guy.text[1], bigGuyAnimIdx, 77, 74, bigGuyTimer, 16, -1, 2.f);
                    exclamation_ani(0);
                    big_guy.velocity.x = -60 * dt;

                }

            }
        }


}

//Bald guy

void bald_code(Bald_guy& bald, int dir1, int dir2, float scale)
{
    if (!gameEnded)
        if (bald.isAlive)
        {
            //Attack bald guy
            if (pirate.isAlive)
                if (pirate.hitbox.getGlobalBounds().intersects(bald.hitbox.getGlobalBounds()) and pirate.hitbox.getGlobalBounds().intersects(bald.range.getGlobalBounds()))
                {

                    if (pirate.hitbox.getPosition().x > bald.hitbox.getPosition().x)
                    {
                        animate(bald.shape, bald.text[2], baldAnimIdx, 63, 67, baldTimer, 12, 1, scale);
                        if (baldTimer <= 0)
                        {
                            if (baldAnimIdx == 3 and bald.hitbox.getGlobalBounds().intersects(pirate.hitbox.getGlobalBounds()))
                            {
                                sound.kickSFX.play();
                                sound.livesdec.play();
                                pirate.isHit = true;
                                playerHearts--;
                            }
                        }

                        bald.velocity.x = 0;

                    }

                    else if (pirate.hitbox.getPosition().x < bald.hitbox.getPosition().x)
                    {
                        animate(bald.shape, bald.text[2], baldAnimIdx, 63, 67, baldTimer, 12, -1, scale);
                        if (baldTimer <= 0)
                        {
                            if (baldAnimIdx == 3 and bald.hitbox.getGlobalBounds().intersects(pirate.hitbox.getGlobalBounds()))
                            {
                                sound.kickSFX.play();
                                sound.livesdec.play();
                                pirate.isHit = true;
                                playerHearts--;
                            }
                        }


                        bald.velocity.x = 0;

                    }

                }
            //Walking bald guy
                else if (!pirate.hitbox.getGlobalBounds().intersects(bald.range.getGlobalBounds())) {
                    exclamation[1].playerInRange = false;
                    if (bald.shape.getPosition().x < dir2 and bald.turn == false)
                    {
                        // bald_guy_run_ani(1);
                        animate(bald.shape, bald.text[1], baldAnimIdx, 63, 67, baldTimer, 14, 1, scale);
                        bald.velocity.x = 50 * dt;

                    }
                    else
                    {

                        bald.velocity.x = -50 * dt;
                        bald.turn = true;
                    }

                    if (bald.shape.getPosition().x >= dir1 and bald.turn == true)
                    {
                        animate(bald.shape, bald.text[1], baldAnimIdx, 63, 67, baldTimer, 14, -1, scale);
                        bald.velocity.x = -50 * dt;

                    }
                    else
                    {
                        bald.velocity.x = 50 * dt;
                        bald.turn = false;
                    }

                }
            //Follow bald guy
                else
                {
                    //cout << "in else" << endl;
                    if (pirate.hitbox.getGlobalBounds().intersects(bald.range.getGlobalBounds()))
                    {
                        exclamation[1].playerInRange = true;
                        if (pirate.hitbox.getPosition().x > bald.hitbox.getPosition().x)
                        {
                            animate(bald.shape, bald.text[1], baldAnimIdx, 63, 67, baldTimer, 14, 1, scale);
                            exclamation_ani(1);

                            bald.velocity.x = 95 * dt;
                        }

                        if (pirate.hitbox.getPosition().x < bald.hitbox.getPosition().x)
                        {
                            animate(bald.shape, bald.text[1], baldAnimIdx, 63, 67, baldTimer, 14, -1, scale);
                            exclamation_ani(1);

                            bald.velocity.x = -95 * dt;
                        }
                    }
                    else
                    {

                    }
                }
        }
}



//Whale
void Whale_code(Whale& whale, int dir, RectangleShape& platform)
{
    if (!gameEnded)
        if (whale.isAlive) {
            //Apply gravity 
            whale.velocity.y += g * dt;

            if (whale.range.getGlobalBounds().intersects(pirate.hitbox.getGlobalBounds()) and pirate.isAlive and !whale.isSwallowing)
            {
                whale.focus = true;
            }
            else if (!whale.isSwallowing)
                whale.focus = false;

            if (whale.shape.getGlobalBounds().intersects(platform.getGlobalBounds()))
            {
                whale.velocity.y = 0;
                whale.isJumping = false;

            }



            if (!whale.focus and whale.isAlive and !whale.isSwallowing and !whale.bombInRange)// idle
            {
                whale.velocity.x = 0;
                if (WhaleJumpCount < 2)
                {
                    animate(whale.shape, whale.textures[0], whaleAnimIdx, 68, 46, whaleTimer, 44, 1, 1.5f);
                    if (whaleTimer <= 0)
                    {
                        if (whaleAnimIdx == 43) WhaleJumpCount++;
                    }

                }
                // jump

                if (WhaleJumpCount >= 2 and !whale.isSwallowing and !whale.bombInRange)
                {
                    if (!whale.isJumping)
                    {
                        whale.isJumping = true;
                        whale.velocity.y = jumpVelocity * dt * 0.65;
                        whale.shape.setTexture(whale.textures[2]);
                        whaleAnimIdx = 0;
                        whale.shape.setTextureRect(IntRect(whaleAnimIdx * 68, 0, 68, 46));
                    }

                    if (whaleTimer <= 0 and whale.velocity.y < 0)
                    {
                        whaleAnimIdx++;
                        whaleAnimIdx %= 4; if (whaleAnimIdx == 3) WhaleJumpCount = 0;
                        whale.shape.setTextureRect(IntRect(whaleAnimIdx * 68, 0, 68, 46));
                        whaleTimer = delay;
                    }
                    else if (whale.velocity.y < 0) whaleTimer -= dt;

                }
                //falling and animation
                if (whale.velocity.y >= 0 and whale.isJumping)
                {
                    if (pirate.shape.getTexture() != &whale.textures[3])
                    {
                        whale.shape.setTexture(whale.textures[3]);
                        whaleAnimIdx = 0;
                        whale.shape.setTextureRect(IntRect(whaleAnimIdx * 68, 0, 68, 46));
                    }
                    if (whaleTimer <= 0)
                    {
                        whaleAnimIdx++;
                        whaleAnimIdx %= 2; if (whaleAnimIdx == 1);
                        whale.shape.setTextureRect(IntRect(whaleAnimIdx * 68, 0, 68, 46));
                        whaleTimer = fallingDelay;
                    }
                    else whaleTimer -= dt;
                }
            }

            if (whale.focus and pirate.isAlive and !whale.isSwallowing) // player is in range
            {
                exclamation[2].playerInRange = true;
                if (pirate.hitbox.getPosition().x > whale.hitbox.getPosition().x) // on R
                {
                    if (!whale.attack and whale.isAlive and !whale.isSwallowing)
                        animate(whale.shape, whale.textures[1], whaleAnimIdx, 68, 46, whaleTimer, 14, -1, 1.5f);

                    exclamation_ani(2);
                    whale.velocity.x = 60 * dt;
                }

                if (pirate.hitbox.getPosition().x < whale.hitbox.getPosition().x)
                {
                    if (!whale.attack and whale.isAlive and !whale.isSwallowing)
                        animate(whale.shape, whale.textures[1], whaleAnimIdx, 68, 46, whaleTimer, 14, 1, 1.5f);

                    exclamation_ani(2);
                    whale.velocity.x = -60 * dt;
                }

                if (whale.isAlive)
                {
                    // Attack whale
                    if (pirate.hitbox.getGlobalBounds().intersects(whale.shape.getGlobalBounds()) and pirate.hitbox.getGlobalBounds().intersects(whale.range.getGlobalBounds()) and pirate.isAlive)
                    {
                        whale.attack = true;
                        if (pirate.hitbox.getPosition().x > whale.hitbox.getPosition().x) // on R
                        {
                            animate(whale.shape, whale.textures[4], whaleAnimIdx, 68, 46, whaleTimer, 11, -1, 1.5f);
                            if (whaleTimer <= 0)
                            {
                                if (whaleAnimIdx == 4 and pirate.hitbox.getGlobalBounds().intersects(whale.hitbox.getGlobalBounds()))
                                {
                                    sound.angry.play();
                                    sound.livesdec.play();
                                    pirate.isHit = true;
                                    playerHearts--;
                                }
                            }


                            whale.velocity.x = 0;

                        }

                        else if (pirate.hitbox.getPosition().x < whale.hitbox.getPosition().x)
                        {

                            animate(whale.shape, whale.textures[4], whaleAnimIdx, 68, 46, whaleTimer, 11, 1, 1.5f);
                            if (whaleTimer <= 0)
                            {
                                if (whaleAnimIdx == 4 and pirate.hitbox.getGlobalBounds().intersects(whale.hitbox.getGlobalBounds()))
                                {
                                    sound.angry.play();
                                    sound.livesdec.play();
                                    pirate.isHit = true;
                                    playerHearts--;
                                }
                            }



                            whale.velocity.x = 0;

                        }
                    }
                    else
                        whale.attack = false;
                }
            }
            else
                exclamation[2].playerInRange = false;
        }
}

void dead_animation(Sprite& s, Texture& text, int& idx, int width, int height, int& timer, int mxframe, int dir, float scale, int limiter, int mt)
{
    s.setScale(dir * scale, scale);
    if (s.getTexture() != &text)
    {
        s.setTexture(text);
        idx = 0;
        s.setTextureRect(IntRect(idx * width, 0, width, height));
    }
    if (idx < limiter)
        if (timer <= 0)
        {
            idx++;
            idx %= mxframe;
            s.setTextureRect(IntRect(idx * width, 0, width, height));
            timer = delay * mt;
        }
        else timer -= dt;
}

void exclamation_ani(int i)
{
    if (exclamationTimer <= 0)
    {
        exclamationAnimIdx++;
        exclamationAnimIdx %= 4;
        //cout << e << endl;
        exclamation[i].shape.setTextureRect(IntRect(exclamationAnimIdx * 20, 0, 20, 22));
        exclamationTimer = delay * 0.03;
    }
    else
        exclamationTimer -= dt;
}

void RepeatedKeyHandle(Event& event, bool isPressed[])
{
    for (int i = 0; i < 120; i++)
    {
        if (event.type == Event::KeyReleased && event.key.code == i)
        {
            isPressed[i] = 0;
        }
    }
}

void animate(Sprite& s, Texture& text, int& idx, int width, int height, int& timer, int mxframe, int dir, float scale, int mt)
{
    s.setScale(dir * scale, scale);
    if (s.getTexture() != &text)
    {
        s.setTexture(text);
        idx = 0;
        s.setTextureRect(IntRect(idx * width, 0, width, height));
    }
    if (timer <= 0)
    {
        idx++;
        idx %= mxframe;
        s.setTextureRect(IntRect(idx * width, 0, width, height));
        timer = delay * mt;
    }
    else timer -= dt;
}


void HitBoxUpdate(Sprite& s, RectangleShape& hitbox, int add)
{
    hitbox.setPosition(s.getPosition().x, s.getPosition().y + add);
}


bool CollisionDetection(RectangleShape& shape1, RectangleShape& shape2)
{
    return (shape1.getGlobalBounds().intersects(shape2.getGlobalBounds()));
}

float posX(int pos)
{
    float POS = (window.getSize().x * pos) / 1920;
    return POS;
}

float posY(int pos)
{
    float POS = (window.getSize().y * pos) / 1080;
    return POS;
}

void captainCode(int dir1, int dir2)
{
    //Attack captain
    if (captain.isAlive and !gameEnded) {
        if (pirate.isAlive)
            if (pirate.hitbox.getGlobalBounds().intersects(captain.hitbox.getGlobalBounds()) and pirate.hitbox.getGlobalBounds().intersects(captain.range.getGlobalBounds()))
            {

                if (pirate.hitbox.getPosition().x > captain.hitbox.getPosition().x)
                {
                    animate(captain.shape, captain.text[1], captainAttackIdx, 80, 74, captainTimer, 7, 1, 2.f);
                    if (captainTimer <= 0)
                    {
                        if (captainAttackIdx == 4 and captain.hitbox.getGlobalBounds().intersects(pirate.hitbox.getGlobalBounds()))
                        {
                            pirate.isHit = true;
                            playerHearts--;
                        }
                    }

                    captain.velocity.x = 0;

                }

                else if (pirate.hitbox.getPosition().x < captain.hitbox.getPosition().x)
                {
                    animate(captain.shape, captain.text[1], captainAttackIdx, 80, 74, captainTimer, 7, -1, 2.f);
                    if (captainTimer <= 0)
                    {
                        if (captainAttackIdx == 4 and captain.hitbox.getGlobalBounds().intersects(pirate.hitbox.getGlobalBounds()))
                        {
                            pirate.isHit = true;
                            playerHearts--;
                        }
                    }


                    captain.velocity.x = 0;

                }

            }
        //Walking captain
            else if (!pirate.hitbox.getGlobalBounds().intersects(captain.range.getGlobalBounds()) and captain.escape == false)
            {
                if (captain.shape.getPosition().x <= dir2 and captain.turn == false)
                {
                    animate(captain.shape, captain.text[0], captainAttackIdx, 80, 74, captainTimer, 14, 1, 2.f);
                    captain.velocity.x = 50 * dt;

                }
                else
                {

                    captain.velocity.x = -50 * dt;
                    captain.turn = true;
                }

                if (captain.shape.getPosition().x >= dir1 and captain.turn == true)
                {
                    animate(captain.shape, captain.text[0], captainAttackIdx, 80, 74, captainTimer, 14, -1, 2.f);
                    captain.velocity.x = -50 * dt;

                }
                else
                {
                    captain.velocity.x = 50 * dt;
                    captain.turn = false;
                }
            }
        //Following captain
            else
            {
                if (pirate.hitbox.getGlobalBounds().intersects(captain.range.getGlobalBounds()) and captain.escape == false)

                {
                    if (pirate.hitbox.getPosition().x > captain.hitbox.getPosition().x)
                    {
                        animate(captain.shape, captain.text[0], captainAttackIdx, 80, 74, captainTimer, 14, 1, 2.f);
                        //exclamation_ani(1);

                        captain.velocity.x = 95 * dt;
                    }

                    if (pirate.hitbox.getPosition().x < captain.hitbox.getPosition().x)
                    {
                        animate(captain.shape, captain.text[0], captainAttackIdx, 80, 74, captainTimer, 14, -1, 2.f);
                        //exclamation_ani(1);

                        captain.velocity.x = -95 * dt;
                    }
                }
            }
        for (int i = 0; i < bomb.bombs.size(); i++)
        {
            if (CollisionDetection(captain.range, bomb.bombs[i].hitbox))
            {
                captain.escape = true;
                if (bomb.bombs[i].hitbox.getPosition().x > captain.hitbox.getPosition().x)  // bomb on R
                {
                    animate(captain.shape, captain.text[2], captainAttackIdx, 80, 74, captainTimer, 12, -1, 2.f);
                    captain.velocity.x = -95 * dt;
                    captain.turn = true;
                }
                if (bomb.bombs[i].hitbox.getPosition().x < captain.hitbox.getPosition().x)  // bomb on L
                {
                    animate(captain.shape, captain.text[2], captainAttackIdx, 80, 74, captainTimer, 12, 1, 2.f);
                    captain.velocity.x = 95 * dt;
                    captain.turn = false;
                }
                if (captain.shape.getPosition().x <= dir1)
                {
                    captain.velocity = { 0,0 };
                    captain.shape.setPosition(dir1, captain.shape.getPosition().y);
                }
                if (captain.shape.getPosition().x >= dir2)
                {
                    captain.velocity = { 0,0 };
                    captain.shape.setPosition(dir2, captain.shape.getPosition().y);
                }
            }

            else captain.escape = false;

        }
    }
}

void cannonCode(Cannon& cannon, RectangleShape object, Sprite explosion, Texture expoTex)
{

    if (player_init)
    {

        for (int i = 0; i < 3; i++)
        {
            HitBoxUpdate(cannonBall[i].shape, cannonBall[i].hitbox);
        }

        for (int i = 0; i < 3; i++)
        {
            if (CollisionDetection(cannonBall[i].hitbox, object) and !vis[i])
            {
                cannonBall[i].isGrounded = true;
                cannonBall[i].velocity = { 0,0 };
                if (i < 2)cannonAniIdx = 0;
                vis[i] = true;
            }
        }

        if (cannonAniIdx < 8)//8 -->>>> 0
        {
            animate(cannon.shape, cannon.textures[1], cannonAniIdx, 62, 46, cannonTimer, 9, 1, 2, 1.5);
        }


        for (int i = 0; i < 3; i++)
        {
            if (cannonBall[i].isShot)
                continue;
            if (!i)
            {
                sound.cannonShoot.play();
                cannonBall[i].velocity = wadi[i];
                //cannonBall[i].velocity.y += g * dt; //apply gravity
                //cannonBall[i].shape.move(cannonBall[i].velocity);
                cannonBall[i].isShot = true;
            }
            else {
                if (!cannonBall[i - 1].isGrounded)
                    break;
                else
                {
                    sound.cannonShoot.play();
                    cannonBall[i].velocity = wadi[i];

                    //cannonBall[i].velocity.y += g * dt; //apply gravity
                    //cannonBall[i].shape.move(cannonBall[i].velocity);
                    cannonBall[i].isShot = true;
                }
            }
        }
        for (auto& it : cannonBall)
        {
            if (it.isShot and !it.isGrounded)
            {
                it.velocity.y += g * dt; //apply gravity
                it.shape.move(it.velocity);
            }

        }

        /*if(vis[2])
        {
            cout << "HERE\n";
            if(cannonExpoAniIdx<8)
            animate(explosion, expoTex, cannonExpoAniIdx, 96, 108, cannonExpoTimer, 9, 1, 2);
        }*/



    }



}

void cucumber_code(Cucumber& cucumber, int dir1, int dir2)
{
    if (!gameEnded)
    {
        if (cucumber.isAlive)
        {
            // Attack cucumber
            if (pirate.hitbox.getGlobalBounds().intersects(cucumber.hitbox.getGlobalBounds()) and pirate.hitbox.getGlobalBounds().intersects(cucumber.range.getGlobalBounds()) and pirate.isAlive and !cucumber.isBlowing)
            {

                //exclamation[0].playerInRange = false;
                if (pirate.hitbox.getPosition().x > cucumber.hitbox.getPosition().x)
                {
                    animate(cucumber.shape, cucumber.text[2], cucumberAnimIdx, 64, 68, cucumberTimer, 11, -1, 2.f);
                    if (cucumberTimer <= 0)
                    {
                        if (cucumberAnimIdx == 4 and pirate.hitbox.getGlobalBounds().intersects(cucumber.hitbox.getGlobalBounds()))
                        {
                            sound.hitSFX.play();
                            sound.livesdec.play();
                            pirate.isHit = true;
                            playerHearts--;
                        }
                    }

                    cucumber.velocity.x = 0;

                }

                else if (pirate.hitbox.getPosition().x < cucumber.hitbox.getPosition().x and !cucumber.isBlowing)
                {
                    animate(cucumber.shape, cucumber.text[2], cucumberAnimIdx, 64, 68, cucumberTimer, 11, 1, 2.f);
                    if (cucumberTimer <= 0)
                    {
                        if (cucumberAnimIdx == 4 and pirate.hitbox.getGlobalBounds().intersects(cucumber.hitbox.getGlobalBounds()))
                        {
                            sound.hitSFX.play();
                            sound.livesdec.play();
                            pirate.isHit = true;
                            playerHearts--;
                        }
                    }

                    cucumber.velocity.x = 0;

                }
                //hitSFX.play();
            }
            //Walking cucumber
            else if (!pirate.hitbox.getGlobalBounds().intersects(cucumber.range.getGlobalBounds()))
            {
                //exclamation[0].playerInRange = false;
                if (cucumber.shape.getPosition().x < dir2 and cucumber.turn == false)
                {

                    animate(cucumber.shape, cucumber.text[1], cucumberAnimIdx, 64, 68, cucumberTimer, 12, -1, 2.f);
                    cucumber.velocity.x = 60 * dt;
                }
                else
                {
                    cucumber.velocity.x = -60 * dt;
                    cucumber.turn = true;
                }

                if (cucumber.shape.getPosition().x >= dir1 and cucumber.turn == true)
                {
                    animate(cucumber.shape, cucumber.text[1], cucumberAnimIdx, 64, 68, cucumberTimer, 12, 1, 2.f);

                    cucumber.velocity.x = -60 * dt;
                }
                else
                {
                    cucumber.velocity.x = 60 * dt;
                    cucumber.turn = false;
                }
            }
            //  cucumber follow player
            else if (pirate.isAlive and !cucumber.isBlowing)
            {
                //exclamation[0].playerInRange = true;
                if (pirate.hitbox.getPosition().x > cucumber.hitbox.getPosition().x)
                {
                    animate(cucumber.shape, cucumber.text[1], cucumberAnimIdx, 64, 68, cucumberTimer, 12, -1, 2.f);
                    //exclamation_ani(0);
                    cucumber.velocity.x = 80 * dt;

                }

                if (pirate.hitbox.getPosition().x < cucumber.hitbox.getPosition().x)
                {
                    animate(cucumber.shape, cucumber.text[1], cucumberAnimIdx, 64, 68, cucumberTimer, 12, 1, 2.f);
                    //exclamation_ani(0);
                    cucumber.velocity.x = -80 * dt;

                }

            }
        }
    }
}

void initialize()
{
    for (int i = 0; i < 15; ++i)
    {
        isSwallowed[i] = false;
    }

    for (int i = 0; i < 120; ++i)
    {
        isPressed[i] = false;
    }

    dt = 0.f, delay = 3.f, fallingDelay = 50.f, bombAnimTime = 0.f, shootTimer = 0.f, bombDelay = 0.1f, BombChargingTimer = 0.f, exclamationTimer = 0.f;
    bombFired = 0;
    pirateAnimIdx = 0, baldAnimIdx = 0, bombAnimIdx = 0, whaleAnimIdx = 0, bigGuyAnimIdx = 0, captainAnimIdx = 0, captainAttackIdx = 0; // pirate , bald , bomb , whale , big guy , captain
    pirateTimer = 0, particleTimer = 0, baldTimer = 0, bigGuyTimer = 0, throwAnim = 0, whaleTimer = 0.f, captainTimer = 0;
    doorAnimIdx = 0; // for door
    particleAnimIdx = 0; // for particles
    chargingBarAnimIdx = 0; //For chargingBar
    whaleSwallowIdx = 0; //index for whale swallowing
    exclamationAnimIdx = 0; // for exclamation
    UpKeyPressed = false;
    player_init = false;
    levelFinished = false;
    pirate.doorInControl = false;
    pirate.player_in_Frame = 0;
    player_in_range = false;
    bigGuyAttackIdx = 0; // index for big guy attack
    oneTime_bigGuy_anim = false;
    baldAttackIdx = 0; // index for bald guy attack
    big_guy.isAlive = true;
    bald.isAlive = true;
    whale.isAlive = true;
    whaleLvl3.isAlive = true;
    bald_lvl3.isAlive = true;
    bigGuyLvl3.isAlive = true;
    captain.isAlive = true;
    bigGuyLvl2.isAlive = true;
    playerHearts = 3;
    WhaleJumpCount = 0;
    bomb.bombs.erase(bomb.bombs.begin(), bomb.bombs.end());
    wadi.clear();
    vis.clear();
    u = 0;
}