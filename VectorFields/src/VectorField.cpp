#include "VectorField.h"

VectorField::Format::Format()
{
	m_grid_resolution = ci::ivec3(15);
	m_grid_size = ci::vec3(4.0f);
	m_vector_function = [](const ci::vec3 &pt) { return pt; };
}

VectorFieldRef VectorField::create(Format t_format)
{
	return std::make_shared<VectorField>(t_format);
}

VectorField::VectorField(Format t_format)
{
	init_format(t_format);
	update_render_batch();
}

void VectorField::init_format(const Format &t_format)
{
	m_grid_resolution = t_format.m_grid_resolution;
	m_grid_size = t_format.m_grid_size;
	m_vector_function = t_format.m_vector_function;
}

void VectorField::update_render_batch()
{	
	auto arrow_src = ci::geom::Cone().height(0.2f).ratio(0.05f);
	auto vbo_mesh_src = ci::gl::VboMesh::create(arrow_src);
	
	// load the shader
	auto vertex_src = ci::app::loadAsset("vertex.glsl");
	auto fragment_src = ci::app::loadAsset("fragment.glsl");
	auto glsl_src = ci::gl::GlslProg::create(vertex_src, fragment_src);

	std::vector<ci::mat4> grid_transformations;
	std::vector<ci::vec3> grid_colors;

	for (size_t x = 0; x < m_grid_resolution.x; ++x)
	{
		float fx = static_cast<float>(x) / m_grid_resolution.x;
		for (size_t y = 0; y < m_grid_resolution.y; ++y)
		{
			float fy = static_cast<float>(y) / m_grid_resolution.y;
			for (size_t z = 0; z < m_grid_resolution.z; ++z)
			{
				float fz = static_cast<float>(z) / m_grid_resolution.z;

				// evaluate the vector-valued function at this position
				ci::vec3 grid_position{ 
					fx * m_grid_size.x - m_grid_size.x * 0.5f,
					fy * m_grid_size.y - m_grid_size.y * 0.5f,
					fz * m_grid_size.z - m_grid_size.z * 0.5f 
				};
				ci::vec3 vector_field_value = m_vector_function(grid_position);
				
				float magnitude = ci::length(vector_field_value);

				// use the vector length to create a color
				ci::vec3 color = ci::lerp(ci::vec3(0.0f, 0.0f, 1.0f), ci::vec3(1.0f, 0.0f, 0.0f), magnitude / 2.8f);
				grid_colors.push_back(color);

				// normalize the direction vector
				vector_field_value = ci::normalize(vector_field_value);
				
				// create the per-instance transformation matrix
				ci::mat4 translation = ci::translate(grid_position);
				ci::mat4 rotation = ci::toMat4(glm::rotation(ci::vec3(0.0f, 1.0f, 0.0f), ci::normalize(vector_field_value)));
				ci::mat4 transform = translation * rotation;
				
				grid_transformations.push_back(transform);
			}
		}
	}

	// setup custom vbo for storing per-instance transformation data
	auto vbo_transform_inst = ci::gl::Vbo::create(GL_ARRAY_BUFFER, grid_transformations.size() * sizeof(ci::mat4), grid_transformations.data(), GL_STATIC_DRAW);
	auto vbo_color_inst = ci::gl::Vbo::create(GL_ARRAY_BUFFER, grid_colors.size() * sizeof(ci::vec3), grid_colors.data(), GL_STATIC_DRAW);
	
	ci::geom::BufferLayout data_transform_layout;
	data_transform_layout.append(ci::geom::Attrib::CUSTOM_0, 16, sizeof(ci::mat4), 0, 1);

	ci::geom::BufferLayout data_color_layout;
	data_color_layout.append(ci::geom::Attrib::COLOR, 3, sizeof(ci::vec3), 0, 1);

	// append the vbo to the mesh
	vbo_mesh_src->appendVbo(data_transform_layout, vbo_transform_inst);
	vbo_mesh_src->appendVbo(data_color_layout, vbo_color_inst);

	// finally, setup the batch
	m_vector_batch = ci::gl::Batch::create(vbo_mesh_src, glsl_src, { {ci::geom::Attrib::CUSTOM_0, "v_instance_transform"}, { ci::geom::Attrib::COLOR, "v_instance_color" } });
}

void VectorField::draw_vector_field() const
{
	auto num_inst = glm::compMul(m_grid_resolution);

	// draw instances
	m_vector_batch->drawInstanced(num_inst);

	// draw bounding box
	const ci::vec3 padding{ 0.2f };
	ci::gl::drawStrokedCube(ci::vec3(0.0f), m_grid_size + padding);
}

ci::vec3 VectorField::query_field_at(const ci::vec3 &t_point) const
{
	return m_vector_function(t_point);
}