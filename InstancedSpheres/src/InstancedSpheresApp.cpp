#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/Rand.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class InstancedSpheresApp : public App
{
public:
	void setup() override;
	void update() override;
	void draw() override;

	gl::BatchRef mSphereBatch;
	vector<mat4> mInstancePositions;
	uint32_t mInstanceCount;
	CameraPersp mCamera;
};

void InstancedSpheresApp::setup()
{
	gl::enableDepth();
	gl::cullFace(GL_BACK);
	gl::enableFaceCulling();

	// generate the per-instance transforms
	mInstanceCount = 2000;
	mInstancePositions.resize(mInstanceCount);
	std::generate(mInstancePositions.begin(), mInstancePositions.end(), []() { return translate(mat4(), randVec3()); });

	// set up render batch
	gl::VboMeshRef sphereVbo = gl::VboMesh::create(geom::Sphere().colors().radius(0.02f).subdivisions(50));

	// custom attribute with instance divisor 
	geom::BufferLayout instanceDataLayout;
	instanceDataLayout.append(geom::Attrib::CUSTOM_0, 16, sizeof(mat4), 0, 1);
	gl::VboRef instanceVbo = gl::Vbo::create(GL_ARRAY_BUFFER, mInstancePositions.size() * sizeof(mat4), mInstancePositions.data(), GL_STATIC_DRAW);

	// append the custom attribute to the vbo
	sphereVbo->appendVbo(instanceDataLayout, instanceVbo);

	// prepare the custom shader program
	auto instanceProgam = gl::GlslProg::create(gl::GlslProg::Format().vertex(loadAsset("sphere.vert")).fragment(loadAsset("sphere.frag")));
	mSphereBatch = gl::Batch::create(sphereVbo, instanceProgam, {{geom::Attrib::CUSTOM_0, "iModelMatrix" }});

	// setup camera
	mCamera.lookAt(vec3(0.0f, 0.0f, -5.0f), vec3(0.0f));
	mCamera.setPerspective(45.0f, getWindowAspectRatio(), 0.1f, 1000.0f);
}

void InstancedSpheresApp::update()
{
}

void InstancedSpheresApp::draw()
{
	gl::clear(Color(1.0f, 0.98f, 0.98f));
	gl::setMatrices(mCamera);
	gl::rotate(getElapsedSeconds() * 0.5f, vec3(0.0f, 1.0f, 0.0f));
	gl::rotate(getElapsedSeconds() * 0.5f, vec3(1.0f, 0.0f, 0.0f));
	mSphereBatch->drawInstanced(mInstanceCount);
}

CINDER_APP(InstancedSpheresApp, RendererGl(RendererGl::Options().msaa(16)))
