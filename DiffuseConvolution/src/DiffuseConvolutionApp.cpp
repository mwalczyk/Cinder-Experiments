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

	pbr::ConvolutionRef m_diffuse_convolution;

	array<gl::Texture2dRef, 6> m_original_face_textures;
	array<gl::Texture2dRef, 6> m_convolved_face_textures;
	gl::TextureCubeMapRef m_convolved_cubemap;

	bool m_transferred_to_texture = false;
};

void DiffuseConvolutionApp::setup()
{
	ImageSourceFileTinyExr::registerSelf();
	ImageTargetFileTinyExr::registerSelf();

	m_diffuse_convolution = pbr::Convolution::create(getAssetPath("env_map.jpg"));

	// create textures from all of the original (input) cubemap faces
	auto& faces = m_diffuse_convolution->get_original_surfaces();
	for (size_t i = 0; i < faces.size(); ++i)
	{
		m_original_face_textures[i] = gl::Texture2d::create(faces[i]);
	}

	// begin convolution
	m_diffuse_convolution->convolve_cubemap();

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
	if (m_diffuse_convolution->is_convolved() && !m_transferred_to_texture)
	{
		for (size_t i = 0; i < 6; ++i)
		{
			m_convolved_face_textures[i] = gl::Texture2d::create(m_diffuse_convolution->get_convolved_surfaces()[i]);
		}

		ImageSourceRef *images = (ImageSourceRef*)m_diffuse_convolution->get_convolved_surfaces().data();
		m_convolved_cubemap = gl::TextureCubeMap::create(images);

		m_transferred_to_texture = true;
	}
}

void DiffuseConvolutionApp::draw()
{
	gl::clear(); 
	gl::setMatricesWindow(getWindowSize());

	// draw all of the cubemap faces
	auto draw_size = ivec2(64.0f, 64.0f);

	for (size_t i = 0; i < m_original_face_textures.size(); ++i)
	{
		Rectf draw_rect(0.0f, 0.0f, draw_size.x, draw_size.y);
		draw_rect.offset({ i * draw_size.x, 0.0f });

		gl::draw(m_original_face_textures[i], draw_rect);
	}

	// only draw if the convolution process has completed
	if (m_transferred_to_texture)
	{
		for (size_t i = 0; i < 6; ++i)
		{
			Rectf draw_rect(0.0f, 0.0f, draw_size.x, draw_size.y);
			draw_rect.offset({ i * draw_size.x, draw_size.y });

			gl::draw(m_convolved_face_textures[i], draw_rect);
		}
		Rectf cross_rect(0.0f, 0.0f, 256.0f, 192.0f);
		cross_rect.offset({ 0, 128 });

		gl::drawHorizontalCross(m_convolved_cubemap, cross_rect);
	}
	
}

CINDER_APP(DiffuseConvolutionApp, RendererGl)
