#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/CameraUi.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class ReflectiveShadowMapsApp : public App 
{
  public:
	void setup() override;
	void update() override;
	void draw() override;

	const int k_shadow_map_resolution_x = 1024;
	const int k_shadow_map_resolution_y = 1024;

	CameraPersp m_scene_camera;
	CameraUi m_scene_camera_ui;
	CameraPersp m_light_camera;

	gl::Texture2dRef m_shadow_texture;
	gl::FboRef m_shadow_fbo;

	gl::BatchRef m_scene_sphere_batch;
	gl::BatchRef m_scene_cube_batch;
	gl::BatchRef m_shadow_sphere_batch;
	gl::BatchRef m_shadow_cube_batch;
};

void ReflectiveShadowMapsApp::setup()
{
	gl::enableDepth();
	
	// create the shadow map texture
	auto depth_format = gl::Texture2d::Format()
		.internalFormat(GL_DEPTH_COMPONENT32F)
		.compareMode(GL_COMPARE_REF_TO_TEXTURE)
		.magFilter(GL_LINEAR)
		.minFilter(GL_LINEAR)
		.wrap(GL_CLAMP_TO_EDGE)
		.compareFunc(GL_LEQUAL);
	m_shadow_texture = gl::Texture2d::create(k_shadow_map_resolution_x, k_shadow_map_resolution_y, depth_format);

	auto color_format = gl::Texture2d::Format()
		.internalFormat(GL_RGB32F);

	// create the FBO for storing the 4 components of the reflective shadow map
	auto fbo_format = gl::Fbo::Format()
		.attachment(GL_DEPTH_ATTACHMENT, m_shadow_texture);
	for (size_t i = 0; i < 3; ++i)
	{
		fbo_format.attachment(GL_COLOR_ATTACHMENT0 + i, gl::Texture2d::create(k_shadow_map_resolution_x, k_shadow_map_resolution_y, color_format));
	}
	m_shadow_fbo = gl::Fbo::create(k_shadow_map_resolution_x, k_shadow_map_resolution_y, fbo_format);

	// setup the scene camera matrix
	m_scene_camera.setPerspective(45.0f, getWindowAspectRatio(), 0.1f, 1000.0f);
	m_scene_camera.lookAt(vec3(0.0f, 0.0f, -5.0f), vec3(0.0f));

	// setup the light camera matrix
	m_light_camera.setPerspective(100.0f, m_shadow_fbo->getAspectRatio(), 0.5f, 10.0f);
	m_light_camera.lookAt(vec3(0.0f, 3.0f, 1.0f), vec3(0.0f));

	// setup render batches
	auto sphere_geom = geom::Sphere().subdivisions(50).radius(0.25f);
	auto cube_geom = geom::Cube().size(vec3(2.0f));

	auto shadow_glsl = gl::GlslProg::create(loadAsset("rsm.vert"), loadAsset("rsm.frag"));
	m_shadow_sphere_batch = gl::Batch::create(sphere_geom, shadow_glsl);
	m_shadow_cube_batch = gl::Batch::create(cube_geom, shadow_glsl);

	m_scene_camera_ui = CameraUi(&m_light_camera, getWindow());
}

void ReflectiveShadowMapsApp::update()
{
	// draw the scene into the shadow map
	gl::ScopedFramebuffer scpFramebuffer(m_shadow_fbo);
	gl::ScopedViewport scpViewport(vec2(0.0f), m_shadow_fbo->getSize());
	gl::clear(Color::black());
	gl::ScopedMatrices scpMatrices;
	gl::setMatrices(m_light_camera);

	gl::color(Color::white());
	m_shadow_sphere_batch->draw();

	gl::color(Color("darkorange"));
	m_shadow_cube_batch->draw();
}

void ReflectiveShadowMapsApp::draw()
{
	gl::clear(); 
	gl::setMatricesWindow(getWindowSize());
	//gl::draw(m_shadow_texture, Rectf(0.0f, 0.0f, getWindowWidth() * 0.25f, getWindowHeight() * 0.25f));

	for (size_t i = 0; i < 3; ++i)
	{
		auto attachment = m_shadow_fbo->getTexture2d(GL_COLOR_ATTACHMENT0 + i);
		vec2 tl = vec2(i * getWindowWidth() * 0.25f, 0.0f);
		vec2 br = vec2((i + 1)* getWindowWidth() * 0.25f, getWindowHeight() * 0.25f);

		gl::draw(attachment, Rectf(tl, br));
	}
}

CINDER_APP(ReflectiveShadowMapsApp, RendererGl, [](App::Settings *settings) {
	settings->setWindowSize(720, 720);
})
