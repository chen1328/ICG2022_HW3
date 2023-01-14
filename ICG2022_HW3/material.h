#ifndef MATERIAL_H
#define MATERIAL_H

#include "headers.h"
#include "shaderprog.h"
#include "imagetexture.h"
using namespace std;


// Material Declarations.
class Material
{
public:
	// Material Public Methods.
	Material() { name = "Default"; }
	~Material() {}

	void SetName(const string& mtlName) { name = mtlName; }
	string GetName() const { return name; }

protected:
	// Material Protected Data.
	string name;
};


// PhongMaterial Declarations.
class PhongMaterial : public Material
{
public:
	// PhongMaterial Public Methods.
	PhongMaterial() 
	{
		Ka = glm::vec3(0.0f, 0.0f, 0.0f);
		Kd = glm::vec3(0.0f, 0.0f, 0.0f);
		Ks = glm::vec3(0.0f, 0.0f, 0.0f);
		Ns = 0.0f;

		hadMapNorm = false;
		mapNorm = nullptr;
		mapNormPath = "";

		hadMapKa = false;
		mapKa = nullptr;
		mapKaPath = "";

		hadMapKa = false;
		mapKa = nullptr;
		mapKaPath = "";

		hadMapKd = false;
		mapKd = nullptr;
		mapKdPath = "";

		hadMapKs = false;
		mapKs = nullptr;
		mapKsPath = "";

		hadMapNs = false;
		mapNs = nullptr;
		mapNsPath = "";
	}
	~PhongMaterial()
	{
		if (mapNorm != nullptr)
		{
			delete mapNorm;
			mapNorm = nullptr;
		}
		if (mapKa != nullptr)
		{
			delete mapKa;
			mapKa = nullptr;
		}
		if (mapKd != nullptr)
		{
			delete mapKd;
			mapKd = nullptr;
		}
		if (mapKs != nullptr)
		{
			delete mapKs;
			mapKs = nullptr;
		}
		if (mapNs != nullptr)
		{
			delete mapNs;
			mapNs = nullptr;
		}
	}

	void SetKa(const glm::vec3 ka) { Ka = ka; }
	void SetKd(const glm::vec3 kd) { Kd = kd; }
	void SetKs(const glm::vec3 ks) { Ks = ks; }
	void SetNs(const float n)      { Ns = n; }

	void SetHadMapNorm(bool hadMpNorm) { hadMapNorm = hadMpNorm; }
	void SetMapNorm(ImageTexture* tex) { mapNorm = tex; }
	void SetMapNormPath(const string& mpNormPath) { mapNormPath = mpNormPath; }

	void SetHadMapKa(bool hadMpKa) { hadMapKa = hadMpKa; }
	void SetMapKa(ImageTexture* tex) { mapKa = tex; }
	void SetMapKaPath(const string& mpKaPath) { mapKaPath = mpKaPath; }

	void SetHadMapKd(bool hadMpKd) { hadMapKd = hadMpKd; }
	void SetMapKd(ImageTexture* tex) { mapKd = tex; }
	void SetMapKdPath(const string& mpKdPath) { mapKdPath = mpKdPath; }

	void SetHadMapKs(bool hadMpKs) { hadMapKs = hadMpKs; }
	void SetMapKs(ImageTexture* tex) { mapKs = tex; }
	void SetMapKsPath(const string& mpKsPath) { mapKsPath = mpKsPath; }

	void SetHadMapNs(bool hadMpNs) { hadMapNs = hadMpNs; }
	void SetMapNs(ImageTexture* tex) { mapNs = tex; }
	void SetMapNsPath(const string& mpNsPath) { mapNsPath = mpNsPath; }

	glm::vec3 GetKa() const { return Ka; }
	glm::vec3 GetKd() const { return Kd; }
	glm::vec3 GetKs() const { return Ks; }
	float     GetNs() const { return Ns; }

	bool GetHadMapNorm() const { return hadMapNorm; }
	ImageTexture* GetMapNorm() const { return mapNorm; }
	string GetMapNormPath() const { return mapNormPath; }

	bool GetHadMapKa() const { return hadMapKa; }
	ImageTexture* GetMapKa() const { return mapKa; }
	string GetMapKaPath() const { return mapKaPath; }

	bool GetHadMapKd() const { return hadMapKd; }
	ImageTexture* GetMapKd() const { return mapKd; }
	string GetMapKdPath() const { return mapKdPath; }

	bool GetHadMapKs() const { return hadMapKs; }
	ImageTexture* GetMapKs() const { return mapKs; }
	string GetMapKsPath() const { return mapKsPath; }

	bool GetHadMapNs() const { return hadMapNs; }
	ImageTexture* GetMapNs() const { return mapNs; }
	string GetMapNsPath() const { return mapNsPath; }

private:
	// PhongMaterial Private Data.
	glm::vec3 Ka;
	glm::vec3 Kd;
	glm::vec3 Ks;
	float Ns;

	bool hadMapNorm;
	ImageTexture* mapNorm;
	string mapNormPath;

	bool hadMapKa;
	ImageTexture* mapKa;
	string mapKaPath;

	bool hadMapKd;
	ImageTexture* mapKd;
	string mapKdPath;

	bool hadMapKs;
	ImageTexture* mapKs;
	string mapKsPath;

	bool hadMapNs;
	ImageTexture* mapNs;
	string mapNsPath;
};


// SkyboxMaterial Declarations.
class SkyboxMaterial : public Material
{
public:
	// SkyboxMaterial Public Methods.
	SkyboxMaterial() { mapKd = nullptr; }
	~SkyboxMaterial() {}
	void SetMapKd(ImageTexture* tex) { mapKd = tex; }
	ImageTexture* GetMapKd() const { return mapKd; }

private:
	// SkyboxMaterial Private Data.
	ImageTexture* mapKd;
};

#endif
