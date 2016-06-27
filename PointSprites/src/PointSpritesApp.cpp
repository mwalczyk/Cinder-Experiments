#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/Rand.h"
#include "cinder/CameraUi.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class PointSpritesApp : public App 
{
  public:
	void setup() override;
	void mouseDown(MouseEvent event) override;
	void mouseDrag(MouseEvent event) override;
	void update() override;
	void draw() override;

	gl::VertBatchRef mSpriteBatch;
	gl::GlslProgRef mSpriteProg;
	gl::Texture2dRef mSpriteTexture;
	CameraPersp mCamera;
	CameraUi mCameraUi;
};

void PointSpritesApp::setup()
{
	gl::enableDepth();
	gl::enableAlphaBlending();
	gl::pointSize(32.0f);

	// load the sprite texture
	mSpriteTexture = gl::Texture2d::create(loadImage(loadAsset("sprite.jpg")));

	const uint32_t kNumVerts = 500;

	// set up the vert batch
	mSpriteBatch = gl::VertBatch::create();
	mSpriteBatch->begin(GL_POINTS);
	for (size_t i = 0; i < kNumVerts; i++)
	{
		vec3 randPos = randVec3();
		mSpriteBatch->vertex(vec4(randPos, 1.0f));
	}
	mSpriteBatch->end();

	// set up the shader
	mSpriteProg = gl::GlslProg::create(loadAsset("ps.vert"), loadAsset("ps.frag"));

	// set up the camera
	mCamera.lookAt(vec3(0.0f, 0.0f, -5.0f), vec3(0.0f));
	mCamera.setPerspective(45.0f, getWindowAspectRatio(), 0.1f, 1000.0f);
	mCameraUi = CameraUi(&mCamera);
}

void PointSpritesApp::mouseDown(MouseEvent event)
{
	mCameraUi.mouseDown(event);
}

void PointSpritesApp::mouseDrag(MouseEvent event)
{
	mCameraUi.mouseDrag(event);
}

void PointSpritesApp::update()
{
}

void PointSpritesApp::draw()
{
	gl::clear(); 
	gl::setMatrices(mCamera);
	
	gl::ScopedTextureBind scpTextureBind(mSpriteTexture, 0);
	gl::ScopedGlslProg scpGlslProg(mSpriteProg);
	mSpriteBatch->draw();
}

CINDER_APP(PointSpritesApp, RendererGl, [](App::Settings *settings) {
	settings->setWindowSize(512, 512);
})
