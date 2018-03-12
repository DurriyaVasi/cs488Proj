#include "A3.hpp"
#include "scene_lua.hpp"
using namespace std;

#include "cs488-framework/GlErrorCheck.hpp"
#include "cs488-framework/MathUtils.hpp"
#include "GeometryNode.hpp"
#include "JointNode.hpp"

#include <imgui/imgui.h>

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/io.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace glm;

static bool show_gui = true;

const size_t CIRCLE_PTS = 48;

//----------------------------------------------------------------------------------------
// Constructor
A3::A3(const std::string & luaSceneFile)
	: m_luaSceneFile(luaSceneFile),
	  m_positionAttribLocation(0),
	  m_normalAttribLocation(0),
	  m_vao_meshData(0),
	  m_vbo_vertexPositions(0),
	  m_vbo_vertexNormals(0),
	  m_vao_arcCircle(0),
	  m_vbo_arcCircle(0),
	  pickingMode(0),
	  do_picking(false),
	  leftMousePressed(false),
	  rightMousePressed(false),
	  middleMousePressed(false),
	  oldX(0),
	  oldY(0),
	  translateNode(NULL),
	  rotateNode(NULL),
	  noTranslate(mat4()),
	  noRotate(mat4()),
	  hasZBuffer(false),
	  hasBackCull(true),
	  hasFrontCull(false),
	  drawCircle(true),
	  jointStack(JointStack()),
	  headSideJoint(NULL),
	  headId(0)
{
	colours.push(vec3(1.0f, 0.0f, 0.0f));
	colours.push(vec3(0.0f, 1.0f, 0.0f));
	colours.push(vec3(0.0f, 0.0f, 1.0f));
	colours.push(vec3(1.0f, 0.0f, 1.0f));
	colours.push(vec3(0.0f, 1.0f, 1.0f));
	colours.push(vec3(1.0f, 0.5f, 0.5f));
	colours.push(vec3(0.5f, 1.0f, 0.5f));
	colours.push(vec3(0.5f, 0.5f, 1.0f));
	/*colours.push(vec3(0.5f, 1.0f, 1.0f));
	colours.push(vec3(1.0f, 0.5f, 1.0f));
	colours.push(vec3(1.0f, 1.0f, 0.5f));
	colours.push(vec3(0.7f, 0.3f, 0.3f));
	colours.push(vec3(0.3f, 0.7f, 0.3f));
	colours.push(vec3(0.3f, 0.3f, 0.7f));
	colours.push(vec3(0.3f, 0.7f, 0.7f));
	colours.push(vec3(0.7f, 0.3f, 0.7f));
	colours.push(vec3(0.7f, 0.7f, 0.3f));
	colours.push(vec3(0.7f, 0.5f, 0.5f));*/
}

//----------------------------------------------------------------------------------------
// Destructor
A3::~A3()
{

}

//----------------------------------------------------------------------------------------
/*
 * Called once, at program start.
 */
void A3::init()
{
	// Set the background colour.
	glClearColor(0.35, 0.35, 0.35, 1.0);

	createShaderProgram();

	glGenVertexArrays(1, &m_vao_arcCircle);
	glGenVertexArrays(1, &m_vao_meshData);
	enableVertexShaderInputSlots();

	processLuaSceneFile(m_luaSceneFile);

	// Load and decode all .obj files at once here.  You may add additional .obj files to
	// this list in order to support rendering additional mesh types.  All vertex
	// positions, and normals will be extracted and stored within the MeshConsolidator
	// class.
	unique_ptr<MeshConsolidator> meshConsolidator (new MeshConsolidator{
			getAssetFilePath("cube.obj"),
			getAssetFilePath("sphere.obj"),
			getAssetFilePath("suzanne.obj")
	});


	// Acquire the BatchInfoMap from the MeshConsolidator.
	meshConsolidator->getBatchInfoMap(m_batchInfoMap);

	// Take all vertex data within the MeshConsolidator and upload it to VBOs on the GPU.
	uploadVertexDataToVbos(*meshConsolidator);

	mapVboDataToVertexShaderInputLocations();

	initPerspectiveMatrix();

	initViewMatrix();

	initLightSources();

	translateNode = &(*m_rootNode);
	rotateNode = m_rootNode->children.front();
	noTranslate = m_rootNode->trans;
	noRotate = m_rootNode->children.front()->trans;

	initSelected(&(*m_rootNode));

	// Exiting the current scope calls delete automatically on meshConsolidator freeing
	// all vertex data resources.  This is fine since we already copied this data to
	// VBOs on the GPU.  We have no use for storing vertex data on the CPU side beyond
	// this point.
}

