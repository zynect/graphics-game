#ifndef GAME_OBJECT_H
#define GAME_OBJECT_H

#include <glm/glm.hpp>

const float gravity = 9.81;
const float maxYVelocity = 10;
const float horizAccel = 1;
enum movement {
	NONE,
	LEFT,
	RIGHT,
	UP,
	DOWN
};

class GameObject {
public:
	GameObject() : position(), width(), height() {};
	GameObject(glm::vec2 pos, int w, int h) : position(pos), width(w), height(h) {};
	~GameObject() {};

	virtual void run(double deltaTime) = 0;

	glm::vec2 position;
	int width;
	int height;
};

class Player : public GameObject {
public:
	Player() : velocity(), isResting(false) {};
	Player(glm::vec2 pos, int w, int h) : GameObject(pos, w, h), velocity(), isResting(false) {};
	~Player() {};
	virtual void run(double deltaTime);
private:
	glm::vec2 velocity;
	bool isResting;

	void checkCollision();
	void updateVelocity(double deltaTime, int key);
};

#endif
