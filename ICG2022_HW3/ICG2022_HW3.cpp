#include "headers.h"
#include "filedialog.h"
#include "trianglemesh.h"
#include "imagetexture.h"
#include "camera.h"
#include "light.h"
#include "shaderprog.h"
#include "skybox.h"
using namespace std;

#define MAX_PATH_SIZE 1024

// Global variables.
int screenWidth = 600;
int screenHeight = 600;
// File dialog.
FileDialog* fileDialog = nullptr;
// Triangle mesh.
TriangleMesh* mesh = nullptr;
// Camera.
Camera* camera = nullptr;
glm::vec3 cameraPos = glm::vec3(0.0f, 1.0f, 5.0f);
glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
float fovy = 30.0f;
float zNear = 0.1f;
float zFar = 1000.0f;
// Lights.
PointLight* pointLight = nullptr;
SpotLight* spotLight = nullptr;
DirectionalLight* dirLight = nullptr;

glm::vec3 ambientLight = glm::vec3(0.2f, 0.2f, 0.2f);
glm::vec3 pointLightPosition = glm::vec3(0.8f, 0.0f, 0.8f);
glm::vec3 pointLightIntensity = glm::vec3(0.5f, 0.1f, 0.1f);

glm::vec3 spotLightPosition = glm::vec3(0.0f, 1.0f, 0.0f);
glm::vec3 spotLightIntensity = glm::vec3(0.25f, 0.25f, 0.1f);
glm::vec3 spotLightDirection = glm::vec3(0.0f, -1.0f, 0.0f);
float spotLightCutoffStartInDegree = 30.0f;
float spotLightTotalWidthInDegree = 45.0f;

glm::vec3 dirLightDirection = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 dirLightRadiance = glm::vec3(0.6f, 0.6f, 0.6f);
// Skybox.
Skybox* skybox = nullptr;
// Shader.
PhongShadingShaderProg* phongShadingShader = nullptr;
FillColorShaderProg* fillColorShader = nullptr;
SkyboxShaderProg* skyboxShader = nullptr;

bool firstSkyboxTex = true;
// UI.
bool isRotated = true;
static float curRotationY = 0.0f;
static float rotDirectionY = 1.0f;
const float rotStep = 0.005f;
const float lightMoveSpeed = 0.2f;

// SceneObject.
struct SceneObject
{
    SceneObject() 
    {
        mesh = nullptr;
        worldMatrix = glm::mat4x4(1.0f);
    }
    TriangleMesh* mesh;
    glm::mat4x4 worldMatrix;
};
SceneObject sceneObj;

// ScenePointLight (for visualization of a point light).
struct ScenePointLight
{
    ScenePointLight() 
    {
        light = nullptr;
        worldMatrix = glm::mat4x4(1.0f);
        visColor = glm::vec3(1.0f, 1.0f, 1.0f);
    }
    PointLight* light;
    glm::mat4x4 worldMatrix;
    glm::vec3 visColor;
};
ScenePointLight pointLightObj;

// SceneSpotLight (for visualization of a spot light).
struct SceneSpotLight
{
    SceneSpotLight()
    {
        light = nullptr;
        worldMatrix = glm::mat4x4(1.0f);
        visColor = glm::vec3(1.0f, 1.0f, 1.0f);
    }
    SpotLight* light;
    glm::mat4x4 worldMatrix;
    glm::vec3 visColor;
};
SceneSpotLight spotLightObj;

// Function prototypes.
void ReleaseResources();
void RenderSceneCB();
void ReshapeCB(int, int);
void ProcessSpecialKeysCB(int, int, int);
void ProcessKeysCB(unsigned char, int, int);
void ProcessMouseInputCB(int, int, int, int);
void ProcessMouseMotionCB(int, int);
void SetupRenderState();
void LoadObjects();
void CreateCamera();
void CreateLights();
void CreateSkybox();
void CreateShaderLib();
void Start();
string GetSubFilePath();


