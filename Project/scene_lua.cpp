//
// CS488 - Introduction to Computer Graphics
//
// scene_lua.cpp
//
// Everything that's needed to parse a scene file using Lua.
// You don't necessarily have to understand exactly everything that
// goes on here, although it will be useful to have a reasonable idea
// if you wish to add new commands to the scene format.
//
// Lua interfaces with C/C++ using a special stack. Everytime you want
// to get something from lua, or pass something back to lua (e.g. a
// return value), you need to use this stack. Thus, most of the lua_
// and luaL_ functions actually manipulate the stack. All the
// functions beginning with "lua_" are part of the Lua C API itself,
// whereas the "luaL_" functions belong to a library of useful
// functions on top of that called lauxlib.
//
// This file consists of a bunch of C function declarations which
// implement functions callable from Lua. There are also two tables
// used to set up the interface between Lua and these functions, and
// the main "driver" function, import_lua, which calls the lua
// interpreter and sets up all the state.
//
// Note that each of the function declarations follow the same format:
// they take as their only argument the current state of the lua
// interpreter, and return the number of values returned back to lua.
//
// For more information see the book "Programming In Lua," available
// online at http://www.lua.org/pil/, and of course the Lua reference
// manual at http://www.lua.org/manual/5.0/.
//
// http://lua-users.org/wiki/LauxLibDocumentation provides a useful
// documentation of the "lauxlib" functions (beginning with luaL_).
//
// -- University of Waterloo Computer Graphics Lab 2005

#include "scene_lua.hpp"
#include <iostream>
#include <cctype>
#include <cstring>
#include <cstdio>
#include <string>
#include <queue>
#include "lua488.hpp"
#include "JointNode.hpp"
#include "GeometryNode.hpp"
#include "Camera.hpp"
#include "Image.hpp"
#include "Ball.hpp"
#include "Paddle.hpp"

// Uncomment the following line to enable debugging messages
//#define GRLUA_ENABLE_DEBUG

#ifdef GRLUA_ENABLE_DEBUG
#  define GRLUA_DEBUG(x) do { std::cerr << x << std::endl; } while (0)
#  define GRLUA_DEBUG_CALL do { std::cerr << __FUNCTION__ << std::endl; } while (0)
#else
#  define GRLUA_DEBUG(x) do { } while (0)
#  define GRLUA_DEBUG_CALL do { } while (0)
#endif

// You may wonder, for the following types, why we use special "_ud"
// types instead of, for example, just allocating SceneNodes directly
// from lua. Part of the answer is that Lua is a C api. It doesn't
// call any constructors or destructors for you, so it's easier if we
// let it just allocate a pointer for the node, and handle
// allocation/deallocation of the node itself. Another (perhaps more
// important) reason is that we will want SceneNodes to stick around
// even after lua is done with them, after all, we want to pass them
// back to the program. If we let Lua allocate SceneNodes directly,
// we'd lose them all when we are done parsing the script. This way,
// we can easily keep around the data, all we lose is the extra
// pointers to it.

// The "userdata" type for a node. Objects of this type will be
// allocated by Lua to represent nodes.
struct gr_node_ud {
  SceneNode* node;
};

// The "userdata" type for a material. Objects of this type will be
// allocated by Lua to represent materials.
struct gr_material_ud {
  Material* material;
};

struct gr_texture_ud {
  Texture* texture;
};

struct gr_background_ud {
  Background* background;
};

std::set<std::string> textureFiles;
std::set<std::string> textureNormalFiles;

// Create a node
extern "C"
int gr_node_cmd(lua_State* L)
{
  GRLUA_DEBUG_CALL;
  
  gr_node_ud* data = (gr_node_ud*)lua_newuserdata(L, sizeof(gr_node_ud));
  data->node = 0;

  const char* name = luaL_checkstring(L, 1);
  data->node = new SceneNode(name);

  luaL_getmetatable(L, "gr.node");
  lua_setmetatable(L, -2);

  return 1;
}

