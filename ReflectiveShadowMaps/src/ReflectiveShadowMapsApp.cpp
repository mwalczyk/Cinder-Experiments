#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/CameraUi.h"

// references:
// http://ericpolman.com/reflective-shadow-maps/
// http://ericpolman.com/reflective-shadow-maps-part-2-the-implementation/
// http://learnopengl.com/#!Advanced-Lighting/Shadows/Shadow-Mapping

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

	gl::GlslProgRef m_scene_glsl;

	gl::BatchRef m_scene_sphere_batch;
	gl::BatchRef m_scene_cube_batch;
	gl::BatchRef m_shadow_sphere_batch;
	gl::BatchRef m_shadow_cube_batch;
};

void ReflectiveShadowMapsApp::setup()
{
	gl::enableDepth();
	gl::enableAlphaBlending();

	// create the shadow map texture
	auto depth_format = gl::Texture2d::Format()
		.internalFormat(GL_DEPTH_COMPONENT32F)
		//.compareMode(GL_COMPARE_REF_TO_TEXTURE)
		.magFilter(GL_LINEAR)
		.minFilter(GL_LINEAR)
		.wrap(GL_CLAMP_TO_EDGE)
		.compareFunc(GL_LEQUAL);
	m_shadow_texture = gl::Texture2d::create(k_shadow_map_resolution_x, k_shadow_map_resolution_y, depth_format);

	auto color_format = gl::Texture2d::Format()
		.internalFormat(GL_RGBA32F);

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
	auto sphere_geom = geom::Sphere()
		.subdivisions(50)
		.radius(0.25f)
		.center(vec3(0.0f, 0.5f, 0.0f));
	auto plane_geom = geom::Plane()
		.size(vec2(4.0f));
	auto cube_geom = geom::Cube()
		.size(vec3(2.0f))
		.colors(ColorAf(1.0f, 0.0f, 0.0f, 1.0f), ColorAf(0.0f, 1.0f, 0.0f, 1.0f),
				ColorAf(0.0f, 0.0f, 0.0f, 0.0f), ColorAf(1.0f, 1.0f, 1.0f, 1.0f),
				ColorAf(0.0f, 0.0f, 0.0f, 0.0f), ColorAf(0.0f, 0.0f, 1.0f, 1.0f));

	auto shadow_glsl = gl::GlslProg::create(loadAsset("rsm.vert"), loadAsset("rsm.frag"));
	m_shadow_sphere_batch = gl::Batch::create(sphere_geom, shadow_glsl);
	m_shadow_cube_batch = gl::Batch::create(plane_geom, shadow_glsl);

	m_scene_glsl = gl::GlslProg::create(loadAsset("combine.vert"), loadAsset("combine.frag"));
	m_scene_sphere_batch = gl::Batch::create(sphere_geom, m_scene_glsl);
	m_scene_cube_batch = gl::Batch::create(plane_geom, m_scene_glsl);

	m_scene_camera_ui = CameraUi(&m_scene_camera, getWindow());
}

void ReflectiveShadowMapsApp::update()
{
	// draw the scene into the shadow map
	gl::ScopedFramebuffer scpFramebuffer(m_shadow_fbo);
	gl::ScopedViewport scpViewport(vec2(0.0f), m_shadow_fbo->getSize());
	gl::clear(Color::black());
	gl::ScopedMatrices scpMatrices;
	gl::setMatrices(m_light_camera);

	{
		gl::ScopedColor scpColor(Color(1.0f, 0.0f, 0.0f));
		m_shadow_cube_batch->draw();
	}
	{
		gl::ScopedColor scpColor(Color::white());
		m_shadow_sphere_batch->draw();
	}
	
}

void ReflectiveShadowMapsApp::draw()
{
	gl::clear(); 
	gl::setMatricesWindow(getWindowSize());
	gl::draw(m_shadow_texture, Rectf(0.0f, 0.0f, getWindowWidth() * 0.25f, getWindowHeight() * 0.25f));

	for (size_t i = 0; i < 3; ++i)
	{
		auto attachment = m_shadow_fbo->getTexture2d(GL_COLOR_ATTACHMENT0 + i);
		vec2 tl = vec2((i + 1) * getWindowWidth() * 0.25f, 0.0f);
		vec2 br = vec2((i + 2)* getWindowWidth() * 0.25f, getWindowHeight() * 0.25f);

		gl::draw(attachment, Rectf(tl, br));
	}

	// draw the combined scene, using the RSM
	gl::setMatrices(m_scene_camera);
	mat4 shadow_matrix = m_light_camera.getProjectionMatrix() * m_light_camera.getViewMatrix();

	m_scene_glsl->uniform("u_shadow_matrix", shadow_matrix);
	gl::ScopedTextureBind scpTextureBind0(m_shadow_fbo->getTexture2d(GL_COLOR_ATTACHMENT0), 0);
	gl::ScopedTextureBind scpTextureBind1(m_shadow_fbo->getTexture2d(GL_COLOR_ATTACHMENT1), 1);
	gl::ScopedTextureBind scpTextureBind2(m_shadow_fbo->getTexture2d(GL_COLOR_ATTACHMENT2), 2);
	gl::ScopedTextureBind scpTextureBind3(m_shadow_texture, 3);

	m_scene_cube_batch->draw();
	m_shadow_sphere_batch->draw();
}

CINDER_APP(ReflectiveShadowMapsApp, RendererGl, [](App::Settings *settings) {
	settings->setWindowSize(720, 720);
})
