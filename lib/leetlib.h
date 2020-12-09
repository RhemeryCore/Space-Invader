#pragma once

#include <windows.h>
#include <vector>

typedef unsigned long       DWORD;

#define PI ((float)3.1415926535)

//Define basic colors
#define c_white D3DCOLOR_ARGB(255, 255, 255, 255)
#define c_black D3DCOLOR_ARGB(255, 0, 0, 0)
#define c_red D3DCOLOR_ARGB(255, 255, 0, 0)
#define c_green D3DCOLOR_ARGB(255, 0, 255, 255)
#define c_blue D3DCOLOR_ARGB(255, 0, 0, 255)

//Define object indexes
#define obj_player 0
#define obj_bullet 1
#define obj_ebullet 2
#define obj_enemy 3
#define obj_particle 4
#define obj_baricade 5

//Define sprite indexes
#define spr_player 0
#define spr_bullet 1
#define spr_ebullet 2
#define spr_enemy 3
#define spr_particle 4
#define spr_baricade 5

//Define particles
#define shape_glow 0
#define shape_fineSmoke1 1
#define shape_fineSmoke2 2
#define shape_Fog1 3
#define shape_Smoke 4

class Sprite
{
public:
	/*
	Sprite variables are vectors so objects can be animated
	You can use object's built-in variable image_index to set/get current sprite frame
	Or you can set image_speed to let it animate
	*/

	//Those vectors holds pointers to raw image data
	std::vector<void*> player;
	std::vector<void*> bullet;
	std::vector<void*> Invader1;
	std::vector<void*> Invader2;
	std::vector<void*> Invader3;
	std::vector<void*> Invader4;
	std::vector<void*> particle;
	std::vector<void*> baricade;

	//Pre-loads all sprites and stores them in variables for later use
	void LoadSprites();
};

class Sound
{
public:
	//Those variables points to raw sound data
	void* NormalShoot;
	void* s_click;
	void* PlayerDestroy;
	void* PlayerHit;
	void* EnemyHit;
	void* EnemyDestroy;
	void* EnemyShot;
	void* ButtonClick;
	void* ButtonHoverOver;

	//Pre-loads all sounds and stores them in variables for later use
	void LoadSounds();
};

class Object
{
private:
	float image_speed_timer;
public:
	bool active; //Is object active? | If no, the object won't be updated or drawn
	bool drawSelf; //Give permission for object to automatically draw self
	bool destroy_event_flag; //Give permission to object to call it's deconstructor when used instance_destroy
	int object_index; //What type of object is this | see defined constans above
	int object_id; //Unique ID give on it's creation by Global_object_id
	std::vector<void*> sprite; //Vector of sprites
	int image_index; //Used for sprite vector and it controlls which sprite will be drawn from that vector
	float image_speed; //Used for object animation. It will increment image_index to make animation
	int sprite_width; //Width of the actual file
	int sprite_height; //Height of the actual file
	float image_width; //Width of the sprite that can be modified
	float image_height; //Height of the sprite that can be modified
	float image_xscale; //Scales object's sprite on X axis
	float image_yscale; //Scales object's sprite on Y axis
	float image_angle; //Angle of the sprite
	DWORD image_blend; //Color to blend with the sprite
	float x;
	float y;
	float xprevious; //X position in previous step
	float yprevious; //Y position in previous step

	float speed; //Speed of the object. It will move the object towards the direction variable
	float hspeed; //Horizontal speed of the object
	float vspeed; //Vertical speed of the object
	float direction; //Used with speed variable

	//Collider information
	float collider_x1;
	float collider_y1;
	float collider_x2;
	float collider_y2;
	float collider_radius;
	float collider_center_x;
	float collider_center_y;

	Object();
	virtual ~Object();

	virtual void Update();

	void DrawSelf();
};

class Player : public Object
{
public:
	float Health;
	float MaxHealth;
	float AttackSpeed;
	float Damage;
	int Cannons;

	float hitShock; //The amount of red blend will be on the sprite. Set this value (0 - 255) when player recieves damage

	Player();
	~Player();
	void pUpdate();
};
class Bullet : public Object
{
public:
	float Damage;

	Bullet();
	~Bullet();
};
class eBullet : public Object
{
public:
	float Damage;

	eBullet();
	~eBullet();
};
class Enemy : public Object
{
public:
	float Health;
	float MaxHealth;
	bool ReadyToShoot; //If yes, the enemy will shoot
	float ChanceToShot;

	float Damage;

	int InvaderLevel;

	float hitShock; //The amount of red blend will be on the sprite. Set this value (0 - 255) when enemy recieves damage

	Enemy();
	~Enemy();
	void eUpdate();
};

class Particle : public Object
{
private:
	//Color of the particle
	int part_color_r;
	int part_color_g;
	int part_color_b;

public:

	/*
	cur sufix = current value
	min sufix = minimum value to choose from
	max sufix = maximum value to choose from
	inc sifox = the amount current value will be incremented
	*/

	float part_life_min;
	float part_life_max;
	float part_size_min;
	float part_size_max;
	float part_size_inc;
	float part_speed_min;
	float part_speed_max;
	float part_speed_inc;
	float part_color_alpha1;
	float part_color_alpha2;
	float part_color_alpha3;
	float part_angle_min;
	float part_angle_max;
	float part_angle_inc;
	float part_direction_min;
	float part_direction_max;
	float part_direction_inc;

