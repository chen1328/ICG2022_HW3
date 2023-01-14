#include "trianglemesh.h"
#include "hashfunction.h"
using namespace std;

unordered_map<string, PhongMaterial> phongMaterials;
unordered_map<vector<int>, unsigned int, HashFunction> subMeshVertexIndices;

// Constructor of a triangle mesh.
TriangleMesh::TriangleMesh()
{
	numPositions = 0;
	numNormals = 0;
	numTexcoords = 0;
	numVertices = 0;
	numTriangles = 0;
	numSubMeshes = 0;
	objCenter = glm::vec3(0.0f, 0.0f, 0.0f);
	objExtent = glm::vec3(0.0f, 0.0f, 0.0f);
	vboId = 0;
}

// Destructor of a triangle mesh.
TriangleMesh::~TriangleMesh()
{
	phongMaterials.clear();
	subMeshVertexIndices.clear();
	vertices.clear();
	subMeshes.clear();
	DeleteBuffers();
}

// Get the last index of a backslash of the file path.
int TriangleMesh::GetSubFilePathIndex(const string& filePath)
{
	int index = static_cast<int>(filePath.rfind("\\"));
	if (index != string::npos)
		return index;
	return -1;
}

// Load the geometry data from an OBJ file.
bool TriangleMesh::LoadObjFile(const string& filePath, const bool normalized)
{
	vector<glm::vec3> positions, normals;
	vector<glm::vec2> texcoords;
	vector<vector<vector<int>>> subMeshesIndices;

	ifstream fileStream(filePath);
	string line = "", materialName = "", lastPrefix = "#";
	string subFilePath = filePath.substr(0, GetSubFilePathIndex(filePath) + 1);
	bool hadMaterialFile = false;
	if (!fileStream)
	{
		cerr << "[ERROR] Couldn't open the obj file. Obj file path: " << filePath << endl;
		exit(1);
	}
	while (getline(fileStream, line))
	{
		stringstream ss;
		string prefix;
		ss << line;
		ss >> prefix;
		if (prefix == "mtllib")
		{
			string materialFileName;
			ss >> materialFileName;
			hadMaterialFile = LoadMaterialFile(subFilePath, materialFileName);
		}
		else if (prefix == "usemtl" || prefix == "f" && lastPrefix == "g")
		{
			SubMesh subMesh;
			subMesh.material = new PhongMaterial();
			if (hadMaterialFile)
			{
				ss >> materialName;
				(subMesh.material)->SetName(phongMaterials[materialName].GetName());
				(subMesh.material)->SetKa(phongMaterials[materialName].GetKa());
				(subMesh.material)->SetKd(phongMaterials[materialName].GetKd());
				(subMesh.material)->SetKs(phongMaterials[materialName].GetKs());
				(subMesh.material)->SetNs(phongMaterials[materialName].GetNs());

				(subMesh.material)->SetHadMapNorm(phongMaterials[materialName].GetHadMapNorm());
				(subMesh.material)->SetMapNorm(phongMaterials[materialName].GetMapNorm());
				(subMesh.material)->SetMapNormPath(phongMaterials[materialName].GetMapNormPath());

				(subMesh.material)->SetHadMapKa(phongMaterials[materialName].GetHadMapKa());
				(subMesh.material)->SetMapKa(phongMaterials[materialName].GetMapKa());
				(subMesh.material)->SetMapKaPath(phongMaterials[materialName].GetMapKaPath());

				(subMesh.material)->SetHadMapKd(phongMaterials[materialName].GetHadMapKd());
				(subMesh.material)->SetMapKd(phongMaterials[materialName].GetMapKd());
				(subMesh.material)->SetMapKdPath(phongMaterials[materialName].GetMapKdPath());

				(subMesh.material)->SetHadMapKs(phongMaterials[materialName].GetHadMapKs());
				(subMesh.material)->SetMapKs(phongMaterials[materialName].GetMapKs());
				(subMesh.material)->SetMapKsPath(phongMaterials[materialName].GetMapKsPath());

				(subMesh.material)->SetHadMapNs(phongMaterials[materialName].GetHadMapNs());
				(subMesh.material)->SetMapNs(phongMaterials[materialName].GetMapNs());
				(subMesh.material)->SetMapNsPath(phongMaterials[materialName].GetMapNsPath());
			}
			subMeshes.push_back(subMesh);
			subMeshesIndices.push_back(vector<vector<int>>(3));
			numSubMeshes++;
		}
		else if (prefix == "v")
		{
			glm::vec3 vertex;
			ss >> vertex.x >> vertex.y >> vertex.z;
			if (ss.fail())
			{
				cerr << "[ERROR] Couldn't parse the obj file. Lack of the vertex position info" << endl;
				exit(1);
			}
			positions.push_back(vertex);
			numPositions++;
		}
		else if (prefix == "vt")
		{
			glm::vec2 texcoord;
			ss >> texcoord.x >> texcoord.y;
			if (ss.fail())
			{
				cerr << "[ERROR] Couldn't parse the obj file. Lack of the vertex texcoord info" << endl;
				exit(1);
			}
			texcoords.push_back(texcoord);
			numTexcoords++;
		}
		else if (prefix == "vn")
		{
			glm::vec3 normal;
			ss >> normal.x >> normal.y >> normal.z;
			if (ss.fail())
			{
				cerr << "[ERROR] Couldn't parse the obj file. Lack of the vertex normal info" << endl;
				exit(1);
			}
			normals.push_back(normal);
			numNormals++;
		}
		else if (prefix == "f")
		{
			string ptnString;
			vector<string> ptnIndices;
			while (ss >> ptnString)
			{
				ptnIndices.push_back(ptnString);
				ptnString.clear();
			}

			string faceMode = GetFaceMode(ptnIndices[0]);
			if (faceMode == "NONE")
			{
				cerr << "[ERROR] Couldn't parse the obj file. Face mode error" << endl;
				exit(1);
			}
			ProcessSlashes(ptnIndices);

			stringstream ss2;
			vector<int> firstPTNindex(3, 0);
			ss2 << ptnIndices[0];
			for (int i = 0; i < 3; ++i)
				ss2 >> firstPTNindex[i];
			ProcessPTNindex(firstPTNindex, faceMode);

			int n = static_cast<int>(ptnIndices.size());
			for (int i = 1; i < n - 1; ++i)
			{
				vector<vector<int>> resPTNindices(2, vector<int>(3, 0));
				for (int j = 0; j < 2; ++j)
				{
					stringstream ss3;
					ss3 << ptnIndices[i + j];
					for (int k = 0; k < 3; ++k)
						ss3 >> resPTNindices[j][k];
					ProcessPTNindex(resPTNindices[j], faceMode);
				}
				for (int j = 0; j < 3; ++j)
				{
					(subMeshesIndices.back()[j]).push_back(firstPTNindex[j]);
					(subMeshesIndices.back()[j]).push_back(resPTNindices[0][j]);
					(subMeshesIndices.back()[j]).push_back(resPTNindices[1][j]);
				}
			}
			numTriangles += (n - 2);
		}
		if (prefix == "usemtl" || prefix == "g" || prefix == "f")
			lastPrefix = prefix;
	}
	fileStream.close();

	for (unsigned int i = 0; i < numSubMeshes; ++i)
	{
		unsigned int vertexIndicesSize = static_cast<unsigned int>(subMeshesIndices[i][0].size());
		for (unsigned int j = 0; j < vertexIndicesSize; ++j)
		{
			vector<int> ptnIndex(3, 0);
			for (int k = 0; k < 3; ++k)
				ptnIndex[k] = subMeshesIndices[i][k][j];
			if (subMeshVertexIndices.find(ptnIndex) != subMeshVertexIndices.end())
				subMeshes[i].vertexIndices.push_back(subMeshVertexIndices[ptnIndex]);
			else
			{
				unsigned int verticesSize = static_cast<unsigned int>(vertices.size());
				subMeshVertexIndices[ptnIndex] = verticesSize;
				subMeshes[i].vertexIndices.push_back(verticesSize);

				VertexPTN vertex;
				if (ptnIndex[0] != -1)
					vertex.position = positions[ptnIndex[0]];
				if (ptnIndex[1] != -1)
					vertex.texcoord = texcoords[ptnIndex[1]];
				if (ptnIndex[2] != -1)
					vertex.normal = normals[ptnIndex[2]];
				vertices.push_back(vertex);
			}
		}
	}
	// ShowVerticesInfo();
	// ShowSubMeshesInfo();

	glm::vec3 maxPosition = vertices[0].position, minPosition = vertices[0].position;
	for (VertexPTN& vertex : vertices)
	{
		maxPosition = glm::max(maxPosition, vertex.position);
		minPosition = glm::min(minPosition, vertex.position);
	}
	objCenter = (maxPosition + minPosition) / 2.0f;
	objExtent = maxPosition - minPosition;

	// Normalize the geometry data.
	if (normalized)
	{
		for (VertexPTN& vertex : vertices)
			vertex.position -= objCenter;
		maxPosition -= objCenter;
		minPosition -= objCenter;
		objCenter = glm::vec3(0.0f, 0.0f, 0.0f);

		float maxiAxis = max(max(maxPosition.x, maxPosition.y), maxPosition.z);
		float ratio = 0.5f / maxiAxis;
		objExtent *= ratio;
		for (VertexPTN& vertex : vertices)
			vertex.position *= ratio;
	}
	return true;
}

