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

#include <glm/glm.hpp>
#include <memory>
#include <map>
#include <stack>
#include <tuple>

struct LightSource {
	glm::vec3 position;
	glm::vec3 rgbIntensity;
};

class A3 : public CS488Window {
public:
	A3(const std::string & luaSceneFile);
	virtual ~A3();

protected:
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
	void initViewMatrix();
	void initLightSources();
	void initSelected(SceneNode *root);
	void setupTextureParameters();
	void setupBackgroundTexture();

	void initPerspectiveMatrix();
	void uploadCommonSceneUniforms();
	void renderSceneGraph(const SceneNode &node);
	void renderGraph(const SceneNode &root, glm::mat4 modelMatrix);
	void renderNode(const SceneNode &node, glm::mat4 modelMatrix);
	void renderArcCircle();
	void renderSkybox();

	void makeJointsInit(SceneNode *node);
	void resetJoints(SceneNode *node);
	void resetPosition();
	void resetOrientation();
	void resetAll();
	void undo();
	void redo();

	glm::mat4 m_perpsective;
	glm::mat4 m_view;

	LightSource m_light;

	GLuint m_fs_texture;
	GLuint m_fs_textureNormals;
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

	//-- GL resources for skybox
	ShaderProgram m_shader_skybox;
	GLuint m_vbo_skybox;
	GLuint m_vao_skybox;
	GLint m_skybox_positionAttribLocation;

	// BatchInfoMap is an associative container that maps a unique MeshId to a BatchInfo
	// object. Each BatchInfo object contains an index offset and the number of indices
	// required to render the mesh with identifier MeshId.
	BatchInfoMap m_batchInfoMap;

	std::string m_luaSceneFile;

	std::shared_ptr<SceneNode> m_rootNode;

	Background m_background;
	SkyboxData m_skyboxData;
	TextureHandler m_textureHandler;

	int pickingMode;
	bool do_picking;
	bool leftMousePressed;
	bool rightMousePressed;
	bool middleMousePressed;
	double oldX;
	double oldY;
	SceneNode *translateNode;
	SceneNode *rotateNode;
	glm::mat4 noTranslate;
	glm::mat4 noRotate;
	bool hasZBuffer;
	bool hasBackCull;
	bool hasFrontCull;
	bool drawCircle;
	std::map<unsigned int, bool> selected; 
	std::stack<glm::vec3> colours;
	std::map<unsigned int, glm::vec3> idToColour;
	std::map<std::tuple<float, float, float>, unsigned int> colourToId;
	std::map<unsigned int, SceneNode*> objectToJoint;
	JointStack jointStack;
	JointNode *headSideJoint;
	unsigned int headId;
};
