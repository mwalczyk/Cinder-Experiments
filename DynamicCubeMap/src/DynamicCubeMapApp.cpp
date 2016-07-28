#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/CameraUi.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class DynamicCubeMapApp : public App 
{
public:
	void setup() override;
	void update() override;
	void draw() override;

protected:
	gl::BatchRef mSphereBatch;
	gl::FboCubeMapRef mFboCubeMap;
	CameraPersp mCamera;
	CameraUi mUi;
};

void DynamicCubeMapApp::setup()
{
	gl::enableDepth();

	// setup render geometry
	auto glsl = gl::getStockShader(gl::ShaderDef().lambert());
	mSphereBatch = gl::Batch::create(geom::Sphere().subdivisions(50), glsl);

	// setup camera and ui
	mCamera.lookAt(vec3(0.0f, 0.0f, -5.0f), vec3(0.0f));
	mCamera.setPerspective(45.0f, getWindowAspectRatio(), 0.1f, 1000.0f);
	mUi = CameraUi(&mCamera, getWindow());
}

void DynamicCubeMapApp::update()
{
}

void DynamicCubeMapApp::draw()
{
	gl::clear(); 
	gl::setMatrices(mCamera);
	mSphereBatch->draw();
}

CINDER_APP(DynamicCubeMapApp, RendererGl(RendererGl::Options().msaa(8)))
