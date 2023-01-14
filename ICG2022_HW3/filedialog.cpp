#include "filedialog.h"
using namespace std;

// Create obj files dialog.
void FileDialog::OpenObjFiles()
{
    objFilePaths.clear();
    auto filePaths = pfd::open_file("Open OBJ files", "./",
        { "Obj Files (*.obj)", "*.obj" }, pfd::opt::multiselect);
    if (filePaths.result().empty())
    {
        cout << "Doesn't select any obj files" << endl;
        return;
    }

    objFilePaths = filePaths.result();
    cout << "Selected obj file paths: " << endl;
    for (string& filePath : objFilePaths)
        cout << filePath << endl;
    cout << endl;
}

// Create skybox texture files dialog.
void FileDialog::OpenSkyboxTexFiles()
{
    skyboxTexFilePath.clear();
    auto filePaths = pfd::open_file("Open skybox texture files", "./",
        { "Texture Files (*.png)", "*.png", "Texture Files (*.jpg)", "*.jpg" }, pfd::opt::none);
    if (filePaths.result().empty())
    {
        cout << "Doesn't select any texture files" << endl;
        return;
    }

    skyboxTexFilePath = (filePaths.result())[0];
    cout << "Selected skybox texture file path: " << skyboxTexFilePath << endl << endl;
}