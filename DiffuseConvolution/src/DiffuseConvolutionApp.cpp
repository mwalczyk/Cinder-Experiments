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
	vec3 get_direction(size_t t_face, size_t t_i, size_t t_j, size_t t_size);
	vec3 get_pixel_at(size_t t_face, size_t t_i, size_t t_j);
	void load_hdr(const fs::path &t_path);
	void setup() override;
	void mouseDown(MouseEvent event) override;
	void fileDrop(FileDropEvent event) override;
	void update() override;
	void draw() override;

	Surface32fRef m_surface_hdr;
	gl::TextureCubeMapRef m_texture_convolved;
	bool m_is_convolved;
};

vec3 DiffuseConvolutionApp::get_direction(size_t t_face, size_t t_i, size_t t_j, size_t t_size)
{
	vec3 direction;

	if (t_face == 0)
	{
		direction = vec3(0.5f, -((t_j + 0.5f) / t_size - 0.5f), -((t_i + 0.5f) / t_size - 0.5f));
	}
	else if (t_face == 1)
	{
		direction = vec3(-0.5f, -((t_j + 0.5f) / t_size - 0.5f), ((t_i + 0.5f) / t_size - 0.5f));
	}
	else if (t_face == 2)
	{
		direction = vec3(((t_i + 0.5f) / t_size - 0.5f), 0.5f, ((t_j + 0.5f) / t_size - 0.5f));
	}
	else if (t_face == 3)
	{
		direction = vec3(((t_i + 0.5f) / t_size - 0.5f), -0.5f, -((t_j + 0.5f) / t_size - 0.5f));
	}
	else if (t_face == 4)
	{
		direction = vec3(((t_i + 0.5f) / t_size - 0.5f), -((t_j + 0.5f) / t_size - 0.5f), 0.5f);
	}
	else if (t_face == 5)
	{
		direction = vec3(-((t_i + 0.5f) / t_size - 0.5f), -((t_j + 0.5f) / t_size - 0.5f), -0.5f);
	}

	return direction;
}

vec3 DiffuseConvolutionApp::get_pixel_at(size_t t_face, size_t t_i, size_t t_j)
{
	return vec3{};
}

namespace {}

void DiffuseConvolutionApp::load_hdr(const fs::path &t_path)
{
	// this should really take two Surface32fRefs src and dst
	// eventually, we will run this code on a background thread
	m_surface_hdr = Surface32f::create(loadImage(t_path));
	console() << "Image resolution: " << m_surface_hdr->getWidth() << " x " << m_surface_hdr->getHeight() << std::endl;

	//Surface32fRef output_hdr = Surface32f::create(m_surface_hdr->clone());

	// calculate the diffuse convolution of the source image
	/*
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
	*/

	// parse cubemap dimensions
	size_t filtered_size = 32;
	size_t original_size = 32;

	// iterate through every texel of the output (filtered) cubemap
	for (size_t filtered_face = 0; filtered_face < 6; ++filtered_face)
	{
		for (size_t filtered_i = 0; filtered_i < filtered_size; ++filtered_i)
		{
			for (size_t filtered_j = 0; filtered_j < filtered_size; ++filtered_j)
			{
				glm::vec3 filtered_direction = glm::normalize(get_direction(filtered_face, filtered_i, filtered_j, filtered_size));
				float total_weight = 0.0f;
				float weight = 0.0f;
				glm::vec3 original_direction;
				glm::vec3 original_face_direction;
				Color filtered_color;

				// sum (integrate) the diffuse illumination received from all texels in the input (original) cubemap
				for (size_t original_face = 0; original_face < 6; original_face++)
				{
					// the normal vector of the face
					original_face_direction = glm::normalize(get_direction(original_face, 1, 1, 3));

					for (size_t original_i = 0; original_i < original_size; ++original_i)
					{
						for (size_t original_j = 0; original_j < original_size; ++original_j)
						{
							// direction to the current texel (light source)
							original_direction = get_direction(original_face, original_i, original_j, original_size);

							// more distant texels contribute less to the final radiance
							weight = 1.0f / glm::pow(glm::length(original_direction), 2.0f);

							original_direction = glm::normalize(original_direction);

							// texels that are tilted away from the face normal contribute less to the final radiance
							weight *= glm::dot(original_face_direction, original_direction);

							// direction filter for diffuse illumination
							weight *= std::max(0.0f, glm::dot(filtered_direction, original_direction));

							// normalize against the maximum illumination
							total_weight += weight;

							// add the illumination
							filtered_color += weight * get_pixel_at(original_face, original_i, original_j);
						}
					}
				}

				// set the pixel color of the filtered cubemap to filtered_color / total_weight
			}
		}
	}

	m_texture_convolved = gl::TextureCubeMap::create(loadImage(t_path));
	console() << m_texture_convolved->getWidth() << " x " << m_texture_convolved->getHeight() << std::endl;
}

void DiffuseConvolutionApp::setup()
{
	ImageSourceFileTinyExr::registerSelf();
	ImageTargetFileTinyExr::registerSelf();

	load_hdr(getAssetPath("env_map.jpg"));
}

void DiffuseConvolutionApp::mouseDown(MouseEvent event)
{
}

void DiffuseConvolutionApp::fileDrop(FileDropEvent event)
{
	auto file = event.getFile(0);
	load_hdr(file);
}

void DiffuseConvolutionApp::update()
{
}

void DiffuseConvolutionApp::draw()
{
	gl::clear(); 

	// if the diffuse convolution is ready, display it...

	gl::setMatricesWindow(getWindowSize());
	gl::drawHorizontalCross(m_texture_convolved, Rectf(0.0f, 0.0f, 400.0f, 300.0f));
}

CINDER_APP(DiffuseConvolutionApp, RendererGl)
