#include "GameWorld.h"
#include "Map.h"
#include <math.h>

float GameWorld::activeTime = 0;
GLuint GameWorld::shaderProgramIndex = -1;
vector<GameObject*> GameWorld::gameObjectPtrCollection = vector<GameObject*>(0);
vector<GameObject*> display = vector<GameObject*>(0);
vector<GameObject*> enemies = vector<GameObject*>(0);
GameObject* GameWorld::playerPtr = nullptr;
bool GameWorld::ballIH = true;
bool GameWorld::shooting = false;
bool GameWorld::atRest = true;
GLuint GameWorld::tex1 = 0;
GLFWwindow* GameWorld::window = nullptr;
int playerIndex, eIndex;
int prevW, prevA, prevS, prevD, prevSp, prevEnt;
char* map;
int l = 20;
int r = 440;

int const numTriangles = 2;
GLsizei const numVerts = numTriangles * 3;
int const floatsPerVert = 5;
GLsizei const numFloats = numVerts * floatsPerVert;

GLfloat imageVerts[] = {
	-0.0333333f, -0.05f,	0.0f, 0.0f, -1.0f,
	-0.0333333f, 0.05f,	0.0f, 1.0f, -1.0f,
	0.0333333f, -0.05f,	1.0f, 0.0f, -1.0f,

	0.0333333f, 0.05f,		1.0f, 1.0f, -1.0f,
	0.0333333f, -0.05f,	1.0f, 0.0f, -1.0f,
	-0.0333333f, 0.05f,	0.0f, 1.0f, -1.0f
};

bool GameWorld::init(GLFWwindow* windowPtr)
{
	window = windowPtr;
	//GLEW
	glewExperimental = GL_TRUE;
	if(glewInit() != GLEW_OK) return false;

	//Time
	activeTime = float(glfwGetTime());

	//BG Color
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	//Load Shader
	shaderProgramIndex = ShaderManager::loadShaderProgram("Shaders/vertexShader.glsl", "Shaders/fragmentShader.glsl");
	if (shaderProgramIndex == 0)
	{
		std::cout << "Shaders failed to load." << endl;
		return false;
	}
	else
	{
		glUseProgram(shaderProgramIndex);
	}

	Map m = Map();
	m.genLevel();
	
	makeBG();
	makePC();
	popEn();
	makeDisplay();
	setHealth(playerPtr, gameObjectPtrCollection[playerIndex+1]);
	

	//int xoff = -playerPtr->position.x;
	//int yoff = -playerPtr->position.y;
	//int set = gameObjectPtrCollection.size();
	//for (int i = 0; i < set; i++)
	//{
	//	if (i != playerIndex && i != playerPtr->hIndex && i != playerPtr->lIndex)
	//	{
	//		gameObjectPtrCollection[i]->position.x -= xoff;
	//		gameObjectPtrCollection[i]->position.y -= yoff;
	//	}
	//	else
	//	{
	//		gameObjectPtrCollection[i]->position.x = 0;
	//		gameObjectPtrCollection[i]->position.y = 0;
	//	}
	//}

	return true;
}

void GameWorld::update()
{
	float dt = getFrameTime();

	//std::cout<<gameObjectPtrCollection[1]->position.x << " " << gameObjectPtrCollection[1]->position.y<<endl;

	checkMove();
	
	checkVis();
}

void GameWorld::draw()
{
	glClear(GL_COLOR_BUFFER_BIT);
	
	//Draw all visible objects
	int n = gameObjectPtrCollection.size();
	for (int i = 0; i < n; i++)
	{
		if (gameObjectPtrCollection[i]->visible)
		{
			gameObjectPtrCollection[i]-> draw();
		}
	}
	cout<<playerPtr->maxHealth<<endl;
	int e = enemies.size();
	for (int i = 0; i < e; i++)
	{
		if(enemies[i]->visible){enemies[i]->draw();}
	}

	int d = display.size();
	for (int i = 0; i < d; i++)
	{
		if(display[i]->visible){display[i]->draw();}
	}


	
	glFlush();
}

