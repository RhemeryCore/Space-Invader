#include "lib/leetlib.h"
#include <math.h>
#include <d3d9.h>
#include <d3dx9.h>
#include <string>
#include <string.h>
#include <sstream>
#include <vector>
#include <time.h>
#include <fstream>
#include <iostream>
#include <conio.h>

/*Helpers:
To create any object, you need to give it a class and position. There is default class Object, but you can put a class that inherits from it.

Object* myObject = instance_create(new Object, 50, 50);
myObject->speed = 1;
myObject->image_yscale = 0.05;

or

Object* theBadGuy = instance_create(new Enemy, 300, 400);
theBadGuy->speed = 1;
theBadGuy->image_yscale = 0.05;
//To access variables that are in inherited classes, you need to use static_cast like this:
static_cast<Enemy*>(theBadGuy)->Health = 50; //(static_cast<ClassPointer>(ObjectVariable)->SpecificVariable = something;

If your object is enemy or bullet, you should call push_back() on proper vector

There are 6 vectors of objects
std::vector<Object*> Objects;
std::vector<Object*> Enemies;
std::vector<Object*> Bullets;
std::vector<Object*> eBullets;
std::vector<Object*> Baricades;
std::vector<Object*> BackgroundParticles;

Objects holds every created object
Enemies holds only objects created with Enemy class
Bullets holds only objects created with Bullet class
BackgroundParticles holds only objects created with Particle class
etc.

These vectors are used mainly for collision detection

When you call instance_destroy(anyObject, callDeconstructor), anyObject is removed from Objects vector
If anyObject is stored in non-array variable, you need to do: anyObject = instance_destroy(anyObject, callDeconstructor);. It will store NULL into the variable. You can use if(anyObject != NULL) to check if anyObject object exists and prevent errors.
Also if you called push_back() when you created an object, you need to also call erase() to remove it from the vector

*/

//Volume
float MusicVolume = 0;
float SfxVolume = 0;

bool ButtonClickPlayed = false; //Controls if click sound from pressing a button was played

//bool inMainMenu = true; 

bool ReturnToMenu = false; //If true, the game will return to main menu from main game
bool QuitGameNow = false; //If true, the program will return from Game()
bool RestartNow = false; //If true, the game will restart and new game will begin

int timer = 0; //Holds number of steps the game has performed
float ftimer = 0; //Holds number of steps * 0.001 the game has performed //Good for sine function

int Score = 0; //Player's score
int anim_Score = 0; //Used for interpoliation from 0 to Score when the game ends
int anim_Score_soundPlay = 0; //Used to play clicking sound effect when anim_Score is less then Score

bool keyPressed = false; //True if any key on keyboard is pressed
char lastChar = ' '; //Holds last pressed key using GetPressedKey()

std::string ScoreName = "Player1"; //Default player name for highscore

bool end_game = false; //If true, Game Over screen is visible and only option is to restat the game
float end_game_alphaFade = 0; //Used for Game Over screen fade in

float ingameFade = 1; //Used for menu to game transition

float midgameTimer = 60 * 4; //Timer that controls breaks between levels
float midgame_musicFade = 0; //Used for controlling music volume after completing boss level to change the music

//Used for text animation between levels
float midgame_title_y = 80;
float midgame_title_yT = 100;
float midgame_text_y = 180;
float midgame_text_yT = 200;
float midgame_text_alpha = 0;
float midgame_text_alphaT = 255;


int Difficulty = 0; //Controls overall difficulty of the game

int BackgroundMusic; //Holds currently playing music
bool musicSwitched = false; //Used to check if the music changed to the correct one

//Used for background color animation
float background_alpha = 0;
float background_r = 255;
float background_g = 255;
float background_b = 255;

float background_alphaT = 0;
float background_rT = 255;
float background_gT = 255;
float background_bT = 255;

//A handy little thing that is used to convert a number to a string. To draw it with DrawSomeText you can do: NumberToString(myNumber).c_str()
//																											 (NumberToString(myNumber) + myString + "someString").c_str()
template <typename T>
std::string NumberToString(T const& value)
{
	std::stringstream str;
	str << value;
	std::string temp_st = str.str();
	return temp_st;
}

inline bool file_exists(const std::string& name)
{
	std::ifstream f(name.c_str());
	return f.good();
}

void SettingsMenu()
{
	
	bool goingToMenu = false;

	float shifting_speed = 0;

	float title_x = 900;

	float slider1_x = 1000;
	float slider2_x = 1000;
	float button1_x = 1000;
	float button2_x = 1000;

Settings:

	if ((WantQuit()))
	{
		QuitGameNow = true;
		return;
	}

	//Animate menu
	if (goingToMenu == false)
	{
		title_x = smooth_approach(title_x, 400, 0.1);
		slider1_x = smooth_approach(slider1_x, 50, 0.1);
		slider2_x = smooth_approach(slider2_x, 50, 0.1);
		button1_x = smooth_approach(button1_x, 650, 0.1);
		button2_x = smooth_approach(button2_x, 50, 0.1);
	}
	else
	{
		shifting_speed += 0.5;


		title_x += shifting_speed * 0.75;

		slider1_x += shifting_speed;
		slider2_x += shifting_speed * 1.5;
		button1_x += shifting_speed * 0.5;
		button2_x += shifting_speed * 2;
	}

	float sliderX = slider1_x;
	float slider_len = 200;

	/*if (DrawButton(button2_x, 500, 100, 20, c_white, c_black, "Toggle fullscreen", 20) && (ButtonClickPlayed == true))
	{
		ChangeFullscreen();
	}*/

	//Draw Music volume slider and text
	MusicVolume = DrawSlider(sliderX, 300, slider_len, MusicVolume, c_red, c_white);
	DrawSomeText(sliderX + slider_len + 10, 300 - 8, 15, c_white, false, NumberToString(round(MusicVolume * 100)).c_str());
	DrawSomeText(sliderX, 270, 15, c_white, false, "Music volume");


	//Draw SFX volume slider and text
	SfxVolume = DrawSlider(sliderX, 400, slider_len, SfxVolume, c_red, c_white);

	DrawSomeText(sliderX + slider_len + 10, 400 - 8, 15, c_white, false, NumberToString(round(SfxVolume * 100)).c_str());
	DrawSomeText(sliderX, 370, 15, c_white, false, "SFX volume");

	//Update Music volume
	ChangeVolume(BackgroundMusic, MusicVolume);

	//Draw exit button
	if (DrawButton(button1_x, 550, 100, 20, c_white, c_black, "Save & back", 20))
	{
		goingToMenu = true;
	}

	//Save settings after exiting the menu
	if (title_x > 1100)
	{
		std::ofstream sFile("settings.txt");

		sFile << MusicVolume << std::endl;
		sFile << SfxVolume << std::endl;

		sFile.close();

		shifting_speed = 0;
		goingToMenu = false;

		return;
	}

	//Draw menu title
	DrawSomeText(title_x, 100, 30, c_white, true, "Settings");

	Flip();

	goto Settings;
}