// Create a joint node
extern "C"
int gr_joint_cmd(lua_State* L)
{
  GRLUA_DEBUG_CALL;

  gr_node_ud* data = (gr_node_ud*)lua_newuserdata(L, sizeof(gr_node_ud));
  data->node = 0;

  const char* name = luaL_checkstring(L, 1);
  JointNode* node = new JointNode(name);

  luaL_checktype(L, 2, LUA_TTABLE);

  luaL_argcheck(L, luaL_len(L, 2) == 3, 2, "Three-tuple expected");

  luaL_checktype(L, 3, LUA_TTABLE);

  luaL_argcheck(L, luaL_len(L, 3) == 3, 3, "Three-tuple expected");

  double x[3], y[3];
  for (int i = 1; i <= 3; i++) {
    lua_rawgeti(L, 2, i);
    x[i - 1] = luaL_checknumber(L, -1);
    lua_rawgeti(L, 3, i);
    y[i - 1] = luaL_checknumber(L, -1);
    lua_pop(L, 2);
  }

  node->set_joint_x(x[0], x[1], x[2]);
  node->set_joint_y(y[0], y[1], y[2]);

  data->node = node;

  luaL_getmetatable(L, "gr.node");
  lua_setmetatable(L, -2);

  return 1;
}
extern "C"
int gr_mesh_cmd(lua_State* L)
{
	GRLUA_DEBUG_CALL;

	gr_node_ud* data = (gr_node_ud*)lua_newuserdata(L, sizeof(gr_node_ud));
	data->node = 0;

	const char* meshId = luaL_checkstring(L, 1);
	const char* name = luaL_checkstring(L, 2);
	data->node = new GeometryNode(meshId, name);

	luaL_getmetatable(L, "gr.node");
	lua_setmetatable(L, -2);

	return 1;
}


// Create a material
extern "C"
int gr_material_cmd(lua_State* L)
{
  GRLUA_DEBUG_CALL;
  
  gr_material_ud* data = (gr_material_ud*)lua_newuserdata(L, sizeof(gr_material_ud));
  data->material = 0;
  
  luaL_checktype(L, 1, LUA_TTABLE);

  luaL_argcheck(L, luaL_len(L, 1) == 3, 1, "Three-tuple expected");

  luaL_checktype(L, 2, LUA_TTABLE);

  luaL_argcheck(L, luaL_len(L, 2) == 3, 2, "Three-tuple expected");

  luaL_checktype(L, 3, LUA_TNUMBER);

  luaL_checktype(L, 4, LUA_TNUMBER);

  double kd[3], ks[3];
  for (int i = 1; i <= 3; i++) {
    lua_rawgeti(L, 1, i);
    kd[i - 1] = luaL_checknumber(L, -1);
    lua_rawgeti(L, 2, i);
    ks[i - 1] = luaL_checknumber(L, -1);
    lua_pop(L, 2);
  }
  double shininess = luaL_checknumber(L, 3);
  double alpha = luaL_checknumber(L, 4);

	data->material = new Material();
	for(int i(0); i < 3; ++i) {
		data->material->kd[i] = kd[i];
		data->material->ks[i] = ks[i];
	}
	data->material->shininess = shininess;
	data->material->alpha = alpha;

  luaL_newmetatable(L, "gr.material");
  lua_setmetatable(L, -2);
  
  return 1;
}

// Create a texture
extern "C"
int gr_texture_cmd(lua_State* L)
{
  GRLUA_DEBUG_CALL;
  
  gr_texture_ud* data = (gr_texture_ud*)lua_newuserdata(L, sizeof(gr_texture_ud));
  data->texture = 0;
  
  luaL_checktype(L, 1, LUA_TNUMBER);
  luaL_checktype(L, 2, LUA_TSTRING);
  luaL_checktype(L, 3, LUA_TNUMBER);
  luaL_checktype(L, 4, LUA_TSTRING);

  bool hasTexture = (luaL_checknumber(L, 1)) == 1;
  const char* file = luaL_checkstring(L, 2);
  bool hasBumps = (luaL_checknumber(L, 3)) == 1;
  const char* normalFile = luaL_checkstring(L, 4);

  if (!hasTexture && hasBumps) {
	std::cerr << "no texture but bumps " << file << " : " << normalFile << std::endl;
  }

  data->texture = new Texture();
  data->texture->hasTexture = hasTexture;
  data->texture->file = file;
  data->texture->hasBumps = hasBumps;
  data->texture->normalFile = normalFile;

  luaL_newmetatable(L, "gr.texture");
  lua_setmetatable(L, -2);

  if (hasTexture) {
	textureFiles.insert(file);
  }
  if (hasBumps) {
	textureNormalFiles.insert(normalFile);
  }

  return 1; 
}

