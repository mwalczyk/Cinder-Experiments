#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/Rand.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class TextureArrayApp : public App 
{
  public:
	void setup() override;
	void update() override;
	void draw() override;

	struct droplet
	{
		droplet(float aX, float aY, float aO) : xOffset(aX), yOffset(aY), orientation(aO), unused(0.0f) {}
		float xOffset;
		float yOffset;
		float orientation;
		float unused;
	};

	vector<droplet> mDroplets;
	gl::Texture3dRef mTextureArray;
	gl::UboRef mUbo;
	gl::GlslProgRef mRenderProg;
	const uint32_t mNumDroplets = 100;
};

void TextureArrayApp::setup()
{
	// setup texture array
	const int w = 512;
	const int h = 512;
	const int d = 3;
	auto format = gl::Texture3d::Format().target(GL_TEXTURE_2D_ARRAY);
	mTextureArray = gl::Texture3d::create(w, h, d, format);
	
	// load textures into the array
	for (size_t i = 0; i < d; ++i)
	{
		std::ostringstream oss;
		oss << "textures/texture_" << i << ".jpg";
		mTextureArray->update(loadImage(loadAsset(oss.str())), i, 0);
	}

	// setup render prog
	mRenderProg = gl::GlslProg::create(gl::GlslProg::Format().vertex(loadAsset("shader.vert")).fragment(loadAsset("shader.frag")));

	// setup uniform buffer object with transform data
	for (size_t i = 0; i < mNumDroplets; ++i)
	{
		float randX = randPosNegFloat(-1.0f, 1.0f);
		float randY = randPosNegFloat(-1.0f, 1.0f);
		float randR = randPosNegFloat(0, 2.0f * M_PI);
		mDroplets.emplace_back(randX, randY, randR);
	}
	mUbo = gl::Ubo::create(sizeof(droplet) * mNumDroplets, mDroplets.data(), GL_STATIC_DRAW);
	mUbo->bindBufferBase(0);
}

void TextureArrayApp::update()
{
	// map the buffer to replace data
	droplet *ptr = (droplet*)mUbo->mapBufferRange(0, sizeof(droplet) * mNumDroplets, GL_MAP_WRITE_BIT);
	for (size_t i = 0; i < mNumDroplets; ++i)
	{
		ptr->xOffset += sinf(getElapsedSeconds() + i) * 0.01f;
		ptr++;
	}
	mUbo->unmap();
}

void TextureArrayApp::draw()
{
	gl::clear(); 

	gl::ScopedTextureBind scpTextureBind(mTextureArray, 0);
	gl::ScopedGlslProg scpGlslProg(mRenderProg);

	// draw the ith quad after giving it an array index
	for (size_t i = 0; i < mNumDroplets; ++i)
	{
		glVertexAttribI1i(0, i);
		gl::drawArrays(GL_TRIANGLE_STRIP, 0, 4);
	}
	
}

CINDER_APP(TextureArrayApp, RendererGl, [](App::Settings *settings) {
	settings->setWindowSize(512, 512);
})
