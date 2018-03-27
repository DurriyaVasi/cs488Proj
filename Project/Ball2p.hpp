#pragma once

#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include "CollisionType.hpp"
#include <cmath>
#include "Paddle.hpp"
#include "Board.hpp"

class Ball2p {

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


        Ball2p(glm::vec3 pos, float radius, glm::vec3 direction, float speed, SceneNode *node)
                : m_pos(pos),
                  m_radius(radius),
                  m_direction(direction),
                  m_speed(speed),
                  m_node(node) {
                m_node->translate(m_pos);
        }

        Ball2p(SceneNode *node)
                : m_pos(glm::vec3(0, 0, 0)),
                  m_radius(0.05),
                  m_direction(glm::vec3(1, 1, 0)),
                  m_speed(0.02),
                  m_node(node) {
                m_node->translate(m_pos);
        }

        Ball2p()
                : m_pos(glm::vec3()),
                  m_radius(0),
                  m_direction(glm::vec3()),
                  m_speed(0),
                  m_node(NULL) {}
	
	 CollisionType move(PaddleLeft p1, PaddleLeft p2, Board b) {
                glm::vec3 newPos = m_pos + (m_speed * m_direction);


                float newCircleBottom = newPos.y - m_radius;
                float newCircleLeft = newPos.x - m_radius;
                float newCircleRight = newPos.x + m_radius;
                float newCircleTop = newPos.y + m_radius;

                float circleBottom = m_pos.y - m_radius;
                float circleLeft = m_pos.x - m_radius;
                float circleRight = m_pos.x + m_radius;
                float circleTop = m_pos.y + m_radius;


		if (this->greaterThanEqualTo(newCircleTop, b.highYBoundary)) {
                        float tY = (b.highYBoundary - circleTop)/(m_direction.y);
                        m_node->translate(tY * m_direction);
                        m_pos = m_pos + (tY * m_direction);
                        m_direction = glm::vec3(m_direction.x, (-1 * m_direction.y), m_direction.z);
                        return CollisionType::BALL_BORDER;
                }
                if (this->lessThanEqualTo(newCircleBottom, b.lowYBoundary)) {
                        float tY = (b.lowYBoundary - circleBottom)/(m_direction.y);
                        m_node->translate(tY * m_direction);
                        m_pos = m_pos + (tY * m_direction);
                        m_direction = glm::vec3(m_direction.x, (-1 * m_direction.y), m_direction.z);
                        return CollisionType::BALL_BORDER;
                }

                // check if hit bottom or paddles
                float paddleTopP1 = p1.m_pos.x + p1.m_height;
                if (lessThanEqualTo(newCircleLeft, paddleTopP1)) {

                        // find position where it hits y of board top
                        float tToPaddleTop = (paddleTopP1 - circleLeft)/(m_direction.x);
                        glm::vec3 posToPaddleTop = m_pos + (tToPaddleTop * m_direction);

                        if (this->lessThanEqualTo((posToPaddleTop.y), (p1.m_pos.y + p1.m_width)) &&
                                this->greaterThanEqualTo((posToPaddleTop.y), (p1.m_pos.y - p1.m_width))) {
                                m_node->translate(tToPaddleTop * m_direction);
                                m_pos = posToPaddleTop;
                                float percentage = (posToPaddleTop.y - p1.m_pos.y)/(p1.m_width) * 2.0f;
                                m_direction = glm::vec3((-1 * m_direction.x), (equalZero(m_direction.y)? percentage: m_direction.y * percentage), m_direction.z);
                                return CollisionType::BALL_PADDLE;
                        }
                        else {
                                m_pos = newPos;
                                m_node->translate(m_speed * m_direction);
                                return CollisionType::BALL_FLOOR;
			}
		}
		float paddleTopP2 = p2.m_pos.x - p2.m_height;
		if (greaterThanEqualTo(newCircleRight, paddleTopP2)) {
			float tToPaddleTop = (paddleTopP2 - circleRight)/(m_direction.x);
			glm::vec3 posToPaddleTop = m_pos + (tToPaddleTop * m_direction);
			if (this->lessThanEqualTo((posToPaddleTop.y), (p2.m_pos.y + p2.m_width)) &&
                                this->greaterThanEqualTo((posToPaddleTop.y), (p2.m_pos.y - p2.m_width))) {
			 	m_node->translate(tToPaddleTop * m_direction);
                                m_pos = posToPaddleTop;
                                float percentage = (posToPaddleTop.y - p2.m_pos.y)/(p2.m_width) * 2.0f;
				m_direction = glm::vec3((-1 * m_direction.x), (equalZero(m_direction.y)? percentage: m_direction.y * percentage), m_direction.z);
                                return CollisionType::BALL_PADDLE;

			}
			else {
                                m_pos = newPos;
                                m_node->translate(m_speed * m_direction);
                                return CollisionType::BALL_FLOOR;
                        }

		}
		else {
			m_pos = newPos;
                	m_node->translate(m_speed * m_direction);
               		return CollisionType::NONE;
		}

		
	}
};
