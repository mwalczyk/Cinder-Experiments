#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/CameraUi.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class BoundingBoxApp : public App 
{
  public:
	void setup() override;
	void update() override;
	void draw() override;
	void mouseDown(MouseEvent event) override;
	void mouseDrag(MouseEvent event) override;

	CameraPersp mCamera;
	CameraUi mCameraUi;
	AxisAlignedBox mBoundingBox;
	gl::BatchRef mRenderBatch;
};

void BoundingBoxApp::setup()
{
	gl::enableDepth();

	// setup camera and ui
	mCamera.lookAt(vec3(0.0f, 0.0f, -5.0f), vec3(0.0f));
	mCamera.setPerspective(45.0f, getWindowAspectRatio(), 0.1f, 1000.0f);
	mCameraUi = CameraUi(&mCamera);

	// set up geometry and AABB
	auto geom = geom::Sphere().subdivisions(50).colors();
	TriMesh mesh(geom);

	// set up render batch
	mBoundingBox = mesh.calcBoundingBox();
	mRenderBatch = gl::Batch::create(mesh, gl::getStockShader(gl::ShaderDef().color().lambert()));
}

void BoundingBoxApp::mouseDown(MouseEvent event)
{
	mCameraUi.mouseDown(event);
}

void BoundingBoxApp::mouseDrag(MouseEvent event)
{
	mCameraUi.mouseDrag(event);
}

void BoundingBoxApp::update()
{
}

void BoundingBoxApp::draw()
{
	gl::clear(); 
	gl::setMatrices(mCamera);

	mRenderBatch->draw();

	gl::color(Color(1.0f, 0.0f, 0.0f));
	gl::drawStrokedCube(mBoundingBox);
}

CINDER_APP( BoundingBoxApp, RendererGl )
