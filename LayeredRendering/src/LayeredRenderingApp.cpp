#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Ubo.h"
#include "cinder/Utilities.h"
#include "cinder/Rand.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class LayeredRenderingApp : public App 
{
  public:
	void setup() override;
	void keyDown(KeyEvent event) override;
	void update() override;
	void draw() override;

	gl::FboRef mFbo;
	gl::BatchRef mBatch;
	gl::UboRef mUbo;
	gl::GlslProgRef mLayerProg;
	CameraPersp mCamera;
	int mCurrLayer;
};

void LayeredRenderingApp::setup()
{
	const int kFboWidth = 512;
	const int kFboHeight = 512;
	const int kNumLayers = 16;

	auto texArray = gl::Texture3d::create(kFboWidth, kFboHeight, kNumLayers, gl::Texture3d::Format().target(GL_TEXTURE_2D_ARRAY));
	auto depthArray = gl::Texture3d::create(kFboWidth, kFboHeight, kNumLayers, gl::Texture3d::Format().target(GL_TEXTURE_2D_ARRAY).internalFormat(GL_DEPTH_COMPONENT));
	auto fboFormat = gl::Fbo::Format().attachment(GL_COLOR_ATTACHMENT0, texArray).attachment(GL_DEPTH_ATTACHMENT, depthArray);
	
	mFbo = gl::Fbo::create(kFboWidth, kFboHeight, fboFormat);

	// set up the geometry
	std::function<Colorf(vec3)> func = [](const vec3 &attrib) { return Colorf(attrib.x, attrib.y, attrib.z) * 0.5f + 0.5f; };
	auto geom = geom::Cube() >> geom::ColorFromAttrib(geom::POSITION, func);
	
	// set up the glsl program
	auto glslFormat = gl::GlslProg::Format()
		.vertex(loadAsset("shader.vert"))
		.geometry(loadAsset("shader.geom"))
		.fragment(loadAsset("shader.frag"));
	auto glsl = gl::GlslProg::create(glslFormat);

	mBatch = gl::Batch::create(geom, glsl);

	// set up the uniform buffer object with per-viewport transformation matrices
	vector<mat4> transformations(kNumLayers);
	std::generate(transformations.begin(), transformations.end(), []() { return glm::rotate(randPosNegFloat(0.0f, 360.0f), randVec3()); });
	mUbo = gl::Ubo::create(kNumLayers * sizeof(mat4), transformations.data(), GL_STATIC_DRAW);
	mUbo->bindBufferBase(0);

	// set up the shader program for render a particular layer of the texture array
	mLayerProg = gl::GlslProg::create(gl::GlslProg::Format().vertex(loadAsset("layer.vert")).fragment(loadAsset("layer.frag")));

	// set up the camera
	mCamera.lookAt(vec3(0.0f, 0.0f, -5.0f), vec3(0.0f));
	mCamera.setPerspective(45.0f, getWindowAspectRatio(), 0.1f, 100.0f);

	mCurrLayer = 0;
}

void LayeredRenderingApp::keyDown(KeyEvent event)
{
	if (event.getCode() == KeyEvent::KEY_LEFT)
	{
		mCurrLayer = (mCurrLayer > 0) ? mCurrLayer - 1 : 0;
	}
	else if (event.getCode() == KeyEvent::KEY_RIGHT)
	{
		mCurrLayer = (mCurrLayer < 16) ? mCurrLayer + 1 : 15;
	}
}

void LayeredRenderingApp::update()
{
	gl::ScopedFramebuffer scpFramebuffer(mFbo);
	gl::ScopedDepth scpDepth(true);
	gl::clear();
	gl::setMatrices(mCamera);
	mBatch->draw();
}

void LayeredRenderingApp::draw()
{
	gl::clear(); 
	gl::setMatricesWindow(getWindowSize());
	
	{
		gl::ScopedTextureBind scpTextureBind(mFbo->getTextureBase(GL_COLOR_ATTACHMENT0), 0);
		gl::ScopedGlslProg scpGlslProg(mLayerProg);
		gl::setDefaultShaderVars();
		mLayerProg->uniform("uLayerIndex", mCurrLayer);
		gl::drawSolidRect(getWindowBounds());
	}

	gl::drawString("Current Layer: " + toString(mCurrLayer), vec2(10.0f));
}

CINDER_APP(LayeredRenderingApp, RendererGl, [](App::Settings *settings) {
	settings->setWindowSize(512, 512);
})
