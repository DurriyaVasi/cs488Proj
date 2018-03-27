#pragma once
#include <glm/glm.hpp>
#include "Board.hpp"
#include "SceneNode.hpp"

class PaddleLeft {
	
	public:
	
        glm::vec3 m_pos;
	float m_height;
	float m_width;
	SceneNode *m_node;

	PaddleLeft() 
		: m_pos(glm::vec3()),
		  m_height(0),
		  m_width(0),
		  m_node(NULL) {}

	PaddleLeft(glm::vec3 pos, float height, float width, SceneNode *node)
		: m_pos(pos),
		  m_height(height),
		  m_width(width),
		  m_node(node) {
		m_node->translate(m_pos);
	}
	
	PaddleLeft(SceneNode *node, bool left)
		: m_pos(glm::vec3(0, -2, 0)),
		  m_height(0.05),
		  m_width(0.25),
		  m_node(node) {
		if (left) {
			node->rotate('z', 90);
			m_pos = glm::vec3(-2, 0, 0);
			node->translate(m_pos);
		}
		else {
			node->rotate('z', -90);
			m_pos = glm::vec3(2, 0, 0);
			node->translate(m_pos);
		}
		m_node->translate(m_pos);
	}

	void move(float amount, Board b) {
		glm::vec3 newPos = glm::vec3(m_pos.x, m_pos.y + amount, m_pos.z);
		if ((newPos.y + m_width) > b.highYBoundary) {
			newPos = glm::vec3(m_pos.x, b.highYBoundary - m_width, m_pos.z);
		}
		else if ((newPos.y - m_width) < b.lowXBoundary) {
			newPos = glm::vec3(m_pos.x, b.lowXBoundary + m_width, m_pos.z);
		}
		
		m_node->translate(newPos - m_pos);
		m_pos = newPos;
	}
};			