	float part_life_start; //What life span value was chosen
	float part_life_cur; //Current life of the particle
	float part_size_cur; //Curernt size (it scales image_width and image_height)
	float part_speed_cur; //Current speed
	float part_color_alpha_cur; //Current transparency
	float part_angle_cur; //Current particle angle (image_angle)
	float part_direction_cur; //Current direction the particle will move at

	Particle();
	void pUpdate();
	void part_set_sprite(std::vector<void*> spr); //Set particle sprite
	void part_set_life(float min, float max); //Get random life span in range
	void part_set_alpha(float alpha1, float alpha2, float alpha3); //Set transparency curve in life span
	void part_set_size(float min, float max, float inc); //Get random size and set incrementation
	void part_set_speed(float min, float max, float inc); //Get random speed and set incrementation
	void part_set_color(int r, int g, int b); //Set particle color
	void part_set_angle(float min, float max, float inc); //Get random angle and set incrementation
	void part_set_direction(float min, float max, float inc); //Get random direction and set incrementation
};

class Baricade : public Object
{
public:
	float Health;
	float MaxHealth;

	float hitShock; //The amount of red blend will be on the sprite. Set this value (0 - 255) when baricade recieves damage

	Baricade();
	~Baricade();
	void bUpdate();
};

extern int Object_global_id;

extern std::vector<Object*> Objects; //Main array of objects
extern std::vector<Object*> Enemies; //(Enemy class)
extern std::vector<Object*> Bullets; //(Bullet class)
extern std::vector<Object*> eBullets; //(eBullet class)
extern std::vector<Object*> Baricades; //(Baricade class)
extern std::vector<Object*> BackgroundParticles; //(Particle class)
extern Sprite Sprites;
extern Sound Sounds;

extern float MusicVolume;
extern float SfxVolume;

extern bool ButtonClickPlayed; //Used for buttons to play click sound only once

extern int Difficulty;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 'system' - screen is always 800 x 600
// simply call StartFrame and Flip alternately to run the game; StartFrame returns false if the user is trying to quit

//void ChangeFullscreen();
//Quits the program
bool WantQuit(DWORD clearcolor = 0);
//Refreshes the screen
void Flip(); // flips the screen, frame locked to 60 hz
//Runs the game
void Game(); // you write this :)

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// input
//Gets current mouse position
void GetMousePos(float& x, float& y); // 0,0 is top left; 800,600 is bottom right
//Returns true if a key is pressed
bool IsKeyDown(int key); // use windows VK_ codes for special keys, eg VK_LEFT; use capital chars for letter keys eg 'A', '0'. use VK_LBUTTON and VK_RBUTTON for mouse buttons. 
//Returns a key that is currently pressed
char GetPressedKey();

bool IsKeyHitSinceLastFlip(int key);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// output
// 'sprite output' 
//Loads a image from a file and returns pointer to it
void* LoadSprite(const char* fname);
//Draws sprite on screen
void DrawSprite(std::vector<void*>& sprite, int image_index, float xcentre, float ycentre, float xsize, float ysize, float rotate_angle_radians = 0, DWORD tint_col_argb = 0xffffffff);
//Draws simple rectange on screen
void DrawRectangle(float x1, float y1, float x2, float y2, DWORD col);
//Draws health bar on screen
void DrawHealthbar(float x1, float y1, float x2, float y2, float value, int color);
//Draws button on screen
bool DrawButton(float x, float y, float w, float h, int color, int textColor, const char* text, int textSize);
//Draws slider on screen
float DrawSlider(float x, float y, float len, float value, int col1, int col2);


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// sound
//Plays music from file
int PlayMusic(const char* fname, float volume = 1); // returns a handle which you only need if you want to change volume later with ChangeVolume
//Stops currently playing music
void StopMusic();

// sorry for this being 'snd' but otherwise it clashes with a windows #define (PlaySound) grr
//Loads sounds from file and return pointer to it
void* LoadSnd(const char* fname, bool loop = false); // if you set loop the sample will go on looping forever, until you call StopSound
//Playes sound
int PlaySnd(void* sound, float volume = 1); // returns a handle which you only need if you are going to call StopSound or ChangeVolume()
//Stops sound
void StopSnd(int handle);
//Changes volume of a sound
void ChangeVolume(int handle, float newvolume = 1);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// handy stuff
//Smoothly approaches a value | use like: current = smooth_approach(current, target, 0-1)
float smooth_approach(float current, float target, float smooth);
//Collision for specific object | returns true if there is object on given area
bool collision_rectangle(float x1, float y1, float x2, float y2, Object* obj);
//Collision for array of objects | returns index used to get specific object from array (returns -1 if there is none)
int collision_rectangle(float x1, float y1, float x2, float y2, std::vector<Object*>& obj);
//Creates instance of an object and returns a pointer to it
Object* instance_create(Object* obj, float x, float y);
//Destroys an object and removes it from main object array (you have to manually remove an object from other arrays)
Object* instance_destroy(Object* obj, bool run_deconstructor = true);
//Clears all object vectors
void ClearObjects();

//Returns a color
unsigned long make_color_argb(int alpha, int r, int g, int b);

//Returns random float number from range
float frand_range(float min, float max);
//Linear interpoliation
constexpr float lerp(float a, float b, float t);
//Returns the horizontal x-component of the vector determined by the indicated length and direction
float lengthdir_x(float length, float direction);
//Returns the vertical y-component of the vector determined by the indicated length and direction.
float lengthdir_y(float length, float direction);

void StartTextBatch(int size = 0);
//Draws text on screen
int DrawSomeText(int x, int y, int size, int col, bool centered, const char* pFormat, ...);
void EndTextBatch();