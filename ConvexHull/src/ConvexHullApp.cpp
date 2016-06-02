#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/ConvexHull.h"
#include "cinder/Rand.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class ConvexHullApp : public App 
{
  public:
	void setup() override;
	void mouseDown(MouseEvent event) override;
	void update() override;
	void draw() override;

	PolyLine2f mPolyLine;
	vector<vec2> mPoints;
	float mDrawRadius;
};

void ConvexHullApp::setup()
{
	gl::enableAlphaBlending();

	mDrawRadius = 10.0f;
}

void ConvexHullApp::mouseDown(MouseEvent event)
{
	mPoints.emplace_back(event.getPos());
}

void ConvexHullApp::update()
{
	// add some points and increase the draw radius
	if (getElapsedFrames() % 4 == 0)
	{
		mPoints.emplace_back(randVec2() * mDrawRadius + getWindowCenter());
	}
	if (getElapsedFrames() % 30 == 0)
	{
		mDrawRadius += 8.0f;
	}

	mPolyLine = calcConvexHull(mPoints);
}

void ConvexHullApp::draw()
{
	gl::clear(Color(1.0f, 0.98f, 0.98f));
	gl::setMatricesWindow(getWindowSize());

	// draw convex hull
	gl::color(Color::gray(0.95f));
	gl::drawSolid(mPolyLine);

	// draw outline of convex hull
	gl::color(Color::gray(0.55f));
	gl::draw(mPolyLine);

	// draw points
	gl::color(ColorA::gray(0.25f, 0.5));
	for (const auto &pt : mPoints)
	{
		gl::drawSolidCircle(pt, 2.0f);
	}
}

CINDER_APP(ConvexHullApp, RendererGl(RendererGl::Options().msaa(16)), [](App::Settings *settings) {
	settings->setWindowSize(512, 512);
})