// Load the material data from a MTL file.
bool TriangleMesh::LoadMaterialFile(const string& filePath, const string& fileName)
{
	ifstream fileStream(filePath + fileName);
	string fileType = fileName.substr(fileName.size() - 4, 4);
	string line = "", materialName = "";
	if (!fileStream || fileType != ".mtl")
	{
		cout << "Couldn't find or open the material file. Material file path: " << filePath + fileName << endl;
		return false;
	}
	while (getline(fileStream, line))
	{
		stringstream ss;
		string prefix;
		ss << line;
		ss >> prefix;
		if (prefix == "newmtl")
		{
			PhongMaterial phongMaterial;
			ss >> materialName;
			phongMaterial.SetName(materialName);
			phongMaterials[materialName] = phongMaterial;
		}
		else if (prefix == "Ka")
		{
			glm::vec3 ka;
			ss >> ka.x >> ka.y >> ka.z;
			if (ss.fail())
			{
				cerr << "[ERROR] Couldn't parse the material file. Lack of the material Ka info" << endl;
				exit(1);
			}
			phongMaterials[materialName].SetKa(ka);
		}
		else if (prefix == "Kd")
		{
			glm::vec3 kd;
			ss >> kd.x >> kd.y >> kd.z;
			if (ss.fail())
			{
				cerr << "[ERROR] Couldn't parse the material file. Lack of the material Kd info" << endl;
				exit(1);
			}
			phongMaterials[materialName].SetKd(kd);
		}
		else if (prefix == "Ks")
		{
			glm::vec3 ks;
			ss >> ks.x >> ks.y >> ks.z;
			if (ss.fail())
			{
				cerr << "[ERROR] Couldn't parse the material file. Lack of the material Ks info" << endl;
				exit(1);
			}
			phongMaterials[materialName].SetKs(ks);
		}
		else if (prefix == "Ns")
		{
			float ns;
			ss >> ns;
			if (ss.fail())
			{
				cerr << "[ERROR] Couldn't parse the material file. Lack of the material Ns info" << endl;
				exit(1);
			}
			phongMaterials[materialName].SetNs(ns);
		}
		else if (prefix == "map_Bump")
		{
			string mapNormPath;
			ss >> mapNormPath;
			if (mapNormPath.empty())
				cerr << "Couldn't find the map_Bump file path" << endl;

			ImageTexture* imageTexNorm = new ImageTexture(filePath + mapNormPath);
			phongMaterials[materialName].SetHadMapNorm(imageTexNorm->GetSuccessLoaded());
			phongMaterials[materialName].SetMapNorm(imageTexNorm);
			phongMaterials[materialName].SetMapNormPath(mapNormPath);
		}
		else if (prefix == "map_Ka")
		{
			string mapKaPath;
			ss >> mapKaPath;
			if (mapKaPath.empty())
				cerr << "Couldn't find the map_Ka file path" << endl;

			ImageTexture* imageTexKa = new ImageTexture(filePath + mapKaPath);
			phongMaterials[materialName].SetHadMapKa(imageTexKa->GetSuccessLoaded());
			phongMaterials[materialName].SetMapKa(imageTexKa);
			phongMaterials[materialName].SetMapKaPath(mapKaPath);
		}
		else if (prefix == "map_Kd")
		{
			string mapKdPath;
			ss >> mapKdPath;
			if (mapKdPath.empty())
				cerr << "Couldn't find the map_Kd file path" << endl;

			ImageTexture* imageTexKd = new ImageTexture(filePath + mapKdPath);
			phongMaterials[materialName].SetHadMapKd(imageTexKd->GetSuccessLoaded());
			phongMaterials[materialName].SetMapKd(imageTexKd);
			phongMaterials[materialName].SetMapKdPath(mapKdPath);
		}
		else if (prefix == "map_Ks")
		{
			string mapKsPath;
			ss >> mapKsPath;
			if (mapKsPath.empty())
				cerr << "Couldn't find the map_Ks file path" << endl;

			ImageTexture* imageTexKs = new ImageTexture(filePath + mapKsPath);
			phongMaterials[materialName].SetHadMapKs(imageTexKs->GetSuccessLoaded());
			phongMaterials[materialName].SetMapKs(imageTexKs);
			phongMaterials[materialName].SetMapKsPath(mapKsPath);
		}
		else if (prefix == "map_Ns")
		{
			string mapNsPath;
			ss >> mapNsPath;
			if (mapNsPath.empty())
				cerr << "Couldn't find the map_Ns file path" << endl;

			ImageTexture* imageTexNs = new ImageTexture(filePath + mapNsPath);
			phongMaterials[materialName].SetHadMapNs(imageTexNs->GetSuccessLoaded());
			phongMaterials[materialName].SetMapNs(imageTexNs);
			phongMaterials[materialName].SetMapNsPath(mapNsPath);
		}
	}
	fileStream.close();
	return true;
}

