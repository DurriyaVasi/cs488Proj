#include <glm/glm.hpp>
#include "CollisionType.hpp"
#include <cmath>

class Ball {
	glm::vec2 pos;
	float radius;
	glm::vec2 direction;
	float speed;
	
	public:
	Ball(glm::vec2 pos, glm::vec2 direction, float speed) 
		: pos(pos),
		  direction(direction),
		  speed(speed) {}

	std::pair<CollsionType, mat4> move(Paddle p, Board b) {
		glm::vec3 newPos = pos + (speed * direction);
		
		// check if hit bottom or paddles
		if (((pos.y - radius) < (b.pos + b.height)) && (pos.x + radius ) {
			  
		

		// check if hit boundaries
		float tX = 0;
		float tY = 0;
		bool hitX = false;
		bool hitY = false;
		if (newPos.x >= b.highXBoundary) {
			tX = newPos.x - b.highXBoundary;
			hitX = true;
		}
		else if (newPos.x <= b.lowXBoundary) {
			tX = lowXBoundary - newPos.x;
			hitX = true;
		}
		if (newPos.y >= b.highYBoundary) {
			tY = newPos.y - b.highYBoundary;
			hitY = true;
		}
		
		if (hitX && hitY) {
			// smaller t hit first
			if (tX > tY) {
				hitX = false;
			}
			else  {
				hitY = false;
			}
		}
		if (hitX) {
			pos = pos + (tX * direction);
			return std::pair<CollsionType, mat4>(BALL_BORDER, glm::translate(glm::mat4(), pos));
		}
		if (hitY) {
			pos = pos + (tY * direction);
			return std::pair<CollsionType, mat4>(BALL_BORDER, glm::translate(glm::mat4(), pos));
		}
		
		 
  	
		
}
