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
	gl::BatchRef mRenderBatch;
	CameraPersp mCamera;
};

void FboSimpleApp::setup()
{
	// enable reading and writing to the depth buffer
	gl::enableDepth();

	// create the fbo
	mFbo = gl::Fbo::create(getWindowWidth(), getWindowHeight(), gl::Fbo::Format().colorTexture().samples(8));

	// create the cube render batch
	auto geom = geom::Cube();
	auto glsl = gl::getStockShader(gl::ShaderDef().texture().lambert());
	mRenderBatch = gl::Batch::create(geom, glsl);

	// setup the camera
	mCamera.lookAt(vec3(0.0f, 0.0f, -5.0f), vec3(0.0f));
	mCamera.setPerspective(45.0f, getWindowAspectRatio(), 0.1f, 1000.0f);
}

void FboSimpleApp::update()
{
	// draw into the fbo
	gl::ScopedFramebuffer scpFramebuffer(mFbo);
	gl::clear(Color(0.15f, 0.15f, 0.17f));
	gl::setMatricesWindow(getWindowSize());
	gl::drawSolidCircle(getWindowCenter(), 100.0f);
} 

void FboSimpleApp::draw()
{
	gl::clear();
	
	// draw the cube
	{
		gl::setMatrices(mCamera);
		gl::ScopedModelMatrix scpModelMatrix;
		gl::ScopedTextureBind scpTextureBind(mFbo->getColorTexture(), 0);
		gl::rotate(getElapsedSeconds(), vec3(0.0f, 1.0f, 0.0f));
		mRenderBatch->draw();
	}

	// draw the texture
	const float fboDrawScale = 0.25f;
	gl::setMatricesWindow(getWindowSize());
	gl::draw(mFbo->getColorTexture(), Rectf(0.0f, 0.0f, mFbo->getWidth() * fboDrawScale, mFbo->getHeight() * fboDrawScale));
}

CINDER_APP(FboSimpleApp, RendererGl, [](App::Settings *settings) {
	settings->setWindowSize(512, 512);
})