//Create a background
extern "C"
int gr_background_cmd(lua_State* L) {
  GRLUA_DEBUG_CALL;

  gr_background_ud* data = (gr_background_ud*)lua_newuserdata(L, sizeof(gr_background_ud));
  luaL_checktype(L, 1, LUA_TSTRING);
  luaL_checktype(L, 2, LUA_TSTRING);
  luaL_checktype(L, 3, LUA_TSTRING);
  luaL_checktype(L, 4, LUA_TSTRING);
  luaL_checktype(L, 5, LUA_TSTRING);
  luaL_checktype(L, 6, LUA_TSTRING);
  luaL_checktype(L, 7, LUA_TNUMBER);

  const char* face1 = luaL_checkstring(L, 1);
  const char* face2 = luaL_checkstring(L, 2);
  const char* face3 = luaL_checkstring(L, 3);
  const char* face4 = luaL_checkstring(L, 4);
  const char* face5 = luaL_checkstring(L, 5);
  const char* face6 = luaL_checkstring(L, 6);
  bool hasSkybox = (luaL_checknumber(L, 7)) == 1;

  data->background = new Background();
  data->background->faces[0] = face1;
  data->background->faces[1] = face2;
  data->background->faces[2] = face3;
  data->background->faces[3] = face4;
  data->background->faces[4] = face5;
  data->background->faces[5] = face6;
  data->background->hasSkybox = hasSkybox;

  luaL_newmetatable(L, "gr.background");
  lua_setmetatable(L, -2);
}

// Add a child to a node
extern "C"
int gr_node_add_child_cmd(lua_State* L)
{
  GRLUA_DEBUG_CALL;
  
  gr_node_ud* selfdata = (gr_node_ud*)luaL_checkudata(L, 1, "gr.node");
  luaL_argcheck(L, selfdata != 0, 1, "Node expected");

  SceneNode* self = selfdata->node;
  
  gr_node_ud* childdata = (gr_node_ud*)luaL_checkudata(L, 2, "gr.node");
  luaL_argcheck(L, childdata != 0, 2, "Node expected");

  SceneNode* child = childdata->node;

  self->add_child(child);

  return 0;
}

// Set a node's material
extern "C"
int gr_node_set_material_cmd(lua_State* L)
{
  GRLUA_DEBUG_CALL;

  gr_node_ud* selfdata = (gr_node_ud*)luaL_checkudata(L, 1, "gr.node");
  luaL_argcheck(L, selfdata != 0, 1, "Node expected");

  GeometryNode* self = dynamic_cast<GeometryNode*>(selfdata->node);

  luaL_argcheck(L, self != 0, 1, "Geometry node expected");

  gr_material_ud* matdata = (gr_material_ud*)luaL_checkudata(L, 2, "gr.material");
  luaL_argcheck(L, matdata != 0, 2, "Material expected");

	Material * material = matdata->material;
	self->material.kd = material->kd;
	self->material.ks = material->ks;
	self->material.shininess = material->shininess;
	self->material.alpha = material->alpha;

  return 0;
}

// Set a node's texture
extern "C"
int gr_node_set_texture_cmd(lua_State* L) {
  GRLUA_DEBUG_CALL;

  gr_node_ud* selfdata = (gr_node_ud*)luaL_checkudata(L, 1, "gr.node");
  luaL_argcheck(L, selfdata != 0, 1, "Node expected");
  
  GeometryNode* self = dynamic_cast<GeometryNode*>(selfdata->node);

  luaL_argcheck(L, self != 0, 1, "Geometry node expected");

  gr_texture_ud* texdata = (gr_texture_ud*)luaL_checkudata(L, 2, "gr.texture");
  luaL_argcheck(L, texdata != 0, 2, "Texture expected");

	Texture * texture = texdata->texture;
	self->texture.hasTexture = texture->hasTexture;
	self->texture.file = texture->file;
	self->texture.hasBumps = texture->hasBumps;
	self->texture.normalFile = texture->normalFile;

  return 0;
} 


