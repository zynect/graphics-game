#ifndef GAME_OBJECT_H
#define GAME_OBJECT_H

#include <glm/glm.hpp>

const float gravity = 981;
const float maxYVelocity = 1000;
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
	GameObject(glm::vec2 pos, int z, glm::vec2 s) : position(pos), zIndex(z), size(s), angle() {};
	~GameObject() {};
	virtual void run(double deltaTime) = 0;

	glm::vec2 position;
	int zIndex;
	glm::vec2 size;
	float angle;
};

class Entity : public GameObject {
public:
	Entity() = delete;
	Entity(glm::vec2 pos, int z, glm::vec2 s) : GameObject(pos, z, s), velocity(), isResting(false) {};
	~Entity() {};
	virtual void run(double deltaTime) = 0;

protected:
	glm::vec2 velocity;
	bool isResting;

	virtual void updatePosition(double deltaTime, int key) = 0;

	void checkCollision();
	void calcGravity(double deltaTime);
};

class Player : public Entity {
public:
	Player() = delete;
	Player(glm::vec2 pos, int z, glm::vec2 s) : Entity(pos, z, s) {};
	~Player() {};
	void run(double deltaTime);

private:
	void updatePosition(double deltaTime, int key);
};

class Enemy : public Entity {
public:
	Enemy() = delete;
	Enemy(glm::vec2 pos, int z, glm::vec2 s) : Entity(pos, z, s) {};
	~Enemy() {};
	void run(double deltaTime);

private:
	void updatePosition(double deltaTime, int key);
};

#endif
