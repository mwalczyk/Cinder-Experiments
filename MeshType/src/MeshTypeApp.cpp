#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/Rand.h"
#include "cinder/Triangulate.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class MeshTypeApp : public App 
{
  public:
	void setup() override;
	void calculateMesh(char aLetter);
	void keyDown(KeyEvent event) override;
	void update() override;
	void draw() override;

	gl::BatchRef mRenderBatch;
	Font mFont;
};

void MeshTypeApp::setup()
{
	mFont = Font("Times New Roman", 64);

	calculateMesh('A');
}

void MeshTypeApp::calculateMesh(char aLetter)
{
	size_t index = mFont.getGlyphChar(aLetter);
	Shape2d glyph = mFont.getGlyphShape(index);

	TriMesh triMesh = Triangulator(glyph).calcMesh();
	mRenderBatch = gl::Batch::create(triMesh, gl::getStockShader(gl::ShaderDef().color()));
}

void MeshTypeApp::keyDown(KeyEvent event)
{
	calculateMesh(event.getChar());
}

void MeshTypeApp::update()
{
}

void MeshTypeApp::draw()
{
	gl::clear(); 
	gl::setMatricesWindow(getWindowSize());
	gl::translate(getWindowCenter());
	gl::color(Color(1.0f, 0.3f, 0.1f));
	mRenderBatch->draw();
}

CINDER_APP(MeshTypeApp, RendererGl(RendererGl::Options().msaa(16)), [](App::Settings *settings) {
	settings->setWindowSize(512, 512);
})
