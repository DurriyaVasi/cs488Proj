#include <glm/glm.hpp>
#include "CollisionType.hpp"
#include <cmath>

class Ball {

	bool lessThanEqualTo(float a, float b) {
 	       return !(a > b);
	}

	bool greaterThanEqualTo(float a, float b) {
        	return !(a < b);
	}


	public:
	glm::vec3 m_pos;
	float m_radius;
	glm::vec3 m_direction;
	float m_speed;
	
	Ball(glm::vec3 pos, float radius, glm::vec3 direction, float speed) 
		: m_pos(pos),
		  m_radius(radius),
		  m_direction(direction),
		  m_speed(speed) {}

	std::pair<CollsionType, glm::mat4> move(Paddle p, Board b) {
		glm::vec3 newPos = m_pos + (speed * direction);
		
		float newCircleBottom = newPos.y - radius;
		float newCircleLeft = newPos.x - radius;
		float newCircleRight = newPos.x + radius;
		float newCircleTop = newPos.y + radius;

		// check if hit bottom or paddles
		float paddleTop = p.m_pos + p.m_height;
		if (lessThanEqualTo(newCircleBottom, paddleTop) {
		
			// find position where it hits y of board top
			glm::vec3 tToPaddleTop = (paddleTop - (m_pos.y))/(m_direction.y);
			glm::vec3 posToPaddleTop = m_pos + (tToPaddleTop * m_direction); 
	
			if ( (lessThanEqualTo((posToPaddleTop.x + radius), (p.m_pos + p.m_width)) &&
				greaterThanEqualTo((posToPaddleTop.x + radius), (p.m_pos - p.m_width))) ||
			     (lessThanEqualTo((posToPaddleTop.x - radius), (p.m_pos + p.m_width)) &&
				greaterThanEqualTo((posToPaddleTop.x - radius), (p.m_pos - p.m_width)))  ) {
				
				m_pos = posToPaddleTop;
				m_direction = glm::vec3(m_direction.x, (-1 * m_direction.y), m_direction.z);
				return std::pair<CollsionType, glm::mat4>(BALL_PADDLE, glm::translate(glm::mat4(), m_pos);
			}
			else {
				m_pos = newPos;
				return std::pair<CollsionType, glm::mat4>(BALL_FLOOR, glm::translate(glm::mat4(), m_pos);
			}	
		}
		
		// check if hit boundaries
		float tX = 0;
		float tY = 0;
		bool hitX = false;
		bool hitY = false;
		if (greaterThanEqualTo(newCircleRight >= b.highXBoundary)) {
			tX = (b.highXBoundary - m_pos.x)/(m_direction.x);
			hitX = true;
		}
		else if (newCircleLeft <= b.lowXBoundary) {
			tX = (b.lowXBoundary - m_pos.x)/(m_direction.x);
			hitX = true;
		}
		if (newCircleTop >= b.highYBoundary) {
			tY = (b.highYBoundary - m_pos.y)/(m_direction.y);
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
			m_pos = m_pos + (tX * m_direction);
			m_direction = glm::vec3((m_direction.x * -1), m_direction.y, m_direction.z);
			return std::pair<CollsionType, glm::mat4>(BALL_BORDER, glm::translate(glm::mat4(), m_pos));
		}
		if (hitY) {
			m_pos = m_pos + (tY * m_direction);
			m_direction = glm::vec3(m_direction.x, (-1 * m_direction.y), m_direction.z);
			return std::pair<CollsionType, glm::mat4>(BALL_BORDER, glm::translate(glm::mat4(), m_pos));
		}

		m_pos = newPos;
		return std::pair<CollsionType, glm::mat4>(NONE, glm::translate(glm::mat4(), m_pos));
}