void MainMenu()
{

	std::ifstream sFile("settings.txt");

	//Create a text string, which is used to output the text file
	std::string textFromFile;
	int ii = 0;
	//Use a while loop together with the getline() function to read the file line by line
	while (std::getline(sFile, textFromFile))
	{
		switch (ii)
		{
			case 0:
				std::istringstream(textFromFile) >> MusicVolume;
			break;
			case 1:
				std::istringstream(textFromFile) >> SfxVolume;
			break;
		}
		ii++;
	}

	//Close the file
	sFile.close();

	//Play main menu music
	BackgroundMusic = PlayMusic("sfx/s_creation.ogg", MusicVolume);

Menu_reinit:

	float newgameFade = 0;

	bool goingToSettings = false;
	bool goingToGame = false;

	float shifting_speed = 0;

	float title_x = 400;

	float button1_x = 400;
	float button2_x = 400;
	float button3_x = 400;


	float title_y = -100;

	float button1_y = 1000;
	float button2_y = 1500;
	float button3_y = 2000;

Menu:
	if ((WantQuit()) || (QuitGameNow == true))
	{
		QuitGameNow = true;
		return;
	}

	//Animate menu
	if (goingToSettings == false)
	{
		title_x = smooth_approach(title_x, 400, 0.1);
		button1_x = smooth_approach(button1_x, 400, 0.1);
		button2_x = smooth_approach(button2_x, 400, 0.1);
		button3_x = smooth_approach(button3_x, 400, 0.1);
	}
	else
	{
		shifting_speed += 0.5;


		title_x -= shifting_speed * 0.75;

		button1_x -= shifting_speed;
		button2_x -= shifting_speed * 1.5;
		button3_x -= shifting_speed;
	}

	if (goingToGame == true)
	{
		shifting_speed += 0.25;
		if (shifting_speed > 10)
		{
			newgameFade += 0.01;
		}

		title_y -= shifting_speed * 0.75;

		button2_y += shifting_speed * 0.75;
		button3_y += shifting_speed;
	}
	else
	{
		title_y = smooth_approach(title_y, 100, 0.1);
		button1_y = smooth_approach(button1_y, 300, 0.1);
		button2_y = smooth_approach(button2_y, 350, 0.1);
		button3_y = smooth_approach(button3_y, 400, 0.1);
	}


	//Draw buttons
	if (DrawButton(button1_x, button1_y, 100, 20, c_white, c_black, "New game", 20))
	{
		goingToGame = true;
		StopMusic();
		if ((goingToSettings == false) && (goingToGame == false))
		{
			
		}
	}
	if (DrawButton(button2_x, button2_y, 100, 20, c_white, c_black, "Settings", 20))
	{
		goingToSettings = true;
		
		if ((goingToSettings == false) && (goingToGame == false))
		{
			goto Menu_reinit;
		}
	}
	if (DrawButton(button3_x, button3_y, 100, 20, c_white, c_black, "Quit", 20))
	{
		if ((goingToSettings == false) && (goingToGame == false))
		{
			QuitGameNow = true;
			return;
		}
	}

	//Go to settings
	if (title_x < -200)
	{
		shifting_speed = 0;
		SettingsMenu();
		goingToSettings = false;
	}

	//Go to the game
	if (newgameFade >= 1)
	{
		RestartNow = true;
		return;
	}

	DrawSomeText(title_x, title_y, 40, c_white, true, "Space Invaders");

	DrawRectangle(0, 0, 800, 600, make_color_argb(newgameFade * 255, 0, 0, 0));

	Flip();

	goto Menu;
}

void PauseMenu()
{
	std::ifstream sFile("settings.txt");

	//Create a text string, which is used to output the text file
	std::string textFromFile;
	int ii = 0;
	//Use a while loop together with the getline() function to read the file line by line
	while (std::getline(sFile, textFromFile))
	{
		switch (ii)
		{
		case 0:
			std::istringstream(textFromFile) >> MusicVolume;
			break;
		case 1:
			std::istringstream(textFromFile) >> SfxVolume;
			break;
		}
		ii++;
	}

	//Close the file
	sFile.close();

Pause_reinit:

	float newgameFade = 0;

	bool goingToSettings = false;
	bool goingToGame = false;
	bool ExitGame = false;

	float shifting_speed = 0;

	float title_x = 400;

	float button1_x = 400;
	float button2_x = 400;
	float button3_x = 400;


	float title_y = 0;

	float button1_y = 350;
	float button2_y = 400;
	float button3_y = 450;

Pause:
	if (WantQuit())
	{
		QuitGameNow = true;
		return;
	}
	/*if (IsKeyDown(VK_ESCAPE))
	{
		return;
	}*/
	if (ExitGame == true)
	{
		ReturnToMenu = true;
		return;
	}

	//Animate menu
	if (goingToSettings == false)
	{
		title_x = smooth_approach(title_x, 400, 0.1);
		button1_x = smooth_approach(button1_x, 400, 0.1);
		button2_x = smooth_approach(button2_x, 400, 0.1);
		button3_x = smooth_approach(button3_x, 400, 0.1);
	}
	else
	{
		shifting_speed += 0.5;


		title_x -= shifting_speed * 0.75;

		button1_x -= shifting_speed;
		button2_x -= shifting_speed * 1.5;
		button3_x -= shifting_speed;
	}

	if (goingToGame == true)
	{
		newgameFade += 0.1;
	}
	else
	{
		title_y = smooth_approach(title_y, 100, 0.1);
		button1_y = smooth_approach(button1_y, 300, 0.1);
		button2_y = smooth_approach(button2_y, 350, 0.1);
		button3_y = smooth_approach(button3_y, 400, 0.1);
	}


	//Draw buttons
	if (DrawButton(button1_x, button1_y, 100, 20, c_white, c_black, "Continue", 20))
	{
		ingameFade = 1;
		goingToGame = true;
		if ((goingToSettings == false) && (goingToGame == false))
		{

		}
	}
	if (DrawButton(button2_x, button2_y, 100, 20, c_white, c_black, "Settings", 20))
	{
		goingToSettings = true;

		if ((goingToSettings == false) && (goingToGame == false))
		{
			goto Pause_reinit;
		}
	}
	if (DrawButton(button3_x, button3_y, 100, 20, c_white, c_black, "Exit", 20))
	{
		if ((goingToSettings == false) && (goingToGame == false))
		{
			StopMusic();
			ClearObjects();
			ReturnToMenu = true;
			return;
		}
	}

	//Go to settings
	if (title_x < -200)
	{
		shifting_speed = 0;
		SettingsMenu();
		goingToSettings = false;
	}

	//Return to the game
	if (newgameFade >= 1)
	{
		ingameFade = 1;
		return;
	}

	DrawSomeText(title_x, title_y, 30, c_white, true, "Puase");

	DrawRectangle(0, 0, 800, 600, make_color_argb(newgameFade * 255, 0, 0, 0));

	Flip();

	goto Pause;
}

