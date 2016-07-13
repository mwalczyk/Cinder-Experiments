#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/Rand.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class TextBoxesApp : public App 
{
  public:
	void setup() override;
	void mouseDown(MouseEvent event) override;
	void update() override;
	void draw() override;

	gl::Texture2dRef mTextBoxTexture;
	gl::Texture2dRef mTextLayoutTexture;
	vector<gl::Texture2dRef> mFontTextures;
	int mScrollAmount;
	bool mToggleContent;
};

void TextBoxesApp::setup()
{	
	// setup a simple text box and load it into an OpenGL texture for drawing
	TextBox box = TextBox()
		.text("Testing a small text box with lots of text.")
		.alignment(TextBox::CENTER)
		.font(Font("Times New Roman", 32))
		.size(100, TextBox::GROW);
	box.setColor(Color(1.0f, 0.98f, 0.98f));
	box.setBackgroundColor(Color::gray(0.15f));

	auto sz = box.measure();
	console() << "Auto-sized text box: " << sz << endl;

	mTextBoxTexture = gl::Texture2d::create(box.render());

	// setup a simple text layout and load it into an OpenGL texture for drawing
	TextLayout layout;
	layout.setFont(Font("Times New Roman", 20));
	layout.clear(Color::gray(0.25f));
	layout.setColor(Color(0.9f, 0.1f, 0.3f));
	layout.addLine("This should be a left-justified line of text that is pretty long.");
	layout.addCenteredLine("This should be a centered line of text.");
	layout.addRightLine("This should be a right-justified line of text.");
	
	mTextLayoutTexture = gl::Texture2d::create(layout.render());

	// create a bunch of textures to showcase all of the system fonts
	for (const auto &fontName : Font::getNames())
	{
		TextBox box = TextBox()
			.text(fontName)
			.alignment(TextBox::CENTER)
			.font(Font(fontName, 18))
			.size(200, TextBox::GROW);
		
		Color bgColor;
		bgColor.set(CM_HSV, randVec3());

		box.setColor(Color(1.0f, 0.98f, 0.98f));
		box.setBackgroundColor(bgColor);
		
		mFontTextures.push_back(gl::Texture2d::create(box.render()));
	}

	mScrollAmount = 0;
	mToggleContent = false;

	getWindow()->getSignalKeyDown().connect([&](KeyEvent event) {
		switch (event.getCode())
		{
		case  KeyEvent::KEY_PAGEDOWN:
			mScrollAmount -= 20;
			break;
		case  KeyEvent::KEY_PAGEUP:
			mScrollAmount += 20; 
			break;
		case  KeyEvent::KEY_SPACE:
			mToggleContent = !mToggleContent;
			break;
		default:
			break;
		}
	});
}

void TextBoxesApp::mouseDown(MouseEvent event)
{
}

void TextBoxesApp::update()
{
}

void TextBoxesApp::draw()
{
	gl::clear(); 
	gl::setMatricesWindow(getWindowSize());

	if (mToggleContent)
	{
		// draw the textures
		gl::draw(mTextBoxTexture, mTextBoxTexture->getBounds());
		gl::draw(mTextLayoutTexture, mTextLayoutTexture->getBounds() + ivec2(mTextBoxTexture->getWidth(), 0));
	}
	else
	{
		// draw all of the font textures
		int yOffset = mScrollAmount;
		for (const auto &tex : mFontTextures)
		{
			auto bounds = tex->getBounds();
			gl::draw(tex, bounds + ivec2(0, yOffset));
			yOffset += bounds.getHeight();

			// don't draw textures beyond the window plane
			if (yOffset > getWindowHeight())
			{
				break;
			}
		}
	}
}

CINDER_APP(TextBoxesApp, RendererGl)
