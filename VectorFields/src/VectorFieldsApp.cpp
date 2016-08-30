#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/CameraUi.h"

#include "VectorField.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class VectorFieldsApp : public App 
{
  public:
	void setup() override;
	void update() override;
	void draw() override;
	void mouseDown(MouseEvent event) override;

	CameraPersp m_camera;
	CameraUi m_ui;
	VectorFieldRef m_vector_field;
};

void VectorFieldsApp::mouseDown(MouseEvent event)
{

}

void VectorFieldsApp::setup()
{
	gl::enableDepth();

	// setup camera
	m_camera.lookAt(vec3(0.0f, 0.0f, -12.0f), vec3(0.0f));
	m_camera.setPerspective(45.0f, getWindowAspectRatio(), 0.1f, 1000.0f);
	m_ui = CameraUi(&m_camera, getWindow());

	// create the vector field
	auto vector_func = [&](const vec3 &pt) { 
		float s = sinf(getElapsedSeconds());
		float c = cosf(getElapsedSeconds());
		float v_x = pt.x + s;
		float v_y = powf(pt.y, 2.0f) + c;
		float v_z = pt.z + s * c;

		return vec3(v_x, v_y, v_z); 
	};
	
	auto format = VectorField::Format()
		.vector_function(vector_func)
		.grid_resolution(ivec3(12));

	m_vector_field = VectorField::create(format);
}

void VectorFieldsApp::update()
{
	m_vector_field->update();
}

void VectorFieldsApp::draw()
{
	gl::clear(); 
	gl::setMatrices(m_camera);
	m_vector_field->draw_vector_field();
}

CINDER_APP(VectorFieldsApp, RendererGl(RendererGl::Options().msaa(8)), [](App::Settings *settings) {
	settings->setWindowSize(720, 720);
})