void GameWorld::checkMove()
{
	int wPress = glfwGetKey(window, GLFW_KEY_W); //all the inputs
	int aPress = glfwGetKey(window, GLFW_KEY_A);
	int sPress = glfwGetKey(window, GLFW_KEY_S);
	int dPress = glfwGetKey(window, GLFW_KEY_D);
	int spPress = glfwGetKey(window, GLFW_KEY_SPACE);
	int entPress = glfwGetKey(window, GLFW_KEY_ENTER);
	int set = gameObjectPtrCollection.size();
	int enem = enemies.size();
	float ldif = 1+playerPtr->position.x; 
	float rdif = 1-playerPtr->position.x;

	if (wPress == GLFW_RELEASE && prevW == GLFW_PRESS) //up
	{
		if (playerPtr->position.y < 0.9 && gameObjectPtrCollection[playerPtr->tile + 1]->type != 'w')
		{
			bool comb = false; 
			int num = 0;
			for(int i = 0; i < enemies.size(); i+=3)
			{
				if(playerPtr->tile + 1 == enemies[i]->tile && enemies[i]->visible)
				{
					comb = true;
					num = i;
				}
			}
			if(comb){combat(playerPtr, enemies[num]);}
			else
			{
				playerPtr->position.y += 0.1f;
				gameObjectPtrCollection[playerPtr->hIndex]->position.y += 0.1f;
				gameObjectPtrCollection[playerPtr->lIndex]->position.y += 0.1f;
				playerPtr->tile += 1;
			}
		}
	}
	if (aPress == GLFW_RELEASE && prevA == GLFW_PRESS) //left
	{
		if (playerPtr->position.x > -0.9333 && gameObjectPtrCollection[playerPtr->tile - 20]->type != 'w')
		{
			bool comb = false; 
			int num = 0;
			for(int i = 0; i < enemies.size(); i+=3)
			{
				if(playerPtr->tile - 20 == enemies[i]->tile && enemies[i]->visible)
				{
					comb = true;
					num = i;
				}
			}
			if(comb){combat(playerPtr, enemies[num]);}
			else
			{
				if(playerPtr->position.x < 0 && playerPtr->tile > 300)
				{
					for (int i = 0; i < set; i++)
					{
						if (i != playerIndex && i != playerPtr->hIndex && i != playerPtr->lIndex)
						{
							gameObjectPtrCollection[i]->position.x += 0.0667f;
						}
					}
					for (int i = 0; i < enem; i++)
					{
						enemies[i]->position.x += 0.0667f;
					}
				
					l-=20;
					//cout<<l<<endl;
					playerPtr->tile -= 20;
				}
				else
				{
					playerPtr->position.x -= 0.0667f;
					gameObjectPtrCollection[playerPtr->hIndex]->position.x -= 0.0667f;
					gameObjectPtrCollection[playerPtr->lIndex]->position.x -= 0.0667f;
					playerPtr->tile -= 20;
				}
			}
		}
	}
	if (sPress == GLFW_RELEASE && prevS == GLFW_PRESS) //down
	{
		if (playerPtr->position.y > -0.9 && gameObjectPtrCollection[playerPtr->tile - 1]->type != 'w')
		{
			bool comb = false; 
			int num = 0;
			for(int i = 0; i < enemies.size(); i+=3)
			{
				if(playerPtr->tile - 1 == enemies[i]->tile && enemies[i]->visible)
				{
					comb = true;
					num = i;
				}
			}
			if(comb){combat(playerPtr, enemies[num]);}
			else
			{
				playerPtr->position.y -= 0.1f;
				gameObjectPtrCollection[playerPtr->hIndex]->position.y -= 0.1f;
				gameObjectPtrCollection[playerPtr->lIndex]->position.y -= 0.1f;
				playerPtr->tile -= 1;
			}
		}
	}
	if (dPress == GLFW_RELEASE && prevD == GLFW_PRESS) //right
	{
		if (playerPtr->position.x < 0.9333 && gameObjectPtrCollection[playerPtr->tile + 20]->type != 'w')
		{
			bool comb = false; 
			int num = 0;
			for(int i = 0; i < enemies.size(); i+=3)
			{
				if(playerPtr->tile + 20 == enemies[i]->tile && enemies[i]->visible)
				{
					comb = true;
					num = i;
				}
			}
			if(comb){combat(playerPtr, enemies[num]);}
			else
			{
				if(playerPtr->position.x > 0 && playerPtr->tile < 460)
				{
					for (int i = 0; i < set; i++)
					{
						if (i != playerIndex && i != playerPtr->hIndex && i != playerPtr->lIndex)
						{
							gameObjectPtrCollection[i]->position.x -= 0.0667f;
						}
					}
					for (int i = 0; i < enem; i++)
					{
						enemies[i]->position.x -= 0.0667f;
					}
				
					l+=20;
					playerPtr->tile += 20;
				}
				else
				{
					playerPtr->position.x += 0.0667f;
					gameObjectPtrCollection[playerPtr->hIndex]->position.x += 0.0667f;
					gameObjectPtrCollection[playerPtr->lIndex]->position.x += 0.0667f;
					playerPtr->tile += 20;
				}
			}
		}
	}
	//if (spPress == GLFW_RELEASE && prevSp == GLFW_PRESS)
	//{
	//	playerPtr->health--;
	//	setHealth(playerPtr, gameObjectPtrCollection[playerPtr->hIndex]);
	//	float dec = (float)playerPtr->health/playerPtr->maxHealth;
	//	display[1]->objscale = vec3(dec, 1, 1);
	//	display[1]->position = vec3(0.6f + (dec*0.16666667f), 0.3f, 0);
	//	cout<<display[1]->position.x<<endl;
	//}
	//if (entPress == GLFW_RELEASE && prevEnt == GLFW_PRESS)
	//{
	//	setLevel(playerPtr->level + 1, gameObjectPtrCollection[playerPtr->lIndex]);
	//	playerPtr->level++;
	//}

	prevW = wPress;
	prevA = aPress;
	prevS = sPress;
	prevD = dPress;
	prevSp = spPress;
	prevEnt = entPress;
}

