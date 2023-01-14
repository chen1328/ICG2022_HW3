#include "imagetexture.h"
using namespace std;

ImageTexture::ImageTexture(const string& filePath)
	: texFilePath(filePath)
{
	successLoaded = false;
	imageWidth = 0;
	imageHeight = 0;
	numChannels = 0;
	textureObj = 0;

	texImage = cv::imread(texFilePath);
	if (texImage.rows == 0 || texImage.cols == 0) 
	{
		cerr << "[ERROR] Failed to load image texture: " << filePath << endl;
		return;
	}
	successLoaded = true;
	imageWidth = texImage.cols;
	imageHeight = texImage.rows;
	numChannels = texImage.channels();

	cv::flip(texImage, texImage, 0);

	glGenTextures(1, &textureObj);
    glBindTexture(GL_TEXTURE_2D, textureObj);
	if(numChannels == 1)
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, imageWidth, imageHeight,
			0, GL_RED, GL_UNSIGNED_BYTE, texImage.ptr());
	else if(numChannels == 3)
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imageWidth, imageHeight,
			0, GL_BGR, GL_UNSIGNED_BYTE, texImage.ptr());
	else if(numChannels == 4)
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight,
			0, GL_BGRA, GL_UNSIGNED_BYTE, texImage.ptr());
	else
		cerr << "[ERROR] Unsupport texture format" << endl;

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	
	glGenerateMipmap(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, textureObj);
}

ImageTexture::~ImageTexture()
{
	glDeleteTextures(1, &textureObj);
	texImage.release();
}

void ImageTexture::Bind(GLenum textureUnit)
{
	glActiveTexture(textureUnit);
    glBindTexture(GL_TEXTURE_2D, textureObj);
}

void ImageTexture::Preview()
{
	string windowText = "[DEBUG] TexturePreview: " + texFilePath;
	cv::Mat previewImg = cv::Mat(texImage.rows, texImage.cols, texImage.type());
	cv::cvtColor(texImage, previewImg, cv::COLOR_BGR2RGB);
	cv::imshow(windowText, previewImg);
	cv::waitKey(0);
}

