#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/Utilities.h"
#include "CinderImGui.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class ImGuiBasicApp : public App 
{
public:
	void setup() override;
	void mouseDown(MouseEvent event) override;
	void update() override;
	void draw() override;
protected:
	gl::BatchRef mRenderBatch;
	Color mColor;
};

void ImGuiBasicApp::setup()
{
	mRenderBatch = gl::Batch::create(geom::Circle().radius(100.0f), gl::getStockShader(gl::ShaderDef().color()));
	mColor = Color::white();
	ui::initialize();
}

void ImGuiBasicApp::mouseDown(MouseEvent event)
{
}

void ImGuiBasicApp::update()
{
	static bool showWindowWithMenu = true;

	if (showWindowWithMenu) {
		{
			ui::ScopedWindow window("Circle Parameters", ImGuiWindowFlags_MenuBar);

			// setup the window menu bar
			if (ui::BeginMenuBar()) {
				if (ui::BeginMenu("Edit")) 
				{
					ui::MenuItem("Copy");
					ui::MenuItem("Cut");
					ui::MenuItem("Paste");
					ui::EndMenu();
				}
				ui::EndMenuBar();
			}

			// add some text
			ui::Text("Circle Color");

			// add a color slider
			ui::ColorEdit3("Color", &mColor[0]);
		}
	}
	{
		ui::ScopedMainMenuBar menuBar;

		// add a file menu
		if (ui::BeginMenu("File")) 
		{
			if (ui::MenuItem("Save Screenshot"))
			{
				writeImage(getHomeDirectory() / "cinder" / "saveImage_" / (toString(getElapsedFrames()) + ".png"), copyWindowSurface());
			}
			ui::EndMenu();
		}

		// and a view menu
		if (ui::BeginMenu("View")) 
		{
			ui::MenuItem("Circle Parameters", nullptr, &showWindowWithMenu);
			ui::EndMenu();
		}
	}
}

void ImGuiBasicApp::draw()
{
	gl::clear();
	
	gl::ScopedMatrices scpMatrices;
	gl::ScopedModelMatrix scpModelMatrix;
	gl::setMatricesWindow(getWindowSize());
	gl::translate(getWindowCenter());
	
	gl::ScopedColor scpColor(mColor);
	mRenderBatch->draw();
}

CINDER_APP(ImGuiBasicApp, RendererGl(RendererGl::Options().msaa(8)))
