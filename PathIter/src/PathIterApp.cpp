#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/Rand.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class PathIterApp : public App 
{
  public:
	void setup() override;
	void update() override;
	void draw() override;
	void generateTracers();

	struct PathTracer
	{
		PathTracer(const Path2d &aPath, float aDistance) : path(aPath), currDistance(aDistance)
		{
			lastPosition = path.getPosition(path.calcTimeForDistance(aDistance));
		}

		const Path2d &path;
		vec2 lastPosition;
		float currDistance;
	};

	Path2d mPath;
	vector<PathTracer> mTracers;
};

void PathIterApp::generateTracers()
{
	mPath.clear();
	mTracers.clear();

	auto center = getWindowCenter();
	mPath.moveTo(randVec2() + center);
	
	const size_t numTracers = 10;
	const uint32_t numPoints = 500;
	const float radius = 200.0f;

	for (uint32_t i = 0; i < numPoints; ++i)
	{
		mPath.lineTo(randVec2() * radius + center);
	}

	for (size_t i = 0; i < numTracers; ++i)
	{
		mTracers.emplace_back(mPath, randPosNegFloat(0.0f, mPath.calcLength()));
	}
}

void PathIterApp::setup()
{
	gl::enableAlphaBlending();
	generateTracers();

	// clear when space bar is pressed
	getWindow()->getSignalKeyDown().connect([this](KeyEvent event) { if(event.getCode() == KeyEvent::KEY_SPACE) generateTracers(); });
}

void PathIterApp::update()
{
}

void PathIterApp::draw()
{
	gl::clear(Color(1.0f, 0.98f, 0.98f)); 
	
	// draw the whole path
	gl::color(ColorA::gray(0.25f, 0.2f));
	gl::draw(mPath);
	
	// draw the tracers
	gl::color(ColorA::gray(0.25f, 0.8f));
	for (auto& tracer : mTracers)
	{
		vec2 currPosition = mPath.getPosition(mPath.calcTimeForDistance(tracer.currDistance));
		gl::drawLine(tracer.lastPosition, currPosition);

		// move forward along the path, wrap-around
		tracer.currDistance += 6.0f;
		tracer.lastPosition = currPosition;
	}
}

CINDER_APP(PathIterApp, RendererGl(RendererGl::Options().msaa(16)), [](App::Settings *settings) {
	settings->setWindowSize(512, 512);
})
