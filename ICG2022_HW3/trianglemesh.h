#ifndef TRIANGLEMESH_H
#define TRIANGLEMESH_H

#include "headers.h"
#include "material.h"
using namespace std;

// VertexPTN Declarations.
struct VertexPTN
{
	VertexPTN()
	{
		position = glm::vec3(0.0f, 0.0f, 0.0f);
		texcoord = glm::vec2(0.0f, 0.0f);
		normal = glm::vec3(0.0f, 1.0f, 0.0f);
	}
	VertexPTN(glm::vec3 p, glm::vec2 uv, glm::vec3 n)
	{
		position = p;
		texcoord = uv;
		normal = n;
	}
	glm::vec3 position;
	glm::vec2 texcoord;
	glm::vec3 normal;
};

// SubMesh Declarations.
struct SubMesh
{
	SubMesh()
	{
		material = nullptr;
		iboId = 1;
	}
	PhongMaterial* material = nullptr;
	vector<unsigned int> vertexIndices;
	GLuint iboId;
};


// TriangleMesh Declarations.
class TriangleMesh
{
public:
	// TriangleMesh Public Methods.
	TriangleMesh();
	~TriangleMesh();

	vector<VertexPTN>& GetVertices() { return vertices; };
	vector<SubMesh>& GetSubMeshes()  { return subMeshes; };

	unsigned int GetNumVertices()  const { return numVertices; }
	unsigned int GetNumNormals()   const { return numNormals; }
	unsigned int GetNumTexcoords() const { return numTexcoords; }
	unsigned int GetNumTriangles() const { return numTriangles; }
	unsigned int GetNumSubMeshes() const { return numSubMeshes; }
	glm::vec3 GetObjCenter() const { return objCenter; }
	glm::vec3 GetObjExtent() const { return objExtent; }

	int GetSubFilePathIndex(const string& filePath);
	bool LoadObjFile(const string& filePath, const bool normalized = true);
	bool LoadMaterialFile(const string& filePath, const string& fileName);
	string GetFaceMode(const string& ptnIndex);
	void ProcessSlashes(vector<string>& ptnIndices);
	void ProcessPTNindex(vector<int>& ptnIndex, const string& faceMode);
	void CreateBuffers();
	void DeleteBuffers();
	void Draw(const unsigned int index);
	void ShowInfo();
	void ShowVerticesInfo();
	void ShowSubMeshesInfo();

private:
	// TriangleMesh Private Data.
	vector<VertexPTN> vertices;
	vector<SubMesh> subMeshes;

	unsigned int numPositions;
	unsigned int numNormals;
	unsigned int numTexcoords;
	unsigned int numVertices;
	unsigned int numTriangles;
	unsigned int numSubMeshes;
	glm::vec3 objCenter;
	glm::vec3 objExtent;
	GLuint vboId;
};

#endif