void ReleaseResources()
{
    // Delete fileDialog.
    if (fileDialog != nullptr)
    {
        delete fileDialog;
        fileDialog = nullptr;
    }
    // Delete scene objects.
    if (mesh != nullptr)
    {
        delete mesh;
        mesh = nullptr;
    }
    // Delete camera.
    if (camera != nullptr)
    {
        delete camera;
        camera = nullptr;
    }
    // Delete lights.
    if (pointLight != nullptr)
    {
        delete pointLight;
        pointLight = nullptr;
    }
    if (spotLight != nullptr)
    {
        delete spotLight;
        spotLight = nullptr;
    }
    if (dirLight != nullptr)
    {
        delete dirLight;
        dirLight = nullptr;
    }
    // Delete skybox.
    if (skybox != nullptr)
    {
        delete skybox;
        skybox = nullptr;
    }
    // Delete shaders.
    if (phongShadingShader != nullptr)
    {
        delete phongShadingShader;
        phongShadingShader = nullptr;
    }
    if (fillColorShader != nullptr)
    {
        delete fillColorShader;
        fillColorShader = nullptr;
    }
    if (skyboxShader != nullptr)
    {
        delete skyboxShader;
        skyboxShader = nullptr;
    }
}

void RenderSceneCB()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // Render a triangle mesh with Gouraud shading.
    TriangleMesh* mesh = sceneObj.mesh;
    PointLight* pointLight = pointLightObj.light;
    SpotLight* spotLight = spotLightObj.light;
    if (camera != nullptr && mesh != nullptr && pointLight != nullptr && spotLight != nullptr)
    {
        // Update transform.
        if (isRotated)
            curRotationY += rotDirectionY * rotStep;
        glm::mat4x4 S = glm::scale(glm::mat4x4(1.0f), glm::vec3(1.5f, 1.5f, 1.5f));
        glm::mat4x4 R = glm::rotate(glm::mat4x4(1.0f), glm::radians(curRotationY), glm::vec3(0.0f, 1.0f, 0.0f));
        sceneObj.worldMatrix = S * R;

        glm::mat4x4 normalMatrix = glm::transpose(glm::inverse(sceneObj.worldMatrix));
        glm::mat4x4 MVP = camera->GetProjMatrix() * camera->GetViewMatrix() * sceneObj.worldMatrix;

        phongShadingShader->Bind();
        glUniformMatrix4fv(phongShadingShader->GetLocM(), 1, GL_FALSE, glm::value_ptr(sceneObj.worldMatrix));
        glUniformMatrix4fv(phongShadingShader->GetLocNM(), 1, GL_FALSE, glm::value_ptr(normalMatrix));
        glUniformMatrix4fv(phongShadingShader->GetLocMVP(), 1, GL_FALSE, glm::value_ptr(MVP));
        glUniform3fv(phongShadingShader->GetLocCameraPos(), 1, glm::value_ptr(camera->GetCameraPos()));

        glUniform3fv(phongShadingShader->GetLocAmbientLight(), 1, glm::value_ptr(ambientLight));
        glUniform3fv(phongShadingShader->GetLocPointLightPos(), 1, glm::value_ptr(pointLight->GetPosition()));
        glUniform3fv(phongShadingShader->GetLocPointLightIntensity(), 1, glm::value_ptr(pointLight->GetIntensity()));

        glUniform3fv(phongShadingShader->GetLocSpotLightPos(), 1, glm::value_ptr(spotLight->GetPosition()));
        glUniform3fv(phongShadingShader->GetLocSpotLightIntensity(), 1, glm::value_ptr(spotLight->GetIntensity()));
        glUniform3fv(phongShadingShader->GetLocSpotLightDir(), 1, glm::value_ptr(spotLight->GetDirection()));
        glUniform1f(phongShadingShader->GetLocCutoffStart(), spotLight->GetCutoffStartInDegree());
        glUniform1f(phongShadingShader->GetLocTotalWidth(), spotLight->GetTotalWidthInDegree());

        glUniform3fv(phongShadingShader->GetLocDirLightDir(), 1, glm::value_ptr(dirLight->GetDirection()));
        glUniform3fv(phongShadingShader->GetLocDirLightRadiance(), 1, glm::value_ptr(dirLight->GetRadiance()));

        unsigned int i = 0;
        for (SubMesh& subMesh : mesh->GetSubMeshes())
        {
            glUniform3fv(phongShadingShader->GetLocKa(), 1, glm::value_ptr((subMesh.material)->GetKa()));
            glUniform3fv(phongShadingShader->GetLocKd(), 1, glm::value_ptr((subMesh.material)->GetKd()));
            glUniform3fv(phongShadingShader->GetLocKs(), 1, glm::value_ptr((subMesh.material)->GetKs()));
            glUniform1f(phongShadingShader->GetLocNs(), (subMesh.material)->GetNs());

            ImageTexture* imageTexNorm = (subMesh.material)->GetMapNorm();
            bool hadMapNorm = (subMesh.material)->GetHadMapNorm();
            glUniform1i(phongShadingShader->GetLocHadMapNorm(), hadMapNorm);
            if (hadMapNorm)
            {
                glUniform1i(phongShadingShader->GetLocMapNorm(), 0);
                imageTexNorm->Bind(GL_TEXTURE0);
            }
            ImageTexture* imageTexKa = (subMesh.material)->GetMapKa();
            bool hadMapKa = (subMesh.material)->GetHadMapKa();
            glUniform1i(phongShadingShader->GetLocHadMapKa(), hadMapKa);
            if (hadMapKa)
            {
                glUniform1i(phongShadingShader->GetLocMapKa(), 1);
                imageTexKa->Bind(GL_TEXTURE1);
            }
            ImageTexture* imageTexKd = (subMesh.material)->GetMapKd();
            bool hadMapKd = (subMesh.material)->GetHadMapKd();
            glUniform1i(phongShadingShader->GetLocHadMapKd(), hadMapKd);
            if (hadMapKd)
            {
                imageTexKd->Bind(GL_TEXTURE2);
                glUniform1i(phongShadingShader->GetLocMapKd(), 2);
            }
            ImageTexture* imageTexKs = (subMesh.material)->GetMapKs();
            bool hadMapKs = (subMesh.material)->GetHadMapKs();
            glUniform1i(phongShadingShader->GetLocHadMapKs(), hadMapKs);
            if (hadMapKs)
            {
                glUniform1i(phongShadingShader->GetLocMapKs(), 3);
                imageTexKs->Bind(GL_TEXTURE3);
            }
            ImageTexture* imageTexNs = (subMesh.material)->GetMapNs();
            bool hadMapNs = (subMesh.material)->GetHadMapNs();
            glUniform1i(phongShadingShader->GetLocHadMapNs(), hadMapNs);
            if (hadMapNs)
            {
                glUniform1i(phongShadingShader->GetLocMapNs(), 4);
                imageTexNs->Bind(GL_TEXTURE4);
            }
            // Render model.
            mesh->Draw(i);
            i++;
        }
        phongShadingShader->UnBind();
    }

    // Visualize the lights with fill color.
    if (camera != nullptr && pointLight != nullptr)
    {
        glm::mat4x4 T = glm::translate(glm::mat4x4(1.0f), pointLight->GetPosition());
        pointLightObj.worldMatrix = T;
        glm::mat4x4 MVP = camera->GetProjMatrix() * camera->GetViewMatrix() * pointLightObj.worldMatrix;

        fillColorShader->Bind();
        glUniformMatrix4fv(fillColorShader->GetLocMVP(), 1, GL_FALSE, glm::value_ptr(MVP));
        glUniform3fv(fillColorShader->GetLocFillColor(), 1, glm::value_ptr(pointLightObj.visColor));
        // Render the point light.
        pointLight->Draw();
        fillColorShader->UnBind();
    }
    if (camera != nullptr && spotLight != nullptr)
    {
        glm::mat4x4 T = glm::translate(glm::mat4x4(1.0f), spotLight->GetPosition());
        spotLightObj.worldMatrix = T;
        glm::mat4x4 MVP = camera->GetProjMatrix() * camera->GetViewMatrix() * spotLightObj.worldMatrix;

        fillColorShader->Bind();
        glUniformMatrix4fv(fillColorShader->GetLocMVP(), 1, GL_FALSE, glm::value_ptr(MVP));
        glUniform3fv(fillColorShader->GetLocFillColor(), 1, glm::value_ptr(spotLightObj.visColor));
        // Render the spot light.
        spotLight->Draw();
        fillColorShader->UnBind();
    }

    // Render skybox.
    if (camera != nullptr && skybox != nullptr)
    {
        if (isRotated)
            curRotationY += rotDirectionY * rotStep;
        skybox->Render(camera, skyboxShader, curRotationY);
    }

    glutSwapBuffers();
}

