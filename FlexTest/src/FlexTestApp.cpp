#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/Rand.h"
#include "cinder/CameraUi.h"
#include "cinder/Perlin.h"
#include "Solver.h"

using namespace ci;
using namespace ci::app;
using namespace std;

// http://docs.nvidia.com/gameworks/content/gameworkslibrary/physx/flex/_static/api/flex_8h.html#struct_flex_params
// https://libcinder.org/docs/guides/cinder-blocks/index.html
class FlexTestApp : public App 
{
  public:
	void setup() override;
	void mouseDown(MouseEvent event) override;
	void mouseDrag(MouseEvent event) override;
	void update() override;
	void draw() override;

	gl::BatchRef mParticleBatch;
	CameraPersp mCamera;
	CameraUi mUi;
	size_t mMaxParticles;
	flex::SolverRef mSolver;
	
	AxisAlignedBox mAABB;
};

void FlexTestApp::setup()
{
	gl::enableDepth();
	gl::enableFaceCulling();
	gl::cullFace(GL_BACK);

	// setup camera
	mCamera.lookAt(vec3(11.0f, 4.0f, -11.0f), vec3(0.0f));
	mCamera.setPerspective(45.0f, getWindowAspectRatio(), 0.05f, 1000.0f);
	mUi = CameraUi(&mCamera);

	// create a vbo mesh and append the custom attribute
	auto flexSeed = geom::Sphere().subdivisions(100);
	TriMeshRef triMesh = TriMesh::create(flexSeed);
	
	mMaxParticles = triMesh->getNumVertices();

	// initialize flex
	auto scene = flex::Scene::create(triMesh);
	mSolver = flex::Solver::create(scene);

	// prepare custom vertex attribute
	gl::VboMeshRef mesh = gl::VboMesh::create(geom::Sphere().radius(0.1f).colors());
	
	geom::BufferLayout layoutPosition;
	layoutPosition.append(geom::CUSTOM_0, 4, 0, 0, 1);
	
	geom::BufferLayout layoutVelocity;
	layoutVelocity.append(geom::CUSTOM_1, 3, 0, 0, 1);

	mesh->appendVbo(layoutPosition, mSolver->getVbo(flex::SolverAttrib::POSITION));
	mesh->appendVbo(layoutVelocity, mSolver->getVbo(flex::SolverAttrib::VELOCITY));

	auto glsl = gl::GlslProg::create(loadAsset("shader.vert"), loadAsset("shader.frag"));

	mParticleBatch = gl::Batch::create(mesh, glsl, { {geom::CUSTOM_0, "ciInstancePosition"}, {geom::CUSTOM_1, "ciInstanceVelocity"} });

	TriMeshRef tri = TriMesh::create(geom::Cube().size(vec3(8.0f)));
	mAABB = tri->calcBoundingBox();
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
	
	static Perlin sPerlin;
	static const float sz = 6.0f;
	float t = getElapsedSeconds();
	vec3 offset{0.0f, 1000.0f, 10000.0f};
	vec3 seed = offset + t;
	vec3 p1 = vec3(sinf(t) * 0.5f + 0.55f, 0.5f, 0.5f);//sPerlin.dfBm(seed) * 0.5f + 0.5f;
	vec3 p2 = -p1;
	
	mAABB.set(p1 * sz, p2 * sz);
	mSolver->setCollisionBox(mAABB);
}

void FlexTestApp::draw()
{
	static const Colorf kBackgroundColor = Color(0.12f, 0.12f, 0.14f);
	gl::clear(kBackgroundColor);
	gl::setMatrices(mCamera);
	mParticleBatch->drawInstanced(mMaxParticles);

	// draw the collision box
	gl::ScopedColor scpColor1(Colorf(1.0f, 0.0f, 0.0f));
	gl::drawStrokedCube(mAABB);
}

CINDER_APP(FlexTestApp, RendererGl(RendererGl::Options().msaa(8)), [](App::Settings *settings) {
	settings->setWindowSize(720, 720);
})
