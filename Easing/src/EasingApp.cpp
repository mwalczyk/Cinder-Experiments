#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/Easing.h"
#include "cinder/Rand.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class BounceBall
{
public:
	BounceBall(float aOffset, float aRadius, vec2 aBounceDirection) :
		mOffset(aOffset),
		mRadius(aRadius),
		mBounceDirection1(aBounceDirection),
		mBounceDirection2(randVec2()),
		mOrigin(getWindowCenter()),
		mFcn(EaseInElastic(2.0f, 1.0f)) { }

	void draw() const
	{
		float currT = (sinf(getElapsedSeconds() * 0.8f + mOffset) + 1.0f) * 0.5f;
		vec2 lerpDirection = mix(mBounceDirection1, mBounceDirection2, powf(currT, 3.0f));
		vec2 currPosition = mOrigin + lerpDirection * 150.0f * mFcn(currT);
		gl::drawSolidCircle(currPosition, mRadius * currT);
	}

private:
	float mOffset;
	float mRadius;
	vec2 mBounceDirection1;
	vec2 mBounceDirection2;
	vec2 mOrigin;
	Color mDrawColor;
	std::function<float(float)> mFcn;
};

class EasingApp : public App 
{
  public:
	EasingApp() : mNumBalls(200) {}
	void setup() override;
	void mouseDown(MouseEvent event) override;
	void update() override;
	void draw() override;

	vector<BounceBall> mBalls;
	const size_t mNumBalls;
};

void EasingApp::setup()
{
	const float minBallRadius = 2.0f;
	const float maxBallRadius = 10.0f;
	for (size_t i = 0; i < mNumBalls; ++i)
	{
		mBalls.emplace_back(i / static_cast<float>(mNumBalls), randFloat(minBallRadius, maxBallRadius), randVec2());
	}
}

void EasingApp::mouseDown(MouseEvent event)
{
}

void EasingApp::update()
{
}

void EasingApp::draw()
{
	gl::clear(Color(1.0f, 0.98f, 0.98f)); 
	gl::color(Color::gray(0.15f));
	for (const auto &ball : mBalls)
	{
		ball.draw();
	}
}

CINDER_APP(EasingApp, RendererGl(RendererGl::Options().msaa(16)), [](App::Settings *settings) {
	settings->setWindowSize(512, 512);
})
