#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Ssbo.h"
#include "cinder/CameraUi.h"
#include "cinder/Rand.h"

using namespace ci;
using namespace ci::app;
using namespace std;

struct Particle
{
	vec3 position;
	float empty0;
	vec3 home;
	float empty1;
	vec3 color;
	float empty2;
};

class ComputeParticlesApp : public App 
{
  public:
	void setup() override;
	void mouseDown(MouseEvent event) override;
	void mouseDrag(MouseEvent event) override;
	void update() override;
	void draw() override;

	const size_t kNumParticles = 1024000;
	const size_t kWorkGroupSize = 128;

	gl::GlslProgRef mComputeProg;
	gl::GlslProgRef mRenderProg;
	gl::VaoRef mRenderVao;
	gl::VboRef mRenderVbo;
	gl::SsboRef mSsboPositions;

	CameraPersp mCamera;
	CameraUi mCameraUi;
};

void ComputeParticlesApp::setup()
{
	gl::enableAlphaBlending();
	glEnable(GL_PROGRAM_POINT_SIZE);

	// setup shader programs
	auto computeFormat = gl::GlslProg::Format().compute(loadAsset("compute.glsl"));
	auto renderFormat = gl::GlslProg::Format().vertex(loadAsset("vertex.glsl")).fragment(loadAsset("fragment.glsl"));
	mComputeProg = gl::GlslProg::create(computeFormat);
	mRenderProg = gl::GlslProg::create(renderFormat);

	vector<Particle> particles;
	for (size_t i = 0; i < kNumParticles; ++i)
	{
		vec3 rPosition = randVec3();
		Particle rParticle;
		rParticle.position = rPosition;
		rParticle.home = rPosition;
		rParticle.color = vec3(1.0f);

		particles.emplace_back(rParticle);
	}
	
	// setup the shader storage buffer object to hold our initial particle positions
	mSsboPositions = gl::Ssbo::create(sizeof(Particle) * particles.size(), particles.data(), GL_STATIC_DRAW);
	mSsboPositions->bindBase(0);

	// setup a vbo to hold indices which we will use to index into the ssbo
	GLuint curr = 0;
	vector<GLuint> indices(kNumParticles);
	std::generate(indices.begin(), indices.end(), [&curr]() { return curr++; });
	mRenderVbo = gl::Vbo::create(GL_ARRAY_BUFFER, indices, GL_STATIC_DRAW);

	// setup the vao for rendering
	mRenderVao = gl::Vao::create();
	gl::ScopedVao scpVao(mRenderVao);
	gl::ScopedBuffer scpBuffer(mRenderVbo);
	gl::enableVertexAttribArray(0);
	gl::vertexAttribIPointer(0, 1, GL_UNSIGNED_INT, sizeof(GLuint), nullptr);

	// setup the camera for viewing the scene
	mCamera.lookAt(vec3(0.0f, 0.0f, -10.0f), vec3(0.0f));
	mCamera.setPerspective(45.0f, getWindowAspectRatio(), 0.1f, 1000.0f);
	mCameraUi = CameraUi(&mCamera);
}

void ComputeParticlesApp::mouseDown(MouseEvent event)
{
	mCameraUi.mouseDown(event);
}

void ComputeParticlesApp::mouseDrag(MouseEvent event)
{
	mCameraUi.mouseDrag(event);
}

void ComputeParticlesApp::update()
{
	gl::ScopedGlslProg scpGlslProg(mComputeProg);
	gl::setDefaultShaderVars();
	gl::dispatchCompute(kNumParticles / kWorkGroupSize, 1, 1);
	gl::memoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
}

void ComputeParticlesApp::draw()
{
	gl::clear(Color(0.06f, 0.06f, 0.08f));
	gl::setMatrices(mCamera);

	gl::ScopedGlslProg scpGlslProg(mRenderProg); 
	gl::ScopedVao scpVao(mRenderVao);
	gl::setDefaultShaderVars();
	gl::drawArrays(GL_POINTS, 0, kNumParticles);
}

CINDER_APP(ComputeParticlesApp, RendererGl(RendererGl::Options().msaa(8)), [](App::Settings *settings) {
	settings->setWindowSize(512, 512);
})