GameWorld::GameWorld(void)
{
}


GameWorld::~GameWorld(void)
{
	int n = gameObjectPtrCollection.size();
	for (int i = 0; i < n; i++)
	{
		delete gameObjectPtrCollection[i];
	}

	playerPtr = nullptr;
}

float GameWorld::getFrameTime()
{
	float currentTime = float(glfwGetTime());
	float dt = currentTime - activeTime;
	activeTime = currentTime;
	return dt;
}

void GameWorld::makePC()
{
	gameObjectPtrCollection.push_back(new GameObject(imageVerts, numVerts, shaderProgramIndex));

	int j = gameObjectPtrCollection.size();
	int loc = 0;
	while (gameObjectPtrCollection[loc]->type != 'f')
	{
		loc = rand()%240;
		cout<<gameObjectPtrCollection[loc]->type<<endl;
	}

	gameObjectPtrCollection[j-1]->position = vec3(((int)loc/20) * 0.0666666f - 0.9666666f, loc%20 * 0.1 - 0.95, 0);
	gameObjectPtrCollection[j-1]->visible = true;
	gameObjectPtrCollection[j-1]->tile = loc;
	playerIndex = j-1;
	gameObjectPtrCollection[j-1]->loadTex("player1.png");
	gameObjectPtrCollection[j-1]->type = 'p';
	gameObjectPtrCollection[j-1]->health = 18;
	gameObjectPtrCollection[j-1]->maxHealth = 18;
	gameObjectPtrCollection[j-1]->init = 7;
	gameObjectPtrCollection[j-1]->atk = 10;
	gameObjectPtrCollection[j-1]->level = 1;
	gameObjectPtrCollection[j-1]->exp = 0;
	playerPtr = gameObjectPtrCollection[j-1];

	gameObjectPtrCollection.push_back(new GameObject(imageVerts, numVerts, shaderProgramIndex));
	gameObjectPtrCollection[j]->position = vec3(((int)loc/20) * 0.0666666f - 0.9666666f, loc%20 * 0.1 - 0.95, 0);
	gameObjectPtrCollection[j]->visible = true;
	gameObjectPtrCollection[j]->tile = loc;
	gameObjectPtrCollection[j-1]->hIndex = j;
	gameObjectPtrCollection[j]->type = 'i';
	//setHealth(j-1, j);

	gameObjectPtrCollection.push_back(new GameObject(imageVerts, numVerts, shaderProgramIndex));
	gameObjectPtrCollection[j+1]->position = vec3(((int)loc/20) * 0.0666666f - 0.9666666f, loc%20 * 0.1 - 0.95, 0);
	gameObjectPtrCollection[j+1]->visible = true;
	gameObjectPtrCollection[j+1]->tile = loc;
	gameObjectPtrCollection[j-1]->lIndex = j+1;
	gameObjectPtrCollection[j+1]->type = 'i';
	setLevel(1, gameObjectPtrCollection[j+1]);
}

void GameWorld::makeBG()
{
	map = loadTextFile("Maps/map.txt");
	int floors = 0;
	for(int a = 0; a < 30; a++)
	{
		for(int b = 0; b < 20; b++)
		{
			gameObjectPtrCollection.push_back(new GameObject(imageVerts, numVerts, shaderProgramIndex));

			int j = gameObjectPtrCollection.size();
			
			gameObjectPtrCollection[j-1]->position = vec3(a * 0.0666666f - 0.9666666f, b * 0.1 - 0.95, 0);
			gameObjectPtrCollection[j-1]->visible = false;
			gameObjectPtrCollection[j-1]->tile = (a)*20 + (b);

			if (map[(a)*20 + (b)]=='f'){gameObjectPtrCollection[j-1]->type='f'; gameObjectPtrCollection[j-1]->loadTex("floor1.png"); floors++;}
			else if (map[(a)*20 + (b)]=='w'){gameObjectPtrCollection[j-1]->type='w'; gameObjectPtrCollection[j-1]->loadTex("wall1.png");}

			//std::cout<<(a)*20 + (b+1)<<endl;
		}
	}
	cout<<floors<<endl;
}

