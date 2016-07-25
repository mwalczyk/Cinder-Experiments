#include "Scene.h"

namespace cinder
{
	namespace flex
	{
		SceneRef Scene::create(const TriMeshRef &aMesh)
		{
			return std::make_shared<Scene>(aMesh);
		}

		SceneRef Scene::create(const geom::Source &aSource)
		{
			auto mesh = TriMesh::create(aSource);
			return std::make_shared<Scene>(mesh);
		}

		Scene::Scene(const TriMeshRef &aMesh)
		{
			vec3 *positions = aMesh->getPositions<3>();
			
			for (size_t i = 0; i < aMesh->getNumVertices(); ++i)
			{
				mPositions.push_back(vec4(positions[i], 1.0f));
			}

			mVelocities = aMesh->getNormals();

			for (size_t i = 0; i < mVelocities.size(); ++i)
			{
				mVelocities.at(i) += randVec3() * 5.0f;
			}
		}
	}
}