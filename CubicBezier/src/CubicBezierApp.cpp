#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/CameraUi.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class CubicBezierApp : public App 
{
  public:
	void setup() override;
	void update() override;
	void draw() override;
	void mouseDown(MouseEvent event) override;
	void mouseDrag(MouseEvent event) override;

	CameraPersp mCamera;
	CameraUi mCameraUi;

	gl::GlslProgRef mControlCageProg;
	gl::GlslProgRef mTessellationProg;
	gl::BatchRef mRenderBatch;
};

void CubicBezierApp::setup()
{
	mCamera.lookAt(vec3(0.0, 0.0, -5.0f), vec3(0.0f));
	mCamera.setPerspective(45.0f, getWindowAspectRatio(), 0.1f, 1000.0f);
	mCameraUi = CameraUi(&mCamera);

	vector<vec3> controlPoints 
	{
		vec3(-1.0f,  -1.0f,  0.0f),
		vec3(-0.33f, -1.0f,  0.0f),
		vec3(0.33f, -1.0f,  0.0f),
		vec3(1.0f,  -1.0f,  0.0f),

		vec3(-1.0f,  -0.33f, 0.0f),
		vec3(-0.33f, -0.33f, 0.0f),
		vec3(0.33f, -0.33f, 0.0f),
		vec3(1.0f,  -0.33f, 0.0f),

		vec3(-1.0f,   0.33f, 0.0f),
		vec3(-0.33f,  0.33f, 0.0f),
		vec3(0.33f,  0.33f, 0.0f),
		vec3(1.0f,   0.33f, 0.0f),

		vec3(-1.0f,   1.0f,  0.0f),
		vec3(-0.33f,  1.0f,  0.0f),
		vec3(0.33f,  1.0f,  0.0f),
		vec3(1.0f,   1.0f,  0.0f),
	};

	// use a sine wave to offset the z-coordinate of each point
	const float kFreq = 16.0f;
	for (size_t i = 0; i < controlPoints.size(); ++i)
	{
		float fi = static_cast<float>(i) / controlPoints.size();
		controlPoints.at(i).z = sinf(kFreq * (0.2f + fi * 0.3f));
	}

	// create a custom buffer layout that we will append to the mesh
	geom::BufferLayout layout;
	layout.append(geom::POSITION, 3, sizeof(vec3), 0, 0);
	gl::VboRef vbo = gl::Vbo::create(GL_ARRAY_BUFFER, controlPoints, GL_STATIC_DRAW);

	auto layoutPair = make_pair(layout, vbo);

	gl::VboMeshRef mesh = gl::VboMesh::create(controlPoints.size(), GL_POINTS, { layoutPair });
	
	// load a stock shader for rendering the control cage
	mControlCageProg = gl::getStockShader(gl::ShaderDef().color());

	// load the tessellation shaders
	auto format = gl::GlslProg::Format()
		.vertex(loadAsset("vert.glsl"))
		.tessellationCtrl(loadAsset("tcs.glsl"))
		.tessellationEval(loadAsset("tes.glsl"))
		.fragment(loadAsset("frag.glsl"));

	mTessellationProg = gl::GlslProg::create(format);

	mRenderBatch = gl::Batch::create(mesh, mTessellationProg);

	gl::patchParameteri(GL_PATCH_VERTICES, 16);
	gl::enableDepth();
}

void CubicBezierApp::mouseDown(MouseEvent event)
{
	mCameraUi.mouseDown(event);
}

void CubicBezierApp::mouseDrag(MouseEvent event)
{
	mCameraUi.mouseDrag(event);
}

void CubicBezierApp::update()
{
}

void CubicBezierApp::draw()
{
	gl::pointSize(4);
	gl::clear(); 
	gl::setMatrices(mCamera);
	gl::drawCoordinateFrame();

	gl::ScopedVao scpVao(mRenderBatch->getVao().get());

	{
		gl::ScopedGlslProg scpGlslProg(mControlCageProg);
		gl::setDefaultShaderVars();
		gl::drawArrays(GL_POINTS, 0, 16);
	}

	{
		// here, we draw the tessellated mesh by overriding gl::Batch's internal draw call
		gl::ScopedGlslProg scpGlslProg(mRenderBatch->getGlslProg());
		gl::setDefaultShaderVars();
		gl::drawArrays(GL_PATCHES, 0, 16);
	}
}

CINDER_APP( CubicBezierApp, RendererGl )