void GameWorld::popEn()
{
	for(int i = 0; i < 39; i++)
	{
		if(i == 38){makeEn(10);}
		else if(i >= 36){makeEn(9);} //2
		else if(i >= 33){makeEn(8);} //3
		else if(i >= 30){makeEn(7);} //3
		else if(i >= 27){makeEn(6);} //3
		else if(i >= 23){makeEn(5);} //4
		else if(i >= 19){makeEn(4);} //4
		else if(i >= 15){makeEn(3);} //4
		else if(i >= 10){makeEn(2);} //5
		else{makeEn(1);}			 //10
	}
}

void GameWorld::makeEn(int lev)
{
	int loc = findEnLoc();
	
	enemies.push_back(new GameObject(imageVerts, numVerts, shaderProgramIndex));

	int j = enemies.size();

	enemies[j-1]->position = vec3(((int)loc/20) * 0.0666666f - 0.9666666f, loc%20 * 0.1 - 0.95, 0);
	enemies[j-1]->visible = false;
	enemies[j-1]->alive = true;
	enemies[j-1]->tile = loc;
	enemies[j-1]->loadTex("daemonblooded.png");
	enemies[j-1]->type = 'e';
	enemies[j-1]->health = (lev + 3)*(lev + 3) - 10; 
	enemies[j-1]->maxHealth = (lev + 3)*(lev + 3) - 10; 
	enemies[j-1]->init = 5;
	enemies[j-1]->level = lev;
	enemies[j-1]->atk = (lev*lev/2)+(5*lev/2);
	
	eIndex = j-1;

	enemies.push_back(new GameObject(imageVerts, numVerts, shaderProgramIndex));
	enemies[j]->position = vec3(((int)loc/20) * 0.0666666f - 0.9666666f, loc%20 * 0.1 - 0.95, 0);
	enemies[j]->visible = false;
	enemies[j]->alive = true;
	enemies[j]->tile = loc;
	enemies[j-1]->hIndex = j;
	enemies[j]->type = 'i';
	setHealth(enemies[j-1], enemies[j]);

	enemies.push_back(new GameObject(imageVerts, numVerts, shaderProgramIndex));
	enemies[j+1]->position = vec3(((int)loc/20) * 0.0666666f - 0.9666666f, loc%20 * 0.1 - 0.95, 0);
	enemies[j+1]->visible = false;
	enemies[j+1]->alive = true;
	enemies[j+1]->tile = loc;
	enemies[j-1]->lIndex = j+1;
	enemies[j+1]->type = 'i';
	setLevel(lev, enemies[j+1]);
}

int GameWorld::findEnLoc()
{
	int ens = enemies.size();
	int loc = 0;
	loc = rand()%600;
	if(gameObjectPtrCollection[loc]->type != 'f') {loc = findEnLoc();}
	if(loc == playerPtr->tile) {loc = findEnLoc();}
	for(int i = 0; i < enemies.size(); i+=3)
	{
		if(enemies[i]->tile == loc)
		{
			loc = findEnLoc();	
		}
	}

	return loc;
}

