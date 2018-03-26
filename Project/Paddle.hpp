#pragma once
#include <glm/glm.hpp>
#include "Board.hpp"
#include "SceneNode.hpp"

class Paddle {
	
	public:
	
        glm::vec3 m_pos;
	float m_height;
	float m_width;
	SceneNode *m_node;

	Paddle() 
		: m_pos(glm::vec3()),
		  m_height(0),
		  m_width(0),
		  m_node(NULL) {}

	Paddle(glm::vec3 pos, float height, float width, SceneNode *node)
		: m_pos(pos),
		  m_height(height),
		  m_width(width),
		  m_node(node) {
		m_node->translate(m_pos);
	}
	
	Paddle(SceneNode *node)
		: m_pos(glm::vec3(0, -2, 0)),
		  m_height(0.05),
		  m_width(0.25),
		  m_node(node) {
		m_node->translate(m_pos);
	}

	void move(float amount, Board b) {
		glm::vec3 newPos = glm::vec3(m_pos.x + amount, m_pos.y, m_pos.z);
		if ((newPos.x + m_width) > b.highXBoundary) {
			newPos = glm::vec3(b.highXBoundary - m_width, m_pos.y, m_pos.z);
		}
		else if ((newPos.x - m_width) < b.lowXBoundary) {
			newPos = glm::vec3(b.lowXBoundary + m_width, m_pos.y, m_pos.z);
		}
		
		m_node->translate(newPos - m_pos);
		m_pos = newPos;
	}
};			
