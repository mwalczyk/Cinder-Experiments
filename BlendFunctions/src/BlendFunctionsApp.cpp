#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class BlendFunctionsApp : public App 
{
  public:
	void setup() override;
	void update() override;
	void draw() override;

	gl::BatchRef mRenderBatch;
	CameraPersp mCamera;
};

void BlendFunctionsApp::setup()
{
	gl::enableDepth();
	gl::enableBlending(true);

	std::function<Colorf(vec3)> func = [](const vec3 &attrib) { return Colorf(attrib.x, attrib.y, attrib.z) * 0.5 + 0.5; };
	auto geom = geom::Cube() >> geom::ColorFromAttrib(geom::POSITION, func);

	mRenderBatch = gl::Batch::create(geom, gl::getStockShader(gl::ShaderDef().color()));

	mCamera.lookAt(vec3(0.0f, 0.0f, 5.0f), vec3(0.0f));
	mCamera.setPerspective(45.0f, getWindowAspectRatio(), 0.1f, 1000.0f);
}

void BlendFunctionsApp::update()
{
}

void BlendFunctionsApp::draw()
{
	// http://www.informit.com/articles/article.aspx?p=1616796&seqNum=5
	static const vector<GLenum> blendModes
	{
		GL_ZERO,
		GL_ONE,
		GL_SRC_COLOR,
		GL_ONE_MINUS_SRC_COLOR,
		GL_DST_COLOR,
		GL_ONE_MINUS_DST_COLOR,
		GL_SRC_ALPHA,
		GL_ONE_MINUS_SRC_ALPHA,
		GL_DST_ALPHA,
		GL_ONE_MINUS_DST_ALPHA,
		GL_CONSTANT_COLOR,
		GL_ONE_MINUS_CONSTANT_COLOR,
		GL_CONSTANT_ALPHA,
		GL_ONE_MINUS_CONSTANT_ALPHA,
		GL_SRC_ALPHA_SATURATE,
		GL_SRC1_COLOR,
		GL_ONE_MINUS_SRC1_COLOR,
		GL_SRC1_ALPHA,
		GL_ONE_MINUS_SRC1_ALPHA
	};

	static const Color clearColor{ 0.6f, 0.4f, 0.1f };

	gl::clear(clearColor);
	glBlendColor(0.2f, 0.5f, 0.7f, 0.5f);

	gl::setMatrices(mCamera);

	float spacing = 0.85f;

	for (size_t i = 0; i < blendModes.size(); ++i)
	{
		
		float fi = static_cast<float>(i) / (blendModes.size() - 1);

		for (size_t j = 0; j < blendModes.size(); ++j)
		{

			float fj = static_cast<float>(j) / (blendModes.size() - 1);

			gl::ScopedModelMatrix scpModelMatrix;
			gl::ScopedBlend scpBlend(blendModes[i], blendModes[j]);
			gl::translate(fj * 10.0f - 5.0f + j * spacing, 
						  fi * 10.0f - 5.0f + i * spacing, 
						 -40.0f);
			mRenderBatch->draw();
		}
	}
}

CINDER_APP(BlendFunctionsApp, RendererGl, [](App::Settings *settings) {
	settings->setWindowSize(720, 720);
})
