#ifndef SKYBOX_H
#define SKYBOX_H

#include "headers.h"
#include "imagetexture.h"
#include "shaderprog.h"
#include "material.h"
#include "camera.h"
using namespace std;

// VertexPT Declarations.
struct VertexPT
{
	VertexPT() 
	{
		position = glm::vec3(0.0f, 0.0f, 0.0f);
		texcoord = glm::vec2(0.0f, 0.0f);
	}
	VertexPT(glm::vec3 p, glm::vec2 uv) 
	{
		position = p;
		texcoord = uv;
	}
	glm::vec3 position;
	glm::vec2 texcoord;
};


// Skybox Declarations.
class Skybox
{
public:
	// Skybox Public Methods.
	Skybox(const string& texImagePath, const int nSlices, 
			const int nStacks, const float radius);
	~Skybox();

	void SetRotation(const float newRotation) { rotationY = newRotation; }
	ImageTexture* GetTexture() { return panorama; }
	float GetRotation() const { return rotationY; }

	void Render(Camera* camera, SkyboxShaderProg* shader, const float curRotationY);

private:
	// Skybox Private Methods.
	static void CreateSphere3D(const int nSlices, const int nStacks, const float radius, 
					vector<VertexPT>& vertices, vector<unsigned int>& indices);

	// Skybox Private Data.
	vector<VertexPT> vertices;
	vector<unsigned int> indices;
	
	SkyboxMaterial* material;
	ImageTexture* panorama;

	float rotationY;
	GLuint vboId;
	GLuint iboId;
};

#endif

