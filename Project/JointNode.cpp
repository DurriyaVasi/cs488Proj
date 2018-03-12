#include "JointNode.hpp"

#include "cs488-framework/MathUtils.hpp"

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

using namespace glm;


//---------------------------------------------------------------------------------------
JointNode::JointNode(const std::string& name)
	: SceneNode(name)
{
	m_nodeType = NodeType::JointNode;
}

//---------------------------------------------------------------------------------------
JointNode::~JointNode() {

}
 //---------------------------------------------------------------------------------------
void JointNode::set_joint_x(double min, double init, double max) {
	m_joint_x.min = min;
	m_joint_x.init = init;
	m_joint_x.max = max;
	angleX = init;
	rotate();
}

//---------------------------------------------------------------------------------------
void JointNode::set_joint_y(double min, double init, double max) {
	m_joint_y.min = min;
	m_joint_y.init = init;
	m_joint_y.max = max;
	angleY = init;
	rotate();
}

void JointNode::reset() {
	angleX = m_joint_x.init;
	angleY = m_joint_y.init;
	rotate();
}

bool JointNode::rotateJoint(char axis, float angle) {
	float oldX = angleX;
	float oldY = angleY;
	bool ret = true;
	switch(axis) {
		case 'x':
			angleX = angleX + angle;
			if (angleX > m_joint_x.max) {
				angleX = m_joint_x.max;
			}
			else if (angleX < m_joint_x.min) {
				angleX = m_joint_x.min;
			}
			if (angleX == oldX) {
				ret = false;
			}
			break;
		case 'y':
			angleY = angleY + angle;
			if (angleY > m_joint_y.max) {
				angleY = m_joint_y.max;
			}
			else if (angleY < m_joint_y.min) {
				angleY = m_joint_y.min;
			}
			if (angleY == oldY) {
				ret = false;
			}
			break;
		default:
			break;
	}
	rotate();
	return ret;
}

void JointNode::rotate() {
	trans = glm::rotate(degreesToRadians(angleX), vec3(1, 0, 0)) * glm::rotate(degreesToRadians(angleY), vec3(0, 0, 1));
}	
