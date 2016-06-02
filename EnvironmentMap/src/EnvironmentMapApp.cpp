#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/CameraUi.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class EnvironmentMapApp : public App 
{
  public:
	void setup() override;
	void mouseDown(MouseEvent event) override;
	void mouseDrag(MouseEvent event) override;
	void update() override;
	void draw() override;

	CameraPersp mCamera;
	CameraUi mCameraUi;
	gl::BatchRef mSphereBatch;
	gl::TextureCubeMapRef mCubeMap;
};

void EnvironmentMapApp::setup()
{
	gl::enableDepth();

	// set eye point and target
	mCamera.lookAt(vec3(0.0f, 0.0f, -2.0f), vec3(0.0f));
	mCamera.setPerspective(45.0f, getWindowAspectRatio(), 0.1f, 1000.0f);

	// pass the camera to the ui
	mCameraUi = CameraUi(&mCamera);

	// set up render batch
	auto envMapProg = gl::GlslProg::create(gl::GlslProg::Format().vertex(loadAsset("shader.vert")).fragment(loadAsset("shader.frag")));
	mSphereBatch = gl::Batch::create(geom::Sphere().radius(0.4f).subdivisions(50).colors(), envMapProg);

	// load cube map
	mCubeMap = gl::TextureCubeMap::create(loadImage(loadAsset("humus_sf.jpg")));
}

void EnvironmentMapApp::mouseDown(MouseEvent event)
{
	mCameraUi.mouseDown(event);
}

void EnvironmentMapApp::mouseDrag(MouseEvent event) 
{
	mCameraUi.mouseDrag(event);
}

void EnvironmentMapApp::update()
{
}

void EnvironmentMapApp::draw()
{
	gl::clear(Color::gray(0.25f)); 

	// draw batch after binding the environemnt map to the first texture unit
	gl::setMatrices(mCamera);
	gl::ScopedTextureBind scpTextureBind(mCubeMap, 0);
	mSphereBatch->draw();

	// draw cube map
	gl::setMatricesWindow(getWindowSize());
	gl::drawHorizontalCross(mCubeMap, mCubeMap->getBounds());
}

CINDER_APP(EnvironmentMapApp, RendererGl(RendererGl::Options().msaa(16)))