// Add a scaling transformation to a node.
extern "C"
int gr_node_scale_cmd(lua_State* L)
{
  GRLUA_DEBUG_CALL;
  
  gr_node_ud* selfdata = (gr_node_ud*)luaL_checkudata(L, 1, "gr.node");
  luaL_argcheck(L, selfdata != 0, 1, "Node expected");

  SceneNode* self = selfdata->node;

  double values[3];
  
  for (int i = 0; i < 3; i++) {
    values[i] = luaL_checknumber(L, i + 2);
  }

    self->scale(glm::vec3(values[0], values[1], values[2]));

  return 0;
}

// Add a translation to a node.
extern "C"
int gr_node_translate_cmd(lua_State* L)
{
  GRLUA_DEBUG_CALL;
  
  gr_node_ud* selfdata = (gr_node_ud*)luaL_checkudata(L, 1, "gr.node");
  luaL_argcheck(L, selfdata != 0, 1, "Node expected");

  SceneNode* self = selfdata->node;

  double values[3];
  
  for (int i = 0; i < 3; i++) {
    values[i] = luaL_checknumber(L, i + 2);
  }

  self->translate(glm::vec3(values[0], values[1], values[2]));

  return 0;
}

// Rotate a node.
extern "C"
int gr_node_rotate_cmd(lua_State* L)
{
  GRLUA_DEBUG_CALL;
  
  gr_node_ud* selfdata = (gr_node_ud*)luaL_checkudata(L, 1, "gr.node");
  luaL_argcheck(L, selfdata != 0, 1, "Node expected");

  SceneNode* self = selfdata->node;

  const char* axis_string = luaL_checkstring(L, 2);

  luaL_argcheck(L, axis_string
                && std::strlen(axis_string) == 1, 2, "Single character expected");
  char axis = std::tolower(axis_string[0]);
  
  luaL_argcheck(L, axis >= 'x' && axis <= 'z', 2, "Axis must be x, y or z");
  
  double angle = luaL_checknumber(L, 3);

  self->rotate(axis, angle);

  return 0;
}

// Garbage collection function for lua.
extern "C"
int gr_node_gc_cmd(lua_State* L)
{
  GRLUA_DEBUG_CALL;
  
  gr_node_ud* data = (gr_node_ud*)luaL_checkudata(L, 1, "gr.node");
  luaL_argcheck(L, data != 0, 1, "Node expected");

  // Note that we don't delete the node here. This is because we still
  // want the scene to be around when we close the lua interpreter,
  // but at that point everything will be garbage collected.
  //
  // If data->node happened to be a reference-counted pointer, this
  // will in fact just decrease lua's reference to it, so it's not a
  // bad thing to include this line.
  data->node = 0;

  return 0;
}

// This is where all the "global" functions in our library are
// declared.
// If you want to add a new non-member function, add it here.
static const luaL_Reg grlib_functions[] = {
  {"node", gr_node_cmd},
  {"joint", gr_joint_cmd},
  {"mesh", gr_mesh_cmd},
  {"material", gr_material_cmd},
  {"texture", gr_texture_cmd},
  {"background", gr_background_cmd},
  {0, 0}
};

// This is where all the member functions for "gr.node" objects are
// declared. Since all the other objects (e.g. materials) are so
// simple, we only really need to make member functions for nodes.
//
// If you want to add a new member function for gr.node, add it
// here.
//
// We could have used inheritance in lua to match the inheritance
// between different node types, but it's easier to just give all
// nodes the same Lua type and then do any additional type checking in
// the appropriate member functions (e.g. gr_node_set_material_cmd
// ensures that the node is a GeometryNode, see above).
static const luaL_Reg grlib_node_methods[] = {
  {"__gc", gr_node_gc_cmd},
  {"add_child", gr_node_add_child_cmd},
  {"set_material", gr_node_set_material_cmd},
  {"set_texture", gr_node_set_texture_cmd},
  {"scale", gr_node_scale_cmd},
  {"rotate", gr_node_rotate_cmd},
  {"translate", gr_node_translate_cmd},
  {0, 0}
};

