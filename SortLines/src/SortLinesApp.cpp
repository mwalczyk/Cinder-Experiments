#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/Rand.h"
#include "cinder/Easing.h"

using namespace ci;
using namespace ci::app;
using namespace std;

struct Sortable
{
	pair<vec2, vec2> mHomePos;
	pair<vec2, vec2> mSortPos;
};

class SortLinesApp : public App 
{
public:
	void setup() override;
	void update() override;
	void draw() override;

protected:
	vector<Sortable> mSortables;
	std::function<float(float)> mEasingFcn;
	const size_t mNumLines = 200;
	const int kPadding = 100;
};

void SortLinesApp::setup()
{
	vec2 center = getWindowCenter();

	// generate kN line segments
	for (size_t i = 0; i < mNumLines; ++i)
	{
		vec2 rPt0 = randVec2() * vec2(getWindowSize()) * 0.5f + center;
		vec2 rPt1 = randVec2() * vec2(getWindowSize()) * 0.5f + center;

		Sortable s;
		s.mHomePos = make_pair(rPt0, rPt1);

		mSortables.push_back(s);
	}

	auto sortOnLength = [&](const Sortable &lhs, const Sortable &rhs) { return distance(lhs.mHomePos.first, lhs.mHomePos.second) < distance(rhs.mHomePos.first, rhs.mHomePos.second); };
	sort(mSortables.begin(), mSortables.end(), sortOnLength);

	// set sorted positions
	for (size_t i = 0; i < mNumLines; ++i)
	{
		float fi = static_cast<float>(i) / mNumLines;
		float length = distance(mSortables.at(i).mHomePos.first, mSortables.at(i).mHomePos.second);
		vec2 sPt0 = { fi * getWindowWidth(), getWindowHeight() * 0.5f + length * 0.5f };
		vec2 sPt1 = { fi * getWindowWidth(), getWindowHeight() * 0.5f - length * 0.5f };
		mSortables.at(i).mSortPos = make_pair(sPt0, sPt1);
	}

	mEasingFcn = EaseInOutElastic(1.4f, 2.0f);
}

void SortLinesApp::update()
{
}

void SortLinesApp::draw()
{
	gl::clear(Color(1.0f, 0.98f, 0.98f));
	gl::setMatricesWindow(getWindowSize());

	// draw the lines
	gl::ScopedColor scpColor(Color::gray(0.15f));
	double time = mEasingFcn(sin(getElapsedSeconds()));
	for (const auto &s : mSortables)
	{
		vec2 cPt0 = mix(s.mHomePos.first, s.mSortPos.first, time);
		vec2 cPt1 = mix(s.mHomePos.second, s.mSortPos.second, time);
		gl::drawLine(cPt0, cPt1);
	}
}


CINDER_APP(SortLinesApp, RendererGl(RendererGl::Options().msaa(16)), [](App::Settings *settings) {
	settings->setWindowSize(512, 512);
})