#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/Url.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class LoadURLImageApp : public App 
{
  public:
	void setup() override;
	void update() override;
	void draw() override;

	gl::TextureRef mTexture;
	Url mUrl;
};

void LoadURLImageApp::setup()
{
	Url url("https://farm8.staticflickr.com/7391/26705133014_4b57ed90a1_b.jpg");
	mTexture = gl::Texture::create(loadImage(loadUrl(url)));
}

void LoadURLImageApp::update()
{
}

void LoadURLImageApp::draw()
{
	gl::clear(Color(0.0f, 0.0f, 0.0f)); 

	Rectf textureBounds = mTexture->getBounds();
	Rectf drawBounds = textureBounds.getCenteredFit(getWindowBounds(), true);

	gl::draw(mTexture, drawBounds);
}

CINDER_APP( LoadURLImageApp, RendererGl )