void GameWorld::combat(GameObject* player, GameObject* enemy)
{
	int pInit = randomize(1, player->init); 
	//cout<<pInit<<endl;
	int eInit = randomize(1, enemy->init);
	//cout<<eInit<<endl;
	int pD = randomize(0, player->atk);
	//cout<<pD<<endl;
	int eD = randomize(0, enemy->atk);
	//cout<<eD<<endl;

	if(pInit > eInit)
	{
		enemy->health -= pD;
		setHealth(enemies[enemy->hIndex-1], enemies[enemy->hIndex]);
		if(enemy->health > 0)
		{
			player->health -= eD;
			setHealth(playerPtr, gameObjectPtrCollection[playerIndex+1]);
			if(player->health <= 0)
			{
				for(int i = 0; i < gameObjectPtrCollection.size(); i++) {gameObjectPtrCollection[i]->visible = false;}
				for(int j = 0; j < enemies.size(); j++) {enemies[j]->visible = false;}
				for(int k = 0; k < display.size(); k++) {display[k]->visible = false;}
			}
		}
		else
		{
			enemies[enemy->lIndex]->visible = false; enemies[enemy->lIndex]->alive = false;
			enemies[enemy->hIndex]->visible = false; enemies[enemy->hIndex]->alive = false;
			enemies[enemy->hIndex-1]->visible = false;enemies[enemy->hIndex-1]->alive = false;
			player->exp+=enemy->level; if(enemy->level > player->level){player->exp += (enemy->level - player->level)*(enemy->level - player->level + 1);}
			int lev = player->nlCalc();
			if(lev > player->level)
			{
				setPStats(player->atk + 5, 18, 23);
				player->maxHealth += 10;
				player->health = player->maxHealth;
				setHealth(player, gameObjectPtrCollection[playerIndex+1]);
			}
			setLevel(lev, gameObjectPtrCollection[playerIndex+2]);
		}
	}
	else
	{
		player->health -= eD;
		setHealth(playerPtr, gameObjectPtrCollection[playerIndex+1]);
		if(player->health > 0)
		{
			enemy->health -= pD;
			setHealth(enemies[enemy->hIndex-1], enemies[enemy->hIndex]);
			if(enemy->health <= 0)
			{
				enemies[enemy->lIndex]->visible = false; enemies[enemy->lIndex]->alive = false;
				enemies[enemy->hIndex]->visible = false; enemies[enemy->hIndex]->alive = false;
				enemies[enemy->hIndex-1]->visible = false;enemies[enemy->hIndex-1]->alive = false;
				player->exp+=enemy->level; if(enemy->level > player->level){player->exp += (enemy->level - player->level)*(enemy->level - player->level + 1);}
				int lev = player->nlCalc();
				if(lev > player->level)
				{
					setPStats(player->atk + 5, 18, 23);
					player->maxHealth += 10; if(player->maxHealth >= 99){player->maxHealth = 99;}
					player->health = player->maxHealth;
					setHealth(player, gameObjectPtrCollection[playerIndex+1]);
					setLevel(lev, gameObjectPtrCollection[playerIndex+2]);
				}
			}
		}
		else
		{
			for(int i = 0; i < gameObjectPtrCollection.size(); i++) {gameObjectPtrCollection[i]->visible = false;}
			for(int j = 0; j < enemies.size(); j++) {enemies[j]->visible = false;}
			for(int k = 0; k < display.size(); k++) {display[k]->visible = false;}
		}
	}
}

