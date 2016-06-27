#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

using namespace ci;
using namespace ci::app;
using namespace std;

// more info about compute shaders here: https://www.opengl.org/wiki/Compute_Shader

class ComputeBasicApp : public App 
{
  public:
	void setup() override;
	void update() override;
	void draw() override;

	gl::GlslProgRef mComputeProg;
	gl::Texture2dRef mInputTex;
	gl::Texture2dRef mOutputTex;
};

void ComputeBasicApp::setup()
{
	// query some stuff 
	GLint maxWorkGroupSizeX;
	GLint maxWorkGroupSizeY;
	GLint maxWorkGroupSizeZ;
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 0, &maxWorkGroupSizeX);	// x
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 1, &maxWorkGroupSizeY);	// y
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 2, &maxWorkGroupSizeZ);	// z
	console() << "Maximum work group size (x, y, z): " << maxWorkGroupSizeX << ", " << maxWorkGroupSizeY << ", " << maxWorkGroupSizeZ << endl;
	
	// setup the compute program
	auto format = gl::GlslProg::Format().compute(loadAsset("compute.glsl"));
	mComputeProg = gl::GlslProg::create(format);

	GLint localSize[3];
	glGetProgramiv(mComputeProg->getHandle(), GL_MAX_COMPUTE_WORK_GROUP_SIZE, localSize);
	console() << "Local work group size (x, y, z) as declared in the shader: " << localSize[0] << ", " << localSize[1] << ", " << localSize[2] << endl;

	// construct the input texture procedurally
	Surface32f surface(512, 512, true);
	Surface32f::Iter pixelIter = surface.getIter();
	int32_t blockSize = surface.getWidth() / 10;

	while (pixelIter.line())
	{
		while (pixelIter.pixel())
		{
			ivec2 currPos = pixelIter.getPos();
			if (currPos.y < 256)
			{
				pixelIter.r() = pixelIter.g() = pixelIter.b() = 0.0f;
			}
			else
			{
				pixelIter.r() = pixelIter.g() = pixelIter.b() = 1.0f;
			}
			pixelIter.a() = 1.0f;
		}
	}

	// setup textures
	auto texFormat = gl::Texture2d::Format().internalFormat(GL_RGBA32F).dataType(GL_FLOAT);
	mInputTex = gl::Texture2d::create(surface, texFormat);
	mOutputTex = gl::Texture2d::create(512, 512, texFormat);
}

void ComputeBasicApp::update()
{
	glBindImageTexture(0, mInputTex->getId(), 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);
	glBindImageTexture(1, mOutputTex->getId(), 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);

	gl::ScopedGlslProg scpGlslProg(mComputeProg);
	gl::dispatchCompute(mInputTex->getWidth() / 32, mInputTex->getHeight() / 32, 1);
	gl::memoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
}

void ComputeBasicApp::draw()
{
	gl::clear(Color::gray(0.15f)); 
	gl::setMatricesWindow(getWindowSize());
	gl::draw(mInputTex, Rectf(0.0f, 0.0f, 100.0f, 100.00f));
	gl::draw(mOutputTex, Rectf(200.0f, 0.0f, 100.0f, 100.00f));
}

CINDER_APP(ComputeBasicApp, RendererGl, [](App::Settings *settings) {
	settings->setWindowSize(512, 512);
})