//----------------------------------------------------------------------------------------
void A3::processLuaSceneFile(const std::string & filename) {
	// This version of the code treats the Lua file as an Asset,
	// so that you'd launch the program with just the filename
	// of a puppet in the Assets/ directory.
	// std::string assetFilePath = getAssetFilePath(filename.c_str());
	// m_rootNode = std::shared_ptr<SceneNode>(import_lua(assetFilePath));

	// This version of the code treats the main program argument
	// as a straightforward pathname.
	m_rootNode = std::shared_ptr<SceneNode>(import_lua(filename));
	if (!m_rootNode) {
		std::cerr << "Could not open " << filename << std::endl;
	}
}

//----------------------------------------------------------------------------------------
void A3::createShaderProgram()
{
	m_shader.generateProgramObject();
	m_shader.attachVertexShader( getAssetFilePath("VertexShader.vs").c_str() );
	m_shader.attachFragmentShader( getAssetFilePath("FragmentShader.fs").c_str() );
	m_shader.link();

	m_shader_arcCircle.generateProgramObject();
	m_shader_arcCircle.attachVertexShader( getAssetFilePath("arc_VertexShader.vs").c_str() );
	m_shader_arcCircle.attachFragmentShader( getAssetFilePath("arc_FragmentShader.fs").c_str() );
	m_shader_arcCircle.link();
}

//----------------------------------------------------------------------------------------
void A3::enableVertexShaderInputSlots()
{
	//-- Enable input slots for m_vao_meshData:
	{
		glBindVertexArray(m_vao_meshData);

		// Enable the vertex shader attribute location for "position" when rendering.
		m_positionAttribLocation = m_shader.getAttribLocation("position");
		glEnableVertexAttribArray(m_positionAttribLocation);

		// Enable the vertex shader attribute location for "normal" when rendering.
		m_normalAttribLocation = m_shader.getAttribLocation("normal");
		glEnableVertexAttribArray(m_normalAttribLocation);

		CHECK_GL_ERRORS;
	}


	//-- Enable input slots for m_vao_arcCircle:
	{
		glBindVertexArray(m_vao_arcCircle);

		// Enable the vertex shader attribute location for "position" when rendering.
		m_arc_positionAttribLocation = m_shader_arcCircle.getAttribLocation("position");
		glEnableVertexAttribArray(m_arc_positionAttribLocation);

		CHECK_GL_ERRORS;
	}

	// Restore defaults
	glBindVertexArray(0);
}