void GameWorld::makeDisplay()
{
	GLfloat imageVerts[] = {
		-0.26666667f, -1.0f,	0.0f, 0.0f, -1.0f,
		-0.26666667f, 1.0f,		0.0f, 1.0f, -1.0f,
		0.26666667f, -1.0f,		1.0f, 0.0f, -1.0f,

		0.26666667f, 1.0f,		1.0f, 1.0f, -1.0f,
		0.26666667f, -1.0f,		1.0f, 0.0f, -1.0f,
		-0.26666667f, 1.0f,		0.0f, 1.0f, -1.0f
	};
	display.push_back(new GameObject(imageVerts, numVerts, shaderProgramIndex));

	int j = display.size();
	//0 - BG
	display[j-1]->position = vec3(0.7333333f, 0, 0);
	display[j-1]->loadTex("Display.png");

	GLfloat barVerts[] = {
		-0.16666667f, -0.0375f,	0.0f, 0.0f, -1.0f,
		-0.16666667f, 0.0375f,	0.0f, 1.0f, -1.0f,
		0.16666667f, -0.0375f,	1.0f, 0.0f, -1.0f,
	
		0.16666667f, 0.0375f,	1.0f, 1.0f, -1.0f,
		0.16666667f, -0.0375f,	1.0f, 0.0f, -1.0f,
		-0.16666667f, 0.0375f,	0.0f, 1.0f, -1.0f
	};
	display.push_back(new GameObject(barVerts, numVerts, shaderProgramIndex));

	j = display.size();
	//1 - hbar
	display[j-1]->position = vec3(0.76666667f, 0.3f, 0);
	display[j-1]->loadTex("red.png");

	display.push_back(new GameObject(barVerts, numVerts, shaderProgramIndex));
	j = display.size();
	//2 - mbar
	display[j-1]->position = vec3(0.76666667f, 0.2f, 0);
	display[j-1]->loadTex("blue.png");

	GLfloat picVerts[] = {
		-0.1333333f, -0.2f,	0.0f, 0.0f, -1.0f,
		-0.1333333f, 0.2f,	0.0f, 1.0f, -1.0f,
		0.1333333f, -0.2f,	1.0f, 0.0f, -1.0f,
	
		0.1333333f, 0.2f,	1.0f, 1.0f, -1.0f,
		0.1333333f, -0.2f,	1.0f, 0.0f, -1.0f,
		-0.1333333f, 0.2f,	0.0f, 1.0f, -1.0f
	};

	display.push_back(new GameObject(picVerts, numVerts, shaderProgramIndex));

	j = display.size();
	//3 - picon
	display[j-1]->position = vec3(0.73333333f, 0.7f, 0);
	display[j-1]->loadTex("player1.png");

	GLfloat numberVerts[] = {
		-0.0111111f, -0.025f,	0.0f, 0.0f, -1.0f,
		-0.0111111f, 0.025f,	0.0f, 1.0f, -1.0f,
		0.0111111f, -0.025f,	1.0f, 0.0f, -1.0f,

		0.0111111f, 0.025f,		1.0f, 1.0f, -1.0f,
		0.0111111f, -0.025f,	1.0f, 0.0f, -1.0f,
		-0.0111111f, 0.025f,	0.0f, 1.0f, -1.0f
	};

	//4 - m1c
	display.push_back(new GameObject(numberVerts, numVerts, shaderProgramIndex));
	j = display.size();
	display[j-1]->position = vec3(0.74333333f, 0.2f, 0);
	display[j-1]->loadTex("Numbers/8.png");

	//5 - m10c
	display.push_back(new GameObject(numberVerts, numVerts, shaderProgramIndex));
	j = display.size();
	display[j-1]->position = vec3(0.71666667f, 0.2f, 0);
	display[j-1]->loadTex("Numbers/8.png");

	//6 - mslash
	display.push_back(new GameObject(numberVerts, numVerts, shaderProgramIndex));
	j = display.size();
	display[j-1]->position = vec3(0.76666667f, 0.2f, 0);
	display[j-1]->loadTex("Numbers/slash.png");

	//7 - m10t
	display.push_back(new GameObject(numberVerts, numVerts, shaderProgramIndex));
	j = display.size();
	display[j-1]->position = vec3(0.78666667f, 0.2f, 0);
	display[j-1]->loadTex("Numbers/8.png");

	//8 - m1t
	display.push_back(new GameObject(numberVerts, numVerts, shaderProgramIndex));
	j = display.size();
	display[j-1]->position = vec3(0.81333333f, 0.2f, 0);
	display[j-1]->loadTex("Numbers/8.png");

	//9 - h1c
	display.push_back(new GameObject(numberVerts, numVerts, shaderProgramIndex));
	j = display.size();
	display[j-1]->position = vec3(0.74333333f, 0.3f, 0);
	display[j-1]->loadTex("Numbers/8.png");

	//10 - h10c
	display.push_back(new GameObject(numberVerts, numVerts, shaderProgramIndex));
	j = display.size();
	display[j-1]->position = vec3(0.71666667f, 0.3f, 0);
	display[j-1]->loadTex("Numbers/8.png");

	//11 - hslash
	display.push_back(new GameObject(numberVerts, numVerts, shaderProgramIndex));
	j = display.size();
	display[j-1]->position = vec3(0.76666667f, 0.3f, 0);
	display[j-1]->loadTex("Numbers/slash.png");

	//12 - h10t
	display.push_back(new GameObject(numberVerts, numVerts, shaderProgramIndex));
	j = display.size();
	display[j-1]->position = vec3(0.78666667f, 0.3f, 0);
	display[j-1]->loadTex("Numbers/8.png");

	//13 - h1t
	display.push_back(new GameObject(numberVerts, numVerts, shaderProgramIndex));
	j = display.size();
	display[j-1]->position = vec3(0.81333333f, 0.3f, 0);
	display[j-1]->loadTex("Numbers/8.png");

	//14 - d1
	display.push_back(new GameObject(numberVerts, numVerts, shaderProgramIndex));
	j = display.size();
	display[j-1]->position = vec3(0.6375f, 0.4f, 0);
	display[j-1]->loadTex("Numbers/1.png");

	//15 - d10
	display.push_back(new GameObject(numberVerts, numVerts, shaderProgramIndex));
	j = display.size();
	display[j-1]->position = vec3(0.6125f, 0.4f, 0);
	display[j-1]->loadTex("Numbers/1.png");

	setPStats(playerPtr->atk, 10, 23);

	for(int k = 0; k < display.size(); k++) {display[k]->visible = true;}
}

