#pragma once

#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include "CollisionType.hpp"
#include <cmath>
#include "Paddle.hpp"
#include "Board.hpp"

class Ball {

	bool lessThanEqualTo(float a, float b) {
 	       return !(a > b);
	}

	bool greaterThanEqualTo(float a, float b) {
        	return !(a < b);
	}

	bool equalZero(float a) {
		return ((a < 0.001) && (a > -0.001));
	}

	public:
	glm::vec3 m_pos;
	float m_radius;
	glm::vec3 m_direction;
	float m_speed;
	SceneNode *m_node;


	Ball(glm::vec3 pos, float radius, glm::vec3 direction, float speed, SceneNode *node)
		: m_pos(pos),
		  m_radius(radius),
		  m_direction(direction),
		  m_speed(speed),
		  m_node(node) {
		m_node->translate(m_pos);
	}

	Ball(SceneNode *node)
		: m_pos(glm::vec3(0, 0, 0)),
		  m_radius(0.05),
		  m_direction(glm::vec3(0, 1, 0)),
		  m_speed(0.02),
		  m_node(node) {
		m_node->translate(m_pos);	
	}

	Ball()
                : m_pos(glm::vec3()),
                  m_radius(0),
                  m_direction(glm::vec3()),
                  m_speed(0),
                  m_node(NULL) {}


	CollisionType move(Paddle p, Board b) {
		glm::vec3 newPos = m_pos + (m_speed * m_direction);


		float newCircleBottom = newPos.y - m_radius;
		float newCircleLeft = newPos.x - m_radius;
		float newCircleRight = newPos.x + m_radius;
		float newCircleTop = newPos.y + m_radius;

		float circleBottom = m_pos.y - m_radius;
		float circleLeft = m_pos.x - m_radius;
		float circleRight = m_pos.x + m_radius;
		float circleTop = m_pos.y + m_radius;

		// check if hit bottom or paddles
		float paddleTop = p.m_pos.y + p.m_height;
		if (lessThanEqualTo(newCircleBottom, paddleTop)) {
		
			// find position where it hits y of board top
			float tToPaddleTop = (paddleTop - circleBottom)/(m_direction.y);
			glm::vec3 posToPaddleTop = m_pos + (tToPaddleTop * m_direction); 
	
			if (this->lessThanEqualTo((posToPaddleTop.x), (p.m_pos.x + p.m_width)) &&
				this->greaterThanEqualTo((posToPaddleTop.x), (p.m_pos.x - p.m_width))) {
				m_node->translate(tToPaddleTop * m_direction);
				m_pos = posToPaddleTop;
				float percentage = (posToPaddleTop.x - p.m_pos.x)/(p.m_width);
				m_direction = glm::vec3((equalZero(m_direction.x)? percentage: m_direction.x * percentage), (-1 * m_direction.y), m_direction.z);
				return CollisionType::BALL_PADDLE;
			}
			else {
				m_pos = newPos;
				m_node->translate(m_speed * m_direction);
				return CollisionType::BALL_FLOOR;
			}	
		}
		
		// check if hit boundaries
		float tX = 0;
		float tY = 0;
		bool hitX = false;
		bool hitY = false;
		bool hitXhigh = false;
		bool hitXlow = false;
		if (this->greaterThanEqualTo(newCircleRight, b.highXBoundary)) {
			tX = (b.highXBoundary - circleRight)/(m_direction.x);
			hitX = true;
			hitXhigh = true;
		}
		else if (this->lessThanEqualTo(newCircleLeft, b.lowXBoundary)) {
			tX = (b.lowXBoundary - circleLeft)/(m_direction.x);
			hitX = true;
			hitXlow = true;
		}
		if (this->greaterThanEqualTo(newCircleTop, b.highYBoundary)) {
			tY = (b.highYBoundary - circleTop)/(m_direction.y);
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
			if (hitXhigh && hitXlow) {
				std::cerr << "hit high adn low x boundary" << std::endl;
			}	
			m_node->translate(tX * m_direction);
			m_pos = m_pos + (tX * m_direction);
			m_direction = glm::vec3((m_direction.x * -1), m_direction.y, m_direction.z);
			return CollisionType::BALL_BORDER;
		}
		if (hitY) {
			m_node->translate(tY * m_direction);
			m_pos = m_pos + (tY * m_direction);
			m_direction = glm::vec3(m_direction.x, (-1 * m_direction.y), m_direction.z);
			return CollisionType::BALL_BORDER;
		}

		m_pos = newPos;
		m_node->translate(m_speed * m_direction);
		return CollisionType::NONE;
	}
};
