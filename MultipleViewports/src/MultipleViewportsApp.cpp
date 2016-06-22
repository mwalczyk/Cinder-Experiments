#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Ubo.h"
#include "cinder/CameraUi.h"
#include "cinder/Rand.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class MultipleViewportsApp : public App 
{
  public:
	void setup() override;
	void mouseDown(MouseEvent event) override;
	void mouseDrag(MouseEvent event) override;
	void update() override;
	void draw() override;

	gl::BatchRef mRenderBatch;
	gl::UboRef mUbo;
	CameraPersp mCamera;
	CameraUi mCameraUi;
};

void MultipleViewportsApp::setup()
{
	gl::enableDepth();

	GLint maxViewports;
	glGetIntegerv(GL_MAX_VIEWPORTS, &maxViewports);
	console() << "Maximum number of viewports: " << maxViewports << std::endl;

	// set up render batch
	std::function<Colorf(vec3)> func = [](const vec3 &attrib) { return Colorf(attrib.x, attrib.y, attrib.z); };
	auto geom = geom::Cube().size(vec3(2.0f)) >> geom::ColorFromAttrib(geom::POSITION, func);

	auto format = gl::GlslProg::Format()
		.vertex(loadAsset("shader.vert"))
		.geometry(loadAsset("shader.geom"))
		.fragment(loadAsset("shader.frag"));
	auto glsl = gl::GlslProg::create(format);

	mRenderBatch = gl::Batch::create(geom, glsl);

	// set up the uniform buffer object with per-viewport transformation matrices
	const uint8_t kNumViewports = 4;
	vector<mat4> transformations(kNumViewports);
	std::generate(transformations.begin(), transformations.end(), []() { return glm::rotate(randPosNegFloat(0.0f, 360.0f), randVec3()); });
	mUbo = gl::Ubo::create(kNumViewports * sizeof(mat4), transformations.data(), GL_STATIC_DRAW);
	mUbo->bindBufferBase(0);

	// setup camera
	mCamera.lookAt(vec3(0.0f, 0.0f, -6.0f), vec3(0.0f));
	mCamera.setPerspective(45.0f, getWindowAspectRatio(), 0.1f, 1000.0f);
	mCameraUi = CameraUi(&mCamera);

	// set up the viewport array - the first argument is the index of the viewport, which we use in the geomtry shader
	glViewportIndexedf(0, 0.0f, 0.0f, getWindowWidth() * 0.5f, getWindowHeight() * 0.5f);											// top left
	glViewportIndexedf(1, getWindowWidth() * 0.5f, 0.0f, getWindowWidth() * 0.5f, getWindowHeight() * 0.5f);						// top right
	glViewportIndexedf(2, 0.0f, getWindowHeight() * 0.5f, getWindowWidth() * 0.5f, getWindowHeight() * 0.5f);						// bottom left
	glViewportIndexedf(3, getWindowWidth() * 0.5f, getWindowHeight() * 0.5f, getWindowWidth() * 0.5f, getWindowHeight() * 0.5f);	// bottom right
}

void MultipleViewportsApp::mouseDown(MouseEvent event)
{
	mCameraUi.mouseDown(event);
}

void MultipleViewportsApp::mouseDrag(MouseEvent event)
{
	mCameraUi.mouseDrag(event);
}

void MultipleViewportsApp::update()
{
}

void MultipleViewportsApp::draw()
{
	gl::clear(Color( 0.15f, 0.15f, 0.17f)); 
	gl::setMatrices(mCamera);
	mRenderBatch->draw();
}

CINDER_APP(MultipleViewportsApp, RendererGl(RendererGl::Options().msaa(8)), [](App::Settings *settings) {
	settings->setWindowSize(720, 720);
})
