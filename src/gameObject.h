#ifndef GAME_OBJECT_H
#define GAME_OBJECT_H

#include <glm/glm.hpp>
#include <vector>
#include <memory>

const float friction = 1.2f;
const float gravity = 1500;
const float horizAccel = 200;
const float maxYVelocity = 400;
const float maxXVelocity = 150;
const float maxXRunVelocity = 250;
const float maxJumpVelocity = 350;
const float jumpVelocity = 200;
const float jumpHoldBoost = 50;
const float enemyBounce = 200;

enum actions {
	NONE,
	LEFT,
	RIGHT,
	UP,
	DOWN,
	COIN
};

extern struct Actions {
	unsigned int left:1;
	unsigned int right:1;
	unsigned int jump:1;
	unsigned int run:1;
} pressed;

extern float flashTimer;

class GameObject {
public:
	GameObject() = delete;
	GameObject(glm::vec2 pos, unsigned int z, glm::vec2 s, unsigned int id, float a = 0.0f) : position(pos), size(s), angle(), textureId(id), zIndex(z), frameId(), facingLeft(), timer() {};
	~GameObject() {};
	virtual void run(double deltaTime) = 0;
	virtual void collide(const std::shared_ptr<GameObject>& obj) = 0;
	glm::mat4 modelMatrix();
	bool checkCollision(const std::shared_ptr<GameObject>& obj);
	actions repelFrom(const std::shared_ptr<GameObject>& obj, glm::vec2 velocity);
	void getCurrentSprite(int& texture, int& frame)
	{
		texture = textureId;
		frame = frameId;
	};

	bool operator < (const GameObject& obj) const;

protected:
	glm::vec2 position;
	int textureId;
	int zIndex;

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
	virtual void collide(const std::shared_ptr<GameObject>& obj) = 0;
	virtual void die() = 0;

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
	void collide(const std::shared_ptr<GameObject>& obj);
	void die() {}

private:
	void updatePosition(double deltaTime, int move);
	bool heldJump = false;
	bool isJumping = false;
};

class Enemy : public Entity {
public:
	Enemy() = delete;
	Enemy(glm::vec2 pos, unsigned int z, glm::vec2 s, unsigned int id, float a = 0.0f) : Entity(pos, z, s, id, a) {};
	~Enemy() {};
	void run(double deltaTime);
	void animate(double deltaTime);
	void collide(const std::shared_ptr<GameObject>& obj);
	void die();

private:
	int movespeed = 20;
	int direction = LEFT;
	bool isDead = false;
	void updatePosition(double deltaTime, int move);
};

class Coin : public Entity {
public: 
	Coin() = delete;
	Coin(glm::vec2 pos, unsigned int z, glm::vec2 s, unsigned int id, float a = 0.0f) : Entity(pos, z, s, id, a) {}
	void run(double deltaTime);
	void animate(double deltaTime);
	void collide(const std::shared_ptr<GameObject>& obj);
	void die();

private:
	bool isDead = false;
	void updatePosition(double deltaTime, int move) {}
};

#endif
