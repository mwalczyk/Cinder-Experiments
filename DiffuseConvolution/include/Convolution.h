#pragma once
#include "cinder/app/App.h"
#include "cinder/ImageFileTinyExr.h"
#include "cinder/ImageIo.h"
#include "cinder/Surface.h"
#include "cinder/ip/Flip.h"
#include "cinder/Thread.h"

#include <array>

namespace pbr
{
	class Convolution;
	using ConvolutionRef = std::shared_ptr<Convolution>;

	class Convolution
	{
	public:
		static const size_t m_num_faces{ 6 };

		static ConvolutionRef create(const ci::fs::path &t_hdr_path);

		Convolution(const ci::fs::path &t_hdr_path);

		struct CubeMapFaceRegion
		{
			ci::Area mArea;
			ci::ivec2 mOffset;
			bool mFlip;
		};

		inline const std::array<ci::Surface32f, m_num_faces>& get_original_surfaces() const { return m_original_surfaces; }
		inline const std::array<ci::Surface32f, m_num_faces>& get_convolved_surfaces() const { return m_convolved_surfaces; }
		inline bool is_convolved() const { return m_thread_counter == m_num_faces; }
		void convolve_cubemap();
	private:
		std::atomic<int> m_thread_counter{ 0 };

		//! used internally to load a cubemap as an array of surfaces
		void load_cubemap_faces(const ci::ImageSourceRef &t_image_src);
		
		std::vector<CubeMapFaceRegion> calc_cubemap_h_regions(const ci::ImageSourceRef &t_image_src);
		std::vector<CubeMapFaceRegion> calc_cubemap_v_regions(const ci::ImageSourceRef &t_image_src);
		std::vector<CubeMapFaceRegion> calc_cubemap_hcross_regions(const ci::ImageSourceRef &t_image_src);
		std::vector<CubeMapFaceRegion> calc_cubemap_vcross_regions(const ci::ImageSourceRef &t_image_src);
		
		ci::vec3 get_direction(size_t t_face, size_t t_i, size_t t_j, size_t t_size);
		ci::vec3 get_pixel_at(size_t t_face, size_t t_i, size_t t_j);
		void convolve_face_threaded(size_t t_face_id, ci::Surface32f &t_filtered_face);

		std::array<std::thread, m_num_faces> m_convolution_threads;
		std::array<ci::Surface32f, m_num_faces> m_original_surfaces;
		std::array<ci::Surface32f, m_num_faces> m_convolved_surfaces;
	};
} // namespace pbr