#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/CameraUi.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class MultipleRenderTargetsApp : public App 
{
  public:
	void setup() override;
	void update() override;
	void draw() override;
	void mouseDown(MouseEvent event) override;
	void mouseDrag(MouseEvent event) override;

	gl::FboRef mFbo;
	gl::BatchRef mRenderBatch;
	CameraPersp mCamera;
	CameraUi mCameraUi;
};

void MultipleRenderTargetsApp::setup()
{
	gl::enableDepth();
	gl::Texture2dRef colorTex0 = gl::Texture2d::create(getWindowWidth() / 2, getWindowHeight());
	gl::Texture2dRef colorTex1 = gl::Texture2d::create(getWindowWidth() / 2, getWindowHeight());

	auto fboFormat = gl::Fbo::Format()
		.attachment(GL_COLOR_ATTACHMENT0, colorTex0)
		.attachment(GL_COLOR_ATTACHMENT1, colorTex1);

	mFbo = gl::Fbo::create(getWindowWidth() / 2, getWindowHeight(), fboFormat);

	// vertex shader source code
	auto vSrc = CI_GLSL(420,
		uniform mat4 ciModelViewProjection;
		in vec4 ciPosition;
		void main()
		{
			gl_Position = ciModelViewProjection * ciPosition;
		});

	// fragment shader source code
	auto fSrc = CI_GLSL(420,
		layout(location = 0) out vec4 oColor0;
		layout(location = 1) out vec4 oColor1;
		void main()
		{
			oColor0 = vec4(1.0, 0.0, 0.0, 1.0);
			oColor1 = vec4(0.0, 1.0, 0.0, 1.0);
		});

	// set up the shader and render batch
	auto glslFormat = gl::GlslProg::Format().vertex(vSrc).fragment(fSrc);
	auto glsl = gl::GlslProg::create(glslFormat);
	mRenderBatch = gl::Batch::create(geom::Cube(), glsl);
	
	mCamera.setPerspective(60.0f, mFbo->getAspectRatio(), 0.1f, 1000.0f);
	mCamera.lookAt(vec3(0.0f, 0.0f, -5.0f), vec3(0.0f));
	mCameraUi = CameraUi(&mCamera);
}

void MultipleRenderTargetsApp::mouseDown(MouseEvent event)
{
	mCameraUi.mouseDown(event);
}

void MultipleRenderTargetsApp::mouseDrag(MouseEvent event)
{
	mCameraUi.mouseDrag(event);
}

void MultipleRenderTargetsApp::update()
{
	gl::ScopedFramebuffer scpFramebuffer(mFbo);
	gl::clear(Color::gray(0.15f));
	gl::ScopedViewport(ivec2(0), mFbo->getSize());
	gl::setMatrices(mCamera);
	mRenderBatch->draw();
}

void MultipleRenderTargetsApp::draw()
{
	gl::clear();
	gl::setMatricesWindow(getWindowSize());

	// draw both color attachments
	auto tex0 = mFbo->getTexture2d(GL_COLOR_ATTACHMENT0);
	auto tex1 = mFbo->getTexture2d(GL_COLOR_ATTACHMENT1);
	gl::draw(tex0, tex0->getBounds());
	gl::draw(tex1, tex1->getBounds() + ivec2(tex0->getWidth(), 0));
}

CINDER_APP(MultipleRenderTargetsApp, RendererGl, [](App::Settings *settings) {
	settings->setWindowSize(1024, 512);
})
