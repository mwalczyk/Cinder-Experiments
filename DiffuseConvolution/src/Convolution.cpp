#include "Convolution.h"

namespace pbr
{
	ConvolutionRef Convolution::create(const ci::fs::path &t_hdr_path)
	{
		return std::make_shared<Convolution>(t_hdr_path);
	}

	Convolution::Convolution(const ci::fs::path &t_hdr_path)
	{
		ci::app::console() << "Loading HDR image: " << t_hdr_path << std::endl;
		auto image_src = ci::loadImage(t_hdr_path);
		load_cubemap_faces(image_src);
	}

	void Convolution::load_cubemap_faces(const ci::ImageSourceRef &t_image_src)
	{
		std::vector<CubeMapFaceRegion> face_regions;
		ci::ivec2 image_size(t_image_src->getWidth(), t_image_src->getHeight());
		float minDim = (float)std::min(image_size.x, image_size.y);
		float maxDim = (float)std::max(image_size.x, image_size.y);
		float aspect = minDim / maxDim;
		if (abs(aspect - 1 / 6.0f) < abs(aspect - 3 / 4.0f)) // closer to 1:6 than to 4:3, so row or column
		{
			ci::app::console() << "Parsing cubemap in row or column format." << std::endl;
			face_regions = (image_size.x > image_size.y) ? calc_cubemap_h_regions(t_image_src) : calc_cubemap_v_regions(t_image_src);
		}
		else // else, horizontal or vertical cross
		{
			ci::app::console() << "Parsing cubemap in horizontal or vertical cross format." << std::endl;
			face_regions = (image_size.x > image_size.y) ? calc_cubemap_hcross_regions(t_image_src) : calc_cubemap_vcross_regions(t_image_src);
		}

		ci::Area face_area = face_regions.front().mArea;
		ci::ivec2 face_size = face_area.getSize();

		// create six floating-point surfaces from the cubemap
		ci::Surface32f master_surface(t_image_src, ci::SurfaceConstraintsDefault());
		ci::Surface32f images[6];

		for (uint8_t f = 0; f < 6; ++f)
		{
			m_cubemap_surfaces[f] = ci::Surface32f(face_size.x, face_size.y, master_surface.hasAlpha(), ci::SurfaceConstraints());
			m_cubemap_surfaces[f].copyFrom(master_surface, face_regions[f].mArea, face_regions[f].mOffset);
			if (face_regions[f].mFlip)
			{
				ci::ip::flipVertical(&images[f]);
				ci::ip::flipHorizontal(&images[f]);
			}
		}

		ci::app::console() << "Successfully loaded all six cubemap faces." << std::endl;
	}

	std::vector<Convolution::CubeMapFaceRegion> Convolution::calc_cubemap_h_regions(const ci::ImageSourceRef &t_image_src)
	{
		std::vector<CubeMapFaceRegion> result;
		ci::ivec2 face_size(t_image_src->getHeight(), t_image_src->getHeight());

		for (uint8_t index = 0; index < 6; ++index) 
		{
			ci::Area area(index * face_size.x, 0.0f, (index + 1) * face_size.x, face_size.y);
			ci::ivec2 offset(-index * face_size.x, 0.0f);
			result.push_back({ area, offset, false });
		}

		return result;
	}

	std::vector<Convolution::CubeMapFaceRegion> Convolution::calc_cubemap_v_regions(const ci::ImageSourceRef &t_image_src)
	{
		std::vector<CubeMapFaceRegion> result;
		ci::ivec2 face_size(t_image_src->getWidth(), t_image_src->getWidth());

		for (uint8_t index = 0; index < 6; ++index) 
		{
			ci::Area area(0.0f, index * face_size.x, face_size.x, (index + 1) * face_size.y);
			ci::ivec2 offset(0.0f, -index * face_size.y);
			result.push_back({ area, offset, false });
		}

		return result;
	}