// This function calls the lua interpreter to do the actual importing
Scene import_lua(const std::string& filename)
{
  GRLUA_DEBUG("Importing scene from " << filename);
  
  // Start a lua interpreter
  lua_State* L = luaL_newstate();

  GRLUA_DEBUG("Loading base libraries");
  
  // Load some base library
  luaL_openlibs(L);


  GRLUA_DEBUG("Setting up our functions");

  // Set up the metatable for gr.node
  luaL_newmetatable(L, "gr.node");
  lua_pushstring(L, "__index");
  lua_pushvalue(L, -2);
  lua_settable(L, -3);

  // Load the gr.node methods

  luaL_setfuncs(L, grlib_node_methods, 0);

  // Load the gr functions
  luaL_setfuncs(L, grlib_functions, 0);
  lua_setglobal(L, "gr");

  GRLUA_DEBUG("Parsing the scene");
  // Now parse the actual scene
  if (luaL_loadfile(L, filename.c_str()) || lua_pcall(L, 0, 1, 0)) {
    std::cerr << "Error loading " << filename << ": " << lua_tostring(L, -1) << std::endl;
    return Scene();
  }

  GRLUA_DEBUG("checking returned table");
  
  luaL_checktype(L, -1, LUA_TTABLE);

  //luaL_argcheck(L, luaL_len(L, -1) == 2, 2, "Two-tuple expected");

  GRLUA_DEBUG("Getting back the node");

  lua_pushstring(L, "r");
  lua_gettable(L, -2);
  gr_node_ud* data = (gr_node_ud*)luaL_checkudata(L, -1, "gr.node");
  if (!data) {
    std::cerr << "Error loading " << filename << ": Must return the root node." << std::endl;
    return Scene();
  }
   
  GRLUA_DEBUG("Getting back the background");

  lua_pop(L, 1);
  lua_pushstring(L, "b");
  lua_gettable(L, -2);
  gr_background_ud* backData = (gr_background_ud*)luaL_checkudata(L, -1, "gr.background");
  if (!backData) {
     std::cerr << "Error loading " << filename << ": Must return the background." << std::endl;
    return Scene();
  }

  GRLUA_DEBUG("Getting back the backgroundfinal");

  lua_pop(L, 1);
  lua_pushstring(L, "z");
  lua_gettable(L, -2);
  gr_background_ud* backFinalData = (gr_background_ud*)luaL_checkudata(L, -1, "gr.background");
  if (!backFinalData) {
     std::cerr << "Error loading " << filename << ": Must return the background final." << std::endl;
    return Scene();
  }

  GRLUA_DEBUG("Gettign the ball node");
  lua_pop(L, 1);
  lua_pushstring(L, "q");
  lua_gettable(L, -2);
  gr_node_ud* ballData = (gr_node_ud*)luaL_checkudata(L, -1, "gr.node");
  if (!ballData) {
    std::cerr << "Error loading " << filename << ": Must return the ball." << std::endl;
    return Scene();
  }

  GRLUA_DEBUG("Getting teh paddle node");
  lua_pop(L, 1);
  lua_pushstring(L, "p");
  lua_gettable(L, -2);
  gr_node_ud* paddleData = (gr_node_ud*)luaL_checkudata(L, -1, "gr.node");
  if (!paddleData) {
    std::cerr << "Error loading " << filename << ": Must return the paddle." << std::endl;
    return Scene();
  }

  GRLUA_DEBUG("Getting teh start node");
  lua_pop(L, 1);
  lua_pushstring(L, "s");
  lua_gettable(L, -2);
  gr_node_ud* startData = (gr_node_ud*)luaL_checkudata(L, -1, "gr.node");
  if (!startData) {
    std::cerr << "Error loading " << filename << ": Must return the start node." << std::endl;
    return Scene();
  }

  GRLUA_DEBUG("Getting teh gameOver node");
  lua_pop(L, 1);
  lua_pushstring(L, "g");
  lua_gettable(L, -2);
  gr_node_ud* gameOverData = (gr_node_ud*)luaL_checkudata(L, -1, "gr.node");
  if (!gameOverData) {
    std::cerr << "Error loading " << filename << ": Must return the gameOver node." << std::endl;
    return Scene();
  }

  GRLUA_DEBUG("Getting teh playAgain node");
  lua_pop(L, 1);
  lua_pushstring(L, "a");
  lua_gettable(L, -2);
  gr_node_ud* playAgainData = (gr_node_ud*)luaL_checkudata(L, -1, "gr.node");
  if (!playAgainData) {
    std::cerr << "Error loading " << filename << ": Must return the playAgain node." << std::endl;
    return Scene();
  }

  GRLUA_DEBUG("Getting teh box node");
  lua_pop(L, 1);
  lua_pushstring(L, "c");
  lua_gettable(L, -2);
  gr_node_ud* boxData = (gr_node_ud*)luaL_checkudata(L, -1, "gr.node");
  if (!boxData) {
    std::cerr << "Error loading " << filename << ": Must return the box node." << std::endl;
    return Scene();
  }

  GRLUA_DEBUG("Getting teh map node");
  lua_pop(L, 1);
  lua_pushstring(L, "m");
  lua_gettable(L, -2);
  gr_node_ud* mapData = (gr_node_ud*)luaL_checkudata(L, -1, "gr.node");
  if (!mapData) {
    std::cerr << "Error loading " << filename << ": Must return the map node." << std::endl;
    return Scene();
  }

  GRLUA_DEBUG("Getting teh onePlayer node");
  lua_pop(L, 1);
  lua_pushstring(L, "o");
  lua_gettable(L, -2);
  gr_node_ud* onePlayerData = (gr_node_ud*)luaL_checkudata(L, -1, "gr.node");
  if (!onePlayerData) {
    std::cerr << "Error loading " << filename << ": Must return the onePlayer node." << std::endl;
    return Scene();
  }

  GRLUA_DEBUG("Getting teh twoPlayer node");
  lua_pop(L, 1);
  lua_pushstring(L, "t");
  lua_gettable(L, -2);
  gr_node_ud* twoPlayerData = (gr_node_ud*)luaL_checkudata(L, -1, "gr.node");
  if (!twoPlayerData) {
    std::cerr << "Error loading " << filename << ": Must return the twoPlayer node." << std::endl;
    return Scene();
  }

  // Store it
  SceneNode* node = data->node;

  SceneNode* ballNode = ballData->node;

  SceneNode* paddleNode = paddleData->node;

  Background* background = backData->background;

  Background* backgroundFinal = backFinalData->background;
	
  SceneNode* startNode = startData->node;

  SceneNode* gameOverNode = gameOverData->node;

  SceneNode* playAgainNode = playAgainData->node;

  SceneNode* boxNode = boxData->node;

  SceneNode* mapNode = mapData->node;

  SceneNode* onePlayerNode = onePlayerData->node;

  SceneNode* twoPlayerNode = twoPlayerData->node;

  GRLUA_DEBUG("Closing the interpreter");
  
  // Close the interpreter, free up any resources not needed
  lua_close(L);

  // And return the node
  std::queue<glm::vec3> shipPositions;
  shipPositions.push(glm::vec3(-6, 0, -3));
  shipPositions.push(glm::vec3(-1.5, 2.0, -3));
  shipPositions.push(glm::vec3(1.5, 2.0, -3));
  shipPositions.push(glm::vec3(6, 0, -3));
  KeyFrame kf = KeyFrame(shipPositions, 0.03);
  Animator spaceship = Animator(node, kf);
  Scene scene;
  scene.spaceship = spaceship;
  scene.textureFiles = textureFiles;
  scene.textureNormalFiles = textureNormalFiles;
  scene.ballNode = ballNode;
  scene.paddleNode = paddleNode;
  scene.startButton = startNode;
  scene.playAgainButton = playAgainNode;
  scene.gameOverText = gameOverNode;
  scene.box = boxNode;
  scene.map = mapNode;
  scene.images[0] = Image(*background, Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -10.0f), glm::vec3(0.0f, 1.0f, 0.0f)), 60.0f);
  scene.images[1] = Image(Background(), Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -5.0f), glm::vec3(0.0f, 1.0f, 0.0f)), 60.0f);
  scene.images[2] = Image(*backgroundFinal, Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -10.0f), glm::vec3(0.0f, 1.0f, 0.0f)), 60.0f);
  scene.onePlayerButton = onePlayerNode;
  scene.twoPlayerButton = twoPlayerNode;	

  return scene;
}
