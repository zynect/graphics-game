#ifndef GAME_OBJECT_H
#define GAME_OBJECT_H

#include <glm/glm.hpp>

const float gravity = 981;
const float maxYVelocity = 1000;
const float horizAccel = 1;
enum actions {
	NONE,
	LEFT,
	RIGHT
};

extern int activeAction;
extern bool isRunning;
extern bool isJumping;

class GameObject {
public:
	GameObject() = delete;
	GameObject(glm::vec2 pos, unsigned int z, glm::vec2 s, unsigned int id, float a = 0.0f) : position(pos), size(s), angle(), key({.textureId = id, .zIndex = z}) {};
	~GameObject() {};
	virtual void run(double deltaTime) = 0;
	glm::mat4 modelMatrix();

	bool operator < (const GameObject& obj) const;

protected:
	glm::vec2 position;
	struct order{
		unsigned int textureId:28;
		unsigned int zIndex:4;
	} key;
	glm::vec2 size;
	float angle;
};

class Entity : public GameObject {
public:
	Entity() = delete;
	Entity(glm::vec2 pos, unsigned int z, glm::vec2 s, unsigned int id, float a = 0.0f) : GameObject(pos, z, s, id, a), velocity(), isResting(false) {};
	~Entity() {};
	virtual void run(double deltaTime) = 0;

protected:
	glm::vec2 velocity;
	bool isResting;

	virtual void updatePosition(double deltaTime, int move) = 0;

	void checkCollision();
	void calcGravity(double deltaTime);
};

class Player : public Entity {
public:
	Player() = delete;
	Player(glm::vec2 pos, unsigned int z, glm::vec2 s, unsigned int id, float a = 0.0f) : Entity(pos, z, s, id, a) {};
	~Player() {};
	void run(double deltaTime);

private:
	void updatePosition(double deltaTime, int move);
};

class Enemy : public Entity {
public:
	Enemy() = delete;
	Enemy(glm::vec2 pos, unsigned int z, glm::vec2 s, unsigned int id, float a = 0.0f) : Entity(pos, z, s, id, a) {};
	~Enemy() {};
	void run(double deltaTime);

private:
	void updatePosition(double deltaTime, int move);
};

#endif
