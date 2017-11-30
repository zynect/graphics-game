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
	GameObject() : position(), width(), height() {}
	GameObject(glm::vec2 pos, int w, int h) : position(pos), width(w), height(h) {}
	~GameObject();

protected:
	glm::vec2 position;
	int width;
	int height;
};

class Player : GameObject {
public:
	Player() : velocity(), isResting(false) {}
	Player(glm::vec2 pos, int w, int h) : GameObject(pos, w, h), velocity(), isResting(false) {}
	~Player();
private:
	glm::vec2 velocity;
	bool isResting;

	void checkCollision();
	void updateVelocity(float deltaTime, int key);
};

#endif
