#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class UniformBufferObjectsApp : public App 
{
  public:
	void setup() override;
	void update() override;
	void draw() override;

	gl::UboRef mUbo;
	gl::BatchRef mRenderBatch;
	CameraPersp mCamera;
};

void UniformBufferObjectsApp::setup()
{
	gl::enableDepth();

	// fill ubo with some dummy data
	vector<vec4> palette{   
		vec4(1.0f, 0.0f, 0.0f, 1.0f),
		vec4(0.0f, 1.0f, 0.0f, 1.0f),
		vec4(0.0f, 0.0f, 1.0f, 1.0f)
	};

	mUbo = gl::Ubo::create(sizeof(vec4) * palette.size(), palette.data(), GL_STATIC_DRAW);
	mUbo->bindBufferBase(0);
	
	auto sphere = geom::Sphere().radius(1.0f).subdivisions(50);

	// inline shader code
	auto vertSrc = CI_GLSL(420,

		uniform mat4 ciModelViewProjection;
		
		layout(std140, binding = 0) uniform MyBlock {
			vec4 palette[3];
		} block;

		in vec4 ciPosition;

		float hash(in vec2 p)
		{
			return fract(sin(dot(p, vec2(12.233, 72.323))) * 43212.23);
		}

		out vec4 vColor;

		void main()
		{
			int index = int(floor(hash(ciPosition.xy) * 3.0));
			vColor = block.palette[index];
			gl_Position = ciModelViewProjection * ciPosition;
		});

	auto fragSrc = CI_GLSL(420,

		in vec4 vColor;

		out vec4 oColor;

		void main()
		{
			oColor = vColor;
		});

	// setup render batch from shader and geometry pair
	auto glsl = gl::GlslProg::create(gl::GlslProg::Format().vertex(vertSrc).fragment(fragSrc));
	mRenderBatch = gl::Batch::create(sphere, glsl);

	GLint index = glsl->getUniformBlockLocation("MyBlock");
	console() << "Block index: " << index << std::endl;

	// setup camera
	mCamera.lookAt(vec3(0.0f, 0.0f, -3.0f), vec3(0.0f));
	mCamera.setPerspective(45.0f, getWindowAspectRatio(), 0.1f, 1000.0f);
}

void UniformBufferObjectsApp::update()
{
}

void UniformBufferObjectsApp::draw()
{
	gl::clear(Color::gray(0.15f)); 
	gl::setMatrices(mCamera);
	mRenderBatch->draw();
}

CINDER_APP( UniformBufferObjectsApp, RendererGl )