void GameWorld::checkVis()
{
	int tileNum = playerPtr->tile;

	
	gameObjectPtrCollection[tileNum]->visible = true;

	if(tileNum%20 != 19)
	{
		gameObjectPtrCollection[tileNum+1]->visible = true; enVis(gameObjectPtrCollection[tileNum+1]);
		if(tileNum>19) {gameObjectPtrCollection[tileNum-19]->visible = true; enVis(gameObjectPtrCollection[tileNum-19]);}
		if(tileNum<579) {gameObjectPtrCollection[tileNum+21]->visible = true; enVis(gameObjectPtrCollection[tileNum+21]);}
	}

	if(tileNum%20 != 0)
	{
		gameObjectPtrCollection[tileNum-1]->visible = true; enVis(gameObjectPtrCollection[tileNum-1]);
		if(tileNum<579) {gameObjectPtrCollection[tileNum+19]->visible = true; enVis(gameObjectPtrCollection[tileNum+19]);}
		if(tileNum>19) {gameObjectPtrCollection[tileNum-21]->visible = true; enVis(gameObjectPtrCollection[tileNum-21]);}
	}
	
	if(tileNum>19) {gameObjectPtrCollection[tileNum-20]->visible = true; enVis(gameObjectPtrCollection[tileNum-20]);}
	if(tileNum<579) {gameObjectPtrCollection[tileNum+20]->visible = true; enVis(gameObjectPtrCollection[tileNum+20]);}
}

void GameWorld::enVis(GameObject* tile)
{
	for(int i = 0; i < enemies.size(); i++)
	{
		if(enemies[i]->alive)
		{
			if(tile->tile == enemies[i]->tile)
			{
				enemies[i]->visible = true;
			}
		}
	}
}

void GameWorld::setPStats(int a, int mc, int mt)
{
	playerPtr->atk = a;
	int ten = (int)a/10;
	switch(ten) {
		case 1: display[15]->loadTex("Numbers/1.png"); break;
		case 2:	display[15]->loadTex("Numbers/2.png"); break;
		case 3:	display[15]->loadTex("Numbers/3.png"); break;
		case 4:	display[15]->loadTex("Numbers/4.png"); break;
		case 5:	display[15]->loadTex("Numbers/5.png"); break;
		case 6:	display[15]->loadTex("Numbers/6.png"); break;
		case 7:	display[15]->loadTex("Numbers/7.png"); break;
		case 8:	display[15]->loadTex("Numbers/8.png"); break;
		case 9:	display[15]->loadTex("Numbers/9.png"); break;
		case 0: display[15]->loadTex("Numbers/blank.png"); break;
		default:	display[15]->loadTex("Numbers/blank.png"); break;
	}
	switch(a%10) {
		case 1: display[14]->loadTex("Numbers/1.png"); break;
		case 2:	display[14]->loadTex("Numbers/2.png"); break;
		case 3:	display[14]->loadTex("Numbers/3.png"); break;
		case 4:	display[14]->loadTex("Numbers/4.png"); break;
		case 5:	display[14]->loadTex("Numbers/5.png"); break;
		case 6:	display[14]->loadTex("Numbers/6.png"); break;
		case 7:	display[14]->loadTex("Numbers/7.png"); break;
		case 8:	display[14]->loadTex("Numbers/8.png"); break;
		case 9:	display[14]->loadTex("Numbers/9.png"); break;
		case 0: display[14]->loadTex("Numbers/0.png"); break;
		default:	display[14]->loadTex("Numbers/blank.png"); break;
	}


}

