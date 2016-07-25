#pragma once

#include "cinder/Cinder.h"
#include "cinder/app/App.h"
#include "cinder/GeomIo.h"
#include "cinder/TriMesh.h"
#include "cinder/Rand.h"

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
			//! factory method for constructing a new scene from a triangle mesh
			static SceneRef create(const TriMeshRef &aMesh);

			//! factory method for constructing a new scene from a geometry source
			static SceneRef create(const geom::Source &aSource);

			//! constructs a scene from a triangle mesh
			Scene(const TriMeshRef &aMesh);
			
			//! constructs a scene from a geometry source
			Scene(const geom::Source &aSource);
		
		protected:
			std::vector<vec4> mPositions;
			std::vector<vec3> mVelocities;

			friend class Solver;
		};
	}
}