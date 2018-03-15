#include "MeshConsolidator.hpp"
using namespace glm;
using namespace std;

#include "cs488-framework/Exception.hpp"
#include "cs488-framework/ObjFileDecoder.hpp"
#include <iostream>


//----------------------------------------------------------------------------------------
// Default constructor
MeshConsolidator::MeshConsolidator()
{

}

//----------------------------------------------------------------------------------------
// Destructor
MeshConsolidator::~MeshConsolidator()
{

}

//----------------------------------------------------------------------------------------
template <typename T>
static void appendVector (
		std::vector<T> & dest,
		const std::vector<T> & source
) {
	// Increase capacity to hold source.size() more elements
	dest.reserve(dest.size() + source.size());

	dest.insert(dest.end(), source.begin(), source.end());
}


//----------------------------------------------------------------------------------------
MeshConsolidator::MeshConsolidator(
		std::initializer_list<ObjFilePath> objFileList
) {

	MeshId meshId;
	vector<vec3> positions;
	vector<vec3> normals;
	vector<vec2> uvCoords;
	vector<vec3> tangents;
	BatchInfo batchInfo;
	unsigned long indexOffset(0);

    for(const ObjFilePath & objFile : objFileList) {
	    ObjFileDecoder::decode(objFile.c_str(), meshId, positions, normals, uvCoords, tangents);

	    uint numIndices = positions.size();

	    if (numIndices != normals.size()) {
		    throw Exception("Error within MeshConsolidator: "
					"positions.size() != normals.size()\n");
	    }

	    if ((uvCoords.empty() && !tangents.empty()) || (tangents.empty() && !uvCoords.empty())) {
		    throw Exception("Error within MeshConsolidator: "
					"only one of uvCoords and tangents is empty\n");
	    }

	    if (uvCoords.empty()) {
		for(int i = 0; i < numIndices; i++) {
			uvCoords.push_back(vec2(0, 0));
		}
	    }
	    if (tangents.empty()) {
		for(int i = 0; i < numIndices; i++) {
			tangents.push_back(vec3(0, 0, 0));
		}
	    }

	    if (numIndices != uvCoords.size()) {
		    throw Exception("Error within MeshConsolidator: "
					"positions.size() != uvCoords.size()\n");
	    }

	    if (numIndices != tangents.size()) {
		    throw Exception("Error within MeshConsolidator: "
					"positions.size() != tangents.size()\n");
	    }

	    batchInfo.startIndex = indexOffset;
	    batchInfo.numIndices = numIndices;

	    m_batchInfoMap[meshId] = batchInfo;
	
	    appendVector(m_vertexPositionData, positions);
	    appendVector(m_vertexNormalData, normals);
	    appendVector(m_vertexTextureData, uvCoords);
	    appendVector(m_vertexTangentData, tangents);

	    indexOffset += numIndices;
    }

}

//----------------------------------------------------------------------------------------
void MeshConsolidator::getBatchInfoMap (
		BatchInfoMap & batchInfoMap
) const {
	batchInfoMap = m_batchInfoMap;
}

//----------------------------------------------------------------------------------------
// Returns the starting memory location for vertex position data.
const float * MeshConsolidator::getVertexPositionDataPtr() const {
	return &(m_vertexPositionData[0].x);
}

//----------------------------------------------------------------------------------------
// Returns the starting memory location for vertex normal data.
const float * MeshConsolidator::getVertexNormalDataPtr() const {
    return &(m_vertexNormalData[0].x);
}

//----------------------------------------------------------------------------------------
// Returns the starting memory location for vertex texture data.
const float * MeshConsolidator::getVertexTextureDataPtr() const {
    return &(m_vertexTextureData[0].x);
}

//----------------------------------------------------------------------------------------
// Returns the starting memory location for vertex tangent data.
const float * MeshConsolidator::getVertexTangentDataPtr() const {
    return &(m_vertexTangentData[0].x);
}

//----------------------------------------------------------------------------------------
// Returns the total number of bytes of all vertex position data.
size_t MeshConsolidator::getNumVertexPositionBytes() const {
	return m_vertexPositionData.size() * sizeof(vec3);
}

//----------------------------------------------------------------------------------------
// Returns the total number of bytes of all vertex normal data.
size_t MeshConsolidator::getNumVertexNormalBytes() const {
	return m_vertexNormalData.size() * sizeof(vec3);
}

//----------------------------------------------------------------------------------------
// Returns the total number of bytes of all vertex texture data.
size_t MeshConsolidator::getNumVertexTextureBytes() const {
        return m_vertexTextureData.size() * sizeof(vec2);
}

//----------------------------------------------------------------------------------------
// Returns the total number of bytes of all vertex normal data.
size_t MeshConsolidator::getNumVertexTangentBytes() const {
        return m_vertexTangentData.size() * sizeof(vec3);
}