// Get the face mode (P, PT, PTN, PN, NONE) from the PTN index.
string TriangleMesh::GetFaceMode(const string& ptnIndex)
{
	int totalSlashes = 0;
	for (int i = 0; i < ptnIndex.size() - 1; ++i)
	{
		if (ptnIndex[i] == '/')
			totalSlashes++;
		if (ptnIndex[i] == '/' && ptnIndex[i + 1] == '/')
			totalSlashes++;
	}
	if (totalSlashes == 0)  return "P";
	if (totalSlashes == 1)  return "PT";
	if (totalSlashes == 2)  return "PTN";
	if (totalSlashes == 3)  return "PN";
	return "NONE";
}

// Replace a slash with a space from the PTN indeices.
void TriangleMesh::ProcessSlashes(vector<string>& ptnIndices)
{
	for (int i = 0; i < ptnIndices.size(); ++i)
		for (int j = 0; j < ptnIndices[i].size(); ++j)
			if (ptnIndices[i][j] == '/')
				ptnIndices[i][j] = ' ';
}

// Convert the PTN index to the 0-indexed PTN index or -1 (invalid PTN index).
void TriangleMesh::ProcessPTNindex(vector<int>& ptnIndex, const string& faceMode)
{
	if (faceMode == "PN")
		swap(ptnIndex[1], ptnIndex[2]);
	for (int i = 0; i < 3; ++i)
	{
		if (ptnIndex[i] == 0)
			ptnIndex[i] = -1;
		else if (ptnIndex[i] > 0)
			ptnIndex[i]--;
		else
		{
			if (i == 0)
				ptnIndex[i] += numPositions;
			else if (i == 1)
				ptnIndex[i] += numTexcoords;
			else
				ptnIndex[i] += numNormals;
		}
	}
}