void ReshapeCB(int w, int h)
{
    // Update viewport.
    screenWidth = w;
    screenHeight = h;
    glViewport(0, 0, screenWidth, screenHeight);
    float aspectRatio = (screenWidth * 1.0f) / (screenHeight * 1.0f);
    if (camera != nullptr)
        camera->UpdateProjection(fovy, aspectRatio, zNear, zFar);
}

void ProcessSpecialKeysCB(int key, int x, int y)
{
    // Handle special (functional) keyboard inputs.
    // Rendering mode.
    if (key == GLUT_KEY_F1)
        glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
    else if (key == GLUT_KEY_F2)
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    else if (key == GLUT_KEY_F3)
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    // Point light control.
    if (pointLight != nullptr)
    {
        if (key == GLUT_KEY_UP)
            pointLight->MoveUp(lightMoveSpeed);
        else if (key == GLUT_KEY_DOWN)
            pointLight->MoveDown(lightMoveSpeed);
        else if (key == GLUT_KEY_LEFT)
            pointLight->MoveLeft(lightMoveSpeed);
        else if (key == GLUT_KEY_RIGHT)
            pointLight->MoveRight(lightMoveSpeed);
    }
}

void ProcessKeysCB(unsigned char key, int x, int y)
{
    // Handle other keyboard inputs those are not defined as special keys.
    if (key == 27)
    {
        ReleaseResources();
        exit(0);
    }
    // Model and skybox rotation control.
    else if (key == 'c' || key == 'C')
        isRotated = true;
    else if (key == 'p' || key == 'P')
        isRotated = false;
    else if (key == 'l' || key == 'L')
        rotDirectionY = -1.0f;
    else if (key == 'r' || key == 'R')
        rotDirectionY = 1.0f;
    // Dynamically load and delete model.
    if (mesh == nullptr && (key == 'o' || key == 'O'))
        Start();
    else if (mesh != nullptr && (key == 'e' || key == 'E'))
    {
        ReleaseResources();
        cout << "The model was deleted!" << endl << endl;
    }
    // Spot light control.
    if (spotLight != nullptr)
    {
        if (key == 'w' || key == 'W')
            spotLight->MoveUp(lightMoveSpeed);
        else if (key == 's' || key == 'S')
            spotLight->MoveDown(lightMoveSpeed);
        else if (key == 'a' || key == 'A')
            spotLight->MoveLeft(lightMoveSpeed);
        else if (key == 'd' || key == 'D')
            spotLight->MoveRight(lightMoveSpeed);
    }
    // Dynamically load and delete skybox texture.
    if (mesh != nullptr)
    {
        if (skybox == nullptr && (key == 't' || key == 'T'))
        {
            // Create skybox texture files dialog.
            fileDialog->OpenSkyboxTexFiles();
            if(!(fileDialog->GetSkyboxTexFilePath()).empty())
                CreateSkybox();
            else
            {
                delete skybox;
                skybox = nullptr;
            }
        }
        else if (skybox != nullptr && (key == 'u' || key == 'U'))
        {
            delete skybox;
            skybox = nullptr;
            cout << "The skybox texture was deleted!" << endl << endl;
        }
    }
    
}

