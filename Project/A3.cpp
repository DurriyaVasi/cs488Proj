#include "A3.hpp"
#include "scene_lua.hpp"
using namespace std;

#include "cs488-framework/GlErrorCheck.hpp"
#include "cs488-framework/MathUtils.hpp"
#include "GeometryNode.hpp"
#include "JointNode.hpp"
#include "Scene.hpp"

#include "stb_image.h"

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
	  m_textureCoordAttribLocation(0),
	  m_tangentAttribLocation(0),
	  m_skybox_texture(0),
	  m_vao_meshData(0),
	  m_vbo_vertexPositions(0),
	  m_vbo_vertexNormals(0),
	  m_vbo_vertexTextureCoords(0),
	  m_vbo_vertexTangents(0),
	  m_vao_arcCircle(0),
	  m_vbo_arcCircle(0),
	  m_vao_skybox(0),
	  m_vbo_skybox(0),
	  m_skybox_positionAttribLocation(0),
	  m_skyboxData(SkyboxData()),
	  m_vao_game(0),
	  m_game_positionAttribLocation(0),
	  do_picking(false),
	  leftMousePressed(false),
	  rightMousePressed(false),
	  middleMousePressed(false),
	  leftKeyPressed(false),
	  rightKeyPressed(false),
	  oldX(0),
	  oldY(0),
	  m_mode(BEFORE_GAME),
	  m_board(Board(-2, -2, 2, 2))
{
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
	glGenVertexArrays(1, &m_vao_skybox);
	glGenVertexArrays(1, &m_vao_game);
	enableVertexShaderInputSlots();

        processLuaSceneFile(m_luaSceneFile);

	glGenTextures(1, &m_skybox_texture);

	setupBackgroundTexture();

	// Load and decode all .obj files at once here.  You may add additional .obj files to
	// this list in order to support rendering additional mesh types.  All vertex
	// positions, and normals will be extracted and stored within the MeshConsolidator
	// class.
	unique_ptr<MeshConsolidator> meshConsolidator (new MeshConsolidator{
			getAssetFilePath("cube.obj"),
			getAssetFilePath("sphere.obj"),
			getAssetFilePath("suzanne.obj"),
			getAssetFilePath("triangle.obj"),
			getAssetFilePath("square.obj")
	});

	// Acquire the BatchInfoMap from the MeshConsolidator.
	meshConsolidator->getBatchInfoMap(m_batchInfoMap);

	// Take all vertex data within the MeshConsolidator and upload it to VBOs on the GPU.
	uploadVertexDataToVbos(*meshConsolidator);

	mapVboDataToVertexShaderInputLocations();

	initLightSources();

//	initSelected(&(*m_rootNode));

	createPerspectiveMatrix();

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

        Scene scene = import_lua(filename);

	if (!scene.spaceship.m_node) {
		std::cerr << "Could not open spaceship " << filename << std::endl;
	}
	if (!scene.ballNode) {
		std::cerr << "Could not open ballnode " << filename << std::endl;
	}
	if (!scene.paddleNode) {	
		std::cerr << "Could not open paddleNode " << filename << std::endl;
	}
	if (!scene.startButton) {
		std::cerr << "Could not open startbutton node " << filename << std::endl;
	}
	if (!scene.playAgainButton) {
                std::cerr << "Could not open playAgainbutton node " << filename << std::endl;
        }
	if (!scene.gameOverText) {
                std::cerr << "Could not open gameoverText node " << filename << std::endl;
        }
	if (!scene.box) {
                std::cerr << "Could not open box node " << filename << std::endl;
        }

	for (int i = 0; i < 3; i++) {
		m_images[i] = scene.images[i];
	}

	m_spaceship = scene.spaceship;
	m_ball = Ball(scene.ballNode);
	m_paddle = Paddle(scene.paddleNode);
	m_startButton = scene.startButton;
	m_playAgainButton = scene.playAgainButton;
	m_gameOverText = scene.gameOverText;
	m_box = scene.box;
	createTextures(scene.textureFiles, scene.textureNormalFiles);
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

	m_shader_skybox.generateProgramObject();
	CHECK_GL_ERRORS;
	m_shader_skybox.attachVertexShader( getAssetFilePath("skybox_VertexShader.vs").c_str() );
	CHECK_GL_ERRORS;
	m_shader_skybox.attachFragmentShader( getAssetFilePath("skybox_FragmentShader.fs").c_str() );
	CHECK_GL_ERRORS;
	m_shader_skybox.link();
	CHECK_GL_ERRORS;

	m_shader_game.generateProgramObject();
	CHECK_GL_ERRORS;
	m_shader_game.attachVertexShader( getAssetFilePath("game_VertexShader.vs").c_str() );
	CHECK_GL_ERRORS;
	m_shader_game.attachFragmentShader( getAssetFilePath("game_FragmentShader.fs" ).c_str());
	CHECK_GL_ERRORS;
	m_shader_game.link();
	CHECK_GL_ERRORS;
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

		// Enable the vertex shader attribute location for "textureCoord" when rendering.
                m_textureCoordAttribLocation = m_shader.getAttribLocation("textureCoord");
                glEnableVertexAttribArray(m_textureCoordAttribLocation);

		// Enable the vertex shader attribute location for "tangent" when rendering.
		m_tangentAttribLocation = m_shader.getAttribLocation("tangent");
                glEnableVertexAttribArray(m_tangentAttribLocation);

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

	{
		glBindVertexArray(m_vao_skybox);
	
		m_skybox_positionAttribLocation = m_shader_skybox.getAttribLocation("position");
		glEnableVertexAttribArray(m_skybox_positionAttribLocation);

		CHECK_GL_ERRORS;
	}

	{
		glBindVertexArray(m_vao_game);
		m_game_positionAttribLocation = m_shader_game.getAttribLocation("position");
		glEnableVertexAttribArray(m_game_positionAttribLocation);
		CHECK_GL_ERRORS;
	}
	// Restore defaults
	glBindVertexArray(0);
}

