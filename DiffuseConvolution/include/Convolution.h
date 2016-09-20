#pragma once
#include "cinder/app/App.h"
#include "cinder/ImageFileTinyExr.h"
#include "cinder/ImageIo.h"
#include "cinder/Surface.h"
#include "cinder/ip/Flip.h"

#include <array>

namespace pbr
{
	class Convolution;
	using ConvolutionRef = std::shared_ptr<Convolution>;

	class Convolution
	{
	public:

		static ConvolutionRef create(const ci::fs::path &t_hdr_path);

		Convolution(const ci::fs::path &t_hdr_path);

		struct CubeMapFaceRegion
		{
			ci::Area mArea;
			ci::ivec2 mOffset;
			bool mFlip;
		};

		inline const std::array<ci::Surface32f, 6>& get_cubemap_surfaces() const { return m_cubemap_surfaces; }
		void convolve_cubemap();
		
		ci::Surface32f m_convolved_face_test;
	private:
		void load_cubemap_faces(const ci::ImageSourceRef &t_image_src);
		
		std::vector<CubeMapFaceRegion> calc_cubemap_h_regions(const ci::ImageSourceRef &t_image_src);
		std::vector<CubeMapFaceRegion> calc_cubemap_v_regions(const ci::ImageSourceRef &t_image_src);
		std::vector<CubeMapFaceRegion> calc_cubemap_hcross_regions(const ci::ImageSourceRef &t_image_src);
		std::vector<CubeMapFaceRegion> calc_cubemap_vcross_regions(const ci::ImageSourceRef &t_image_src);
		
		ci::vec3 get_direction(size_t t_face, size_t t_i, size_t t_j, size_t t_size);
		ci::vec3 get_pixel_at(size_t t_face, size_t t_i, size_t t_j);

		std::array<ci::Surface32f, 6> m_cubemap_surfaces;
	};
} // namespace pbr