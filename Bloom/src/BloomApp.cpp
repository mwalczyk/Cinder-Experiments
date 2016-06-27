#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/CameraUi.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class BloomApp : public App 
{
  public:
	void setup() override;
	void update() override;
	void draw() override;
	void mouseDown(MouseEvent event) override;
	void mouseDrag(MouseEvent event) override;

	gl::FboRef mSceneFbo;
	array<gl::FboRef, 2> mBlurFbos;

	gl::BatchRef mSceneBatch;
	gl::BatchRef mBlurBatch;
	gl::BatchRef mResolveBatch;

	CameraPersp mSceneCamera;
	CameraUi mSceneUi;
};

void BloomApp::setup()
{
	// set up the floating-point textures
	auto texRef0 = gl::Texture2d::create(512, 512, gl::Texture2d::Format().internalFormat(GL_RGBA16F));	
	auto texRef1 = gl::Texture2d::create(512, 512, gl::Texture2d::Format().internalFormat(GL_RGBA16F));

	// set up the scene fbo with two floating-point texture attachments
	auto sceneFboFormat = gl::Fbo::Format()
		.attachment(GL_COLOR_ATTACHMENT0, texRef0)			// scene
		.attachment(GL_COLOR_ATTACHMENT1, texRef1);			// luminance
	mSceneFbo = gl::Fbo::create(512, 512, sceneFboFormat);

	// set up the blur fbo with one floating-point texture attachment
	for (size_t i = 0; i < mBlurFbos.size(); ++i)
	{
		auto blurTexRef = gl::Texture2d::create(512, 512, gl::Texture2d::Format().internalFormat(GL_RGBA16F));
		auto blurFboFormat = gl::Fbo::Format()
			.attachment(GL_COLOR_ATTACHMENT0, blurTexRef);
		mBlurFbos[i] = gl::Fbo::create(512, 512, blurFboFormat);
	}

	// set up the render batches
	auto sceneGeom = geom::Sphere().subdivisions(50);
	auto sceneGlsl = gl::GlslProg::create(loadAsset("bloom.vert"), loadAsset("bloom.frag"));
	mSceneBatch = gl::Batch::create(sceneGeom, sceneGlsl);
	
	auto fsQuad = geom::Rect().texCoords(vec2(0.0f, 0.0f), vec2(1.0f, 0.0f), vec2(1.0f, 1.0f), vec2(0.0f, 1.0f));

	auto blurGlsl = gl::GlslProg::create(loadAsset("blur.vert"), loadAsset("blur.frag"));
	mBlurBatch = gl::Batch::create(fsQuad, blurGlsl);

	auto resolveGlsl = gl::GlslProg::create(loadAsset("res.vert"), loadAsset("res.frag"));
	mResolveBatch = gl::Batch::create(fsQuad, resolveGlsl);

	// set up the camera
	mSceneCamera.lookAt(vec3(0.0f, 0.0f, -5.0f), vec3(0.0f));
	mSceneCamera.setPerspective(45.0f, getWindowAspectRatio(), 0.1f, 1000.0f);
	mSceneUi = CameraUi(&mSceneCamera);
}

void BloomApp::update()
{
	{
		// enable depth testing and draw the scene into the fbo
		gl::ScopedDepth scpDepth(true);
		gl::ScopedFramebuffer scpFramebuffer(mSceneFbo);
		gl::ScopedMatrices scpMatrices;
		gl::clear();
		gl::setMatrices(mSceneCamera);
		mSceneBatch->draw();
	}

	// prepare matrices for full-screen quad
	gl::ScopedModelMatrix scpModelMatrix;
	gl::translate(getWindowCenter());
	gl::scale(getWindowSize());

	{
		// blur the luminance pass from the last stage
		gl::ScopedTextureBind scpTextureBind(mSceneFbo->getTexture2d(GL_COLOR_ATTACHMENT1), 0);
		gl::ScopedFramebuffer scpFramebuffer(mBlurFbos[0]);
		gl::clear();
		mBlurBatch->draw();
	}

	{
		// blur in the opposite direction
		gl::ScopedTextureBind scpTextureBind(mBlurFbos[0]->getTexture2d(GL_COLOR_ATTACHMENT0), 0);
		gl::ScopedFramebuffer scpFramebuffer(mBlurFbos[1]);
		gl::clear();
		mBlurBatch->draw();
	}
}

void BloomApp::mouseDown(MouseEvent event)
{
	mSceneUi.mouseDown(event);
}

void BloomApp::mouseDrag(MouseEvent event)
{
	mSceneUi.mouseDrag(event);
}

void BloomApp::draw()
{
	gl::clear(); 
	
	gl::ScopedModelMatrix scpModelMatrix;
	gl::translate(getWindowCenter());
	gl::scale(getWindowSize());

	gl::ScopedTextureBind scpTextureBind0(mSceneFbo->getTexture2d(GL_COLOR_ATTACHMENT0), 0);
	gl::ScopedTextureBind scpTextureBind1(mBlurFbos[1]->getTexture2d(GL_COLOR_ATTACHMENT0), 1);
	mResolveBatch->draw();
}

CINDER_APP(BloomApp, RendererGl, [](App::Settings *settings) {
	settings->setWindowSize(512, 512);
})