void Game()
{
	
	Sprites.LoadSprites(); //Pre-load all sprites
	Sounds.LoadSounds(); //Pre-load all sounds

	MainMenu();

	
init:

	BackgroundMusic = PlayMusic("sfx/Music/s_Forest_combatAmbient.mp3", MusicVolume);

	//Reset all variables after restart
	RestartNow = false;

	timer = 0;
	ftimer = 0;

	Score = 0;
	anim_Score = 0;

	keyPressed = false;
	lastChar = ' ';

	ScoreName = "Player1";

	end_game = false;
	end_game_alphaFade = 0;

	Difficulty = 0;
	int enemyCountStart = 0;

	bool bonuses_set = false;

	srand((unsigned)time(0));
	

	float UX_target = 400; //Player X target position
	float UY_target = 550; //Player Y target position

	float PlayerMovementSpeed = 7; //Player movement speed

	float hspeed = 0; //Player current horizontal speed
	float vspeed = 0; //Player current vertical speed

	//Create instance of an object Player
	Object* player = instance_create(new Player, 0, 0);
	player->x = 400;
	player->y = 540;
	player->image_xscale = 0.20;
	player->image_yscale = 0.25;

	//Create baricades
	for (int i = 1; i <= 4; i++)
	{
		Object* baricade = instance_create(new Baricade, 0, 0);
		baricade->image_xscale = 0.25;
		baricade->image_yscale = 0.25;
		baricade->x = (800/5)*(i);
		baricade->y = 400;

		Baricades.push_back(baricade);
	}

NextLevel:
	ftimer = 0;
	
	//Create enemies
	if (Difficulty < 30)
	{
		//Loop thru spawning area
		for (int yy = 0; yy < 5; yy++)
		{
			for (int xx = -5; xx <= 5; xx++)
			{
				//Create an enemy
				Object* enemy = instance_create(new Enemy, 0, 0);

				//Determine enemy's level
				if (yy == 0)
				{
					enemy->sprite = Sprites.Invader3;
					static_cast<Enemy*>(enemy)->InvaderLevel = 3;
					static_cast<Enemy*>(enemy)->Damage = 50 + ((Difficulty * 10));
				}
				if ((yy == 1) || (yy == 2))
				{
					enemy->sprite = Sprites.Invader2;
					static_cast<Enemy*>(enemy)->InvaderLevel = 2;
					static_cast<Enemy*>(enemy)->Damage = 20 + ((Difficulty * 10));
				}
				if ((yy == 3) || (yy == 4))
				{
					enemy->sprite = Sprites.Invader1;
					static_cast<Enemy*>(enemy)->InvaderLevel = 1;
					static_cast<Enemy*>(enemy)->Damage = 10 + ((Difficulty * 10));
				}

				//Place enemy on correct spot and give him Health
				enemy->image_xscale = 0.5;
				enemy->image_yscale = 0.5;
				enemy->x = ((800 / 2) + (xx * enemy->image_width * (enemy->image_xscale * 3)));
				enemy->y = ((50) + (yy * enemy->image_height * (enemy->image_yscale * 4)));
				static_cast<Enemy*>(enemy)->MaxHealth = 1 * (Difficulty * 2);
				static_cast<Enemy*>(enemy)->Health = 1 * (Difficulty * 2);

				Enemies.push_back(enemy);
			}
		}
		//Every 5th level spawn additional boss invader
		if (((Difficulty + 1) % 5 == 0) && (Difficulty > 1))
		{
			Object* enemy = instance_create(new Enemy, 0, 0);

			enemy->sprite = Sprites.Invader4;
			static_cast<Enemy*>(enemy)->InvaderLevel = 4;
			static_cast<Enemy*>(enemy)->Damage = 100;

			enemy->image_xscale = 1;
			enemy->image_yscale = 1;
			enemy->x = ((800 / 2) + (0 * enemy->image_width * (enemy->image_xscale * 3)));
			enemy->y = 15;

			static_cast<Enemy*>(enemy)->MaxHealth = 10 * ((Difficulty + 1) * 10);
			static_cast<Enemy*>(enemy)->Health = 10 * ((Difficulty + 1) * 10);

			Enemies.push_back(enemy);
		}
	}
	//Create a lot of enemies
	else
	{
		//Loop thru spawn area
		for (int yy = -15; yy < 5; yy++)
		{
			int xxx = 5;
			//If yy is pointing to the last 3 lines, spawn less invaders, because they're bigger (they wouldn't fit otherwise)
			if (yy <= -13)
			{
				xxx = 2;
			}
			for (int xx = -xxx; xx <= xxx; xx++)
			{
				//Spawn an enemy
				Object* enemy = instance_create(new Enemy, 0, 0);

				enemy->image_xscale = 0.5;
				enemy->image_yscale = 0.5;

				enemy->x = ((800 / 2) + (xx * enemy->image_width * (enemy->image_xscale * 3)));
				enemy->y = ((50) + (yy * enemy->image_height * (enemy->image_yscale * 4)));

				//If last 3 lines, spawn boss invader
				if (yy <= -13)
				{
					enemy->sprite = Sprites.Invader4;
					static_cast<Enemy*>(enemy)->InvaderLevel = 4;
					static_cast<Enemy*>(enemy)->Damage = 100 + ((Difficulty * 5));
					enemy->image_xscale = 1;
					enemy->image_yscale = 1;

					enemy->x = ((800 / 2) + (xx * enemy->image_width * (enemy->image_xscale * 3)));
					enemy->y = ((50) + (yy * enemy->image_height * (enemy->image_yscale * 4))) + 400;

					static_cast<Enemy*>(enemy)->MaxHealth = 10 * ((Difficulty + 1) * 10);
					static_cast<Enemy*>(enemy)->Health = 10 * ((Difficulty + 1) * 10);
				}
				else
				{
					static_cast<Enemy*>(enemy)->MaxHealth = 1 * (Difficulty * 2);
					static_cast<Enemy*>(enemy)->Health = 1 * (Difficulty * 2);
				}

				//Otherwise spawn normal invaders
				if ((yy > -13) && (yy <= 0))
				{
					enemy->sprite = Sprites.Invader3;
					static_cast<Enemy*>(enemy)->InvaderLevel = 3;
					static_cast<Enemy*>(enemy)->Damage = 50 + ((Difficulty * 5));
				}
				if ((yy == 1) || (yy == 2))
				{
					enemy->sprite = Sprites.Invader2;
					static_cast<Enemy*>(enemy)->InvaderLevel = 2;
					static_cast<Enemy*>(enemy)->Damage = 20 + ((Difficulty * 5));
				}
				if ((yy == 3) || (yy == 4))
				{
					enemy->sprite = Sprites.Invader1;
					static_cast<Enemy*>(enemy)->InvaderLevel = 1;
					static_cast<Enemy*>(enemy)->Damage = 10 + ((Difficulty * 5));
				}

				Enemies.push_back(enemy);
			}
		}
	}

	//Set how many enemies we spawned
	enemyCountStart = Enemies.size();

	float enemy_hspeed = -1; //Controls enemy's move direction on X axis
	Difficulty++;

	//Prepare text between levels
	std::string bonuses_title = "Level completed";
	std::string bonuses = "";

Step: //Main game loop

	timer++;
	ftimer += 0.001;
	if ((WantQuit()) || (QuitGameNow == true))
	{
		return;
	}
	if (IsKeyDown(VK_ESCAPE))
	{
		PauseMenu();
	}
	if (ReturnToMenu == true)
	{
		ReturnToMenu = false;
		StopMusic();
		ClearObjects();
		MainMenu();
	}

	/*static_cast<Player*>(player)->Cannons = 13;
	static_cast<Player*>(player)->Damage = 100;
	static_cast<Player*>(player)->AttackSpeed = 10;*/

	//Animate and draw background color
	background_alpha = smooth_approach(background_alpha, background_alphaT, 0.05);
	background_r = smooth_approach(background_r, background_rT, 0.05);
	background_g = smooth_approach(background_g, background_gT, 0.05);
	background_b = smooth_approach(background_b, background_bT, 0.05);

	DrawRectangle(0, 0, 800, 600, make_color_argb(background_alpha, background_r, background_g, background_b));

	//Emit background particles
	for (int i = 0; i < 1; i++)
	{
		//Create a particle and set some parameters
		Object* part = instance_create(new Particle, 0, 0);
		
		part->x = frand_range(-50, 810);
		part->y = -100;
		part->active = true;
		part->drawSelf = false;
		part->image_index = shape_Fog1;
		static_cast<Particle*>(part)->part_set_life(60*5, 60*10);
		static_cast<Particle*>(part)->part_set_alpha(0, 0.05, 0.1);
		static_cast<Particle*>(part)->part_set_size(1, 1, 0.0001);
		static_cast<Particle*>(part)->part_set_speed(5, 5, 0);
		static_cast<Particle*>(part)->part_set_angle(0, 360, frand_range(-1, 1));
		static_cast<Particle*>(part)->part_set_direction(-85, -95, 0);
		
		background_alphaT = 0;

		//Set particle color by current level group
		if (Difficulty <= 5)
		{
			static_cast<Particle*>(part)->part_set_color(10, 250, 10);
		}
		if ((Difficulty > 5) && (Difficulty <= 10))
		{
			static_cast<Particle*>(part)->part_set_color(255, 180, 65);
		}
		if ((Difficulty > 10) && (Difficulty <= 15))
		{
			static_cast<Particle*>(part)->part_set_color(150, 150, 150);
		}
		if ((Difficulty > 15) && (Difficulty <= 20))
		{
			background_alphaT = 0;
			if ((rand() % 100) < 20)
			{
				part->image_index = shape_glow;
				static_cast<Particle*>(part)->part_set_alpha(0.5, 0.75, 1);
				static_cast<Particle*>(part)->part_set_size(0.1, 0.1, 0.0001);
				static_cast<Particle*>(part)->part_set_speed(5, 10, 0);
			}
			static_cast<Particle*>(part)->part_set_color(255, 255, 255);
		}
		if ((Difficulty > 20) && (Difficulty <= 25))
		{
			background_alphaT = 10;
			static_cast<Particle*>(part)->part_set_alpha(0.05, 0.1, 0.5);
			static_cast<Particle*>(part)->part_set_color(10, 10, 10);
		}
		if ((Difficulty > 25) && (Difficulty <= 30))
		{
			part->image_index = shape_fineSmoke1;
			static_cast<Particle*>(part)->part_set_alpha(0.5, 0.75, 1);
			static_cast<Particle*>(part)->part_set_color(150, 0, 200);
		}
		if (Difficulty > 30)
		{
			part->image_index = part->sprite.size() - 1;
			static_cast<Particle*>(part)->part_set_color((rand() % 255), (rand() % 255), (rand() % 255));
		}
		BackgroundParticles.push_back(part);
	}

	//Check if particle is outside of the screen. If yes, delete it. If no, draw it
	for (int i = 0; i < BackgroundParticles.size(); i++)
	{
		if (BackgroundParticles[i]->y > 700)
		{
			BackgroundParticles[i]->active = false;
		}
		if (BackgroundParticles[i]->active == true)
		{
			if ((BackgroundParticles[i]->image_index == shape_glow) && ((rand() % 100) < 5))
			{
				static_cast<Particle*>(BackgroundParticles[i])->part_set_direction(-85, -95, frand_range(-1, 1));
			}
			DrawSprite(BackgroundParticles[i]->sprite, BackgroundParticles[i]->image_index, BackgroundParticles[i]->x, BackgroundParticles[i]->y, BackgroundParticles[i]->image_width, BackgroundParticles[i]->image_height, BackgroundParticles[i]->image_angle, BackgroundParticles[i]->image_blend);
		}
		else
		{
			BackgroundParticles.erase(BackgroundParticles.begin() + i);
		}
	}
	

	midgameTimer--;
	//Fade music in and keep midgame variables on default
	if (Enemies.size() > 0)
	{
		midgameTimer = 60 * 4;
		midgame_musicFade += 0.01;

		midgame_title_yT = 50;
		midgame_text_yT = 100;
		midgame_text_alphaT = 0;
		midgame_title_y = 50;
		midgame_text_y = 100;
		midgame_text_alpha = 0;
	}
	//Heal the player and give him bonuses
	if ((midgameTimer != 60 * 4) && (bonuses_set == false))
	{
		bonuses_set = true;
		
		int randomBoost = (rand() % 100);
		if (player != NULL)
		{
			if (Difficulty != 0)
			{
				int noHit = 0;

				static_cast<Player*>(player)->Health = static_cast<Player*>(player)->MaxHealth + 50;
				if (static_cast<Player*>(player)->Health > static_cast<Player*>(player)->MaxHealth)
				{
					static_cast<Player*>(player)->MaxHealth = static_cast<Player*>(player)->Health;
				}
				static_cast<Player*>(player)->AttackSpeed += 1;
				static_cast<Player*>(player)->Damage += 1;

				bonuses += "HP maxed out\n";
				bonuses += "Attack speed up\n";

				if ((Difficulty + 1) % 5 == 0)
				{
					static_cast<Player*>(player)->Cannons += 2;
					static_cast<Player*>(player)->Damage /= 2;
					bonuses += "More cannons\n";
					bonuses += "Damage down\n";
				}
				else
				{
					bonuses += "Damage up\n";
				}
			}

			if (static_cast<Player*>(player)->AttackSpeed > 10)
			{
				static_cast<Player*>(player)->AttackSpeed = 10;
			}
			if (static_cast<Player*>(player)->Cannons > 13)
			{
				static_cast<Player*>(player)->Cannons = 13;
				static_cast<Player*>(player)->Damage *= 2;
				//static_cast<Player*>(player)->Damage += 5;
				bonuses += "Damage bonus\n";
			}
		}

		//Heal baricades
		for (int i = 0; i < Baricades.size(); i++)
		{
			static_cast<Baricade*>(Baricades[i])->MaxHealth += 100;
			static_cast<Baricade*>(Baricades[i])->Health = static_cast<Baricade*>(Baricades[i])->MaxHealth;
		}
	}
	//Fade music out and prepare it for change
	if (((Difficulty) % 5 == 0))
	{
		midgame_musicFade -= 0.01;
	}
	//Animate text between levels
	if ((midgameTimer < 60 * 4))
	{
		midgame_title_yT = 100;
		midgame_text_yT = 200;
		midgame_text_alphaT = 255;
		if ((midgameTimer < 60 * 1))
		{
			midgame_title_yT = 150;
			midgame_text_yT = 300;
			midgame_text_alphaT = 0;
		}
		midgame_title_y = smooth_approach(midgame_title_y, midgame_title_yT, 0.05);
		midgame_text_y = smooth_approach(midgame_text_y, midgame_text_yT, 0.05);
		midgame_text_alpha = smooth_approach(midgame_text_alpha, midgame_text_alphaT, 0.1);
		DrawSomeText(400, midgame_title_y, 20, make_color_argb(midgame_text_alpha, 255, 255, 255), true, bonuses_title.c_str());
		DrawSomeText(400, midgame_text_y, 16, make_color_argb(midgame_text_alpha, 255, 255, 255), true, bonuses.c_str());
	}

	
	/*if (midgame_musicFade > 1)
	{
		midgame_musicFade = 1;
	}*/
	//Change music if player finished boss level
	if (midgame_musicFade < 0)
	{
		//midgame_musicFade = 0;
		if (musicSwitched == false)
		{
			if ((Difficulty) % 5 == 0)
			{
				switch (Difficulty)
				{
					case 0:
						BackgroundMusic = PlayMusic("sfx/Music/s_Forest_combatAmbient.mp3", 0);
					break;
					case 5:
						BackgroundMusic = PlayMusic("sfx/Music/s_Desert_combatAmbient.mp3", 0);
					break;
					case 10:
						BackgroundMusic = PlayMusic("sfx/Music/s_ColdDesert_combatAmbient.mp3", 0);
					break;
					case 15:
						BackgroundMusic = PlayMusic("sfx/Music/s_Mountain_combatAmbient.mp3", 0);
					break;
					case 20:
						BackgroundMusic = PlayMusic("sfx/Music/s_Cave_combatAmbient.mp3", 0);
					break;
					case 25:
						BackgroundMusic = PlayMusic("sfx/Music/s_Fortress_combatAmbient.mp3", 0);
					break;
					default:
						BackgroundMusic = PlayMusic("sfx/Music/s_SystemVoid_bossPhase1.mp3", 0);
					break;
				}
			}
			musicSwitched = true;
		}
	}

	midgame_musicFade = min(max(midgame_musicFade, 0), 1);

	//Update music volume
	ChangeVolume(BackgroundMusic, midgame_musicFade * MusicVolume);

	//Go to next level
	if (midgameTimer <= 0)
	{
		musicSwitched = false;
		bonuses_set = false;
		goto NextLevel;
	}

	//If player is alive
	if (end_game == false)
	{
		if (player != NULL)
		{
			static_cast<Player*>(player)->Cannons = min(max(static_cast<Player*>(player)->Cannons, 1), 13);
		}
		float enemiesMissing = enemyCountStart - Enemies.size();
		float wiggleSpeed = 1 + pow((enemiesMissing / enemyCountStart) * 10, 2);

		bool enemyShiftDown = false;

		//Enemy control
		for (int i = 0; i < Enemies.size(); i++)
		{
			//Check if enemy is close to screen border. If yes, change moving direction and prepare for shifting down
			if (Enemies[i]->x + 50 > 800)
			{
				enemy_hspeed = -1;
				enemyShiftDown = true;
			}
			else if (Enemies[i]->x - 50 < 0)
			{
				enemy_hspeed = 1;
				enemyShiftDown = true;
			}
			
			static_cast<Enemy*>(Enemies[i])->ChanceToShot = 1 + (Difficulty) * ((enemiesMissing / enemyCountStart) * 1);
			/*if (static_cast<Enemy*>(Enemies[0])->invader_level == 4)
			{
				static_cast<Enemy*>(Enemies[0])->ChanceToShot = 90 + ((Difficulty + 1) * 10);
				if (Enemies[0]->y > 300)
				{
					Enemies[0]->y = 300;
				}
			}*/

			//Enemy attacking
			//ReadyToShoot is controlled in Enemy::eUpdate()
			if (static_cast<Enemy*>(Enemies[i])->ReadyToShoot == true)
			{
				//If current enemy is boss invader...
				if (static_cast<Enemy*>(Enemies[i])->InvaderLevel == 4)
				{
					//...create 3 bullets
					for (int j = 0; j < 3; j++)
					{
						float ii = 0;
						float iii = 0;
						ii = j - (3 / 2);
						iii = j - (3 / 2);

						Object* bullet = instance_create(new eBullet, 0, 0);
						bullet->x = Enemies[i]->x + (ii * 10);
						bullet->y = Enemies[i]->y;
						bullet->image_xscale = 0.5;
						bullet->image_yscale = 0.5;
						bullet->image_angle = -90 + (iii * 5);
						bullet->direction = bullet->image_angle;
						bullet->speed = 8;
						static_cast<Bullet*>(bullet)->Damage = static_cast<Enemy*>(Enemies[i])->Damage;
						
						PlaySnd(Sounds.EnemyShot, SfxVolume);

						static_cast<Enemy*>(Enemies[i])->ReadyToShoot = false;
						eBullets.push_back(bullet);
					}
				}
				//Otherwise create only one bullet
				else
				{
					Object* bullet = instance_create(new eBullet, 0, 0);
					bullet->x = Enemies[i]->x;
					bullet->y = Enemies[i]->y;
					bullet->image_xscale = 0.5;
					bullet->image_yscale = 0.5;
					bullet->image_angle = -90;
					bullet->direction = bullet->image_angle;
					bullet->speed = 8;
					static_cast<Bullet*>(bullet)->Damage = static_cast<Enemy*>(Enemies[i])->Damage;

					PlaySnd(Sounds.EnemyShot, SfxVolume);

					static_cast<Enemy*>(Enemies[i])->ReadyToShoot = false;
					eBullets.push_back(bullet);
				}
			}
		}
		int enemiesOutOfScreen = 0;
		//Enemy movement control
		for (int i = 0; i < Enemies.size(); i++)
		{
			//Shift enemy down when it changed direction
			if (enemyShiftDown == true)
			{
				static_cast<Enemy*>(Enemies[i])->y += 10 + Difficulty;//(10 + ((float)(enemyCountStart - Enemies.size()) / 500)) + (Difficulty * 2);
			}

			//For Difficulty > 30 | If enemy is above normal spawning area, increment enemiesOutOfScreen to then check how many enemies are above spawning area
			if (static_cast<Enemy*>(Enemies[i])->y < 150)
			{
				enemiesOutOfScreen++;
			}

			//Determine sprite animation speed and movement speed
			static_cast<Enemy*>(Enemies[i])->image_speed = (wiggleSpeed / 2);
			if (Difficulty <= 30)
			{
				static_cast<Enemy*>(Enemies[i])->hspeed = (enemy_hspeed * 0.1) * wiggleSpeed;
				//If last inveder is boss, slow him down
				if (Enemies.size() == 1)
				{
					if (static_cast<Enemy*>(Enemies[0])->InvaderLevel == 4)
					{
						static_cast<Enemy*>(Enemies[i])->hspeed = enemy_hspeed;
					}
				}
			}
			else
			{
				static_cast<Enemy*>(Enemies[i])->hspeed = ((enemy_hspeed * 0.1) * wiggleSpeed) / 2;
			}

			//Destroy self and create bullets to damage the player to force game over
			if (static_cast<Enemy*>(Enemies[i])->y >= 550)
			{
				Object* bullet = instance_create(new eBullet, 0, 0);
				bullet->x = Enemies[i]->x;
				bullet->y = 550;
				bullet->image_xscale = 0.5;
				bullet->image_yscale = 0.5;
				bullet->image_angle = 0;
				bullet->direction = bullet->image_angle;
				bullet->speed = 5;
				static_cast<Bullet*>(bullet)->Damage = static_cast<Enemy*>(Enemies[i])->Damage;
				eBullets.push_back(bullet);

				bullet = instance_create(new eBullet, 0, 0);
				bullet->x = Enemies[i]->x;
				bullet->y = Enemies[i]->y;
				bullet->image_xscale = 0.5;
				bullet->image_yscale = 0.5;
				bullet->image_angle = 180;
				bullet->direction = bullet->image_angle;
				bullet->speed = 5;
				static_cast<Bullet*>(bullet)->Damage = static_cast<Enemy*>(Enemies[i])->Damage;
				eBullets.push_back(bullet);

				instance_destroy(Enemies[i]);
				Enemies.erase(Enemies.begin() + i);
			}
		}

		//If too many enemies are above spawning area, quickly shift them down
		if (Difficulty > 30)
		{
			if (enemiesOutOfScreen >= (Enemies.size()))
			{
				for (int i = 0; i < Enemies.size(); i++)
				{
					static_cast<Enemy*>(Enemies[i])->y += 5;
				}
			}
		}


		enemyShiftDown = false;

		//Player movement input
		if (((IsKeyDown('A')) || (IsKeyDown(VK_LEFT))) && (UX_target > 40))
		{
			UX_target -= PlayerMovementSpeed;
		}
		if (((IsKeyDown('D')) || (IsKeyDown(VK_RIGHT))) && (UX_target < 760))
		{
			UX_target += PlayerMovementSpeed;
		}

		if (player != NULL)
		{
			//Smoothly move the player
			player->x = smooth_approach(player->x, UX_target, 0.15f);

			//Update player current speed
			hspeed = player->x - player->xprevious;

			//Tilt the player by it's speed
			player->image_angle = -hspeed;

			//Player attack
			static int count = 0;
			if (count)
			{
				count--;
			}
			if (((IsKeyDown(VK_LBUTTON)) || (IsKeyDown(VK_SPACE))) && count == 0)
			{
				//Create bullets
				for (int i = 0; i < static_cast<Player*>(player)->Cannons; i++)
				{
					float ii = 0;
					float iii = 0;
					if (static_cast<Player*>(player)->Cannons <= 5)
					{
						ii = i - (static_cast<Player*>(player)->Cannons / 2);
					}
					else
					{
						iii = i - (static_cast<Player*>(player)->Cannons / 2);
					}
					Object* bullet = instance_create(new Bullet, 0, 0);
					bullet->x = player->x + (ii * 5);
					bullet->y = player->y;
					bullet->image_xscale = 0.5;
					bullet->image_yscale = 0.5;
					bullet->image_angle = 90 + (iii * 1.25);
					bullet->direction = bullet->image_angle;
					bullet->speed = 10;
					static_cast<Bullet*>(bullet)->Damage = static_cast<Player*>(player)->Damage;

					Bullets.push_back(bullet);
				}
				
				PlaySnd(Sounds.NormalShoot, SfxVolume);

				
				count = 60 / static_cast<Player*>(player)->AttackSpeed;
			}
		}

		//Collision checking for player bullets
		for (int i = 0; i < Bullets.size(); i++)
		{
			if (Bullets[i] != NULL)
			{
				if (Bullets[i]->active == true)
				{
					bool destroyTheBullet = false;
					bool outOfBounds = false;

					//Get enemy object index that bullet collided with
					int collided_enemy_index = collision_rectangle(Bullets[i]->x - Bullets[i]->image_width, Bullets[i]->y - Bullets[i]->image_height, Bullets[i]->x + Bullets[i]->image_width, Bullets[i]->y + Bullets[i]->image_height, Enemies);
					if (collided_enemy_index != -1) //Bullet collided with enemy
					{
						//Decrease enemy's health
						static_cast<Enemy*>(Enemies[collided_enemy_index])->hitShock = 100;
						static_cast<Enemy*>(Enemies[collided_enemy_index])->Health -= static_cast<Bullet*>(Bullets[i])->Damage;
						if (static_cast<Enemy*>(Enemies[collided_enemy_index])->Health <= 0)
						{
							//Remove that enemy from the game
							Score += 100 * static_cast<Enemy*>(Enemies[collided_enemy_index])->InvaderLevel;
							instance_destroy(Enemies[collided_enemy_index]);
							Enemies.erase(Enemies.begin() + collided_enemy_index);
						}
						else
						{
							PlaySnd(Sounds.EnemyHit, SfxVolume / 2);
						}
						//...and remove the bullet too
						destroyTheBullet = true;
					}

					//Get baricade object index that bullet collided with
					int collided_baricade = collision_rectangle(Bullets[i]->x - Bullets[i]->image_width, Bullets[i]->y - Bullets[i]->image_height, Bullets[i]->x + Bullets[i]->image_width, Bullets[i]->y + Bullets[i]->image_height, Baricades);
					if (collided_baricade != -1)
					{
						//Decrease baricade's health
						static_cast<Baricade*>(Baricades[collided_baricade])->hitShock = 100;
						static_cast<Baricade*>(Baricades[collided_baricade])->Health -= static_cast<Bullet*>(Bullets[i])->Damage * 2;

						//Spawn 3 bullets, but with less damage
						for (int i = 0; i < 3; i++)
						{
							float ii = 0;
							float iii = i - 1;
							Object* bullet = instance_create(new Bullet, 0, 0);
							bullet->x = Baricades[collided_baricade]->x;
							bullet->y = Baricades[collided_baricade]->y - 50;
							bullet->image_xscale = 0.5;
							bullet->image_yscale = 0.5;
							bullet->image_angle = 90 + ((rand() % 60) - 30);
							bullet->direction = bullet->image_angle;
							bullet->speed = 10;
							static_cast<Bullet*>(bullet)->Damage = static_cast<Player*>(player)->Damage / 3;

							Bullets.push_back(bullet);
						}

						//If baricade has 0 health, remove it from the game
						if (static_cast<Baricade*>(Baricades[collided_baricade])->Health <= 0)
						{
							instance_destroy(Baricades[collided_baricade]);
							Baricades.erase(Baricades.begin() + collided_baricade);
						}
						else
						{
							PlaySnd(Sounds.EnemyHit, SfxVolume);
						}
						destroyTheBullet = true;
					}
					//Bullet didn't collide with anything, but is out of bounds
					else if ((Bullets[i]->x < 0) || (Bullets[i]->x > 800) || (Bullets[i]->y < 0) || (Bullets[i]->y > 600))
					{
						destroyTheBullet = true;
						outOfBounds = true;
					}

					if (destroyTheBullet == true)
					{
						instance_destroy(Bullets[i], !outOfBounds);
						Bullets.erase(Bullets.begin() + i);
					}
				}
			}
		}

		//Collision checking for baricades
		for (int i = 0; i < Baricades.size(); i++)
		{
			if (Baricades[i] != NULL)
			{
				if (Baricades[i]->active == true)
				{
					bool destroyTheEnemy = false;
					int collided_enemy = collision_rectangle(Baricades[i]->x - Baricades[i]->image_width, Baricades[i]->y - Baricades[i]->image_height, Baricades[i]->x + Baricades[i]->image_width, Baricades[i]->y + Baricades[i]->image_height, Enemies);
					if (collided_enemy != -1)
					{
						//Remove that baricade from the game
						instance_destroy(Baricades[i]);
						Baricades.erase(Baricades.begin() + i);
						PlaySnd(Sounds.EnemyHit, SfxVolume);

						destroyTheEnemy = true;
					}

					//...and that enemy too
					if (destroyTheEnemy == true)
					{
						instance_destroy(Enemies[collided_enemy], false);
						Enemies.erase(Enemies.begin() + collided_enemy);
					}
				}
			}
		}

		//Collision checking for enemy bullets
		if (player != NULL)
		{
			for (int i = 0; i < eBullets.size(); i++)
			{
				if (eBullets[i] != NULL)
				{
					if (eBullets[i]->active == true)
					{
						bool outOfBounds = false;
						bool destroyTheBullet = false;

						//Get baricade object index that bullet collided with
						int collided_baricade = collision_rectangle(eBullets[i]->x - eBullets[i]->image_width, eBullets[i]->y - eBullets[i]->image_height, eBullets[i]->x + eBullets[i]->image_width, eBullets[i]->y + eBullets[i]->image_height, Baricades);
						if (collided_baricade != -1)
						{
							//Decrease baricade's health
							static_cast<Baricade*>(Baricades[collided_baricade])->hitShock = 100;
							static_cast<Baricade*>(Baricades[collided_baricade])->Health -= static_cast<eBullet*>(eBullets[i])->Damage;
							if (static_cast<Baricade*>(Baricades[collided_baricade])->Health <= 0)
							{
								//Remove that baricade from the game
								instance_destroy(Baricades[collided_baricade]);
								Baricades.erase(Baricades.begin() + collided_baricade);
							}
							else
							{
								PlaySnd(Sounds.EnemyHit, SfxVolume);
							}
							destroyTheBullet = true;
						}

						//Check if the bullet collided with the player
						if (collision_rectangle(eBullets[i]->x - eBullets[i]->image_width, eBullets[i]->y - eBullets[i]->image_height, eBullets[i]->x + eBullets[i]->image_width, eBullets[i]->y + eBullets[i]->image_height, player))
						{
							//Decrease player's health
							static_cast<Player*>(player)->Health -= static_cast<eBullet*>(eBullets[i])->Damage;
							static_cast<Player*>(player)->hitShock += 200;
							static_cast<Player*>(player)->Cannons -= 1;

							//Check if player's health is below 0. If yes, destroy him and set game over
							if (static_cast<Player*>(player)->Health <= 0)
							{
								instance_destroy(player);
								end_game = true;
							}
							else
							{
								PlaySnd(Sounds.PlayerHit, SfxVolume);
							}
							destroyTheBullet = true;
						}
						//Bullet didn't collide with anything, but is out of bounds
						else if ((eBullets[i]->x < 0) || (eBullets[i]->x > 800) || (eBullets[i]->y < 0) || (eBullets[i]->y > 600))
						{
							destroyTheBullet = true;
							outOfBounds = true;
						}

						if (destroyTheBullet == true)
						{
							instance_destroy(eBullets[i], !outOfBounds);
							eBullets.erase(eBullets.begin() + i);
						}
					}
				}
			}
		}
	}
	
	//Update all objects
	for (int i = 0; i < Objects.size(); i++)
	{
		Objects[i]->xprevious = Objects[i]->x;
		Objects[i]->yprevious = Objects[i]->y;
		Objects[i]->Update();
	}

	//Draw player's health bar
	if (player != NULL)
	{
		
		int x1 = 10;
		int y1 = 600 - 15;
		int x2;
		if (static_cast<Player*>(player)->MaxHealth / 10 < 600)
		{
			x2 = 90 + (static_cast<Player*>(player)->MaxHealth / 10);
		}
		else
		{
			x2 = 600;
		}
		int y2 = 600 - 10;
		int col = make_color_argb(255, 100, 255, 100);

		DrawHealthbar(x1, y1, x2, y2, (static_cast<Player*>(player)->Health / static_cast<Player*>(player)->MaxHealth), col);
		DrawSomeText(x2 + 20, ((y2 - y1) / 2) + y1 - 5, 10, c_white, false, NumberToString(round(static_cast<Player*>(player)->Health)).c_str());
	}

	//Draw baricade's health bar
	for (int i = 0; i < Baricades.size(); i++)
	{
		int x1 = Baricades[i]->x - 20;
		int y1 = 450 - 15;
		int x2 = 40;
		int y2 = 450 - 10;
		int col = make_color_argb(255, 100, 255, 100);

		DrawHealthbar(x1, y1, x2, y2, (static_cast<Baricade*>(Baricades[i])->Health / static_cast<Baricade*>(Baricades[i])->MaxHealth), col);
		DrawSomeText(Baricades[i]->x, Baricades[i]->y + 20, 10, c_white, true, NumberToString(round(static_cast<Baricade*>(Baricades[i])->Health)).c_str());
	}

	//If invader is boss, draw health bar above him
	for (int i = 0; i < Enemies.size(); i++)
	{
		if (static_cast<Enemy*>(Enemies[i])->InvaderLevel == 4)
		{
			int x1 = static_cast<Enemy*>(Enemies[i])->x - 40;
			int y1 = static_cast<Enemy*>(Enemies[i])->y - 35;
			int x2 = 80;
			int y2 = static_cast<Enemy*>(Enemies[i])->y - 30;
			int col = make_color_argb(255, 255, 100, 100);

			DrawHealthbar(x1, y1, x2, y2, (static_cast<Enemy*>(Enemies[i])->Health / static_cast<Enemy*>(Enemies[i])->MaxHealth), col);
		}
	}

	//Draw all objects
	for (int i = 0; i < Objects.size(); i++)
	{
		Objects[i]->DrawSelf();
	}

	//Draw Score and Level
	DrawSomeText(10, 10, 20, 0xffffffff, false, ("Score  " + NumberToString(Score)).c_str());
	DrawSomeText(10, 30, 15, 0xffffffff, false, ("Level " + NumberToString(Difficulty)).c_str());

	//Game Over screen
	if (end_game == true)
	{
		StopMusic();

		//Get last pressed key for custom name in highscore
		char key = GetPressedKey();
		if (key != -1)
		{
			if ((keyPressed == false) && (lastChar != key))
			{
				lastChar = key;

				if ((lastChar != '=') && (ScoreName.length() < 12))
				{
					//Add character
					ScoreName += lastChar;
				}
				else if (lastChar == '=')
				{
					//Remove character
					if (!ScoreName.empty())
					{
						ScoreName.pop_back();
					}
				}
			}
		}
		else
		{
			keyPressed = false;
			lastChar = 0;
		}

		//If highscore.txt doesn't exists, create it and fill it with some score
		if (!file_exists("highscores.txt"))
		{
			// Create and open a text file
			std::ofstream MyFile("highscores.txt");

			for (int i = 0; i < 8; i++)
			{
				// Write to the file
				MyFile << "RandomGuy" << i << std::endl;
				// Write to the file
				MyFile << (10000 - (i * 100)) << std::endl;
			}

			// Close the file
			MyFile.close();
		}
		
		//Used for highscore table
		int ii = 0;
		std::string names[9];
		int scores[9];
		names[0] = "";
		scores[0] = 0;

		bool score_switch = false;

		std::ifstream hFile("highscores.txt");

		//Create a text string, which is used to output the text file
		std::string textFromFile;

		//Use a while loop together with the getline() function to read the file line by line
		while (std::getline(hFile, textFromFile))
		{
			if (score_switch == false)
			{
				names[ii] = textFromFile;
			}
			else
			{
				std::istringstream(textFromFile) >> scores[ii];
				ii++;
			}
			score_switch = !score_switch;
		}

		//Close the file
		hFile.close();

		//Draw black, transparent rectangle on screen
		end_game_alphaFade = smooth_approach(end_game_alphaFade, 1, 0.01);
		DrawRectangle(0, 0, 800, 600, make_color_argb(end_game_alphaFade*200, 0, 0, 0));

		//Iterpolate anim_Score
		if ((anim_Score != Score) && (end_game_alphaFade > 0.5))
		{
			if (Score - anim_Score > 100)
			{
				anim_Score = smooth_approach(anim_Score, Score, 0.01);
			}
			else
			{
				anim_Score++;
			}
			anim_Score_soundPlay++;
			if (anim_Score_soundPlay >= 10)
			{
				anim_Score_soundPlay = 0;
				PlaySnd(Sounds.s_click, SfxVolume);
			}
		}

		//Draw main text
		int col = make_color_argb(end_game_alphaFade * 255, 255, 255, 255);
		DrawSomeText(400, 85 + (end_game_alphaFade * 20), 40, make_color_argb(end_game_alphaFade * 255, 255, 100, 100), true, "Game Over");

		DrawSomeText(250, 140 + (end_game_alphaFade * 20), 20, col, false, ScoreName.c_str());
		DrawSomeText(500, 140 + (end_game_alphaFade * 20), 20, col, false, NumberToString(anim_Score).c_str());


		//Loop thru highscores and find a place for the player
		bool PlayerScoreDrawn = false;
		bool qualified = false;
		int iv = 0;
		int PlayerScorePos = 9;
		int boardLen = 8;
		for (int i = 0; i < boardLen; i++)
		{
			iv = i;
			if ((scores[i] > Score) && (PlayerScoreDrawn == false))
			{
				
			}
			else if (PlayerScoreDrawn == false)
			{
				//Spot for player found
				PlayerScorePos = iv;
				PlayerScoreDrawn = true;
				qualified = true;
				boardLen = 8;
			}

			//If spot for player found, shift everyone below player down
			if (PlayerScoreDrawn == true)
			{
				iv++;
			}

			//Draw names from highscore table
			DrawSomeText(250, 180 + (iv * 20) + (end_game_alphaFade * 20), 15, col, false, (NumberToString(iv + 1) + ". " + names[i]).c_str());
			//Draw scores from highscore table
			DrawSomeText(500, 180 + (iv * 20) + (end_game_alphaFade * 20), 15, col, false, NumberToString(scores[i]).c_str());
		}

		if (qualified == true)
		{
			//Draw player name to highscore table
			DrawSomeText(250, 180 + (PlayerScorePos * 20) + (end_game_alphaFade * 20), 15, col, false, (NumberToString(PlayerScorePos + 1) + ". " + ScoreName).c_str());
			//Draw player score to highscore table
			DrawSomeText(500, 180 + (PlayerScorePos * 20) + (end_game_alphaFade * 20), 15, col, false, NumberToString(Score).c_str());

			DrawSomeText(400, 500 + (end_game_alphaFade * 20), 10, col, true, "Hit 'ENTER' to save your score and restart the game");
		}
		else
		{
			DrawSomeText(400, 500 + (end_game_alphaFade * 20), 10, col, true, "Hit 'ENTER' to restart the game");
		}

		if ((IsKeyDown(VK_RETURN)) && (ScoreName != ""))
		{
			RestartNow = true;
		}

		//Restart the game after game over
		if (RestartNow == true)
		{
			ClearObjects();
			
			//Save highscore
			if (qualified == true)
			{
				std::ofstream hFile("highscores.txt");

				for (int i = 0; i < 8; i++)
				{
					if (PlayerScorePos != i)
					{
						hFile << names[i] << std::endl;
						hFile << scores[i] << std::endl;
					}
					else
					{
						hFile << ScoreName << std::endl;
						hFile << Score << std::endl;
					}
				}
				hFile.close();
			}

			//...and return back
			goto init;
		}
	}
	//Draw black rectangle on foreground for smooth transition
	if (ingameFade > 0)
	{
		ingameFade = smooth_approach(ingameFade, 0, 0.1);
	}
	DrawRectangle(0, 0, 800, 600, make_color_argb(ingameFade * 255, 0, 0, 0));

	//Restart the game
	if (RestartNow == true)
	{
		goto init;
	}

	Flip();
    
	goto Step;

}