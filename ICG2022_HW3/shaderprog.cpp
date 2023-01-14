#include "shaderprog.h"
using namespace std;

#define MAX_BUFFER_SIZE 1024


ShaderProg::ShaderProg()
{
    // Create OpenGL shader program.
    shaderProgId = glCreateProgram();
    if (shaderProgId == 0)
    {
        cerr << "[ERROR] Failed to create shader program" << endl;
        exit(1);
    }
    locMVP = -1;
}

ShaderProg::~ShaderProg() { glDeleteProgram(shaderProgId); }

bool ShaderProg::LoadFromFiles(const string& vsFilePath, const string& fsFilePath)
{
    // Load the vertex shader from a source file and attach it to the shader program.
    string vs = "", fs = "";
    if (!LoadShaderTextFromFile(vsFilePath, vs))
    {
        cerr << "[ERROR] Failed to load vertex shader source: " << vsFilePath << endl;
        return false;
    }
    GLuint vsId = AddShader(vs, GL_VERTEX_SHADER);

    // Load the fragment shader from a source file and attach it to the shader program.
    if (!LoadShaderTextFromFile(fsFilePath, fs))
    {
        cerr << "[ERROR] Failed to load fragment shader source: " << fsFilePath << endl;
        return false;
    };
    GLuint fsId = AddShader(fs, GL_FRAGMENT_SHADER);

    // Link and compile shader programs.
    GLint success = 0;
    GLchar errorLog[MAX_BUFFER_SIZE] = { 0 };
    glLinkProgram(shaderProgId);
    glGetProgramiv(shaderProgId, GL_LINK_STATUS, &success);
    if (success == 0)
    {
        glGetProgramInfoLog(shaderProgId, sizeof(errorLog), NULL, errorLog);
        cerr << "[ERROR] Failed to link shader program: " << errorLog << endl;
        return false;
    }

    // Now the program already has all stage information, we can delete the shaders now.
    glDeleteShader(vsId);
    glDeleteShader(fsId);

    // Validate program.
    glValidateProgram(shaderProgId);
    glGetProgramiv(shaderProgId, GL_VALIDATE_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(shaderProgId, sizeof(errorLog), NULL, errorLog);
        cerr << "[ERROR] Invalid shader program: " << errorLog << endl;
        return false;
    }

    // Update the location of uniform variables.
    GetUniformVariableLocation();

    return true;
}

void ShaderProg::GetUniformVariableLocation()
{
    locMVP = glGetUniformLocation(shaderProgId, "MVP");
}

GLuint ShaderProg::AddShader(const string& sourceText, GLenum shaderType)
{
    GLuint shaderObj = glCreateShader(shaderType);
    if (shaderObj == 0)
    {
        cerr << "[ERROR] Failed to create shader with type " << shaderType << endl;
        exit(1);
    }

    const GLchar* p[1];
    p[0] = sourceText.c_str();
    GLint lengths[1];
    lengths[0] = (GLint)(sourceText.length());
    glShaderSource(shaderObj, 1, p, lengths);
    glCompileShader(shaderObj);

    GLint success;
    glGetShaderiv(shaderObj, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        GLchar infoLog[MAX_BUFFER_SIZE];
        glGetShaderInfoLog(shaderObj, MAX_BUFFER_SIZE, NULL, infoLog);
        cerr << "[ERROR] Failed to compile shader with type: " << shaderType << ". Info: " << infoLog << endl;
        exit(1);
    }

    glAttachShader(shaderProgId, shaderObj);

    return shaderObj;
}

bool ShaderProg::LoadShaderTextFromFile(const string& filePath, string& sourceText)
{
    ifstream sourceFile(filePath.c_str());
    if (!sourceFile)
    {
        cerr << "[ERROR] Failed to open shader source file: " << filePath << endl;
        return false;
    }
    sourceText.assign((istreambuf_iterator< char >(sourceFile)), istreambuf_iterator< char >());
    return true;
}


FillColorShaderProg::FillColorShaderProg()
{
    locFillColor = -1;
}