void A3::createTextures(std::set<string> textureFiles, std::set<string> textureNormalFiles) {
	for(std::set<string>::iterator it = textureFiles.begin(); it != textureFiles.end(); ++it) {
		m_textureHandler.addTexture(*it);
	}
	for (std::set<string>::iterator it = textureNormalFiles.begin(); it != textureNormalFiles.end(); ++it) {
		m_textureHandler.addTextureNormal(*it);
	}
} 

void A3::setupBackgroundTexture() {
	if (m_images[m_mode].background.hasSkybox) {
		{
                	glBindTexture(GL_TEXTURE_CUBE_MAP, m_skybox_texture);
                	CHECK_GL_ERRORS;

                	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                	CHECK_GL_ERRORS;
                	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                	CHECK_GL_ERRORS;
                	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                	CHECK_GL_ERRORS;
                	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
                	CHECK_GL_ERRORS;
                	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
                	CHECK_GL_ERRORS;
        	}
		{
			glBindTexture(GL_TEXTURE_CUBE_MAP, m_skybox_texture);
                        CHECK_GL_ERRORS;
	
			int width, height, nrChannels;
			for (unsigned int i = 0; i < 6; i++) {
				unsigned char *data = stbi_load(m_images[m_mode].background.faces[i].c_str(), &width, &height, &nrChannels,0);
				if (data) {
					 glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 
                     				      0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
					 CHECK_GL_ERRORS;
					 stbi_image_free(data);
				}
				else {
					std::cerr << "Cubemap texture failed to load at path: " << m_images[m_mode].background.faces[i] << std::endl;
					stbi_image_free(data);
				}
			}

		}
	}
	//TODO: add background colour
	//glClearColor(m_images[m_mode].background.colour);
	CHECK_GL_ERRORS;
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
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

	// Generate VBO to store all vertex texture data
	{
		glGenBuffers(1, &m_vbo_vertexTextureCoords);

		glBindBuffer(GL_ARRAY_BUFFER, m_vbo_vertexTextureCoords);

		glBufferData(GL_ARRAY_BUFFER, meshConsolidator.getNumVertexTextureBytes(),
				meshConsolidator.getVertexTextureDataPtr(), GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		CHECK_GL_ERRORS;
	}

	{
		glGenBuffers(1, &m_vbo_vertexTangents);
	
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo_vertexTangents);

		glBufferData(GL_ARRAY_BUFFER, meshConsolidator.getNumVertexTangentBytes(),
				meshConsolidator.getVertexTangentDataPtr(), GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		CHECK_GL_ERRORS;
	}

	// Generate VBO to store the skybox data
	{
		glGenBuffers( 1, &m_vbo_skybox);
		glBindBuffer( GL_ARRAY_BUFFER, m_vbo_skybox );
		glBufferData(GL_ARRAY_BUFFER, sizeof(m_skyboxData.vertices), m_skyboxData.vertices, GL_STATIC_DRAW);	
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

	// Tell GL how to map data from the vertex buffer "m_vbo_vertexTextureCoords" into the
        // "textureCoord" vertex attribute location for any bound vertex shader program.
        glBindBuffer(GL_ARRAY_BUFFER, m_vbo_vertexTextureCoords);
        glVertexAttribPointer(m_textureCoordAttribLocation, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

	// Tell GL how to map data from the vertex buffer "m_vbo_vertexTangents" into the
        // "tangent" vertex attribute location for any bound vertex shader program.
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo_vertexTangents);
	glVertexAttribPointer(m_tangentAttribLocation, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

	//-- Unbind target, and restore default values:
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	CHECK_GL_ERRORS;
	
	// Bind VAO in order to record the data mapping.
	glBindVertexArray(m_vao_skybox);
	
	CHECK_GL_ERRORS;	
	// Tell GL how to map data from the vertex buffer "m_vbo_skybox" into the
        // "position" vertex attribute location for any bound vertex shader program.
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo_skybox);
	CHECK_GL_ERRORS;
	glVertexAttribPointer(m_skybox_positionAttribLocation, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
	CHECK_GL_ERRORS;

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

	CHECK_GL_ERRORS;

	glBindVertexArray(m_vao_game);
        CHECK_GL_ERRORS;
        glBindBuffer(GL_ARRAY_BUFFER, m_vbo_vertexPositions);
        CHECK_GL_ERRORS;
        glVertexAttribPointer(m_game_positionAttribLocation, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
	CHECK_GL_ERRORS;


	//-- Unbind target, and restore default values:
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	CHECK_GL_ERRORS;

}

//----------------------------------------------------------------------------------------
void A3::createPerspectiveMatrix()
{
	float aspect = ((float)m_windowWidth) / m_windowHeight;
	float perspectiveDegrees = m_images[m_mode].perspectiveDegrees;
	m_perspective = glm::perspective(degreesToRadians(perspectiveDegrees), aspect, 0.1f, 100.0f);
}


//----------------------------------------------------------------------------------------
void A3::initLightSources() {
	// World-space position
	m_light.position = vec3(2.0f, 2.0f, 5.0f);
	m_light.rgbIntensity = vec3(0.8f); // White light
}

void A3::initSelected(SceneNode *root) {

/*	if (root->m_nodeType == NodeType::GeometryNode) {

		selected[root->m_nodeId] = false;
	}
	else if (root->m_nodeType == NodeType::JointNode) {
		list<SceneNode*> children = root->children.front()->children;
        	for (list<SceneNode*>::iterator it = children.begin(); it != children.end(); ++it) {
                	SceneNode *node = *it;
			if (node->m_nodeType == NodeType::GeometryNode) {
				objectToJoint[node->m_nodeId] = root;
				break;
			}
        	}
	}

	list<SceneNode*> children = root->children;
        for (list<SceneNode*>::iterator it = children.begin(); it != children.end(); ++it) {
        	initSelected(*it);
        } */
}

void A3::uploadCommonImageUniforms() {
	glm::mat4 perspective = m_perspective;

	m_shader.enable();
	{
		GLint location = m_shader.getUniformLocation("Perspective");
                glUniformMatrix4fv(location, 1, GL_FALSE, value_ptr(perspective));
                CHECK_GL_ERRORS;
	}
	m_shader.disable();
	
	m_shader_skybox.enable();
        {
                //-- Set Perpsective matrix uniform for the scene:
                GLint location = m_shader_skybox.getUniformLocation("Perspective");
                glUniformMatrix4fv(location, 1, GL_FALSE, value_ptr(perspective));
                CHECK_GL_ERRORS;
        }
        m_shader_skybox.disable();

	m_shader_game.enable();
	{
		GLint location= m_shader_game.getUniformLocation("Perspective");
		glUniformMatrix4fv(location, 1, GL_FALSE, value_ptr(perspective));
		CHECK_GL_ERRORS;
	}
	m_shader_game.disable();
}

//----------------------------------------------------------------------------------------
void A3::uploadCommonSceneUniforms() {
	m_shader.enable();
	{
		//-- Set LightSource uniform for the scene:
		{
			GLint location = m_shader.getUniformLocation("light.position");
			glUniform3fv(location, 1, value_ptr(m_light.position));
			location = m_shader.getUniformLocation("light.rgbIntensity");
			glUniform3fv(location, 1, value_ptr(m_light.rgbIntensity));
			CHECK_GL_ERRORS;
		}

		//-- Set background light ambient intensity
		{
			GLint location = m_shader.getUniformLocation("ambientIntensity");
			vec3 ambientIntensity(0.005f);
			glUniform3fv(location, 1, value_ptr(ambientIntensity));
			CHECK_GL_ERRORS;
		}

		{
			GLint location = m_shader.getUniformLocation("textureData");
			glUniform1i(location, 0);
			location = m_shader.getUniformLocation("textureNormals");	
			glUniform1i(location, 1);
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
	uploadCommonImageUniforms();
	if (m_mode == BEFORE_GAME) {
		uploadCommonSceneUniforms();
	}
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

		ImGui::Text( "Framerate: %.1f FPS", ImGui::GetIO().Framerate );

	ImGui::End();
}

static void updateGameShaderUniforms(
	const ShaderProgram & shader,
	const GeometryNode & node,
        const glm::mat4 & viewMatrix) {
	
	shader.enable();
	{
		//-- Set ModelView matrix:
                GLint location = shader.getUniformLocation("ModelView");
                mat4 modelView = viewMatrix * node.trans;
                glUniformMatrix4fv(location, 1, GL_FALSE, value_ptr(modelView));
                CHECK_GL_ERRORS;

                location = shader.getUniformLocation("colour");
		glUniform3fv(location, 1, value_ptr(node.material.kd));
		CHECK_GL_ERRORS;
	}
	shader.disable();
}

//----------------------------------------------------------------------------------------
// Update mesh specific shader uniforms:
static void updateShaderUniforms(
		const ShaderProgram & shader,
		const GeometryNode & node,
		const glm::mat4 & viewMatrix,
		const glm::mat4 & modelMatrix,
		bool pickingMode
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
		location = shader.getUniformLocation("material.alpha");
		glUniform1f(location, node.material.alpha);
		CHECK_GL_ERRORS;	

		//-- Set Texture values:
		location = shader.getUniformLocation("hasTexture");
		glUniform1i(location, node.texture.hasTexture ? 1 : 0);
		CHECK_GL_ERRORS;

		//--Set Texture normal values:
		location = shader.getUniformLocation("hasBumps");
		glUniform1i(location, node.texture.hasBumps ? 1 : 0);
	
		//-- Set picking values
		int drawPickingMode = shader.getUniformLocation("pickingMode");
		glUniform1i(drawPickingMode, pickingMode ? 1 : 0);
		CHECK_GL_ERRORS;
		int colour = shader.getUniformLocation("colour");
		unsigned int idx = node.m_nodeId;
		float r = float(idx&0xff) / 255.0f;
                float g = float((idx>>8)&0xff) / 255.0f;
                float b = float((idx>>16)&0xff) / 255.0f;
		glUniform3f(colour, r, g, b);
		CHECK_GL_ERRORS;
	}
	shader.disable();

}


//----------------------------------------------------------------------------------------
/*
 * Called once per frame, after guiLogic().
 */
void A3::draw() {

	if (m_mode == BEFORE_GAME) {
		
		renderBeforeGame();
	}

	else if (m_mode == DURING_GAME) {
		renderGame();
	}

	else if(m_mode == AFTER_GAME) {
		renderAfterGame();
	}
}

void A3::renderBeforeGame() {
		glEnable( GL_CULL_FACE );
                glCullFace( GL_BACK );
                glEnable( GL_DEPTH_TEST );
                glEnable(GL_BLEND);
                glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		renderSceneGraph(*m_box);
		renderSceneGraph(*m_startButton);

		m_spaceship.move();
                renderSceneGraph(*(m_spaceship.m_node));

                glDepthFunc(GL_LEQUAL);
                renderSkybox();
                glDepthFunc(GL_LESS);

		glDisable( GL_CULL_FACE );
                glDisable( GL_DEPTH_TEST );
		glDisable(GL_BLEND);
}

void A3::renderAfterGame() {
		glEnable( GL_CULL_FACE );
                glCullFace( GL_BACK );
                glEnable( GL_DEPTH_TEST );
                glEnable(GL_BLEND);
                glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		renderSceneGraph(*m_gameOverText);
                renderSceneGraph(*m_playAgainButton);

                glDepthFunc(GL_LEQUAL);
                renderSkybox();
                glDepthFunc(GL_LESS);

                glDisable( GL_CULL_FACE );
                glDisable( GL_DEPTH_TEST );
                glDisable(GL_BLEND);
}

void A3::renderGame() {
	glBindVertexArray(m_vao_game);
	drawPaddle();
	CollisionType collision = drawBall();
	glBindVertexArray(0);
	CHECK_GL_ERRORS;
	if (collision == CollisionType::BALL_FLOOR) {
		switchMode(AFTER_GAME);
	}
}

void A3::drawPaddle() {
	if (leftKeyPressed) {
		m_paddle.move(0.1, m_board);
	}
	if (rightKeyPressed) {
		m_paddle.move(-0.1, m_board);
	}
	renderGameNode(*(m_paddle.m_node));
		
}

CollisionType A3::drawBall() {
	CollisionType collision = m_ball.move(m_paddle, m_board);
	renderGameNode(*(m_ball.m_node));
	return collision;
	//return CollisionType::NONE;	
}

//----------------------------------------------------------------------------------------
void A3::renderSceneGraph(const SceneNode & root) {

	// Bind the VAO once here, and reuse for all GeometryNode rendering below.
	glBindVertexArray(m_vao_meshData);

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

void A3::renderGameNode(const SceneNode &node) {
	if (node.m_nodeType == NodeType::GeometryNode) {
		const GeometryNode * geometryNode = static_cast<const GeometryNode *>(&node);
		updateGameShaderUniforms(m_shader_game, *geometryNode,  m_images[m_mode].camera.getViewMatrix());
		BatchInfo batchInfo = m_batchInfoMap[geometryNode->meshId];
		m_shader_game.enable();
		glDrawArrays(GL_TRIANGLES, batchInfo.startIndex, batchInfo.numIndices);
                CHECK_GL_ERRORS;
		m_shader_game.disable();
	}
	else {
		std::cerr << "render game node that is not geometry node" << std::endl;
	}
}

void A3::renderNode(const SceneNode &node, glm::mat4 modelMatrix) { 

	if (node.m_nodeType == NodeType::GeometryNode) {
	
        	const GeometryNode * geometryNode = static_cast<const GeometryNode *>(&node);

        	updateShaderUniforms(m_shader, *geometryNode, m_images[m_mode].camera.getViewMatrix(), modelMatrix, do_picking);

        	//Get the BatchInfo corresponding to the GeometryNode's unique MeshId.
        	BatchInfo batchInfo = m_batchInfoMap[geometryNode->meshId];

        	//-- Now render the mesh:
        	m_shader.enable();

		if (geometryNode->texture.hasTexture) { 
			glActiveTexture(GL_TEXTURE0);
			CHECK_GL_ERRORS;
			glBindTexture(GL_TEXTURE_2D, m_textureHandler.getTexture(geometryNode->texture.file));
			CHECK_GL_ERRORS;
		}
		if (geometryNode->texture.hasBumps) {
			glActiveTexture(GL_TEXTURE1);
			CHECK_GL_ERRORS;
			glBindTexture(GL_TEXTURE_2D, m_textureHandler.getTextureNormal(geometryNode->texture.normalFile));
			CHECK_GL_ERRORS;
		}
        	glDrawArrays(GL_TRIANGLES, batchInfo.startIndex, batchInfo.numIndices);
		CHECK_GL_ERRORS;
		glBindTexture(GL_TEXTURE_2D, 0);
		CHECK_GL_ERRORS;

		m_shader.disable();
	}	
}


void A3::renderSkybox() {
	glBindVertexArray(m_vao_skybox);
	 glBindTexture(GL_TEXTURE_CUBE_MAP, m_skybox_texture);
	CHECK_GL_ERRORS;

	m_shader_skybox.enable();
	GLint location = m_shader_skybox.getUniformLocation( "View" );
	glUniformMatrix4fv( location, 1, GL_FALSE, value_ptr(glm::mat4(glm::mat3(m_images[m_mode].camera.getViewMatrix()))));
	CHECK_GL_ERRORS;

	glDrawArrays(GL_TRIANGLES, 0, 36);
	CHECK_GL_ERRORS;

	m_shader_skybox.disable();
	
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	CHECK_GL_ERRORS;
}	

void A3::switchMode(Mode newMode) {
	if (newMode == BEFORE_GAME) {
		processLuaSceneFile(m_luaSceneFile);
	}
	m_mode = newMode;
	createPerspectiveMatrix();
	setupBackgroundTexture();
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
		/*if (m_mode == BEFORE_GAME) {
			if (leftMousePressed) {
				translateNode->translate(vec3(xDiff/500, (-1 * yDiff)/500, 0));
				eventHandled = true;
			}
			else if (middleMousePressed) {
				translateNode->translate(vec3(0, 0, yDiff/500));
				eventHandled = true;
			}			
		}*/
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
		if (m_mode == BEFORE_GAME || m_mode == AFTER_GAME) {
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

			unsigned int what = buffer[0] + (buffer[1] << 8) + (buffer[2] << 16);
			if (what == m_startButton->m_nodeId) {
				switchMode(DURING_GAME);
			}

			else if (what == m_playAgainButton->m_nodeId) {
				switchMode(BEFORE_GAME);
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
	createPerspectiveMatrix(); // fix this
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
			switch(m_mode) {
				case BEFORE_GAME:
					this->switchMode(DURING_GAME);
					break;
				case DURING_GAME:
					this->switchMode(AFTER_GAME);
					break;
				case AFTER_GAME:
					this->switchMode(BEFORE_GAME);
					break;
				deafult:
					std::cerr << "invalid mode" << std::endl;
			}
			eventHandled = true;	
		}
		if (key == GLFW_KEY_Q) {
			glfwSetWindowShouldClose(m_window, GL_TRUE);
                        eventHandled = true;
                }
		if (key == GLFW_KEY_LEFT) {
			leftKeyPressed = true;
			eventHandled = true;
		}
		if (key == GLFW_KEY_RIGHT) {
			rightKeyPressed = true;
			eventHandled = true;
		}
	}
	if(action == GLFW_RELEASE) {
		if (key == GLFW_KEY_LEFT) {
                        leftKeyPressed = false;
                        eventHandled = true;
                }
                if (key == GLFW_KEY_RIGHT) {
                        rightKeyPressed = false;
                        eventHandled = true;
                }
	}
	// Fill in with event handling code...

	return eventHandled;
}
