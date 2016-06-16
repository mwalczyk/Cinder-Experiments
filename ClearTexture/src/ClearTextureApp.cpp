#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/Rand.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class ClearTextureApp : public App 
{
  public:
	void setup() override;
	void mouseDown(MouseEvent event) override;
	void update() override;
	void draw() override;

	gl::Texture2dRef mTexture;
};

void ClearTextureApp::setup()
{
	const int textureW = 256;
	const int textureH = 256;

	auto textureFormat = gl::Texture2d::Format().internalFormat(GL_RGBA32F);
	mTexture = gl::Texture2d::create(textureW, textureH, textureFormat);

	// cpu-side array of random color data
	int allocationSize = mTexture->getWidth() * mTexture->getHeight() * 4;
	vector<float> textureData(allocationSize);
	std::generate(textureData.begin(), textureData.end(), []() { return randFloat();  });

	// bind the texture and copy data to gpu
	gl::ScopedTextureBind scpTextureBind(mTexture);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, textureW, textureH, GL_RGBA, GL_FLOAT, textureData.data());
}

void ClearTextureApp::mouseDown(MouseEvent event)
{
}

void ClearTextureApp::update()
{
}

void ClearTextureApp::draw()
{
	gl::clear();
	gl::setMatricesWindow(getWindowSize());
	
	gl::draw(mTexture, mTexture->getBounds());
}

CINDER_APP( ClearTextureApp, RendererGl )
