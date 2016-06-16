#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class FboSimpleApp : public App 
{
  public:
	void setup() override;
	void update() override;
	void draw() override;

	gl::FboRef mFbo;
};

void FboSimpleApp::setup()
{
	mFbo = gl::Fbo::create(getWindowWidth(), getWindowHeight(), gl::Fbo::Format().colorTexture().samples(8));
}

void FboSimpleApp::update()
{
	// draw into the fbo
	gl::ScopedFramebuffer scpFramebuffer(mFbo);
	gl::setMatricesWindow(getWindowSize());
	gl::drawSolidCircle(getWindowCenter(), 100.0f);
} 

void FboSimpleApp::draw()
{
	gl::clear();
	
	// this...
	gl::ScopedGlslProg scpGlslProg(gl::getStockShader(gl::ShaderDef().texture()));
	gl::ScopedTextureBind scpTextureBind(mFbo->getColorTexture(), 0);
	gl::drawSolidRect(getWindowBounds());

	// or...
	gl::draw(mFbo->getColorTexture(), mFbo->getBounds());
}

CINDER_APP( FboSimpleApp, RendererGl )
