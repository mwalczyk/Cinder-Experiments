#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/Perlin.h"
#include "cinder/Rand.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class TreeTracer
{
public:
	TreeTracer() :
		mHeight(-1.0f),
		mHeightIncr(0.01f),
		mNoiseOffsetX(randPosNegFloat(0.0f, 1000.0f)),
		mNoiseOffsetY(randPosNegFloat(0.0f, 1000.0f)),
		mNoiseAmplitude(randPosNegFloat(0.1f, 0.5f))
	{
	}

	static Perlin sPerlin;

	void update()
	{
		float x = sPerlin.noise(mHeight + mNoiseOffsetX) * mNoiseAmplitude;
		float y = sPerlin.noise(mHeight + mNoiseOffsetX * 2.0f) * mNoiseAmplitude;
		float z = sPerlin.noise(mHeight + mNoiseOffsetY) * mNoiseAmplitude;
		mCurrPos = vec3(x, y, z);
		mNoiseAmplitude = (sinf(getElapsedSeconds()) + 1.0f) * 3.0f;// powf(mHeightIncr, 2.0);
		mHeight += mHeightIncr;
		
	}

	void draw() const
	{
		gl::draw(mPolyLine);
	}

	vec3 pos() const
	{
		return mCurrPos;
	}

private:
	float mHeight;
	float mHeightIncr;
	float mNoiseOffsetX;
	float mNoiseOffsetY;
	float mNoiseAmplitude;
	vec3 mCurrPos;
	PolyLine3f mPolyLine;
};

Perlin TreeTracer::sPerlin = Perlin(4);

class CharcoalApp : public App 
{
  public:
	void setup() override;
	void update() override;
	void draw() override;

	Perlin mPerlin;
	gl::FboRef mCharcoalFbo;
	gl::FboRef mBlurFbo;
	gl::GlslProgRef mCharcoalProg;
	gl::BatchRef mSphereBatch;
	CameraPersp mCamera;

	vector<TreeTracer> mTracers;
};

void CharcoalApp::setup()
{
	gl::enableAlphaBlending();

	// set up fbo and shader
	mCharcoalFbo = gl::Fbo::create(getWindowWidth(), getWindowHeight(), gl::Fbo::Format().colorTexture());
	mBlurFbo = gl::Fbo::create(getWindowWidth() / 18, getWindowHeight() / 18, gl::Fbo::Format().colorTexture());
	mCharcoalProg = gl::GlslProg::create(gl::GlslProg::Format().vertex(loadAsset("shader.vert")).fragment(loadAsset("shader.frag")));

	// set up camera and render batch
	mSphereBatch = gl::Batch::create(geom::Sphere().subdivisions(50), gl::getStockShader(gl::ShaderDef().lambert()));
	mCamera.lookAt(vec3(0.0f, 2.0f, -14.0f), vec3(0.0));
	mCamera.setPerspective(45.0f, getWindowAspectRatio(), 0.1f, 1000.0f);

	// clear the fbo to an off-white color
	gl::ScopedFramebuffer scpFramebuffer(mCharcoalFbo);
	gl::clear(Color(1.0f, 0.98f, 0.98f));
	
	mTracers.resize(45);
}

void CharcoalApp::update()
{
	// draw into the fbo
	{
		gl::ScopedFramebuffer scpFramebuffer(mCharcoalFbo);
		gl::ScopedDepth scpDepth(true);
		gl::ScopedMatrices scpMatrices;
		gl::ScopedColor scpColor(ColorA::gray(0.15f, 0.2f));

		gl::setMatrices(mCamera);

		for (auto& tr : mTracers)
		{
			tr.update();
			gl::drawSphere(tr.pos(), 0.18f);
		}
	}
	{
		gl::ScopedFramebuffer scpFramebuffer(mBlurFbo);
		gl::ScopedViewport scpViewport(mBlurFbo->getSize());

		gl::setMatricesWindow(mBlurFbo->getSize());
		gl::draw(mCharcoalFbo->getColorTexture(), mBlurFbo->getBounds());
	}
}

void CharcoalApp::draw()
{
	gl::setMatricesWindow(getWindowSize());
	
	{
		gl::ScopedGlslProg scpGlslProg(mCharcoalProg);
		gl::ScopedTextureBind scpTextureBind(mCharcoalFbo->getColorTexture(), 0);
		mCharcoalProg->uniform("uTime", 0.0f);// static_cast<float>(getElapsedSeconds()));
		gl::drawSolidRect(getWindowBounds());
	}
	{
		gl::color(ColorA(1.0f, 1.0f, 1.0f, 0.6f));
		gl::draw(mBlurFbo->getColorTexture(), getWindowBounds());
	}
}

CINDER_APP(CharcoalApp, RendererGl(RendererGl::Options().msaa(16)))
