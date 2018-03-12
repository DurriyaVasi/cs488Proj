#include "SceneNode.hpp"
#include "JointNode.hpp"
#include <stack>

using namespace std;

class JointTransform {
	public:
        stack<JointNode*> node;
        stack<double> xAngle;
	stack<double> yAngle;
	JointTransform(stack<JointNode*> node, stack<double> xAngle, stack<double> yAngle) {
		this->node = node;
		this->xAngle = xAngle;
		this->yAngle = yAngle;
	}
	JointTransform(){
	}
};

class JointStackObject {
	public:
	JointTransform curr;
	JointStackObject *next;
	JointStackObject *previous;
	JointStackObject(JointTransform curr, JointStackObject *previous, JointStackObject *next) {
		this->curr = curr;
		this->next = next;
		this->previous = previous;
	}
	~JointStackObject() {
		delete next;
	}
};

class JointStack {
	public:
	JointStackObject *stackPointer;
	JointStackObject *first;
	JointStackObject *last;
	bool bottom;
	bool top;
	bool undone;

	JointStack() {
		stackPointer = NULL;
		first = NULL;
		last = NULL;
		bottom = true;
		top = true;
		undone = false;
	}
	
	~JointStack() {
		delete first;
	}

	bool addToStack(JointTransform j) {
		if (undone) {
                	if(bottom) {
                        	delete first;
				first = NULL;
                                last = NULL;
                                stackPointer = NULL;
                         }
                         else {
                         	delete stackPointer->next;
                                stackPointer->next = NULL;
                                last = stackPointer;
                          }
                }
                bottom = false;
                top = true;
                undone = false;
		if (first == NULL) {
			JointStackObject *so = new JointStackObject(j, NULL, NULL);
                        first = so;
                        last = so;
			stackPointer = so;
                }
		else {
			JointStackObject *so = new JointStackObject(j, last, NULL);
			last->next = so;
			last = so;
			stackPointer = so;
		}
	}
	
	bool undo(JointTransform &j) {
		if (bottom) {
			return false;
		}
		else {
			if (stackPointer->previous == NULL) {
				bottom = true;
			}
			j.node = stackPointer->curr.node;
			j.xAngle = stackPointer->curr.xAngle;
			j.yAngle = stackPointer->curr.yAngle;
			stackPointer = stackPointer->previous;
			undone = true;
			top = false;
			return true;
		}
	}

	bool redo(JointTransform &j) {
		if (top) {
			return false;
		}
		if (bottom) {
			stackPointer = first;
		}
		else {
			stackPointer = stackPointer->next;
		}
		j.node = stackPointer->curr.node;
                j.xAngle = stackPointer->curr.xAngle;
                j.yAngle = stackPointer->curr.yAngle;
			
		if (stackPointer->next == NULL) {
			top = true;
		}
		return true;
	}
};			
			
			
