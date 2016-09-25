#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class DiffuseConvolutionCSApp : public App 
{
public:
	void setup() override;
	void mouseDown(MouseEvent event) override;
	void update() override;
	void draw() override;

	gl::TextureCubeMapRef m_input_cubemap;
	gl::TextureCubeMapRef m_output_cubemap;
	gl::GlslProgRef m_compute_program;
};

void DiffuseConvolutionCSApp::setup()
{
	auto image_src = loadImage(getAssetPath("env_map.jpg"));
	auto cubemap_format = gl::TextureCubeMap::Format().internalFormat(GL_RGBA32F);
	m_input_cubemap = gl::TextureCubeMap::create(image_src, cubemap_format);
	m_output_cubemap = gl::TextureCubeMap::create(m_input_cubemap->getWidth(), m_input_cubemap->getHeight(), cubemap_format);

	console() << gl::getVersion().first << ", " << gl::getVersion().second << std::endl;
	
	// query some stuff: http://antongerdelan.net/opengl/compute.html
	GLint maxWorkGroupSizeX;
	GLint maxWorkGroupSizeY;
	GLint maxWorkGroupSizeZ;
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 0, &maxWorkGroupSizeX);	// x
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 1, &maxWorkGroupSizeY);	// y
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 2, &maxWorkGroupSizeZ);	// z
	console() << "Maximum work group size (x, y, z): " << maxWorkGroupSizeX << ", " << maxWorkGroupSizeY << ", " << maxWorkGroupSizeZ << endl;

	auto compute_format = gl::GlslProg::Format().compute(loadAsset("compute.glsl"));
	m_compute_program = gl::GlslProg::create(compute_format);

	console() << "Cubemap dimensions: " << m_input_cubemap->getWidth() << ", " << m_input_cubemap->getHeight() << std::endl;
}

void DiffuseConvolutionCSApp::mouseDown(MouseEvent event)
{
	console() << "here" << std::endl;
	// parameters:
	// 1) the index of the image unit to bind the texture to
	// 2) the name (id) of the texture that will be bound
	// 3) the mipmap level of the texture that will be bound
	// 4) whether the texture is layered (GL_TRUE or GL_FALSE)
	// 5) the layer of the texture that will be bound (only relevant if layered is GL_FALSE)
	// 6) a token indicating the type of access that will be performed
	// 7) the format that the elements of the image will be treated as for the purpose of formatted stores
	glBindImageTexture(0, m_input_cubemap->getId(), 0, GL_TRUE, 0, GL_READ_ONLY, GL_RGBA32F);
	glBindImageTexture(1, m_output_cubemap->getId(), 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_RGBA32F);

	const ivec2 local_size{ 1, 1 };
	gl::ScopedGlslProg scp_glsl_prog{ m_compute_program };
	gl::dispatchCompute(m_input_cubemap->getWidth() / local_size.x, m_input_cubemap->getHeight() / local_size.y, 6);
	
	gl::memoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
}

void DiffuseConvolutionCSApp::update()
{
}

void DiffuseConvolutionCSApp::draw()
{
	gl::clear(); 
	gl::setMatricesWindow(getWindowSize());
	
	Rectf draw_rect{ 0.0f, 0.0f, 256.0f, 192.0f };
	gl::drawHorizontalCross(m_input_cubemap, draw_rect);
	gl::drawHorizontalCross(m_output_cubemap, draw_rect.getOffset({ 0.0f, 192.0f }));

}

CINDER_APP(DiffuseConvolutionCSApp, RendererGl, [](App::Settings *settings) {
	settings->setWindowSize(512, 512);
})