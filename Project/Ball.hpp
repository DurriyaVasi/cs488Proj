#include <glm/glm.hpp>
#include "CollisionType.hpp"
#include <cmath>

bool lessThanEqualTo(float a, float b) {
	return !(a > b);
}

bool greaterThanEqualTo(float a, float b) {
	return !(a < b);
}

class Ball {
	glm::vec2 m_pos;
	float m_radius;
	glm::vec2 m_direction;
	float m_speed;
	
	public:
	Ball(glm::vec2 pos, float radius, glm::vec2 direction, float speed) 
		: m_pos(pos),
		  m_radius(radius),
		  m_direction(direction),
		  m_speed(speed) {}

	std::pair<CollsionType, mat4> move(Paddle p, Board b) {
		glm::vec3 newPos = m_pos + (speed * direction);
		
		float newCircleBottom = newPos.y - radius;
		float newCircleLeft = newPos.x - radius;
		float newCircleRight = newPos.x + radius;
		float newCircleTop = newPos.y + radius;

		// check if hit bottom or paddles
		float paddleTop = b.pos + b.height;
		if (lessThanEqualTo((newPos.y - radius), paddleTop) {
		
			// find position where it hits y of board top
			glm::vec3 tToPaddleTop = (paddleTop - (m_pos.y))/(direction.y)
	
				

			if ( (lessThanEqualTo((newPos.x + radius), (b.pos + b.width)) &&
				greaterThanEqualTo((newPos.x + radius), (b.pos - b.width))) ||
			     (lessThanEqualTo((newPos.x - radius), (b.pos + b.width)) &&
				greaterThanEqualTo((newPos.x - radius), (b.pos - b.width)))  ) {
				
				
		

		// check if hit boundaries
		float tX = 0;
		float tY = 0;
		bool hitX = false;
		bool hitY = false;
		if (newCircleRight >= b.highXBoundary) {
			tX = (b.highXBoundary - m_pos.x)/(direction.x);
			hitX = true;
		}
		else if (newCircleLeft <= b.lowXBoundary) {
			tX = (b.lowXBoundary - m_pos.x)/(direction.x);
			hitX = true;
		}
		if (newCircleTop >= b.highYBoundary) {
			tY = (b.highYBoundary - m_pos.y)/(direction.y);
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
