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
	GameObject() = delete;
	GameObject(glm::vec2 pos, int z, glm::vec2 s) : position(pos), zIndex(z), size(s) {};
	~GameObject() {};

	virtual void run(double deltaTime) = 0;
	void checkCollision();

	glm::vec2 position;
	int zIndex;
	glm::vec2 size;
	float angle;
};

class Player : public GameObject {
public:
	Player() = delete;
	Player(glm::vec2 pos, int z, glm::vec2 s) : GameObject(pos, z, s), velocity(), isResting(false) {};
	~Player() {};
	virtual void run(double deltaTime);
private:
	glm::vec2 velocity;
	bool isResting;

	void updateVelocity(double deltaTime, int key);
};

#endif