//----------------------------------------------------------------------------------------
void A3::uploadVertexDataToVbos (
		const MeshConsolidator & meshConsolidator
) {
	// Generate VBO to store all vertex position data
	{
		glGenBuffers(1, &m_vbo_vertexPositions);

		glBindBuffer(GL_ARRAY_BUFFER, m_vbo_vertexPositions);

		glBufferData(GL_ARRAY_BUFFER, meshConsolidator.getNumVertexPositionBytes(),
				meshConsolidator.getVertexPositionDataPtr(), GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		CHECK_GL_ERRORS;
	}

	// Generate VBO to store all vertex normal data
	{
		glGenBuffers(1, &m_vbo_vertexNormals);

		glBindBuffer(GL_ARRAY_BUFFER, m_vbo_vertexNormals);

		glBufferData(GL_ARRAY_BUFFER, meshConsolidator.getNumVertexNormalBytes(),
				meshConsolidator.getVertexNormalDataPtr(), GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		CHECK_GL_ERRORS;
	}

	// Generate VBO to store the trackball circle.
	{
		glGenBuffers( 1, &m_vbo_arcCircle );
		glBindBuffer( GL_ARRAY_BUFFER, m_vbo_arcCircle );

		float *pts = new float[ 2 * CIRCLE_PTS ];
		for( size_t idx = 0; idx < CIRCLE_PTS; ++idx ) {
			float ang = 2.0 * M_PI * float(idx) / CIRCLE_PTS;
			pts[2*idx] = cos( ang );
			pts[2*idx+1] = sin( ang );
		}

		glBufferData(GL_ARRAY_BUFFER, 2*CIRCLE_PTS*sizeof(float), pts, GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		CHECK_GL_ERRORS;
	}
}

//----------------------------------------------------------------------------------------
void A3::mapVboDataToVertexShaderInputLocations()
{
	// Bind VAO in order to record the data mapping.
	glBindVertexArray(m_vao_meshData);

	// Tell GL how to map data from the vertex buffer "m_vbo_vertexPositions" into the
	// "position" vertex attribute location for any bound vertex shader program.
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo_vertexPositions);
	glVertexAttribPointer(m_positionAttribLocation, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

	// Tell GL how to map data from the vertex buffer "m_vbo_vertexNormals" into the
	// "normal" vertex attribute location for any bound vertex shader program.
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo_vertexNormals);
	glVertexAttribPointer(m_normalAttribLocation, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

	//-- Unbind target, and restore default values:
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	CHECK_GL_ERRORS;

	// Bind VAO in order to record the data mapping.
	glBindVertexArray(m_vao_arcCircle);

	// Tell GL how to map data from the vertex buffer "m_vbo_arcCircle" into the
	// "position" vertex attribute location for any bound vertex shader program.
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo_arcCircle);
	glVertexAttribPointer(m_arc_positionAttribLocation, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

	//-- Unbind target, and restore default values:
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	CHECK_GL_ERRORS;
}

//----------------------------------------------------------------------------------------
void A3::initPerspectiveMatrix()
{
	float aspect = ((float)m_windowWidth) / m_windowHeight;
	m_perpsective = glm::perspective(degreesToRadians(60.0f), aspect, 0.1f, 100.0f);
}


//----------------------------------------------------------------------------------------
void A3::initViewMatrix() {
	m_view = glm::lookAt(vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 0.0f, -1.0f),
			vec3(0.0f, 1.0f, 0.0f));
}

//----------------------------------------------------------------------------------------
void A3::initLightSources() {
	// World-space position
	m_light.position = vec3(0.0f, 0.0f, 5.0f);
	m_light.rgbIntensity = vec3(0.8f); // White light
}

void A3::initSelected(SceneNode *root) {

	if (root->m_nodeType == NodeType::GeometryNode) {

		//cout << root->m_name << endl;

		selected[root->m_nodeId] = false;

		if (root->m_name == "headScale") {
			headId = root->m_nodeId;
		}
		//idToColour[root->m_nodeId] = colours.top();
		//colourToId[std::make_tuple(colours.top().r, colours.top().g, colours.top().b)] = root->m_nodeId;
		//colours.pop();
	}
	else if (root->m_nodeType == NodeType::JointNode) {
		list<SceneNode*> children = root->children.front()->children;
		//list<SceneNode*> children = root->children.front()->children.front()->children;
        	for (list<SceneNode*>::iterator it = children.begin(); it != children.end(); ++it) {
                	SceneNode *node = *it;
			if (node->m_nodeType == NodeType::GeometryNode) {
				objectToJoint[node->m_nodeId] = root;
				break;
			}
        	}
		if (root->m_name == "headJoint") {
			headSideJoint =  static_cast<JointNode *>(root);
		}
	}

	list<SceneNode*> children = root->children;
        for (list<SceneNode*>::iterator it = children.begin(); it != children.end(); ++it) {
        	initSelected(*it);
        }
}

//----------------------------------------------------------------------------------------
void A3::uploadCommonSceneUniforms() {
	m_shader.enable();
	{
		//-- Set Perpsective matrix uniform for the scene:
		GLint location = m_shader.getUniformLocation("Perspective");
		glUniformMatrix4fv(location, 1, GL_FALSE, value_ptr(m_perpsective));
		CHECK_GL_ERRORS;


		//-- Set LightSource uniform for the scene:
		{
			location = m_shader.getUniformLocation("light.position");
			glUniform3fv(location, 1, value_ptr(m_light.position));
			location = m_shader.getUniformLocation("light.rgbIntensity");
			glUniform3fv(location, 1, value_ptr(m_light.rgbIntensity));
			CHECK_GL_ERRORS;
		}

		//-- Set background light ambient intensity
		{
			location = m_shader.getUniformLocation("ambientIntensity");
			vec3 ambientIntensity(0.05f);
			glUniform3fv(location, 1, value_ptr(ambientIntensity));
			CHECK_GL_ERRORS;
		}
	}
	m_shader.disable();
}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, before guiLogic().
 */
void A3::appLogic()
{
	// Place per frame, application logic here ...

	uploadCommonSceneUniforms();
}

void A3::undo() {
	JointTransform j;
	bool valid = jointStack.undo(j);
	if (!valid) {
		cerr << "Undo not valid" << endl;
		return;
	}
	else {
		while (!j.node.empty()) {
			JointNode *joint = j.node.top();
			double xDiff = j.xAngle.top() * -1;
			double yDiff = j.yAngle.top() * -1;
			joint->rotateJoint('y', yDiff);
			joint->rotateJoint('x', xDiff);
			j.node.pop();
			j.xAngle.pop();
			j.yAngle.pop();
		}
	}
}

void A3::redo() {
        JointTransform j;
        bool valid = jointStack.redo(j);
        if (!valid) {
                cerr << "Redo not valid" << endl;
                return;
        }
        else {
		while (!j.node.empty()) {
                        JointNode *joint = j.node.top();
                        double xDiff = j.xAngle.top();
                        double yDiff = j.yAngle.top();
                        joint->rotateJoint('y', yDiff);
                        joint->rotateJoint('x', xDiff);
			j.node.pop();
                        j.xAngle.pop();
                        j.yAngle.pop();
                }
        }
}
	

void A3::resetPosition() {
	translateNode->set_transform(noTranslate);
}

void A3::resetOrientation() {
	rotateNode->set_transform(noRotate);
}

void A3::makeJointsInit(SceneNode *node) {
	if (node->m_nodeType == NodeType::JointNode) {
		JointNode * joint = static_cast<JointNode *>(node);
		joint->reset();
	}
	list<SceneNode*> children = node->children;
	for (list<SceneNode*>::iterator it = children.begin(); it != children.end(); ++it) {
		makeJointsInit(*it);
	}
}

void A3::resetJoints(SceneNode *node) {
	makeJointsInit(node);
	jointStack = JointStack();
}

void A3::resetAll() {
	resetPosition();
	resetOrientation();
	resetJoints(&(*m_rootNode));
}
//----------------------------------------------------------------------------------------
/*
 * Called once per frame, after appLogic(), but before the draw() method.
 */
void A3::guiLogic()
{
	if( !show_gui ) {
		return;
	}

	static bool firstRun(true);
	if (firstRun) {
		ImGui::SetNextWindowPos(ImVec2(50, 50));
		firstRun = false;
	}

	static bool showDebugWindow(true);
	ImGuiWindowFlags windowFlags(ImGuiWindowFlags_AlwaysAutoResize);
	float opacity(0.5f);

	ImGui::Begin("Properties", &showDebugWindow, ImVec2(100,100), opacity,
			windowFlags);


		// Add more gui
		if (ImGui::BeginMainMenuBar()) {
			if (ImGui::BeginMenu("Application")) {
				if (ImGui::MenuItem("Reset Position", "I")) {
					resetPosition();
				}
				if (ImGui::MenuItem("Reset Orientation", "O")) {
					resetOrientation();
                                }
				if (ImGui::MenuItem("Reset Joints", "N")) {
					resetJoints(&(*m_rootNode));
                                }
				if (ImGui::MenuItem("Reset All", "A")) {
					resetAll();
                                }
				if (ImGui::MenuItem("Quit", "Q")) {
					glfwSetWindowShouldClose(m_window, GL_TRUE);
                                }
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Edit")) {
				if (ImGui::MenuItem("Undo", "U")) {
					undo();
				}
				if (ImGui::MenuItem("Redo", "R")) {
					redo();
				}
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Options")) {
				ImGui::MenuItem("Circle", "C", &drawCircle);
				ImGui::MenuItem("Z-buffer", "Z", &hasZBuffer);
				ImGui::MenuItem("Backface culling", "B", &hasBackCull);
				ImGui::MenuItem("Frontface culling", "F", &hasFrontCull);
				ImGui::EndMenu();
			}
			ImGui::EndMainMenuBar();
		}

		ImGui::RadioButton( "Position/Orientation", &pickingMode, 0);
		ImGui::RadioButton( "Joints", &pickingMode, 1);

		// Create Button, and check if it was clicked:
		if( ImGui::Button( "Quit Application" ) ) {
			glfwSetWindowShouldClose(m_window, GL_TRUE);
                }

		ImGui::Text( "Framerate: %.1f FPS", ImGui::GetIO().Framerate );

	ImGui::End();
}

//----------------------------------------------------------------------------------------
// Update mesh specific shader uniforms:
static void updateShaderUniforms(
		const ShaderProgram & shader,
		const GeometryNode & node,
		const glm::mat4 & viewMatrix,
		const glm::mat4 & modelMatrix,
		bool pickingMode,
		bool highlight,
		vec3 pickingColour
) {

	shader.enable();
	{
		//-- Set ModelView matrix:
		GLint location = shader.getUniformLocation("ModelView");
		mat4 modelView = viewMatrix * modelMatrix * node.trans;
		glUniformMatrix4fv(location, 1, GL_FALSE, value_ptr(modelView));
		CHECK_GL_ERRORS;

		//-- Set NormMatrix:
		location = shader.getUniformLocation("NormalMatrix");
		mat3 normalMatrix = glm::transpose(glm::inverse(mat3(modelView)));
		glUniformMatrix3fv(location, 1, GL_FALSE, value_ptr(normalMatrix));
		CHECK_GL_ERRORS;


		//-- Set Material values:
		location = shader.getUniformLocation("material.kd");
		vec3 kd = node.material.kd;
		glUniform3fv(location, 1, value_ptr(kd));
		CHECK_GL_ERRORS;
		location = shader.getUniformLocation("material.ks");
		vec3 ks = node.material.ks;
		glUniform3fv(location, 1, value_ptr(ks));
		CHECK_GL_ERRORS;
		location = shader.getUniformLocation("material.shininess");
		glUniform1f(location, node.material.shininess);
		CHECK_GL_ERRORS;
		
		//-- Set picking values
		int drawPickingMode = shader.getUniformLocation("pickingMode");
		glUniform1i(drawPickingMode, pickingMode ? 1 : 0);
		CHECK_GL_ERRORS;
		int colour = shader.getUniformLocation("colour");
		if (!highlight) {
			//glUniform3f(colour, pickingColour.r, pickingColour.g, pickingColour.b);
			unsigned int idx = node.m_nodeId;
			float r = float(idx&0xff) / 255.0f;
                	float g = float((idx>>8)&0xff) / 255.0f;
                	float b = float((idx>>16)&0xff) / 255.0f;
			glUniform3f(colour, r, g, b);
		}
		else {
			glUniform3f(colour, 1.0, 1.0, 0.0);
		}
	}
	shader.disable();

}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, after guiLogic().
 */
void A3::draw() {

	if (hasZBuffer) {
		glEnable( GL_DEPTH_TEST );
	}
	if (hasBackCull) {
		glEnable( GL_CULL_FACE );
		glCullFace( GL_BACK );
	}
	if (hasFrontCull) {
		glEnable( GL_CULL_FACE );
		glCullFace( GL_FRONT );
	}

	renderSceneGraph(*m_rootNode);


	glDisable( GL_DEPTH_TEST );
	glDisable(GL_CULL_FACE);

	if (drawCircle) {
		renderArcCircle();
	}
}

//----------------------------------------------------------------------------------------
void A3::renderSceneGraph(const SceneNode & root) {

	// Bind the VAO once here, and reuse for all GeometryNode rendering below.
	glBindVertexArray(m_vao_meshData);

	// This is emphatically *not* how you should be drawing the scene graph in
	// your final implementation.  This is a non-hierarchical demonstration
	// in which we assume that there is a list of GeometryNodes living directly
	// underneath the root node, and that we can draw them in a loop.  It's
	// just enough to demonstrate how to get geometry and materials out of
	// a GeometryNode and onto the screen.

	// You'll want to turn this into recursive code that walks over the tree.
	// You can do that by putting a method in SceneNode, overridden in its
	// subclasses, that renders the subtree rooted at every node.  Or you
	// could put a set of mutually recursive functions in this class, which
	// walk down the tree from nodes of different types.

	renderGraph(root, mat4());

	glBindVertexArray(0);
	CHECK_GL_ERRORS;
}

void A3::renderGraph(const SceneNode &root, glm::mat4 modelMatrix) {
	list<SceneNode*> children = root.children;

	renderNode(root, modelMatrix);

	if (root.m_nodeType == NodeType::SceneNode) {
		modelMatrix = modelMatrix * root.trans;
	}
	else if (root.m_nodeType == NodeType::GeometryNode) {
                modelMatrix = modelMatrix * root.trans;
        }
	else if (root.m_nodeType == NodeType::JointNode) {
		modelMatrix = modelMatrix * root.trans;
	}

        if (!children.empty()) {
                for (list<SceneNode*>::iterator it = children.begin(); it != children.end(); ++it) {
			renderGraph(**it, modelMatrix);
                }
        }
}

void A3::renderNode(const SceneNode &node, glm::mat4 modelMatrix) {
	if (node.m_nodeType == NodeType::GeometryNode) {
		
		//cout << node.m_name << endl;

        	const GeometryNode * geometryNode = static_cast<const GeometryNode *>(&node);

        	updateShaderUniforms(m_shader, *geometryNode, m_view, modelMatrix, (pickingMode == 1), (!do_picking && selected[geometryNode->m_nodeId]), idToColour[geometryNode->m_nodeId]);


        	//Get the BatchInfo corresponding to the GeometryNode's unique MeshId.
        	BatchInfo batchInfo = m_batchInfoMap[geometryNode->meshId];

        	//-- Now render the mesh:
        	m_shader.enable();
        	glDrawArrays(GL_TRIANGLES, batchInfo.startIndex, batchInfo.numIndices);
        	m_shader.disable();
	}	
}

//----------------------------------------------------------------------------------------
// Draw the trackball circle.
void A3::renderArcCircle() {
	glBindVertexArray(m_vao_arcCircle);

	m_shader_arcCircle.enable();
		GLint m_location = m_shader_arcCircle.getUniformLocation( "M" );
		float aspect = float(m_framebufferWidth)/float(m_framebufferHeight);
		glm::mat4 M;
		if( aspect > 1.0 ) {
			M = glm::scale( glm::mat4(), glm::vec3( 0.5/aspect, 0.5, 1.0 ) );
		} else {
			M = glm::scale( glm::mat4(), glm::vec3( 0.5, 0.5*aspect, 1.0 ) );
		}
		glUniformMatrix4fv( m_location, 1, GL_FALSE, value_ptr( M ) );
		glDrawArrays( GL_LINE_LOOP, 0, CIRCLE_PTS );
	m_shader_arcCircle.disable();

	glBindVertexArray(0);
	CHECK_GL_ERRORS;
}

//----------------------------------------------------------------------------------------
/*
 * Called once, after program is signaled to terminate.
 */
void A3::cleanup()
{

}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles cursor entering the window area events.
 */
bool A3::cursorEnterWindowEvent (
		int entered
) {
	bool eventHandled(false);

	// Fill in with event handling code...

	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles mouse cursor movement events.
 */
bool A3::mouseMoveEvent (
		double xPos,
		double yPos
) {
	bool eventHandled(false);

	double xDiff = xPos - oldX;
	double yDiff = yPos - oldY;
	oldX = xPos;
	oldY = yPos;
	
	if (!ImGui::IsMouseHoveringAnyWindow()) {
		if (pickingMode == 1) {
			stack<JointNode*> nodes;
                        stack<double> xAngles;
                        stack<double> yAngles;
			if (middleMousePressed) {
				for (map<unsigned int, bool>::iterator it = selected.begin(); it != selected.end(); ++it) {
					if (it->second) {
						if (objectToJoint.find(it->first) != objectToJoint.end()) {
							JointNode * joint = static_cast<JointNode *>(objectToJoint.at(it->first));
							bool rotY = joint->rotateJoint('y', yDiff);
							bool rotX = joint->rotateJoint('x', yDiff);
							//jointStack.addToStack(JointTransform(joint, xDiff, yDiff)); 
							if (rotY || rotX) {
								nodes.push(joint);
								xAngles.push(rotX ? yDiff : 0);
								yAngles.push(rotY ? yDiff : 0);
							}
						}
					}
				}
			}
			if (rightMousePressed) {
				if (selected[headId]) {
					bool rotX = headSideJoint->rotateJoint('x', yDiff);
					bool rotY = headSideJoint->rotateJoint('y', yDiff);
					if(rotY || rotX) {
						nodes.push(headSideJoint);
						xAngles.push(rotX ? yDiff : 0);
						yAngles.push(rotY ? yDiff : 0);
					}		
				}
			}
			if (!nodes.empty()) {
				jointStack.addToStack(JointTransform(nodes, xAngles, yAngles));
			}
                        eventHandled = true;
		}
		else {
			if (leftMousePressed) {
				translateNode->translate(vec3(xDiff/500, (-1 * yDiff)/500, 0));
				eventHandled = true;
			}
			else if (middleMousePressed) {
				translateNode->translate(vec3(0, 0, yDiff/500));
				eventHandled = true;
			}			
		}
	}

	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles mouse button events.
 */
bool A3::mouseButtonInputEvent (
		int button,
		int actions,
		int mods
) {
	bool eventHandled(false);

	if (button == GLFW_MOUSE_BUTTON_LEFT && actions == GLFW_PRESS) {
		leftMousePressed = true;
		if (pickingMode == 1) {
                	double xpos, ypos;
                	glfwGetCursorPos( m_window, &xpos, &ypos );

                	do_picking = true;

                	uploadCommonSceneUniforms();
                	glClearColor(1.0, 1.0, 1.0, 1.0 );
                	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
                	glClearColor(0.35, 0.35, 0.35, 1.0);

                	draw();

			CHECK_GL_ERRORS;

			// Ugly -- FB coordinates might be different than Window coordinates
                	// (e.g., on a retina display).  Must compensate.
                	xpos *= double(m_framebufferWidth) / double(m_windowWidth);
                	// WTF, don't know why I have to measure y relative to the bottom of
                	// the window in this case.
                	ypos = m_windowHeight - ypos;
                	ypos *= double(m_framebufferHeight) / double(m_windowHeight);

                	GLubyte buffer[ 4 ] = { 0, 0, 0, 0 };
                	// A bit ugly -- don't want to swap the just-drawn false colours
                	// to the screen, so read from the back buffer.
                	glReadBuffer( GL_BACK );
                	// Actually read the pixel at the mouse location.
                	glReadPixels( int(xpos), int(ypos), 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, buffer );
                	CHECK_GL_ERRORS;

                	// Reassemble the object ID.
			//std::tuple<float, float, float> pickingColour = std::make_tuple(((float)buffer[0])/255.0f, ((float)buffer[1])/255.0f, ((float)buffer[2])/255.0f);
			//if (colourToId.find(pickingColour) != colourToId.end()){
				//unsigned int what = colourToId[pickingColour];
                        	//selected[what] = !selected[what];
                	//}
			unsigned int what = buffer[0] + (buffer[1] << 8) + (buffer[2] << 16);
			if (selected.find(what) != selected.end()) {
				selected[what] = !selected[what];
			}



                	do_picking = false;

                	CHECK_GL_ERRORS;
		}
		eventHandled = true;
	}
	if (button == GLFW_MOUSE_BUTTON_LEFT && actions == GLFW_RELEASE) {
		leftMousePressed = false;
		eventHandled = true;
	}
	if (button == GLFW_MOUSE_BUTTON_RIGHT && actions == GLFW_PRESS) {
		rightMousePressed = true;
		eventHandled = true;
	}
	if (button == GLFW_MOUSE_BUTTON_MIDDLE && actions == GLFW_PRESS) {
		middleMousePressed = true;
		eventHandled = true;
	}
	if (button == GLFW_MOUSE_BUTTON_RIGHT && actions == GLFW_RELEASE) {
        	rightMousePressed = false;
                eventHandled = true;
        }
        if (button == GLFW_MOUSE_BUTTON_MIDDLE && actions == GLFW_RELEASE) {
        	middleMousePressed = false;
                eventHandled = true;
         }
		

	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles mouse scroll wheel events.
 */
bool A3::mouseScrollEvent (
		double xOffSet,
		double yOffSet
) {
	bool eventHandled(false);

	// Fill in with event handling code...

	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles window resize events.
 */
bool A3::windowResizeEvent (
		int width,
		int height
) {
	bool eventHandled(false);
	initPerspectiveMatrix();
	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles key input events.
 */
bool A3::keyInputEvent (
		int key,
		int action,
		int mods
) {
	bool eventHandled(false);

	if( action == GLFW_PRESS ) {
		if( key == GLFW_KEY_M ) {
			show_gui = !show_gui;
			eventHandled = true;
		}
		if (key == GLFW_KEY_I) {
			resetPosition();
			eventHandled = true;
		}
		if (key == GLFW_KEY_O) {
			resetOrientation();
			eventHandled = true;
		}
		if (key == GLFW_KEY_N) {
			resetJoints(&(*m_rootNode));
                        eventHandled = true;
                }
		if (key == GLFW_KEY_A) {
			resetAll();
                        eventHandled = true;
                }
		if (key == GLFW_KEY_Q) {
			glfwSetWindowShouldClose(m_window, GL_TRUE);
                        eventHandled = true;
                }
		if (key == GLFW_KEY_U) {
			undo();
                        eventHandled = true;
                }
		if (key == GLFW_KEY_R) {
			redo();
                        eventHandled = true;
                }
		if (key == GLFW_KEY_C) {
			drawCircle = !drawCircle;
                        eventHandled = true;
                }
		if (key == GLFW_KEY_Z) {
			hasZBuffer = !hasZBuffer;
                        eventHandled = true;
                }
		if (key == GLFW_KEY_B) {
			hasBackCull = !hasBackCull;
                        eventHandled = true;
                }
		if (key == GLFW_KEY_F) {
			hasFrontCull = !hasFrontCull;
                        eventHandled = true;
                }
		if (key == GLFW_KEY_P) {
			pickingMode = 0;
                        eventHandled = true;
                }
		if (key == GLFW_KEY_J) {
			pickingMode = 1;
                        eventHandled = true;
                }
	}
	// Fill in with event handling code...

	return eventHandled;
}
