#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/CameraUi.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class DepthOfFieldApp : public App 
{
  public:
	void setup() override;
	void mouseDown(MouseEvent event) override;
	void mouseDrag(MouseEvent event) override;
	void update() override;
	void draw() override;

	gl::FboRef mRenderFbo;
	gl::GlslProgRef mSATProg;
	gl::BatchRef mRenderBatch;
	CameraPersp mCamera;
	CameraUi mUi;
};

void DepthOfFieldApp::setup()
{
	gl::enableDepth();

	const int kSize = 512;

	// create the fbo that we will render the scene to
	auto colorTex = gl::Texture2d::create(kSize, kSize, gl::Texture2d::Format()
		.internalFormat(GL_RGBA32F));
	auto depthTex = gl::Texture2d::create(kSize, kSize, gl::Texture2d::Format()
		.internalFormat(GL_DEPTH_COMPONENT32F)
		.minFilter(GL_LINEAR)
		.magFilter(GL_LINEAR));

	auto renderFboFormat = gl::Fbo::Format()
		.attachment(GL_COLOR_ATTACHMENT0, colorTex)
		.attachment(GL_DEPTH_ATTACHMENT, depthTex);
	mRenderFbo = gl::Fbo::create(kSize, kSize, renderFboFormat);
	
	// load the compute shader for calculating the summed area table
	mSATProg = gl::GlslProg::create(gl::GlslProg::Format().compute(loadAsset("sat.glsl")));

	// setup the render batch
	mRenderBatch = gl::Batch::create(geom::Sphere().subdivisions(50).colors(), gl::getStockShader(gl::ShaderDef().color()));

	// setup the camera and camera ui controls
	mCamera.lookAt(vec3(2.0f, 0.0f, -5.0f), vec3(0.0f));
	mCamera.setPerspective(45.0f, getWindowAspectRatio(), 0.1f, 1000.0f);
	mUi = CameraUi(&mCamera);
}

void DepthOfFieldApp::mouseDown(MouseEvent event)
{
	mUi.mouseDown(event);
}

void DepthOfFieldApp::mouseDrag(MouseEvent event)
{
	mUi.mouseDrag(event);
}

void DepthOfFieldApp::update()
{
	gl::ScopedFramebuffer scpFramebuffer(mRenderFbo);
	gl::clear();
	gl::setMatrices(mCamera);
	
	const uint8_t kNumSpheres = 8;
	const float kDisplaceAmount = 3.0f;

	for (size_t i = 0; i < kNumSpheres; ++i)
	{
		gl::ScopedModelMatrix scpModelMatrix;
		float displacement = (i - kNumSpheres / 2.0f) * kDisplaceAmount;
		gl::translate(0.0f, 0.0f, displacement);
		mRenderBatch->draw();
	}
}

void DepthOfFieldApp::draw()
{
	gl::clear(); 
	gl::setMatricesWindow(getWindowSize());
	gl::draw(mRenderFbo->getTexture2d(GL_COLOR_ATTACHMENT0), mRenderFbo->getBounds());
}

CINDER_APP(DepthOfFieldApp, RendererGl(RendererGl::Options().msaa(16)), [](App::Settings *settings) {
	settings->setWindowSize(512, 512);
})
