#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "Convolution.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class DiffuseConvolutionApp : public App 
{ 
public:
	void setup() override;
	void mouseDown(MouseEvent event) override;
	void fileDrop(FileDropEvent event) override;
	void update() override;
	void draw() override;

	Surface32fRef m_surface_hdr;
	pbr::ConvolutionRef m_diffuse_convolution;
	array<gl::Texture2dRef, 6> m_face_textures;
	gl::Texture2dRef m_convolved_texture_test;
};

void DiffuseConvolutionApp::setup()
{
	ImageSourceFileTinyExr::registerSelf();
	ImageTargetFileTinyExr::registerSelf();

	m_diffuse_convolution = pbr::Convolution::create(getAssetPath("env_map.jpg"));

	// create textures from all of the cubemap faces
	auto& faces = m_diffuse_convolution->get_cubemap_surfaces();
	for (size_t i = 0; i < faces.size(); ++i)
	{
		m_face_textures[i] = gl::Texture2d::create(faces[i]);
	}

	m_diffuse_convolution->convolve_cubemap();
	m_convolved_texture_test = gl::Texture2d::create(m_diffuse_convolution->m_convolved_face_test);
}

void DiffuseConvolutionApp::mouseDown(MouseEvent event)
{
}

void DiffuseConvolutionApp::fileDrop(FileDropEvent event)
{
	auto file = event.getFile(0);
	
	// ...do something with the file
}

void DiffuseConvolutionApp::update()
{
}

void DiffuseConvolutionApp::draw()
{
	gl::clear(); 
	gl::setMatricesWindow(getWindowSize());

	// draw all of the cubemap faces
	auto draw_size = ivec2(64.0f, 64.0f);

	for (size_t i = 0; i < m_face_textures.size(); ++i)
	{
		Rectf draw_rect(0.0f, 0.0f, draw_size.x, draw_size.y);
		draw_rect.offset({ i * draw_size.x, 0.0f });

		gl::draw(m_face_textures[i], draw_rect);
	}

	Rectf draw_rect(0.0f, 0.0f, draw_size.x, draw_size.y);
	draw_rect.offset({ 0.0f, draw_size.y });

	gl::draw(m_convolved_texture_test, draw_rect);
	
}

CINDER_APP(DiffuseConvolutionApp, RendererGl)
