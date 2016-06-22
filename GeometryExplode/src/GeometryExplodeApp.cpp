#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/CameraUi.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class GeometryExplodeApp : public App 
{
  public:
	void setup() override;
	void update() override;
	void draw() override;
	void mouseDown(MouseEvent event) override;
	void mouseDrag(MouseEvent event) override;

	gl::BatchRef mRenderBatch;
	CameraPersp mCamera;
	CameraUi mCameraUi;
};

void GeometryExplodeApp::setup()
{
	gl::enableDepth();
	gl::enableAlphaBlending();

	// unlike the other shader stages, geometry shaders can generate vertices
	GLint maxOutputVertices;
	glGetIntegerv(GL_MAX_GEOMETRY_OUTPUT_VERTICES, &maxOutputVertices);
	console() << "Maximum output vertices from geomtry shader: " << maxOutputVertices << endl;

	auto geom = geom::Sphere()
		.subdivisions(25)
		.colors();

	// load shaders
	try 
	{
		auto format = gl::GlslProg::Format()
			.vertex(loadAsset("shader.vert"))
			.geometry(loadAsset("shader.geom"))
			.fragment(loadAsset("shader.frag"));
		auto glsl = gl::GlslProg::create(format);
		mRenderBatch = gl::Batch::create(geom, glsl);
	}
	catch (gl::GlslProgCompileExc ex) 
	{
		cout << ex.what() << endl;
		quit();
	}

	// setup camera
	mCamera.lookAt(vec3(0.0f, 0.0f, -5.0f), vec3(0.0f));
	mCamera.setPerspective(45.0f, getWindowAspectRatio(), 0.1f, 1000.0f);
	mCameraUi = CameraUi(&mCamera);
}

void GeometryExplodeApp::mouseDown(MouseEvent event)
{
	mCameraUi.mouseDown(event);
}

void GeometryExplodeApp::mouseDrag(MouseEvent event)
{
	mCameraUi.mouseDrag(event);
}

void GeometryExplodeApp::update()
{
}

void GeometryExplodeApp::draw()
{
	// set global draw state
	gl::clear(Color(0.15f, 0.15f, 0.17f));
	gl::setMatrices(mCamera);

	// draw the batch
	gl::ScopedModelMatrix scpModelMatrix;
	gl::rotate(getElapsedSeconds() * 0.5f, vec3(0.0f, 1.0f, 0.0f));
	mRenderBatch->draw();
}

CINDER_APP(GeometryExplodeApp, RendererGl(RendererGl::Options().msaa(8)), [](App::Settings *settings) {
	settings->setWindowSize(512, 512);
})
