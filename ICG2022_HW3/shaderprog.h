#ifndef SHADER_PROGRAM_H
#define SHADER_PROGRAM_H

#include "headers.h"
using namespace std;


// ShaderProg Declarations.
class ShaderProg
{
public:
	// ShaderProg Public Methods.
	ShaderProg();
	~ShaderProg();

	bool LoadFromFiles(const string& vsFilePath, const string& fsFilePath);
	void Bind() { glUseProgram(shaderProgId); };
	void UnBind() { glUseProgram(0); };

	GLint GetLocMVP() const { return locMVP; }

protected:
	// ShaderProg Protected Methods.
	virtual void GetUniformVariableLocation();
	// ShaderProg Protected Data.
	GLuint shaderProgId;

private:
	// ShaderProg Private Methods.
	GLuint AddShader(const string& sourceText, GLenum shaderType);
	static bool LoadShaderTextFromFile(const string& filePath, string& sourceText);
	// ShaderProg Private Data.
	GLint locMVP;
};


// FillColorShaderProg Declarations.
class FillColorShaderProg : public ShaderProg
{
public:
	// FillColorShaderProg Public Methods.
	FillColorShaderProg();
	~FillColorShaderProg();

	GLint GetLocFillColor() const { return locFillColor; }

protected:
	// FillColorShaderProg Protected Methods.
	void GetUniformVariableLocation();

private:
	// FillColorShaderProg Private Data.
	GLint locFillColor;
};


// PhongShadingDemoShaderProg Declarations.
class PhongShadingShaderProg : public ShaderProg
{
public:
	// PhongShadingDemoShaderProg Public Methods.
	PhongShadingShaderProg();
	~PhongShadingShaderProg();

	GLint GetLocM()  const { return locM; }
	GLint GetLocNM() const { return locNM; }
	GLint GetLocCameraPos() const { return locCameraPos; }

	GLint GetLocKa() const { return locKa; }
	GLint GetLocKd() const { return locKd; }
	GLint GetLocKs() const { return locKs; }
	GLint GetLocNs() const { return locNs; }

	GLint GetLocHadMapNorm() const { return locHadMapNorm; }
	GLint GetLocMapNorm() const { return locMapNorm; }
	GLint GetLocHadMapKa() const { return locHadMapKa; }
	GLint GetLocMapKa() const { return locMapKa; }
	GLint GetLocHadMapKd() const { return locHadMapKd; }
	GLint GetLocMapKd() const { return locMapKd; }
	GLint GetLocHadMapKs() const { return locHadMapKs; }
	GLint GetLocMapKs() const { return locMapKs; }
	GLint GetLocHadMapNs() const { return locHadMapNs; }
	GLint GetLocMapNs() const { return locMapNs; }

	GLint GetLocAmbientLight()  const { return locAmbientLight; }
	GLint GetLocPointLightPos() const { return locPointLightPos; }
	GLint GetLocPointLightIntensity() const { return locPointLightIntensity; }

	GLint GetLocSpotLightPos() const { return locSpotLightPos; }
	GLint GetLocSpotLightDir() const { return locSpotLightDir; }
	GLint GetLocCutoffStart()  const { return locCutoffStart; }
	GLint GetLocTotalWidth()   const { return locTotalWidth; }
	GLint GetLocSpotLightIntensity()  const { return locSpotLightIntensity; }

	GLint GetLocDirLightDir()      const { return locDirLightDir; }
	GLint GetLocDirLightRadiance() const { return locDirLightRadiance; }

protected:
	// PhongShadingDemoShaderProg Protected Methods.
	void GetUniformVariableLocation();

private:
	// PhongShadingDemoShaderProg Private Data.
	// Transformation matrix.
	GLint locM;
	GLint locNM;
	GLint locCameraPos;
	// Material properties.
	GLint locKa;
	GLint locKd;
	GLint locKs;
	GLint locNs;
	// Texture data.
	GLint locHadMapNorm;
	GLint locMapNorm;
	GLint locHadMapKa;
	GLint locMapKa;
	GLint locHadMapKd;
	GLint locMapKd;
	GLint locHadMapKs;
	GLint locMapKs;
	GLint locHadMapNs;
	GLint locMapNs;
	// Light data.
	GLint locAmbientLight;
	GLint locPointLightPos;
	GLint locPointLightIntensity;

	GLint locSpotLightPos;
	GLint locSpotLightDir;
	GLint locCutoffStart;
	GLint locTotalWidth;
	GLint locSpotLightIntensity;

	GLint locDirLightDir;
	GLint locDirLightRadiance;
};


// SkyboxShaderProg Declarations.
class SkyboxShaderProg : public ShaderProg
{
public:
	// SkyboxShaderProg Public Methods.
	SkyboxShaderProg();
	~SkyboxShaderProg();

	GLint GetLocMapKd() const { return locMapKd; }

protected:
	// PhongShadingDemoShaderProg Protected Methods.
	void GetUniformVariableLocation();

private:
	// SkyboxShaderProg Private Data.
	GLint locMapKd;
};

#endif
