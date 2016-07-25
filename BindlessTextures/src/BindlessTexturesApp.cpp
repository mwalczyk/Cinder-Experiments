#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Ubo.h"
#include "cinder/Rand.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class BindlessTexturesApp : public App 
{
  public:
	void setup() override;
	void mouseDown(MouseEvent event) override;
	void update() override;
	void draw() override;
	void cleanup() override;

	const size_t kNumTextures = 10;
	const int kTextureSize = 512;
	int mCurrIndex = 0;

	gl::BatchRef mRenderBatch;
	gl::UboRef mUbo;
	vector<gl::Texture2dRef> mTextures;
	vector<GLuint64> mTextureHandles;
	CameraPersp mCamera;
};

void BindlessTexturesApp::setup()
{
	gl::enableDepth();

	// setup the render batch
	auto glsl = gl::GlslProg::create(loadAsset("shader.vert"), loadAsset("shader.frag"));
	mRenderBatch = gl::Batch::create(geom::Sphere().subdivisions(50).colors(), glsl);

	// set up the bindless textures
	auto format = gl::Texture2d::Format().internalFormat(GL_RGB32F);

	for (size_t i = 0; i < kNumTextures; ++i)
	{	
		auto texture = gl::Texture2d::create(kTextureSize, kTextureSize, format);
		auto clearColor = randVec3() * 0.5f + 0.5f;
		
		// clear the current texture to a random color
		glClearTexSubImage(texture->getId(),
			0,				// level
			0, 0, 0,		// offsets	
			kTextureSize,	// width
			kTextureSize,	// height
			1,				// depth
			GL_RGB,			// format
			GL_FLOAT,		// type
			value_ptr(clearColor));
		
		console() << "Clear color: " << clearColor << endl;
		
		GLuint64 handle = glGetTextureHandleARB(texture->getId());
		glMakeTextureHandleResidentARB(handle);
		mTextures.push_back(texture);
		mTextureHandles.push_back(handle);
	}

	mUbo = gl::Ubo::create(sizeof(GLuint64) * mTextureHandles.size() * 2, mTextureHandles.data(), GL_STATIC_DRAW);
	mUbo->bindBufferBase(0);

	// setup the camera
	mCamera.lookAt(vec3(0.0f, 0.0f, -5.0f), vec3(0.0f));
	mCamera.setPerspective(45.0f, getWindowAspectRatio(), 0.1f, 1000.0f);
}

void BindlessTexturesApp::mouseDown(MouseEvent event)
{
	int nextIndex = (mCurrIndex + 1) % kNumTextures;
	mRenderBatch->getGlslProg()->uniform("uIndex", nextIndex);
	
	mCurrIndex = nextIndex;

	console() << "Current texture index: " << mCurrIndex << endl;
}

void BindlessTexturesApp::update()
{
}

void BindlessTexturesApp::draw()
{
	gl::clear(); 
	gl::setMatrices(mCamera);
	mRenderBatch->draw();
}

void BindlessTexturesApp::cleanup()
{
	for (const auto &handle : mTextureHandles)
	{
		glMakeTextureHandleNonResidentARB(handle);
	}
}

CINDER_APP(BindlessTexturesApp, RendererGl(RendererGl::Options().version(4,4)), [](App::Settings *settings) {
	settings->setWindowSize(512, 512);
})
