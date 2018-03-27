#pragma once

#include "cs488-framework/CS488Window.hpp"
#include "cs488-framework/OpenGLImport.hpp"
#include "cs488-framework/ShaderProgram.hpp"
#include "cs488-framework/MeshConsolidator.hpp"

#include "SceneNode.hpp"
#include "JointStack.hpp"
#include "Background.hpp"
#include "SkyboxData.hpp"
#include "TextureHandler.hpp"
#include "Ball.hpp"
#include "Paddle.hpp"
#include "Image.hpp"
#include "Board.hpp"
#include "Camera.hpp"
#include "CollisionType.hpp"
#include "Animator.hpp"
#include "PaddleLeft.hpp"
#include "Ball2p.hpp"

#include <glm/glm.hpp>
#include <memory>
#include <map>
#include <stack>
#include <tuple>
#include <set>
#include <irrKlang.h>

using namespace irrklang;

struct LightSource {
	glm::vec3 position;
	glm::vec3 rgbIntensity;
};

class A3 : public CS488Window {
public:
	A3(const std::string & luaSceneFile);
	virtual ~A3();

protected:

	enum Mode {
                BEFORE_GAME,
                DURING_GAME,
                AFTER_GAME
        };

	virtual void init() override;
	virtual void appLogic() override;
	virtual void guiLogic() override;
	virtual void draw() override;
	virtual void cleanup() override;


	//-- Virtual callback methods
	virtual bool cursorEnterWindowEvent(int entered) override;
	virtual bool mouseMoveEvent(double xPos, double yPos) override;
	virtual bool mouseButtonInputEvent(int button, int actions, int mods) override;
	virtual bool mouseScrollEvent(double xOffSet, double yOffSet) override;
	virtual bool windowResizeEvent(int width, int height) override;
	virtual bool keyInputEvent(int key, int action, int mods) override;

	//-- One time initialization methods:
	void processLuaSceneFile(const std::string & filename);
	void createShaderProgram();
	void enableVertexShaderInputSlots();
	void uploadVertexDataToVbos(const MeshConsolidator & meshConsolidator);
	void mapVboDataToVertexShaderInputLocations();
	glm::mat4 createViewMatrix(Camera camera);
	void initLightSources();
	void initSelected(SceneNode *root);
	void setupBackgroundTexture();
	void createTextures(std::set<string> textureFiles, std::set<string> textureNormalFiles);

	void changePlayer(bool singlePlayer);

	void createPerspectiveMatrix();
	void uploadCommonSceneUniforms();
	void uploadCommonImageUniforms();
	void renderSceneGraph(const SceneNode &node);
	void renderGraph(const SceneNode &root, glm::mat4 modelMatrix);
	void renderNode(const SceneNode &node, glm::mat4 modelMatrix);
	void renderSkybox();
	void renderBeforeGame();
	void renderAfterGame();
	void renderMap(const SceneNode &box);
	
	void renderGame();
	void drawPaddle();
	void drawPaddleLeft();
	void drawPaddleRight();
	CollisionType drawBall();
	CollisionType drawBall2();
	void renderGameNode(const SceneNode &node);

	void undo();
	void redo();
	void switchMode(Mode newMode);

	glm::mat4 m_perspective;

	LightSource m_light;

	GLuint m_skybox_texture;

	//-- GL resources for mesh geometry data:
	GLuint m_vao_meshData;
	GLuint m_vbo_vertexPositions;
	GLuint m_vbo_vertexNormals;
	GLuint m_vbo_vertexTextureCoords;
	GLuint m_vbo_vertexTangents;
	GLint m_positionAttribLocation;
	GLint m_normalAttribLocation;
	GLint m_textureCoordAttribLocation;
	GLint m_tangentAttribLocation;
	ShaderProgram m_shader;

	//-- GL resources for trackball circle geometry:
	GLuint m_vbo_arcCircle;
	GLuint m_vao_arcCircle;
	GLint m_arc_positionAttribLocation;
	ShaderProgram m_shader_arcCircle;

	//-- GL resources for game geometry
	GLuint m_vao_game;
	GLint m_game_positionAttribLocation;
	ShaderProgram m_shader_game;

	//-- GL resources for skybox
	ShaderProgram m_shader_skybox;
	GLuint m_vbo_skybox;
	GLuint m_vao_skybox;
	GLint m_skybox_positionAttribLocation;

	ShaderProgram m_shader_map;
	GLuint m_vao_map;
	GLint m_map_positionAttribLocation;
	GLint m_map_normalAttribLocation;

	// BatchInfoMap is an associative container that maps a unique MeshId to a BatchInfo
	// object. Each BatchInfo object contains an index offset and the number of indices
	// required to render the mesh with identifier MeshId.
	BatchInfoMap m_batchInfoMap;

	std::string m_luaSceneFile;

	SkyboxData m_skyboxData;
	TextureHandler m_textureHandler;

	bool do_picking;
	bool leftMousePressed;
	bool rightMousePressed;
	bool middleMousePressed;
	bool leftKeyPressed;
	bool rightKeyPressed;
	bool upKeyPressed;
	bool downKeyPressed;
	bool dKeyPressed;
	bool xKeyPressed;
	double oldX;
	double oldY;
	std::map<unsigned int, bool> selected; 
	std::map<unsigned int, SceneNode*> objectToJoint;
	
	ISoundEngine *m_soundEngine = createIrrKlangDevice();

	Mode m_mode;	

	Image m_images[3];

	Ball m_ball;
	Ball2p m_ball2;
	Paddle m_paddle;
	PaddleLeft m_leftPaddle;
	PaddleLeft m_rightPaddle;
	Board m_board;
	Animator m_spaceship;
	SceneNode *m_startButton;
	SceneNode *m_playAgainButton;
	SceneNode *m_gameOverText;
	SceneNode *m_onePlayerButton;
	SceneNode *m_twoPlayerButton;
	SceneNode *m_box;
	SceneNode *m_map;
	bool multiPlayer;
};
