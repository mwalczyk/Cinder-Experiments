#pragma once

#include "cinder/Cinder.h"
#include "cinder/app/App.h"
#include "cinder/GeomIo.h"
#include <vector>

namespace cinder
{
	namespace flex
	{
		class Scene;
		using SceneRef = std::shared_ptr<Scene>;

		class Scene
		{
		public:
			//! constructs a scene from a geometry source
			Scene(const geom::Source &aSource);
			
		private:
			std::vector<vec4> mPositions;
			std::vector<vec3> mVelocities;
			std::vector<int> mPhases;
			std::vector<int> mActiveIndices;
		};
	}
}