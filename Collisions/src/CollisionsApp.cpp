#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "Box2D/Box2D.h"
#include "cinder/Rand.h"

using namespace ci;
using namespace ci::app;
using namespace std;

struct Box {
	Box(shared_ptr<b2World> aWorldPtr, vec2 aPosition) :
		mWorldPtr(aWorldPtr),
		mSize(8.0f)
	{
		// body def
		b2BodyDef bodyDef;
		bodyDef.position.Set(aPosition.x, aPosition.y);
		bodyDef.linearVelocity = b2Vec2(0.0f, randFloat(0.0f, 50.0f));
		bodyDef.type = b2_dynamicBody;
		bodyDef.active = true;

		// create body from body def
		mBody = mWorldPtr->CreateBody(&bodyDef);

		// shape
		b2PolygonShape dynamicBox;
		dynamicBox.SetAsBox(mSize.x, mSize.y);

		// fixture
		b2FixtureDef fixtureDef;
		fixtureDef.shape = &dynamicBox;
		fixtureDef.density = 1.0f;
		fixtureDef.friction = 1.0f;
		fixtureDef.restitution = 0.4f;

		// attach fixture to body
		mBody->CreateFixture(&fixtureDef);
	}

	bool isInsideWindow() const
	{
		return getWindowBounds().contains(mCurrentPosition);
	}

	shared_ptr<b2World> mWorldPtr;
	vec2 mCurrentPosition;
	vec2 mSize;
	b2Body *mBody;

	void draw() const
	{
		float a = mBody->GetAngle();
		b2Vec2 p = mBody->GetPosition();
		gl::ScopedModelMatrix scpModelMatrix;
		gl::translate(p.x, p.y);
		gl::rotate(a);
		gl::drawSolidRect(Rectf(-mSize.x, -mSize.x, mSize.x, mSize.x));
	}
};

class CollisionsApp : public App 
{
  public:
	void setup() override;
	void mouseDown(MouseEvent event) override;
	void update() override;
	void draw() override;

	vector<Box> mBoxes;
	shared_ptr<b2World> mWorldRef;
	b2Body *mColliderBody;
	double mLastTime;
};

void CollisionsApp::setup()
{
	b2Vec2 gravity(0, 10);
	mWorldRef = make_shared<b2World>(gravity);

	// initialize collider
	b2BodyDef colliderDef;
	colliderDef.type = b2_staticBody;
	colliderDef.active = true;
	colliderDef.position = b2Vec2(getWindowWidth() * 0.5f, getWindowHeight() * 0.5f);

	// create body from definition
	mColliderBody = mWorldRef->CreateBody(&colliderDef);
	
	// shape
	b2PolygonShape colliderBox;
	colliderBox.SetAsBox(100.0f, 100.0f);
	
	// fixture
	b2FixtureDef colliderFixture;
	colliderFixture.shape = &colliderBox;
	colliderFixture.density = 1.0f;
	colliderFixture.friction = 0.3f;

	mColliderBody->CreateFixture(&colliderFixture);
	
	mLastTime = 0.0;
}

void CollisionsApp::mouseDown(MouseEvent event)
{
	mBoxes.emplace_back(mWorldRef, event.getPos());
	console() << "Size: " << mBoxes.size() << std::endl;
}

void CollisionsApp::update()
{
	// incr time and move the simulation forward
	double currentTime = getElapsedSeconds();
	double timeStep = currentTime - mLastTime;
	mLastTime = currentTime;
	mWorldRef->Step(timeStep, 8, 3);
}

void CollisionsApp::draw()
{
	gl::setMatricesWindow(getWindowSize());
	gl::clear(Color(1.0f, 0.98f, 0.98f)); 

	{
		gl::ScopedModelMatrix scpModelMatrix;
		gl::ScopedColor scpColor(Color::gray(0.25f));
		b2Vec2 colliderPosition = mColliderBody->GetPosition();
		gl::translate(colliderPosition.x, colliderPosition.y);
		gl::drawSolidRect(Rectf(-100.0f, -100.0f, 100.0f, 100.0f));
	}
	
	gl::color(Color::gray(0.15f));
	for (const auto& box : mBoxes)
	{
		box.draw();
	}
}

CINDER_APP( CollisionsApp, RendererGl )
