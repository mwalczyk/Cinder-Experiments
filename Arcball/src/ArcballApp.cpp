#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/Arcball.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class ArcBallApp : public App {
  public:
	void setup() override;
	void mouseDown(MouseEvent event) override;
	void mouseDrag(MouseEvent event) override;
	void update() override;
	void draw() override;
	
	Sphere mSphereArcball;
	gl::BatchRef mSphereBatch;
	Arcball mArcball;
	CameraPersp mCamera;
};

void ArcBallApp::setup()
{
	// enable reading and writing to the depth buffer
	gl::enableDepth();
	
	// set up rendering batch
	mSphereArcball = Sphere(vec3(0.0f), 1.0f);
	mSphereBatch = gl::Batch::create(geom::Sphere(mSphereArcball).colors().subdivisions(50), gl::getStockShader(gl::ShaderDef().color()));

	// set up camera and arcball
	mCamera.lookAt(vec3(0.0f, 0.0f, 5.0f), vec3(0.0f));
	mCamera.setPerspective(45.0f, getWindowAspectRatio(), 0.1f, 1000.0f);
	mArcball = Arcball(&mCamera, mSphereArcball);
}

void ArcBallApp::mouseDown( MouseEvent event )
{
	mArcball.mouseDown(event);
}

void ArcBallApp::mouseDrag(MouseEvent event)
{
	mArcball.mouseDrag(event);
}

void ArcBallApp::update()
{
}

void ArcBallApp::draw()
{
	// clear the background
	gl::clear(Color::gray(0.15f)); 

	// set view and projection matrices based on camera settings
	gl::setMatrices(mCamera);
	
	{	
		// rotate from arcball quaternion and draw
		gl::ScopedModelMatrix scpModelMatrix;
		gl::rotate(mArcball.getQuat());
		mSphereBatch->draw();
	}
}

CINDER_APP(ArcBallApp, RendererGl(RendererGl::Options().msaa(16)), [](App::Settings *settings) {
	settings->setWindowSize(512, 512);
})
