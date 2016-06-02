#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "KNNClassifier.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class KNearestNeighborsApp : public App 
{
  public:
	void setup() override;
	void mouseDown(MouseEvent event) override;
	void update() override;
	void draw() override;
	
	KNNClassifier<char, double> clf;
};

void KNearestNeighborsApp::setup()
{
	std::map<char, vector<vector<double>>> data = {
		{'a', 
			{
				{1.0, 1.0},
				{2.0, 0.0 },
				{2.0, 1.0 }
			}
		},
		{ 'b',
			{
				{5.0, 6.0 },
				{7.0, 8.0 },
				{8.0, 6.0 }
			}
		},
	};

	clf.loadTrainingData(data);
	clf.predict({ 9.0, 7.0 });
}

void KNearestNeighborsApp::mouseDown(MouseEvent event)
{
}

void KNearestNeighborsApp::update()
{
}

void KNearestNeighborsApp::draw()
{
	gl::clear(Color::gray(0.15f)); 
	clf.drawTrainingData();
}

CINDER_APP(KNearestNeighborsApp, RendererGl)
