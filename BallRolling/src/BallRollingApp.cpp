#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/BSpline.h"
#include "cinder/CameraUi.h"
#include "cinder/Rand.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class BallRollingApp : public App 
{
  public:
	void setup() override;
	void mouseDown(MouseEvent event) override;
	void mouseDrag(MouseEvent event) override;
	void update() override;
	void draw() override;

	const float kBallRadius = 0.1f;
	const size_t kNumPoints = 50;
	const size_t kSplineSubdivisions = 500;
	const int kSphereSubdivisions = 50;

	gl::BatchRef mBallBatch;
	gl::BatchRef mSplineBatch;
	BSpline3f mBSpline;
	CameraPersp mCamera;
	CameraUi mUi;
	float mT;
	float mLastTime;
	vec3 mLastPosition;
	quat mQuat;
};

void BallRollingApp::setup()
{
	gl::enableDepth();

	// create a random b-spline
	vector<vec3> points;
	for (size_t i = 0; i < kNumPoints; ++i)
	{
		points.push_back(randVec3());
	}
	mBSpline = BSpline3f(points, 3, true, false);

	// set up render batches
	auto colorProgram = gl::getStockShader(gl::ShaderDef().color());
	mBallBatch = gl::Batch::create(geom::Sphere().subdivisions(kSphereSubdivisions).radius(kBallRadius).colors(), colorProgram);
	mSplineBatch = gl::Batch::create(geom::BSpline(mBSpline, kSplineSubdivisions), colorProgram);

	// setup camera
	mCamera.lookAt(vec3(0.0f, 0.0f, -5.0f), vec3(0.0f));
	mCamera.setPerspective(45.0f, getWindowAspectRatio(), 0.1f, 1000.0f);
	mUi = CameraUi(&mCamera);

	// setup spline params
	mT = 0.0f;
	mLastTime = getElapsedSeconds();
	mLastPosition = mBSpline.getPosition(mT);
	mQuat = quat();
}

void BallRollingApp::mouseDown(MouseEvent event)
{
	mUi.mouseDown(event);
}

void BallRollingApp::mouseDrag(MouseEvent event)
{
	mUi.mouseDrag(event);
}

void BallRollingApp::update()
{
	float elapsedTime = getElapsedSeconds() - mLastTime;
	mLastTime += elapsedTime;

	mT += elapsedTime * 0.05f;
}

void BallRollingApp::draw()
{
	gl::clear(Color(1.0f, 0.98f, 0.98f)); 
	gl::setMatrices(mCamera);

	// draw the path
	gl::color(Color::gray(0.2f));
	mSplineBatch->draw();

	// calculate the position and rotation of the ball
	vec3 position = mBSpline.getPosition(mT);
	vec3 delta = position - mLastPosition;
	vec3 normal = vec3(delta.z, 0.0f, -delta.x);
	float rotation = length(delta) / kBallRadius;

	// increment the quaternion
	quat incrQuat = angleAxis(rotation, normalize(normal));
	mQuat = incrQuat * mQuat;
	
	// draw the ball
	gl::ScopedModelMatrix scpModelMatrix;
	gl::translate(position);
	gl::rotate(mQuat);
	mBallBatch->draw();

	mLastPosition = position;
}

CINDER_APP(BallRollingApp, RendererGl(RendererGl::Options().msaa(16)), [](App::Settings *settings) {
	settings->setWindowSize(720, 720);
})
