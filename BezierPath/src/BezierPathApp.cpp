#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/Path2d.h"
#include "cinder/Perlin.h"
#include "cinder/Rand.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class BezierPathApp : public App 
{
  public:
	void setup() override;
	void update() override;
	void draw() override;
	BezierPathApp() : mPerlin(2) {}

	Perlin mPerlin;
	CameraPersp mCamera;
	PolyLine3f mPolyline;
	const size_t mMaxPoints = 2000;
	const float mNoiseOffset = 1000.0f;
	const float mNoiseSpeed = 1.0f;
	gl::GlslProgRef mRenderProgram;
};

void BezierPathApp::setup()
{	
	mCamera.setPerspective(45.0f, getWindowAspectRatio(), 0.1f, 1000.0f);
	mCamera.lookAt(vec3(0.0f, 0.0f, -2.0f), vec3(0.0f));
	mPerlin.setSeed(10);
	mRenderProgram = gl::GlslProg::create(gl::GlslProg::Format().vertex(loadAsset("shader.vert")).fragment(loadAsset("shader.frag")));

	auto keyEvent = [this](KeyEvent event) { if (event.getCode() == KeyEvent::KEY_SPACE) mPolyline.getPoints().clear(); };
	getWindow()->getSignalKeyDown().connect(keyEvent);

	gl::enableAlphaBlending();
}

void BezierPathApp::update()
{
	if (mPolyline.size() > mMaxPoints)
	{
		mPolyline.getPoints().erase(mPolyline.getPoints().begin());
	}
	else
	{
		vec3 n = mPerlin.dfBm(getElapsedSeconds(), getElapsedSeconds() * mNoiseSpeed, getElapsedSeconds() * mNoiseSpeed * 2.0) * 0.4f;
		mPolyline.push_back(n + randVec3() * 0.001f);
	}
}

void BezierPathApp::draw()
{
	gl::clear(Color(1.0f, 0.97f, 0.97f));

	gl::ScopedGlslProg scpGlslProg(mRenderProgram);
	mRenderProgram->uniform("uTime", static_cast<float>(getElapsedSeconds()));

	gl::setMatrices(mCamera);
	gl::rotate(getElapsedSeconds() * 0.2f, vec3(0.0f, 1.0f, 0.0f));
	gl::draw(mPolyline);
}

CINDER_APP(BezierPathApp, RendererGl(RendererGl::Options().msaa(16)), [](App::Settings *settings) {
	settings->setWindowSize(512, 512);
})
