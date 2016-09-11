#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/ImageFileTinyExr.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class DiffuseConvolutionApp : public App 
{ 
public:
	void loadHdr(const fs::path &t_path);
	void setup() override;
	void mouseDown(MouseEvent event) override;
	void fileDrop(FileDropEvent event) override;
	void update() override;
	void draw() override;

	Surface32fRef m_surface_hdr;
	gl::TextureCubeMap m_texture_convolved;
	bool m_is_convolved;
};

void DiffuseConvolutionApp::loadHdr(const fs::path &t_path)
{
	// this should really take two Surface32fRefs src and dst
	// eventually, we will run this code on a background thread
	m_surface_hdr = Surface32f::create(loadImage(t_path));
	console() << "Image resolution: " << m_surface_hdr->getWidth() << " x " << m_surface_hdr->getHeight() << std::endl;

	Surface32fRef output_hdr = Surface32f::create(m_surface_hdr->clone());

	// calculate the diffuse convolution of the source image
	auto pixel_iter = m_surface_hdr->getIter();
	while (pixel_iter.line())
	{
		while (pixel_iter.pixel())
		{
			Colorf color(pixel_iter.r(), pixel_iter.g(), pixel_iter.b());
			float pixel_x = pixel_iter.x();
			float pixel_y = pixel_iter.y();
			//console() << "Pixel (" << pixel_x << ", " << pixel_y << "): " << color << std::endl;
		}
	}
}

void DiffuseConvolutionApp::setup()
{
	ImageSourceFileTinyExr::registerSelf();
	ImageTargetFileTinyExr::registerSelf();

	loadHdr(getAssetPath("techgate.hdr"));
}

void DiffuseConvolutionApp::mouseDown(MouseEvent event)
{
}

void DiffuseConvolutionApp::fileDrop(FileDropEvent event)
{
	auto file = event.getFile(0);
	loadHdr(file);
}

void DiffuseConvolutionApp::update()
{
}

void DiffuseConvolutionApp::draw()
{
	gl::clear(); 

	// if the diffuse convolution is ready, display it...
}

CINDER_APP(DiffuseConvolutionApp, RendererGl)
