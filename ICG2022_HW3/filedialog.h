#ifndef FILE_DIALOG_H
#define FILE_DIALOG_H

#include "headers.h"
using namespace std;


// FileDialog Declarations.
class FileDialog
{
public:
	// FileDialog Public Methods.
	FileDialog() { skyboxTexFilePath = "textures/photostudio_02_2k.png"; }
	~FileDialog() { objFilePaths.clear(); }

	vector<string>& GetObjFilePaths() { return objFilePaths; }
	string GetSkyboxTexFilePath() const { return skyboxTexFilePath; }
	void OpenObjFiles();
	void OpenSkyboxTexFiles();

private:
	// FileDialog Private data.
	vector<string> objFilePaths;
	string skyboxTexFilePath;
};

#endif
