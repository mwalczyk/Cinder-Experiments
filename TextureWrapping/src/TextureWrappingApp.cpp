#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class TextureWrappingApp : public App 
{
  public:
	void setup() override;
	void update() override;
	void draw() override;

	array<gl::Texture2dRef, 4> mTextures;
	gl::BatchRef mQuadBatch;
};

void TextureWrappingApp::setup()
{
	// set up textures with different wrap modes
	mTextures[0] = gl::Texture2d::create(loadImage(loadAsset("stone.jpg")));
	mTextures[0]->setWrap(GL_REPEAT, GL_REPEAT);

	mTextures[1] = gl::Texture2d::create(loadImage(loadAsset("stone.jpg")));
	mTextures[1]->setWrap(GL_MIRRORED_REPEAT, GL_MIRRORED_REPEAT);

	mTextures[2] = gl::Texture2d::create(loadImage(loadAsset("stone.jpg")));
	mTextures[2]->setWrap(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);

	mTextures[3] = gl::Texture2d::create(loadImage(loadAsset("stone.jpg")));
	mTextures[3]->setWrap(GL_CLAMP_TO_BORDER, GL_CLAMP_TO_BORDER);
	
	// set the border color associated with texture 3's sampler
	gl::ScopedTextureBind scpTextureBind(mTextures[3], 0);
	float bColor[4] = { 1.0f, 0.0f, 0.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, bColor);

	// set up render batch from a stock shader and uv-mapped quad
	auto glsl = gl::getStockShader(gl::ShaderDef().texture());
	auto geom = geom::Rect().rect(Rectf(0.0f, 0.0f, getWindowWidth() * 0.5f, getWindowHeight() * 0.5f))
							.texCoords(vec2(0.0f, 0.0f),
									   vec2(4.0f, 0.0f), 
									   vec2(4.0f, 4.0f), 
									   vec2(0.0f, 4.0f));

	mQuadBatch = gl::Batch::create(geom, glsl);
}

void TextureWrappingApp::update()
{
}

void TextureWrappingApp::draw()
{
	gl::clear();
	gl::setMatricesWindow(getWindowSize());

	for (size_t i = 0; i < mTextures.size(); ++i)
	{
		// bind texture
		gl::ScopedTextureBind scpTexureBind(mTextures[i], 0);

		// translate
		gl::ScopedModelMatrix scpModelMatrix;
		float hw = getWindowWidth() * 0.5f;
		float hh = getWindowHeight() * 0.5f;
		gl::translate(hw * (i % 2), hh * (i / 2));

		mQuadBatch->draw();
	}
}

CINDER_APP( TextureWrappingApp, RendererGl )
