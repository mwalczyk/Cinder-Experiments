#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Query.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class QueryTimeApp : public App 
{
  public:
	void setup() override;
	void update() override;
	void draw() override;
	void mouseDown(MouseEvent event) override;

	bool mShouldQuery;
	gl::BatchRef mBatch;
	gl::QueryTimeSwappedRef mQuery;
	CameraPersp mCamera;
};

void QueryTimeApp::setup()
{
	gl::enableDepth();

	mShouldQuery = false;
	
	// setup the render batch
	auto geom = geom::Sphere().subdivisions(50).colors();
	auto glsl = gl::getStockShader(gl::ShaderDef().color());
	mBatch = gl::Batch::create(geom, glsl);

	// create an OpenGL query object
	mQuery = gl::QueryTimeSwapped::create();

	// setup the camera
	mCamera.lookAt(vec3(0.0f, 0.0f, -5.0f), vec3(0.0f));
	mCamera.setPerspective(45.0f, getWindowAspectRatio(), 0.1f, 100.0f);
}

void QueryTimeApp::mouseDown(MouseEvent event)
{
	mShouldQuery = true;
}

void QueryTimeApp::update()
{
}

void QueryTimeApp::draw()
{
	gl::clear(); 
	gl::setMatrices(mCamera);
	
	if (mShouldQuery)
	{
		mQuery->begin();	// begin timing
	}

	gl::ScopedModelMatrix scpModelMatrix;
	gl::rotate(getElapsedSeconds(), vec3(0.0f, 1.0f, 0.0f));
	mBatch->draw();

	if (mShouldQuery)
	{
		mQuery->end();		// end timing
		console() << "Time queried: " << mQuery->getElapsedMilliseconds() << endl;
		mShouldQuery = false;
	}
}

CINDER_APP( QueryTimeApp, RendererGl )
