#ifndef GAME_OBJECT_H
#define GAME_OBJECT_H

#include <glm/glm.hpp>
#include <vector>
#include <memory>

const float friction = 1.2f;
const float gravity = 1000;
const float horizAccel = 200;
const float maxYVelocity = 400;
const float maxXVelocity = 150;
const float maxXRunVelocity = 250;
const float maxJumpVelocity = 80;
const float jumpVelocity = 400;
const float jumpHoldBoost = 300;

enum actions {
	NONE,
	LEFT,
	RIGHT,
	UP,
	DOWN
};

extern int activeAction;
extern bool isRunning;
extern bool isJumping;

class GameObject {
public:
	GameObject() = delete;
	GameObject(glm::vec2 pos, unsigned int z, glm::vec2 s, unsigned int id, float a = 0.0f) : position(pos), size(s), angle(), key({.textureId = id, .zIndex = z}), frameId(), facingLeft(), timer() {};
	~GameObject() {};
	virtual void run(double deltaTime) = 0;
	virtual void collide(const std::shared_ptr<GameObject>& obj) = 0;
	glm::mat4 modelMatrix();
	bool checkCollision(const std::shared_ptr<GameObject>& obj);
	actions repelFrom(const std::shared_ptr<GameObject>& obj);
	void getCurrentSprite(unsigned int& textureId, unsigned int& frame)
	{
		textureId = key.textureId;
		frame = frameId;
	};

	bool operator < (const GameObject& obj) const;

protected:
	glm::vec2 position;
	struct {
		unsigned int textureId:28;
		unsigned int zIndex:4;
	} key;

	int frameId;
	bool facingLeft;
	float timer;
	glm::vec2 size;
	float angle;
};

extern std::vector<std::shared_ptr<GameObject>> objects;

class Platform : public GameObject {
public:
	Platform() = delete;
	Platform(glm::vec2 pos, unsigned int z, glm::vec2 s, unsigned int id, float a = 0.0f) : GameObject(pos, z, s, id, a) {}
	void run(double deltaTime) {}
	void collide(const std::shared_ptr<GameObject>& obj) {}
};

class Entity : public GameObject {
public:
	Entity() = delete;
	Entity(glm::vec2 pos, unsigned int z, glm::vec2 s, unsigned int id, float a = 0.0f) : GameObject(pos, z, s, id, a), velocity(), isResting(false) {};
	~Entity() {};
	virtual void run(double deltaTime) = 0;
	virtual void animate(double deltaTime) = 0;
	void collide(const std::shared_ptr<GameObject>& obj);

protected:
	glm::vec2 velocity;
	bool isResting;

	virtual void updatePosition(double deltaTime, int move) = 0;

	void checkForCollisions();
	void calcGravity(double deltaTime);
};

class Player : public Entity {
public:
	Player() = delete;
	Player(glm::vec2 pos, unsigned int z, glm::vec2 s, unsigned int id, float a = 0.0f) : Entity(pos, z, s, id, a) {};
	~Player() {};
	void run(double deltaTime);
	void animate(double deltaTime);

private:
	void updatePosition(double deltaTime, int move);
	bool firstJump = false;
};

class Enemy : public Entity {
public:
	Enemy() = delete;
	Enemy(glm::vec2 pos, unsigned int z, glm::vec2 s, unsigned int id, float a = 0.0f) : Entity(pos, z, s, id, a) {};
	~Enemy() {};
	void run(double deltaTime);
	void animate(double deltaTime);

private:
	void updatePosition(double deltaTime, int move);
};

#endif
