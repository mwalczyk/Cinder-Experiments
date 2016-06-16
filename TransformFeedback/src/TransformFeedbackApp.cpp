#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/CameraUi.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class TransformFeedbackApp : public App 
{
  public:
	void setup() override;
	void mouseDown(MouseEvent event) override;
	void mouseDrag(MouseEvent event) override;
	void update() override;
	void draw() override;
	
	array<gl::VaoRef, 2> mVaos;
	array<gl::VboRef, 2> mVboPositions;
	array<gl::VboRef, 2> mVboVelocities;
	array<gl::VboRef, 2> mVboConnections;
	
	array<gl::BufferTextureRef, 2> mBufferTextures;

	gl::GlslProgRef mUpdateProg;
	gl::GlslProgRef mRenderProg;

	CameraPersp mCamera;
	CameraUi mCameraUi;

	const uint32_t kPointsX = 50;
	const uint32_t kPointsY = 50;
	const uint32_t kPointsTotal = kPointsX * kPointsY;

	int mIndex = 0;
};

void TransformFeedbackApp::setup()
{
	gl::enableDepth();

	vector<vec4> positions(kPointsTotal);
	vector<vec3> velocities(kPointsTotal);
	vector<ivec4> connections(kPointsTotal);

	int n = 0;

	for (size_t j = 0; j < kPointsY; ++j)
	{
		float fj = static_cast<float>(j) / kPointsY; 
		for (size_t i = 0; i < kPointsX; ++i)
		{
			float fi = static_cast<float>(i) / kPointsX; 

			// positions
			positions.at(n) = vec4((fi - 0.5f) * kPointsX,		// position X
								   (fj - 0.5f) * kPointsY,		// position Y
								   0.6f * sinf(fi) * cos(fj),	// position Z
								   1.0f);						// mass
			
			// velocities
			velocities.at(n) = vec3(0.0f);
			
			// connections
			connections.at(n) = ivec4(-1);						// -1 implies no connection

			if (j != (kPointsY - 1))
			{
				if (i != 0)
				{
					connections.at(n).x = n - 1;
				}
				if (j != 0)
				{
					connections.at(n).y = n - kPointsX;
				}
				if (i != (kPointsX - 1))
				{
					connections.at(n).z = n + 1;
				}
				if (j != (kPointsY - 1))
				{
					connections.at(n).w = n + kPointsX;
				}
			}
			n++;
		}
	}

	// set up vaos and vbos
	for (size_t i = 0; i < 2; ++i)
	{	
		mVaos[i] = gl::Vao::create();
		gl::ScopedVao scpVao(mVaos[i]);
		{
			// positions
			mVboPositions[i] = gl::Vbo::create(GL_ARRAY_BUFFER, positions, GL_DYNAMIC_COPY);
			gl::ScopedBuffer scpBuffer(mVboPositions[i]);
			gl::vertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, NULL);
			gl::enableVertexAttribArray(0);
		}

		{
			// velocities
			mVboVelocities[i] = gl::Vbo::create(GL_ARRAY_BUFFER, velocities, GL_DYNAMIC_COPY);
			gl::ScopedBuffer scpBuffer(mVboVelocities[i]);
			gl::vertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
			gl::enableVertexAttribArray(1);
		}

		{
			// connections
			mVboConnections[i] = gl::Vbo::create(GL_ARRAY_BUFFER, connections, GL_DYNAMIC_COPY);
			gl::ScopedBuffer scpBuffer(mVboConnections[i]);
			gl::vertexAttribIPointer(2, 4, GL_INT, 0, NULL);
			gl::enableVertexAttribArray(2);
		}
	}
	
	mBufferTextures[0] = gl::BufferTexture::create(mVboPositions[0], GL_RGBA32F);
	mBufferTextures[1] = gl::BufferTexture::create(mVboPositions[1], GL_RGBA32F);

	mUpdateProg = gl::GlslProg::create(gl::GlslProg::Format().vertex(loadAsset("update.vert")).feedbackVaryings({"tfPosition", "tfVelocity"}).feedbackFormat(GL_SEPARATE_ATTRIBS));
	mRenderProg = gl::GlslProg::create(gl::GlslProg::Format().vertex(loadAsset("render.vert")).fragment(loadAsset("render.frag")));
	
	mCamera.lookAt(vec3(0.0f, 0.0f, -90.0f), vec3(0.0f));
	mCamera.setPerspective(45.0f, getWindowAspectRatio(), 0.1f, 1000.0f);

	mCameraUi = CameraUi(&mCamera);
}

void TransformFeedbackApp::mouseDown(MouseEvent event)
{
	mCameraUi.mouseDown(event);
}

void TransformFeedbackApp::mouseDrag(MouseEvent event)
{
	mCameraUi.mouseDrag(event);
}

void TransformFeedbackApp::update()
{
	gl::ScopedState scpState(GL_RASTERIZER_DISCARD, true);
	gl::ScopedGlslProg scpGlslProg(mUpdateProg);

	for (size_t i = 0; i < 16; ++i)
	{
		gl::ScopedVao scpVao(mVaos[mIndex & 1]);

		mBufferTextures[mIndex & 1]->bindTexture(0);
		
		// incr the index and bind the other buffers
		mIndex++;
		gl::bindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, mVboPositions[mIndex & 1]);
		gl::bindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 1, mVboVelocities[mIndex & 1]);

		gl::beginTransformFeedback(GL_POINTS);	// TF starts
		gl::drawArrays(GL_POINTS, 0, kPointsTotal);
		gl::endTransformFeedback();				// TF ends

		mBufferTextures[mIndex & 1]->unbindTexture();
	}
}

void TransformFeedbackApp::draw()
{
	gl::clear(); 
	gl::pointSize(2.0f);

	gl::ScopedGlslProg scpGlslProg(mRenderProg);
	gl::setMatrices(mCamera);
	gl::setDefaultShaderVars();

	gl::ScopedVao scp(mVaos[mIndex & 1]);
	gl::drawArrays(GL_POINTS, 0, kPointsTotal);
}

CINDER_APP( TransformFeedbackApp, RendererGl )