void ProcessMouseInputCB(int button, int state, int x, int y)
{
    // Handle mouse input.
    if (camera != nullptr)
        camera->ProcessMouseInput(button, state, x, y);
}

void ProcessMouseMotionCB(int x, int y)
{
    // Handle mouse motion.
    if (camera != nullptr)
        camera->ProcessMouseMotion(x, y);
}

void SetupRenderState()
{
    firstSkyboxTex = true;
    isRotated = true;
    curRotationY = 0.0f;
    rotDirectionY = 1.0f;

    glEnable(GL_DEPTH_TEST);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glm::vec4 clearColor = glm::vec4(0.44f, 0.57f, 0.75f, 1.00f);
    glClearColor(
        (GLclampf)(clearColor.r),
        (GLclampf)(clearColor.g),
        (GLclampf)(clearColor.b),
        (GLclampf)(clearColor.a)
    );
}

void LoadObjects()
{
    // Create obj files dialog.
    fileDialog = new FileDialog();
    fileDialog->OpenObjFiles();
    
    mesh = new TriangleMesh();
    if (!fileDialog->GetObjFilePaths().empty())
    {
        // Load model.
        if (mesh->LoadObjFile((fileDialog->GetObjFilePaths())[0], true))
        {
            mesh->ShowInfo();
            sceneObj.mesh = mesh;
            (sceneObj.mesh)->CreateBuffers();
        }
        else
        {
            delete mesh;
            mesh = nullptr;
        }
    }
    else
    {
        delete fileDialog;
        fileDialog = nullptr;
        delete mesh;
        mesh = nullptr;
    }
}