	std::vector<Convolution::CubeMapFaceRegion> Convolution::calc_cubemap_hcross_regions(const ci::ImageSourceRef &t_image_src)
	{
		std::vector<CubeMapFaceRegion> result;

		ci::ivec2 face_size(t_image_src->getWidth() / 4, t_image_src->getHeight() / 3);
		ci::Area face_area(0, 0, face_size.x, face_size.y);

		ci::Area area;
		ci::ivec2 offset;

		// GL_TEXTURE_CUBE_MAP_POSITIVE_X
		area = face_area + ci::ivec2(face_size.x * 2, face_size.y * 1);
		offset = -ci::ivec2(face_size.x * 2, face_size.y * 1);
		result.push_back({ area, offset, false });
		// GL_TEXTURE_CUBE_MAP_NEGATIVE_X
		area = face_area + ci::ivec2(face_size.x * 0, face_size.y * 1);
		offset = -ci::ivec2(face_size.x * 0, face_size.y * 1);
		result.push_back({ area, offset, false });
		// GL_TEXTURE_CUBE_MAP_POSITIVE_Y
		area = face_area + ci::ivec2(face_size.x * 1, face_size.y * 0);
		offset = -ci::ivec2(face_size.x * 1, face_size.y * 0);
		result.push_back({ area, offset, false });
		// GL_TEXTURE_CUBE_MAP_NEGATIVE_Y
		area = face_area + ci::ivec2(face_size.x * 1, face_size.y * 2);
		offset = -ci::ivec2(face_size.x * 1, face_size.y * 2);
		result.push_back({ area, offset, false });
		// GL_TEXTURE_CUBE_MAP_POSITIVE_Z
		area = face_area + ci::ivec2(face_size.x * 1, face_size.y * 1);
		offset = -ci::ivec2(face_size.x * 1, face_size.y * 1);
		result.push_back({ area, offset, false });
		// GL_TEXTURE_CUBE_MAP_NEGATIVE_Z
		area = face_area + ci::ivec2(face_size.x * 3, face_size.y * 1);
		offset = -ci::ivec2(face_size.x * 3, face_size.y * 1);
		result.push_back({ area, offset, false });

		return result;
	}

	std::vector<Convolution::CubeMapFaceRegion> Convolution::calc_cubemap_vcross_regions(const ci::ImageSourceRef &t_image_src)
	{
		std::vector<CubeMapFaceRegion> result;

		ci::ivec2 face_size(t_image_src->getWidth() / 3, t_image_src->getHeight() / 4);
		ci::Area face_area(0, 0, face_size.x, face_size.y);

		ci::Area area;
		ci::ivec2 offset;

		// GL_TEXTURE_CUBE_MAP_POSITIVE_X
		area = face_area + ci::ivec2(face_size.x * 2, face_size.y * 1);
		offset = -ci::ivec2(face_size.x * 2, face_size.y * 1);
		result.push_back({ area, offset, false });
		// GL_TEXTURE_CUBE_MAP_NEGATIVE_X
		area = face_area + ci::ivec2(face_size.x * 0, face_size.y * 1);
		offset = -ci::ivec2(face_size.x * 0, face_size.y * 1);
		result.push_back({ area, offset, false });
		// GL_TEXTURE_CUBE_MAP_POSITIVE_Y
		area = face_area + ci::ivec2(face_size.x * 1, face_size.y * 0);
		offset = -ci::ivec2(face_size.x * 1, face_size.y * 0);
		result.push_back({ area, offset, false });
		// GL_TEXTURE_CUBE_MAP_NEGATIVE_Y
		area = face_area + ci::ivec2(face_size.x * 1, face_size.y * 2);
		offset = -ci::ivec2(face_size.x * 1, face_size.y * 2);
		result.push_back({ area, offset, false });
		// GL_TEXTURE_CUBE_MAP_POSITIVE_Z
		area = face_area + ci::ivec2(face_size.x * 1, face_size.y * 1);
		offset = -ci::ivec2(face_size.x * 1, face_size.y * 1);
		result.push_back({ area, offset, false });
		// GL_TEXTURE_CUBE_MAP_NEGATIVE_Z
		area = face_area + ci::ivec2(face_size.x * 1, face_size.y * 3);
		offset = -ci::ivec2(face_size.x * 1, face_size.y * 3);
		result.push_back({ area, offset, true });

		return result;
	}

