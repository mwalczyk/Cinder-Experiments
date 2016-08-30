#pragma once

#include "cinder/app/App.h"
#include "cinder/gl/Batch.h"
#include "cinder/gl/GlslProg.h"
#include "cinder/gl/Shader.h"
#include "cinder/gl/draw.h"
#include "cinder/GeomIo.h"
#include "cinder/Vector.h"

#include "glm/gtx/component_wise.hpp"

#include <functional>

class VectorField;
using VectorFieldRef = std::shared_ptr<VectorField>;

class VectorField
{
public:
	using VectorFunction = std::function<ci::vec3(const ci::vec3 &)>;

	struct Format
	{
		Format();
		
		//! set the number of arrows to visualize along each of the three cardinal axes
		Format& grid_resolution(const ci::ivec3 &t_grid_resolution) { m_grid_resolution = t_grid_resolution; return *this; }
		
		//! set the size of the vector field's draw region
		Format& grid_size(const ci::vec3 &t_grid_size) { m_grid_size = t_grid_size; return *this; }

		//! set the vector-valued function that will be used to evaluate the vector field
		Format& vector_function(const VectorFunction &t_vector_function) { m_vector_function = t_vector_function; return *this; }

		ci::ivec3 m_grid_resolution;
		ci::vec3 m_grid_size;
		VectorFunction m_vector_function;
	};

	static VectorFieldRef create(Format t_format = Format());

	VectorField(Format t_format = Format());

	void draw_vector_field() const;
	ci::vec3 query_field_at(const ci::vec3 &t_point) const;
	void update();
	ci::vec3 operator()(const ci::vec3 &t_point) const;

private:
	void init_format(const Format &t_format);
	void prepare_render_batch();

	ci::ivec3 m_grid_resolution;
	ci::vec3 m_grid_size;
	VectorFunction m_vector_function;
	ci::gl::VboRef m_vbo_transform_inst;
	ci::gl::VboRef m_vbo_color_inst;
	ci::gl::BatchRef m_vector_batch;
};