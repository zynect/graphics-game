#include "gameObject.h"

void Player::checkCollision()
{

}

void Player::updateVelocity(double deltaTime)
{
  //Gravity
  if(!isResting)
  {
    if(velocity.y < maxYVelocity)
    {
      position.y += (velocity.y * deltaTime) + (.5 * gravity * deltaTime * deltaTime);
      velocity.y += gravity * deltaTime;
    }
    else
    {
      position.y += velocity.y * deltaTime;
    }
  }
}