FillColorShaderProg::~FillColorShaderProg()
{}

void FillColorShaderProg::GetUniformVariableLocation()
{
    ShaderProg::GetUniformVariableLocation();
    locFillColor = glGetUniformLocation(shaderProgId, "fillColor");
}


PhongShadingShaderProg::PhongShadingShaderProg()
{
    locM = -1;
    locNM = -1;
    locCameraPos = -1;

    locKa = -1;
    locKd = -1;
    locKs = -1;
    locNs = -1;

    locHadMapNorm = -1;
    locMapNorm = -1;
    locHadMapKa = -1;
    locMapKa = -1;
    locHadMapKd = -1;
    locMapKd = -1;
    locHadMapKs = -1;
    locMapKs = -1;
    locHadMapNs = -1;
    locMapNs = -1;

    locAmbientLight = -1;
    locPointLightPos = -1;
    locPointLightIntensity = -1;

    locSpotLightPos = -1;
    locSpotLightIntensity = -1;
    locSpotLightDir = -1;
    locCutoffStart = -1;
    locTotalWidth = -1;

    locDirLightDir = -1;
    locDirLightRadiance = -1;
}

PhongShadingShaderProg::~PhongShadingShaderProg()
{}

void PhongShadingShaderProg::GetUniformVariableLocation()
{
    ShaderProg::GetUniformVariableLocation();
    locM = glGetUniformLocation(shaderProgId, "worldMatrix");
    locNM = glGetUniformLocation(shaderProgId, "normalMatrix");
    locCameraPos = glGetUniformLocation(shaderProgId, "cameraPos");

    locKa = glGetUniformLocation(shaderProgId, "Ka");
    locKd = glGetUniformLocation(shaderProgId, "Kd");
    locKs = glGetUniformLocation(shaderProgId, "Ks");
    locNs = glGetUniformLocation(shaderProgId, "Ns");

    locHadMapNorm = glGetUniformLocation(shaderProgId, "hadMapNorm");
    locMapNorm = glGetUniformLocation(shaderProgId, "mapNorm");
    locHadMapKa = glGetUniformLocation(shaderProgId, "hadMapKa");
    locMapKa = glGetUniformLocation(shaderProgId, "mapKa");
    locHadMapKd = glGetUniformLocation(shaderProgId, "hadMapKd");
    locMapKd = glGetUniformLocation(shaderProgId, "mapKd");
    locHadMapKs = glGetUniformLocation(shaderProgId, "hadMapKs");
    locMapKs = glGetUniformLocation(shaderProgId, "mapKs");
    locHadMapNs = glGetUniformLocation(shaderProgId, "hadMapNs");
    locMapNs = glGetUniformLocation(shaderProgId, "mapNs");

    locAmbientLight = glGetUniformLocation(shaderProgId, "ambientLight");
    locPointLightPos = glGetUniformLocation(shaderProgId, "pointLightPos");
    locPointLightIntensity = glGetUniformLocation(shaderProgId, "pointLightIntensity");

    locSpotLightPos = glGetUniformLocation(shaderProgId, "spotLightPos");
    locSpotLightIntensity = glGetUniformLocation(shaderProgId, "spotLightIntensity");
    locSpotLightDir = glGetUniformLocation(shaderProgId, "spotLightDir");
    locCutoffStart = glGetUniformLocation(shaderProgId, "cutoffStart");
    locTotalWidth = glGetUniformLocation(shaderProgId, "totalWidth");

    locDirLightDir = glGetUniformLocation(shaderProgId, "dirLightDir");
    locDirLightRadiance = glGetUniformLocation(shaderProgId, "dirLightRadiance");
}


SkyboxShaderProg::SkyboxShaderProg()
{
    locMapKd = -1;
}

SkyboxShaderProg::~SkyboxShaderProg()
{}

void SkyboxShaderProg::GetUniformVariableLocation()
{
    ShaderProg::GetUniformVariableLocation();
    locMapKd = glGetUniformLocation(shaderProgId, "mapKd");
}
