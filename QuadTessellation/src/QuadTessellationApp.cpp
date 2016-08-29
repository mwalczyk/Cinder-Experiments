#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/CameraUi.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class QuadTessellationApp : public App 
{
  public:
	void setup() override;
	void update() override;
	void draw() override;
	void mouseDown(MouseEvent event) override;
	void mouseDrag(MouseEvent event) override;

	gl::GlslProgRef mTessellationProg;

	CameraPersp mCamera;
	CameraUi mCameraUi;

	bool mShowWireframe;
};

void QuadTessellationApp::setup()
{
	gl::enableDepth();

	auto format = gl::GlslProg::Format().vertex(loadAsset("vert.glsl"))
		.tessellationCtrl(loadAsset("tcs.glsl"))
		.tessellationEval(loadAsset("tes.glsl"))
		.fragment(loadAsset("frag.glsl"));

	mTessellationProg = gl::GlslProg::create(format);

	mCamera.lookAt(vec3(0.0, 1.0, -2.0f), vec3(0.0));
	mCamera.setPerspective(45.0f, getWindowAspectRatio(), 0.1f, 1000.0f);
	mCameraUi = CameraUi(&mCamera);

	// a patch primitive is a general purpose primitive, where every n​ vertices is a new patch primitive
	// the number of vertices per patch can be defined on the application level using the following:
	gl::patchParameteri(GL_PATCH_VERTICES, 4);

	// the tessellation control shader is actually optional - we can specify default tessellation levels like so:
	vec4 dOuterLevel{ 10.0f };
	gl::patchParameterfv(GL_PATCH_DEFAULT_OUTER_LEVEL, value_ptr(dOuterLevel));

	vec2 dInnerLevel{ 5.0f };
	gl::patchParameterfv(GL_PATCH_DEFAULT_INNER_LEVEL, value_ptr(dOuterLevel));

	mShowWireframe = false;

	// custom lambda that will fire whenever the mouse moves: update shader uniforms
	getWindow()->getSignalMouseMove().connect([&](MouseEvent event)
	{	
		float tessLevelInner = lmap(static_cast<float>(event.getPos().x), 0.0f, static_cast<float>(getWindowWidth()), 1.0f, 100.0f);
		float tessLevelOuter = lmap(static_cast<float>(event.getPos().x), 0.0f, static_cast<float>(getWindowWidth()), 15.0f, 45.0f);
		float noiseIntensity = lmap(static_cast<float>(event.getPos().y), 0.0f, static_cast<float>(getWindowHeight()), 1.0f, 8.0f);
		mTessellationProg->uniform("uTessLevelInner", tessLevelInner);
		mTessellationProg->uniform("uTessLevelOuter", tessLevelOuter);
		mTessellationProg->uniform("uNoiseIntensity", noiseIntensity);
	});

	// custom lambda that will fire whenever the space bar is pressed: toggle wireframe
	getWindow()->getSignalKeyDown().connect([&](KeyEvent event)
	{
		if (event.getCode() == KeyEvent::KEY_SPACE)
		{
			mShowWireframe = !mShowWireframe;
			if (mShowWireframe) gl::enableWireframe();
			else gl::disableWireframe();
		}
	});
}

void QuadTessellationApp::update()
{
}

void QuadTessellationApp::mouseDown(MouseEvent event)
{
	mCameraUi.mouseDown(event);
}

void QuadTessellationApp::mouseDrag(MouseEvent event)
{
	mCameraUi.mouseDrag(event);
}

void QuadTessellationApp::draw()
{
	gl::clear(Color::gray(0.15f)); 
	gl::setMatrices(mCamera);
	gl::enableWireframe();
	gl::ScopedGlslProg scpGlslProg(mTessellationProg);
	gl::setDefaultShaderVars();
	
	gl::drawArrays(GL_PATCHES, 0, 4);
}

CINDER_APP(QuadTessellationApp, RendererGl)
