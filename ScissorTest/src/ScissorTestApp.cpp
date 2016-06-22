#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/CameraUi.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class ScissorTestApp : public App 
{
  public:
	void setup() override;
	void mouseDown(MouseEvent event) override;
	void mouseDrag(MouseEvent event) override;
	void mouseUp(MouseEvent event) override;
	void update() override;
	void draw() override;

	gl::BatchRef mRenderBatch;
	CameraPersp mCamera;
	CameraUi mCameraUi;
	pair<ivec2, ivec2> mScissorArea;
};

void ScissorTestApp::setup()
{
	gl::enableDepth();

	std::function<Colorf(vec3)> func = [](const vec3 &attrib) { return Colorf(attrib.x, attrib.y, attrib.z) * 0.5f + 0.5f; };
	auto geom = geom::Cube() >> geom::ColorFromAttrib(geom::POSITION, func);
	auto glsl = gl::getStockShader(gl::ShaderDef().color());

	mRenderBatch = gl::Batch::create(geom, glsl);

	mCamera.lookAt(vec3(0.0f, 0.0f, -5.0f), vec3(0.0f));
	mCamera.setPerspective(45.0f, getWindowAspectRatio(), 0.1f, 1000.0f);

	mScissorArea = make_pair(ivec2(0), getWindowCenter());
}

void ScissorTestApp::mouseDown(MouseEvent event)
{
	mScissorArea.first = event.getPos();
	mScissorArea.second = event.getPos();
}

void ScissorTestApp::mouseDrag(MouseEvent event)
{
	mScissorArea.second = event.getPos();
}

void ScissorTestApp::mouseUp(MouseEvent event)
{
	mScissorArea.second = event.getPos();
}

void ScissorTestApp::update()
{
}

void ScissorTestApp::draw()
{
	gl::clear(Color(0.15f, 0.15f, 0.17f)); 
	
	{
		gl::ScopedMatrices scpMatrices;
		ivec2 lowerLeft = ivec2(mScissorArea.first.x, getWindowHeight() - mScissorArea.first.y - mScissorArea.second.y);
		gl::ScopedScissor scpScissor(lowerLeft, mScissorArea.second);
		gl::setMatrices(mCamera);
		mRenderBatch->draw();
	}

	{
		gl::ScopedColor scpColor(Color(1.0f, 0.0f, 0.0f));
		gl::ScopedMatrices scpMatrices;
		gl::setMatricesWindow(getWindowSize());
		gl::drawStrokedRect(Rectf(mScissorArea.first, mScissorArea.second));
	}
}

CINDER_APP(ScissorTestApp, RendererGl)
