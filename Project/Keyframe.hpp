#pragma  once
#include <glm/glm.hpp>
#include <queue>
#include <iostream>

class KeyFrame {

	public:
	std::queue<glm::vec3> m_positions;
	float m_speed;
	glm::vec3 m_direction;
	bool m_start;
	glm::vec3 m_pos;
	bool m_done;

	bool lessThanEqualTo(float a, float b) {
		return !(a > b);
	}

	KeyFrame(std::queue<glm::vec3> positions, float speed)
		: m_positions(positions),
		  m_speed(speed),
		  m_pos(glm::vec3()),
		  m_direction(glm::vec3()),
		  m_start(true),
		  m_done(false) {
		if (m_positions.size() == 0) {
			std::cerr << "no positions" << std::endl;
		}
	}

	KeyFrame()
		: m_speed(0),
		  m_direction(glm::vec3()),
		  m_start(true),
		  m_done(true),
		  m_pos(glm::vec3()) {}

	glm::vec3 popQueue() {
		glm::vec3 movement = m_positions.front() - m_pos;
		m_pos = m_positions.front();
                m_positions.pop();
		if (m_positions.size() == 0) {
			m_done = true;
		}
		else {
                	m_direction = glm::normalize(m_positions.front() - m_pos); 
		}
		return movement;
	}

	glm::vec3 move() {
		if (m_done) {
			return glm::vec3(0, 0, 0);
		}
		if (m_start) {
			if (m_positions.size() == 0) {
				std::cerr << "no positions to pop at start" << std::endl;
			}
			m_start = false;
			return popQueue();
		}
		else {
			glm::vec3 newPos = m_pos + (m_speed * m_direction);
			float tToPositionsTop = ((m_positions.front() - m_pos)/m_direction).x;
			if (lessThanEqualTo(tToPositionsTop, m_speed)) {
				return popQueue();
			}
			else {
				m_pos = newPos;
				return (m_speed * m_direction);
			}
		}
	}
};
			
