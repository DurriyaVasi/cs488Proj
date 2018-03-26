#pragma once
#include <glm/glm.hpp>
#include "SceneNode.hpp"
#include "Keyframe.hpp"

class Animator {
	public:

	SceneNode *m_node;
	KeyFrame m_keyFrame;

	Animator() :
		m_node(NULL),
		m_keyFrame(KeyFrame()) {}
		
	Animator(SceneNode *node, KeyFrame keyFrame) :
		m_node(node),
		m_keyFrame(keyFrame) {}

	void move() {
		m_node->translate(m_keyFrame.move());
	}
};	