	ci::vec3 Convolution::get_direction(size_t t_face, size_t t_i, size_t t_j, size_t t_size)
	{
		ci::vec3 direction;

		if (t_face == 0)
		{
			direction = ci::vec3(0.5f, -((t_j + 0.5f) / t_size - 0.5f), -((t_i + 0.5f) / t_size - 0.5f));
		}
		else if (t_face == 1)
		{
			direction = ci::vec3(-0.5f, -((t_j + 0.5f) / t_size - 0.5f), ((t_i + 0.5f) / t_size - 0.5f));
		}
		else if (t_face == 2)
		{
			direction = ci::vec3(((t_i + 0.5f) / t_size - 0.5f), 0.5f, ((t_j + 0.5f) / t_size - 0.5f));
		}
		else if (t_face == 3)
		{
			direction = ci::vec3(((t_i + 0.5f) / t_size - 0.5f), -0.5f, -((t_j + 0.5f) / t_size - 0.5f));
		}
		else if (t_face == 4)
		{
			direction = ci::vec3(((t_i + 0.5f) / t_size - 0.5f), -((t_j + 0.5f) / t_size - 0.5f), 0.5f);
		}
		else if (t_face == 5)
		{
			direction = ci::vec3(-((t_i + 0.5f) / t_size - 0.5f), -((t_j + 0.5f) / t_size - 0.5f), -0.5f);
		}

		return direction;
	}

	ci::vec3 Convolution::get_pixel_at(size_t t_face, size_t t_i, size_t t_j)
	{
		return ci::vec3{};
	}

	void Convolution::convolve_cubemap()
	{
		// input and output cubemap dimensions
		ci::ivec2 original_dims{ m_cubemap_surfaces.front().getSize() };
		ci::ivec2 filtered_dims{ original_dims };
		bool has_alpha = m_cubemap_surfaces.front().hasAlpha();
		ci::app::console() << "Beginning cubemap convolution | input dimensions: " << original_dims << std::endl;

		// for a horizontal cross, faces are loaded in this order:
		// +X, -X, +Y, -Y, +Z, -Z

		// each face of the filtered (output) cubemap
		for (size_t filtered_face_id = 0; filtered_face_id < 1; ++filtered_face_id)
		{
			ci::app::console() << "Beginning convolution of filtered face #" << filtered_face_id << std::endl;
			m_convolved_face_test = ci::Surface32f{ original_dims.x, original_dims.y, has_alpha };
	
			// iterate through every pixel of the current filtered (output) face
			auto output_pixel_iter = m_convolved_face_test.getIter();
			while (output_pixel_iter.line())
			{
				ci::app::console() << "| Processing row " << output_pixel_iter.y() << " of " << original_dims.y << std::endl;
				while (output_pixel_iter.pixel())
				{
					ci::vec3 filtered_direction = ci::normalize(get_direction(filtered_face_id, output_pixel_iter.x(), output_pixel_iter.y(), filtered_dims.x));
					float total_weight = 0.0f;
					float weight = 0.0f;
					ci::vec3 original_direction;
					ci::vec3 original_face_direction;
					ci::Color filtered_color;

					// each face of the original (input) cubemap
					for (size_t original_face_id = 0; original_face_id < 6; ++original_face_id)
					{
						original_face_direction = ci::normalize(get_direction(original_face_id, 1, 1, 3));
						//ci::app::console() << "----Examining face #" << original_face_id << " of the original cubemap." << std::endl;
						//ci::app::console() << "----Face normal: " << original_face_direction << std::endl;

						// iterate through every pixel of the current original (input) face
						auto input_pixel_iter = m_cubemap_surfaces[original_face_id].getIter();
						while (input_pixel_iter.line())
						{
							while (input_pixel_iter.pixel())
							{
								// direction to the current texel (light source)
								original_direction = get_direction(original_face_id, input_pixel_iter.x(), input_pixel_iter.y(), original_dims.x);

								// more distant texels contribute less to the final radiance
								weight = 1.0f / glm::pow(ci::length(original_direction), 2.0f);

								original_direction = ci::normalize(original_direction);

								// texels that are tilted away from the face normal contribute less to the final radiance
								weight *= ci::dot(original_face_direction, original_direction);

								// direction filter for diffuse illumination
								weight *= std::max(0.0f, glm::dot(filtered_direction, original_direction));

								// normalize against the maximum illumination
								total_weight += weight;

								// add the illumination
								float r = input_pixel_iter.r();
								float g = input_pixel_iter.g();
								float b = input_pixel_iter.b();
								filtered_color += weight * ci::Colorf{ r, g, b };
							}
						}
					}

					// set the pixel color of the filtered cubemap to filtered_color / total_weight
					ci::Colorf final_color = filtered_color / total_weight;
					output_pixel_iter.r() = final_color.r;
					output_pixel_iter.g() = final_color.g;
					output_pixel_iter.b() = final_color.b;
				}
			}
		}
		
	}
} // namespace pbr