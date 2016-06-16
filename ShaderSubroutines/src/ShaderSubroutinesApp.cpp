#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class ShaderSubroutinesApp : public App 
{
  public:
	void setup() override;
	void update() override;
	void draw() override;

	gl::GlslProgRef mRenderProg;
};

void ShaderSubroutinesApp::setup()
{
	auto format = gl::GlslProg::Format().vertex(loadAsset("shader.vert")).fragment(loadAsset("shader.frag"));
	mRenderProg = gl::GlslProg::create(format);
	
	// here, we query OpenGL for the index of each subroutine
	// manually specifying indices inside of the shader via layout qualifiers is only supported in GLSL 430+
	GLuint subroutineIndex1 = glGetProgramResourceIndex(mRenderProg->getHandle(), GL_FRAGMENT_SUBROUTINE, "myFunc1");
	GLuint subroutineIndex2 = glGetProgramResourceIndex(mRenderProg->getHandle(), GL_FRAGMENT_SUBROUTINE, "myFunc2");
	console() << "First index: " << subroutineIndex1 << std::endl;
	console() << "Second index: " << subroutineIndex2 << std::endl;
}

void ShaderSubroutinesApp::update()
{
}

void ShaderSubroutinesApp::draw()
{
	gl::clear(); 
	gl::setMatricesWindow(getWindowSize());

	// bind the glsl program to the current context
	gl::ScopedGlslProg scpGlslProg(mRenderProg);
	
	// flip between 0 and 1
	GLuint currIndex = static_cast<GLuint>(getElapsedSeconds()) % 2;
	glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, &currIndex);

	gl::drawSolidRect(getWindowBounds());
}

CINDER_APP(ShaderSubroutinesApp, RendererGl)