// Create vertex and index buffers.
void TriangleMesh::CreateBuffers()
{
	glGenBuffers(1, &vboId);
	glBindBuffer(GL_ARRAY_BUFFER, vboId);
	glBufferData(GL_ARRAY_BUFFER, sizeof(VertexPTN) * vertices.size(), &(vertices[0]), GL_STATIC_DRAW);
	for (SubMesh& subMesh : subMeshes)
	{
		glGenBuffers(1, &subMesh.iboId);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, subMesh.iboId);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * subMesh.vertexIndices.size(), &(subMesh.vertexIndices[0]), GL_STATIC_DRAW);
	}
}

// Delete vertex and index buffers.
void TriangleMesh::DeleteBuffers()
{
	glDeleteBuffers(1, &vboId);
	for (SubMesh& subMesh : subMeshes)
		glDeleteBuffers(1, &subMesh.iboId);
}

// Draw the model.
void TriangleMesh::Draw(const unsigned int index)
{
	glBindBuffer(GL_ARRAY_BUFFER, vboId);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexPTN), (const GLvoid*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VertexPTN), (const GLvoid*)offsetof(VertexPTN, normal));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(VertexPTN), (const GLvoid*)offsetof(VertexPTN, texcoord));

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, subMeshes[index].iboId);
	glDrawElements(GL_TRIANGLES, (GLsizei)(subMeshes[index].vertexIndices.size()), GL_UNSIGNED_INT, 0);

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
}

