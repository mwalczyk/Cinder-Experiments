#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/Rand.h"
#include "cinder/CameraUi.h"
#include "Solver.h"

using namespace ci;
using namespace ci::app;
using namespace std;

// http://docs.nvidia.com/gameworks/content/gameworkslibrary/physx/flex/_static/api/flex_8h.html#struct_flex_params

class FlexTestApp : public App 
{
  public:
	void setup() override;
	void mouseDown(MouseEvent event) override;
	void mouseDrag(MouseEvent event) override;
	void update() override;
	void draw() override;

	gl::BatchRef mParticleBatch;
	gl::VboRef mInstanceDataVbo;
	CameraPersp mCamera;
	CameraUi mUi;

	int mMaxParticles;

	flex::SolverRef mSolver;
};

void FlexTestApp::setup()
{
	// setup camera
	mCamera.lookAt(vec3(0.0f, -2.0f, -10.0f), vec3(0.0f));
	mCamera.setPerspective(45.0f, getWindowAspectRatio(), 0.1f, 1000.0f);
	mUi = CameraUi(&mCamera);

	// prepare custom vertex attribute
	geom::BufferLayout layout;
	layout.append(geom::CUSTOM_0, 4, 0, 0, 1);

	// create a vbo mesh and append the custom attribute
	auto sphere = geom::Cube().subdivisions(40);
	TriMeshRef triMesh = TriMesh::create(sphere);
	
	mMaxParticles = triMesh->getNumVertices();
	
	gl::VboMeshRef mesh = gl::VboMesh::create(geom::Sphere().radius(0.02f));
	mInstanceDataVbo = gl::Vbo::create(GL_ARRAY_BUFFER, mMaxParticles * sizeof(vec4), nullptr, GL_DYNAMIC_DRAW);
	mesh->appendVbo(layout, mInstanceDataVbo);

	auto glsl = gl::GlslProg::create(loadAsset("shader.vert"), loadAsset("shader.frag"));

	mParticleBatch = gl::Batch::create(mesh, glsl, { {geom::CUSTOM_0, "ciInstancePosition"} });

	flex::SceneRef scene = flex::Scene::create(triMesh);
	mSolver = flex::Solver::create(scene);
}

void FlexTestApp::mouseDown(MouseEvent event)
{
	mUi.mouseDown(event);
}

void FlexTestApp::mouseDrag(MouseEvent event)
{
	mUi.mouseDrag(event);
}

void FlexTestApp::update()
{
	mSolver->update();
	mSolver->transferToVbo(mInstanceDataVbo, mMaxParticles);
}

void FlexTestApp::draw()
{
	gl::clear(); 
	gl::setMatrices(mCamera);
	mParticleBatch->drawInstanced(mMaxParticles);
}

CINDER_APP(FlexTestApp, RendererGl(RendererGl::Options().msaa(8)), [](App::Settings *settings) {
	settings->setWindowSize(720, 720);
})
