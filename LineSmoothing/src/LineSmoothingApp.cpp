#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/CameraUi.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class LineSmoothingApp : public App 
{
  public:
	void setup() override;
	void mouseDown(MouseEvent event) override;
	void mouseDrag(MouseEvent event) override;
	void keyDown(KeyEvent event) override;
	void update() override;
	void draw() override;

	gl::BatchRef mBatch;
	CameraPersp mCamera;
	CameraUi mCameraUi;
	bool mSmoothingEnabled;
};

void LineSmoothingApp::setup()
{
	auto glsl = gl::getStockShader(gl::ShaderDef().color());
	auto geom = geom::Cube();
	mBatch = gl::Batch::create(geom, glsl);

	mCamera.lookAt(vec3(0.0f, 0.0f, -5.0f), vec3(0.0f));
	mCamera.setPerspective(45.0f, getWindowAspectRatio(), 0.1f, 100.0f);
	mCameraUi = CameraUi(&mCamera);

	mSmoothingEnabled = false;
}

void LineSmoothingApp::mouseDown(MouseEvent event)
{
	mCameraUi.mouseDown(event);
}

void LineSmoothingApp::mouseDrag(MouseEvent event)
{
	mCameraUi.mouseDrag(event);
}

void LineSmoothingApp::keyDown(KeyEvent event)
{
	if (event.getCode() == KeyEvent::KEY_SPACE)
	{
		mSmoothingEnabled = !mSmoothingEnabled;
		console() << "Space pressed!" << endl;
	}

	if (mSmoothingEnabled)
	{
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_LINE_SMOOTH);
		console() << "Smoothing enabled!" << endl;
	}
	else
	{
		glDisable(GL_BLEND);
		glDisable(GL_LINE_SMOOTH);
		console() << "Smoothing disabled!" << endl;
	}
}

void LineSmoothingApp::update()
{
}

void LineSmoothingApp::draw()
{
	gl::clear(); 
	gl::setMatrices(mCamera);
	gl::enableWireframe();
	mBatch->draw();
}

CINDER_APP( LineSmoothingApp, RendererGl )
