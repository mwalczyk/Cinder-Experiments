#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/Rand.h"
#include "cinder/Easing.h"
#include <numeric>

using namespace ci;
using namespace ci::app;
using namespace std;

struct Sortable
{
	float mRadius;
	vec2 mHomePos;
	vec2 mSortPos;
	Color mColor;
	int mNumSegments;
};

class SortInteractiveApp : public App 
{
public:
	void setup() override;
	void mouseDown(MouseEvent event) override;
	void update() override;
	void draw() override;

protected:
	void init();

	vector<Sortable> mSortables;

	std::function<float(float)> mEasingFcn;

	size_t mNumCircles = 300;
	const float kMinR = 1.0f;
	const float kMaxR = 10.0f;
	const int kPadding = 100;
};

void SortInteractiveApp::init()
{
	// generate kN random radii
	for (size_t i = 0; i < mNumCircles; ++i)
	{
		float rX = randFloat(kPadding, getWindowWidth() - kPadding);
		float rY = randFloat(kPadding, getWindowHeight() - kPadding);

		Sortable s;
		s.mRadius = randFloat(kMinR, kMaxR);
		s.mHomePos = s.mSortPos = vec2(rX, rY);
		s.mColor = Color(CM_HSV, randFloat(), 0.65f, 1.0f);
		s.mNumSegments = randInt(4, 32);

		mSortables.push_back(s);
	}

	// sort indices based on some attribute
	auto sortOnRadius = [&](const Sortable &lhs, const Sortable &rhs) { return lhs.mRadius < rhs.mRadius; };
	auto sortOnColor = [&](const Sortable &lhs, const Sortable &rhs) { return rgbToHsv(lhs.mColor).r < rgbToHsv(rhs.mColor).r; };
	auto sortOnSegments = [&](const Sortable &lhs, const Sortable &rhs) { return lhs.mNumSegments < rhs.mNumSegments; };

	sort(mSortables.begin(), mSortables.end(), sortOnColor);

	// set sorted positions
	vec2 center = getWindowCenter();
	const float angleRad = 200.0f;
	const float angleDiv = (M_PI * 2.0f) / mNumCircles;
	for (size_t i = 0; i < mNumCircles; ++i)
	{
		float fi = static_cast<float>(i) / mNumCircles;
		vec2 sorted = { 
			cosf(i * angleDiv) * angleRad + center.x,
			sinf(i * angleDiv) * angleRad + center.y
		};
		mSortables.at(i).mSortPos = sorted;
	}
}

void SortInteractiveApp::setup()
{
	init();
	mEasingFcn = EaseInOutElastic(3.0f, 2.0f);
}

void SortInteractiveApp::mouseDown(MouseEvent event)
{
}

void SortInteractiveApp::update()
{
}

void SortInteractiveApp::draw()
{
	gl::clear(Color(1.0f, 0.98f, 0.98f));
	gl::setMatricesWindow(getWindowSize());

	// draw the circles
	double time = mEasingFcn(sin(getElapsedSeconds()));
	for (const auto &s : mSortables)
	{
		gl::ScopedColor scpColor(s.mColor);
		vec2 curr = mix(s.mHomePos, s.mSortPos, time);
		gl::drawSolidCircle(curr, s.mRadius, s.mNumSegments);
	}
}

CINDER_APP(SortInteractiveApp, RendererGl(RendererGl::Options().msaa(8)), [](App::Settings *settings) {
	settings->setWindowSize(512, 512);
})