void CreateCamera()
{
    // Create a camera and update view and proj matrices.
    float aspectRatio = (screenWidth * 1.0f) / (screenHeight * 1.0f);
    camera = new Camera(aspectRatio);
    camera->UpdateView(cameraPos, cameraTarget, cameraUp);
    camera->UpdateProjection(fovy, aspectRatio, zNear, zFar);
}

void CreateLights()
{
    // Create a point light.
    pointLight = new PointLight(pointLightPosition, pointLightIntensity);
    pointLightObj.light = pointLight;
    pointLightObj.visColor = glm::normalize((pointLightObj.light)->GetIntensity());
    // Create a spot light.
    spotLight = new SpotLight(spotLightPosition, spotLightIntensity, spotLightDirection,
        spotLightCutoffStartInDegree, spotLightTotalWidthInDegree);
    spotLightObj.light = spotLight;
    spotLightObj.visColor = glm::normalize((spotLightObj.light)->GetIntensity());
    // Create a directional light.
    dirLight = new DirectionalLight(dirLightDirection, dirLightRadiance);
}

void CreateSkybox()
{
    // Get skybox texture file path.
    string filePath = fileDialog->GetSkyboxTexFilePath();
    if (!filePath.empty() && firstSkyboxTex)
    {
        filePath = GetSubFilePath() + filePath;
        firstSkyboxTex = false;
    }
    // Create skybox.
    const int numSlices = 36;
    const int numStacks = 18;
    const float radius = 50.0f;
    skybox = new Skybox(filePath, numSlices, numStacks, radius);
    if (skybox->GetTexture() == nullptr)
    {
        delete skybox;
        skybox = nullptr;
    }
}

void CreateShaderLib()
{
    // Get sub file path.
    string subFilePath = GetSubFilePath();
    // Create Shaders.
    fillColorShader = new FillColorShaderProg();
    if (!fillColorShader->LoadFromFiles(subFilePath + "shaders/fixed_color.vs", subFilePath + "shaders/fixed_color.fs"))
        exit(1);

    phongShadingShader = new PhongShadingShaderProg();
    if (!phongShadingShader->LoadFromFiles(subFilePath + "shaders/phong_shading.vs", subFilePath + "shaders/phong_shading.fs"))
        exit(1);

    skyboxShader = new SkyboxShaderProg();
    if (!skyboxShader->LoadFromFiles(subFilePath + "shaders/skybox.vs", subFilePath + "shaders/skybox.fs"))
        exit(1);
}

void Start()
{
    // Initialization.
    SetupRenderState();
    LoadObjects();
    if (mesh != nullptr)
    {
        CreateCamera();
        CreateLights();
        CreateSkybox();
        CreateShaderLib();
    }
    // Register callback functions.
    glutDisplayFunc(RenderSceneCB);
    glutIdleFunc(RenderSceneCB);
    glutReshapeFunc(ReshapeCB);
    glutSpecialFunc(ProcessSpecialKeysCB);
    glutKeyboardFunc(ProcessKeysCB);
    glutMouseFunc(ProcessMouseInputCB);
    glutMotionFunc(ProcessMouseMotionCB);

    // Start rendering loop.
    glutMainLoop();
}

string GetSubFilePath()
{
    char path[MAX_PATH_SIZE] = { 0 };
    stringstream ss;
    string subFilePath = "";
    if (_fullpath(path, "./", MAX_PATH_SIZE) != nullptr)
    {
        ss << path;
        if (ss.fail())
        {
            cerr << "[Error] Couldn't read the file path" << endl;
            exit(1);
        }
        subFilePath = ss.str();
        if (subFilePath.rfind("Debug") != string::npos || subFilePath.rfind("Release") != string::npos)
            subFilePath += "../../ICG2022_HW3/";
    }
    return subFilePath;
}

int main(int argc, char** argv)
{
    // Setting window properties.
    glutInit(&argc, argv);
    glutSetOption(GLUT_MULTISAMPLE, 4);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_MULTISAMPLE);
    glutInitWindowSize(screenWidth, screenHeight);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Texture Mapping");

    // Initialize GLEW.
    // Must be done after glut is initialized!
    GLenum res = glewInit();
    if (res != GLEW_OK) 
    {
        cerr << "GLEW initialization error: " << glewGetErrorString(res) << endl;
        return 1;
    }

    Start();

    return 0;
}