// Show model information.
void TriangleMesh::ShowInfo()
{
	cout << "# Positions: " << numPositions << endl;
	cout << "# Texcoords: " << numTexcoords << endl;
	cout << "# Normals: " << numNormals << endl;
	cout << "# Vertices: " << numVertices << endl;
	cout << "# Triangles: " << numTriangles << endl << endl;

	cout << "Total " << numSubMeshes << " subMeshes loaded" << endl;
	for (unsigned int i = 0; i < numSubMeshes; i++)
	{
		if (subMeshes[i].material != nullptr)
			cout << "SubMesh " << i << " with material: " << (subMeshes[i].material)->GetName() << endl;
		cout << "Num. triangles in the subMesh: " << subMeshes[i].vertexIndices.size() / 3 << endl;
	}
	cout << endl;

	cout << "Model Center: " << objCenter.x << ", " << objCenter.y << ", " << objCenter.z << endl;
	cout << "Model Extent: " << objExtent.x << " x " << objExtent.y << " x " << objExtent.z << endl << endl;
}

// Show vertices information.
void TriangleMesh::ShowVerticesInfo()
{
	cout << "P/T/N: " << endl;
	for (VertexPTN& vertex : vertices)
	{
		cout << vertex.position.x << ", " << vertex.position.y << ", " << vertex.position.z << "/"
			<< vertex.texcoord.x << ", " << vertex.texcoord.y << "/"
			<< vertex.normal.x << ", " << vertex.normal.y << ", " << vertex.normal.z << endl;
	}
	cout << endl;
}

// Show submeshes information.
void TriangleMesh::ShowSubMeshesInfo()
{
	for (SubMesh& subMesh : subMeshes)
	{
		cout << "Material Name: " << (subMesh.material)->GetName() << endl;
		cout << "Ka: "
			<< (subMesh.material)->GetKa().x << ", "
			<< (subMesh.material)->GetKa().y << ", "
			<< (subMesh.material)->GetKa().z << endl;
		cout << "Kd: "
			<< (subMesh.material)->GetKd().x << ", "
			<< (subMesh.material)->GetKd().y << ", "
			<< (subMesh.material)->GetKd().z << endl;
		cout << "Ks: "
			<< (subMesh.material)->GetKs().x << ", "
			<< (subMesh.material)->GetKs().y << ", "
			<< (subMesh.material)->GetKs().z << endl;
		cout << "Ns: "
			<< (subMesh.material)->GetNs() << endl;
		cout << "map_Kd: " << (subMesh.material)->GetMapKdPath() << endl << endl;
		for (unsigned int j = 0; j < subMesh.vertexIndices.size(); j += 3)
		{
			cout << subMesh.vertexIndices[j] << "/"
				<< subMesh.vertexIndices[j + 1] << "/"
				<< subMesh.vertexIndices[j + 2] << endl;
		}
		cout << endl;
	}
}