#ifndef IMAGE_TEXTURE_H
#define IMAGE_TEXTURE_H

#include "headers.h"
using namespace std;


// Texture Declarations.
class ImageTexture
{
public:
	// Texture Public Methods.
	ImageTexture(const string& filePath);
	~ImageTexture();

	bool GetSuccessLoaded() const { return successLoaded; }
	string GetPath() const { return texFilePath; }
	void Bind(GLenum textureUnit);
	void Preview();

private:
	// Texture Private Data.
	bool successLoaded;
	string texFilePath;
	GLuint textureObj;
	int imageWidth;
	int imageHeight;
	int numChannels;
	cv::Mat texImage;
};

#endif

