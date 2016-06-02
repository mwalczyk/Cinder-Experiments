#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/CameraUi.h"
#include "cinder/BSpline.h"
#include "cinder/Rand.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class SplineExtrusionApp : public App 
{
  public:
	void setup() override;
	void mouseDown(MouseEvent event) override;
	void mouseDrag(MouseEvent event) override;
	void update() override;
	void draw() override;

	BSpline3f mBSpline;
	gl::BatchRef mExtrusionBatch;
	CameraPersp mCamera;
	CameraUi mCameraUi;
};

void SplineExtrusionApp::setup()
{
	gl::enableDepth();
	gl::enableAlphaBlending();

	mCamera.lookAt(vec3(0.0f, 0.0f, -5.0f), vec3(0.0f));
	mCamera.setPerspective(45.0f, getWindowAspectRatio(), 0.1f, 1000.0f);
	mCameraUi = CameraUi(&mCamera);

	// construct a random spline
	const float splineRadius = 2.0f;
	std::vector<vec3> points(400);
	std::generate(points.begin(), points.end(), [&]() { return randVec3() * splineRadius;  });
	mBSpline = BSpline3f(points, 5, true, false);
	
	auto program = gl::getStockShader(gl::ShaderDef().color());
	std::function<Colorf(vec3)> mapping = [&](vec3 position) ->Colorf { return Colorf::gray(splineRadius - length(position));  };

	mExtrusionBatch = gl::Batch::create(geom::BSpline(mBSpline, 5000) >> geom::ColorFromAttrib(geom::POSITION, mapping), program);
}

void SplineExtrusionApp::mouseDown(MouseEvent event)
{
	mCameraUi.mouseDown(event);
}

void SplineExtrusionApp::mouseDrag(MouseEvent event)
{
	mCameraUi.mouseDrag(event);
}

void SplineExtrusionApp::update()
{
}

void SplineExtrusionApp::draw()
{
	gl::clear(Color(1.0f, 0.98f, 0.98f)); 
	gl::setMatrices(mCamera);

	gl::color(ColorA::gray(0.25f, 0.8f));
	mExtrusionBatch->draw();
}

CINDER_APP(SplineExtrusionApp, RendererGl(RendererGl::Options().msaa(16)))
