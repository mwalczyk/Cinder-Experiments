#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/Rand.h"
#include "cinder/Easing.h"
#include <numeric>

using namespace ci;
using namespace ci::app;
using namespace std;

class SortCircleSizesApp : public App 
{
  public:
	void setup() override;
	void mouseDown(MouseEvent event) override;
	void update() override;
	void draw() override;

	vector<float> mCircleSizes;
	vector<vec2> mCirclePositions;
	vector<size_t> mOriginIds;
	vector<size_t> mSortedIds;

	std::function<float(float)> mEasingFcn;

	const size_t kN = 25;
	const float kMinR = 1.0f;
	const float kMaxR = 20.0f;
	const int kPadding = 100;
};

void SortCircleSizesApp::setup()
{
	// generate kN random radii
	mCircleSizes.resize(kN);
	std::generate(mCircleSizes.begin(), mCircleSizes.end(), [&]() { return randFloat(kMinR, kMaxR); });

	// place circles along a line
	int compWidth = getWindowWidth() - kPadding;
	vec2 center = getWindowCenter();
	vec2 bounds{center.x - compWidth * 0.5f, center.x + compWidth * 0.5f};

	for (size_t i = 0; i < kN; ++i)
	{	
		float fi = static_cast<float>(i) / kN;
		float newX = lerp(bounds[0], bounds[1], fi);
		float newY = center.y;
		mCirclePositions.push_back(vec2(newX, newY));
	}

	// initialize starting indices - 0, 1, 2, 3...
	mOriginIds.resize(kN);
	iota(mOriginIds.begin(), mOriginIds.end(), 0);
	
	// initialize sorted ids (we overwrite this below)
	mSortedIds = mOriginIds;

	// sort indices based radius 
	sort(mSortedIds.begin(), mSortedIds.end(), [&](size_t i1, size_t i2) {
		return mCircleSizes[i1] < mCircleSizes[i2];
	});

	// debug log
	console() << "Indices: ";
	for (const auto &i : mSortedIds)
	{
		console() << i << " ";
	}
	console() << endl;

	console() << "Radii: ";
	for (const auto &r : mCircleSizes)
	{
		console() << r << " ";
	}
	console() << endl;

	mEasingFcn = EaseInOutElastic(2.0f, 2.0f);
}

void SortCircleSizesApp::mouseDown(MouseEvent event)
{

}

void SortCircleSizesApp::update()
{
}

void SortCircleSizesApp::draw()
{
	gl::clear(Color(1.0f, 0.98f, 0.98f)); 
	gl::setMatricesWindow(getWindowSize());

	// draw the circles
	gl::ScopedColor scpColor(Color::gray(0.15f));
	double time = mEasingFcn(sin(getElapsedSeconds()));
	for (size_t i = 0; i < kN; ++i)
	{
		// the two ids of the current circle
		size_t originId = mOriginIds[i];
		size_t sortedId = mSortedIds[i];

		// locate the starting position of this circle as well as its sorted position - interpolate between them
		vec2 origin = mCirclePositions[originId];
		vec2 sorted = mCirclePositions[sortedId];
		vec2 curr = mix(origin, sorted, time);

		gl::drawSolidCircle(curr, mCircleSizes[sortedId]);
	}
}

CINDER_APP(SortCircleSizesApp, RendererGl(RendererGl::Options().msaa(8)), [](App::Settings *settings) {
	settings->setWindowSize(720, 200);
})
