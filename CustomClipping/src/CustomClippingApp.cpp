#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/CameraUi.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class CustomClippingApp : public App 
{
  public:
	void setup() override;
	void mouseDown(MouseEvent event) override;
	void mouseDrag(MouseEvent event) override;
	void update() override;
	void draw() override;

	CameraPersp mCamera;
	CameraUi mCameraUi;
	gl::BatchRef mClipSphereBatch;
	gl::BatchRef mWholeSphereBatch;
};

void CustomClippingApp::setup()
{
	gl::enableDepth();
	gl::enableAlphaBlending();
	gl::enable(GL_CLIP_DISTANCE0);

	GLint max;
	glGetIntegerv(GL_MAX_CLIP_DISTANCES, &max);
	console() << "Maxiumum user-defined clip distances: " << max << std::endl;

	mCamera.lookAt(vec3(0.0f, 0.0f, -5.0f), vec3(0.0f));
	mCamera.setPerspective(45.0f, getWindowAspectRatio(), 0.1f, 1000.0f);
	mCameraUi = CameraUi(&mCamera);

	auto geom = geom::Sphere().subdivisions(500).radius(0.8f);
	auto glsl = gl::GlslProg::create(gl::GlslProg::Format().vertex(loadAsset("shader.vert")).fragment(loadAsset("shader.frag")));
	mClipSphereBatch = gl::Batch::create(geom, glsl);

	mWholeSphereBatch = gl::Batch::create(geom::Sphere().subdivisions(50).radius(0.7f), gl::getStockShader(gl::ShaderDef().lambert()));
}

void CustomClippingApp::mouseDown(MouseEvent event)
{
	mCameraUi.mouseDown(event);
}

void CustomClippingApp::mouseDrag(MouseEvent event) 
{
	mCameraUi.mouseDrag(event);
}

void CustomClippingApp::update()
{
}

void CustomClippingApp::draw()
{
	gl::clear(Color(0.15f, 0.15f, 0.17f));
	gl::setMatrices(mCamera);
	mWholeSphereBatch->draw();
	
	const uint8_t kNumSpheres = 16;
	for (int n = 0; n < kNumSpheres; ++n)
	{
		float ratio = static_cast<float>(n) / kNumSpheres;
		vec3 scaleFactor = vec3(powf(ratio, 3.0) + 1.0f);

		gl::ScopedModelMatrix scpModelMatrix;
		gl::scale(scaleFactor);
		mClipSphereBatch->getGlslProg()->uniform("uLayer", n);
		mClipSphereBatch->draw();
	}
}

CINDER_APP(CustomClippingApp, RendererGl(RendererGl::Options().msaa(16)), [](App::Settings *settings) {
	settings->setWindowSize(512, 512);
})