void GameWorld::setHealth(GameObject* cPtr, GameObject* iPtr)
{
	float cH = cPtr->health / (float)cPtr->maxHealth;
	//cout << cH << endl;
	if(cH > 0.857114f){iPtr->loadTex("hfull.png");}
	else if(cH > 0.714285f){iPtr->loadTex("h6.png");}
	else if(cH > 0.571428f){iPtr->loadTex("h5.png");}
	else if(cH > 0.428571f){iPtr->loadTex("h4.png");}
	else if(cH > 0.245714f){iPtr->loadTex("h3.png");}
	else if(cH > 0.142857f){iPtr->loadTex("h2.png");}
	else if(cH > 0.0f){iPtr->loadTex("h1.png");}

	if(cPtr->type == 'p')
	{
		int hc = cPtr->health;
		int hm = cPtr->maxHealth;
		int tenc = (int)hc/10;
		int tenm = (int)hm/10;
		switch(tenc) {
			case 1: display[10]->loadTex("Numbers/1.png"); break;
			case 2:	display[10]->loadTex("Numbers/2.png"); break;
			case 3:	display[10]->loadTex("Numbers/3.png"); break;
			case 4:	display[10]->loadTex("Numbers/4.png"); break;
			case 5:	display[10]->loadTex("Numbers/5.png"); break;
			case 6:	display[10]->loadTex("Numbers/6.png"); break;
			case 7:	display[10]->loadTex("Numbers/7.png"); break;
			case 8:	display[10]->loadTex("Numbers/8.png"); break;
			case 9:	display[10]->loadTex("Numbers/9.png"); break;
			case 0: display[10]->loadTex("Numbers/blank.png"); break;
			default:	display[10]->loadTex("Numbers/blank.png"); break;
		}
		switch(hc%10) {
			case 1: display[9]->loadTex("Numbers/1.png"); break;
			case 2:	display[9]->loadTex("Numbers/2.png"); break;
			case 3:	display[9]->loadTex("Numbers/3.png"); break;
			case 4:	display[9]->loadTex("Numbers/4.png"); break;
			case 5:	display[9]->loadTex("Numbers/5.png"); break;
			case 6:	display[9]->loadTex("Numbers/6.png"); break;
			case 7:	display[9]->loadTex("Numbers/7.png"); break;
			case 8:	display[9]->loadTex("Numbers/8.png"); break;
			case 9:	display[9]->loadTex("Numbers/9.png"); break;
			case 0: display[9]->loadTex("Numbers/0.png"); break;
			default:	display[9]->loadTex("Numbers/blank.png"); break;
		}

		switch(tenm) {
			case 1: display[12]->loadTex("Numbers/1.png"); break;
			case 2:	display[12]->loadTex("Numbers/2.png"); break;
			case 3:	display[12]->loadTex("Numbers/3.png"); break;
			case 4:	display[12]->loadTex("Numbers/4.png"); break;
			case 5:	display[12]->loadTex("Numbers/5.png"); break;
			case 6:	display[12]->loadTex("Numbers/6.png"); break;
			case 7:	display[12]->loadTex("Numbers/7.png"); break;
			case 8:	display[12]->loadTex("Numbers/8.png"); break;
			case 9:	display[12]->loadTex("Numbers/9.png"); break;
			case 0: display[12]->loadTex("Numbers/0.png"); break;
			default:	display[12]->loadTex("Numbers/blank.png"); break;
		}
		switch(hm%10) {
			case 1: display[13]->loadTex("Numbers/1.png"); break;
			case 2:	display[13]->loadTex("Numbers/2.png"); break;
			case 3:	display[13]->loadTex("Numbers/3.png"); break;
			case 4:	display[13]->loadTex("Numbers/4.png"); break;
			case 5:	display[13]->loadTex("Numbers/5.png"); break;
			case 6:	display[13]->loadTex("Numbers/6.png"); break;
			case 7:	display[13]->loadTex("Numbers/7.png"); break;
			case 8:	display[13]->loadTex("Numbers/8.png"); break;
			case 9:	display[13]->loadTex("Numbers/9.png"); break;
			case 0: display[13]->loadTex("Numbers/blank.png"); break;
			default:	display[13]->loadTex("Numbers/blank.png"); break;
		}
	}
}

void GameWorld::setLevel(int lvl, GameObject* ptr)
{
	switch(lvl) {
		case 1: ptr->loadTex("l1.png"); break;
		case 2:	ptr->loadTex("l2.png"); break;
		case 3:	ptr->loadTex("l3.png"); break;
		case 4:	ptr->loadTex("l4.png"); break;
		case 5:	ptr->loadTex("l5.png"); break;
		case 6:	ptr->loadTex("l6.png"); break;
		case 7:	ptr->loadTex("l7.png"); break;
		case 8:	ptr->loadTex("l8.png"); break;
		case 9:	ptr->loadTex("l9.png"); break;
		case 10: ptr->loadTex("lboss.png"); break;
		default:	ptr->loadTex("l1.png"); break;
	}
}

char* GameWorld::loadTextFile(const char* file)
{
	//Open
	ifstream is(file, ios::binary);
	if(!is.is_open()) return 0;
	
	//Length
	is.seekg(0, ios::end);
	int length = (int)is.tellg();
	is.seekg(0, ios::beg);

	//Read
	char* fileContents = new char[length + 1];
	is.read(fileContents, length);
	fileContents[length] = 0;

	//Close
	is.close();

	return fileContents;
}

int GameWorld::randomize(int min, int max)
{
	if (max-min > 0)
	{
		return (rand() % (max-min+1)) + min;
	}
	else {
		return min;
	}
}
