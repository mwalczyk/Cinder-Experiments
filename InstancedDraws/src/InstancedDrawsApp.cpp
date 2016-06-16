#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/CameraUi.h"
#include "cinder/Rand.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class InstancedDrawsApp : public App 
{
  public:
	void setup() override;
	void update() override;
	void draw() override;
	void mouseDown(MouseEvent event) override;
	void mouseDrag(MouseEvent event) override;

	gl::BatchRef mBatch;
	const uint32_t kNumInstances = 1000;
	CameraPersp mCamera;
	CameraUi mCameraUi;
};

void InstancedDrawsApp::setup()
{
	gl::cullFace(GL_BACK);
	gl::enableDepth();
	gl::enableFaceCulling();

	// generate random offsets
	vector<vec3> offsets(kNumInstances);
	std::generate(offsets.begin(), offsets.end(), []() { return randVec3(); });

	// create buffer layout to describe the attribute
	geom::BufferLayout layout;
	layout.append(geom::CUSTOM_0, 3, sizeof(vec3), 0, 1);

	// create buffer to hold per-instance transforms and append it to a mesh
	auto geom = geom::Sphere().subdivisions(50).radius(0.02f).colors();
	auto vbo = gl::Vbo::create(GL_ARRAY_BUFFER, sizeof(vec3) * kNumInstances, offsets.data(), GL_STATIC_DRAW);
	auto mesh = gl::VboMesh::create(geom);
	mesh->appendVbo(layout, vbo);	

	// create render batch
	auto glsl = gl::GlslProg::create(gl::GlslProg::Format().vertex(loadAsset("shader.vert")).fragment(loadAsset("shader.frag")));
	mBatch = gl::Batch::create(mesh, glsl, { {geom::CUSTOM_0, "offset"} });

	// set up camera params
	mCamera.lookAt(vec3(0.0f, 0.0f, -5.0f), vec3(0.0f));
	mCamera.setPerspective(45.0f, getWindowAspectRatio(), 0.1f, 1000.0f);
	mCameraUi = CameraUi(&mCamera);
}

void InstancedDrawsApp::mouseDown(MouseEvent event)
{
	mCameraUi.mouseDown(event);
}

void InstancedDrawsApp::mouseDrag(MouseEvent event)
{
	mCameraUi.mouseDrag(event);
}

void InstancedDrawsApp::update()
{
}

void InstancedDrawsApp::draw()
{
	gl::clear(); 
	gl::setMatrices(mCamera);
	mBatch->drawInstanced(kNumInstances);
}

CINDER_APP(InstancedDrawsApp, RendererGl(RendererGl::Options().msaa(8)))
