#include <glm/glm.hpp>

class Paddle {
	
	public:
	
        glm::vec3 m_pos;
	float m_height;
	float m_width;

	Paddle(glm::vec3 pos, float height, float width)
		: m_pos(pos),
		  m_height(height),
		  m_width(width) {}

	glm::mat4 move(float amount, Board b) {
		glm::vec3 newPos = glm::vec3(m_pos.x + amount, m_pos.y, m_pos.z);
		if ((newPos.x + m_width) > b.highXBoundary) {
			newPos = glm::vec3(b.highXBoundary - amount, m_pos.y, m_pos.z);
		}
		else if ((newPos.x - m_width) < b.lowXBoundary) {
			newPos = glm::vec3(b.lowXBoundary + amount, m_pos.y, m_pos.z);
		}
		m_pos = newPos;
		return glm::translate(glm::mat4(), m_pos);
	}
}			
