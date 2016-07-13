#pragma once

#include "cinder/Cinder.h"
#include "cinder/app/App.h"
#include "cinder/AxisAlignedBox.h"
#include "cinder/Rand.h"
#include "cinder/gl/Vbo.h"
#include "flex.h"
#include "Scene.h"

namespace cinder 
{
	namespace flex
	{
		class Solver;
		using SolverRef = std::shared_ptr<Solver>;

		class Solver
		{
		public:
			//! an enumeration for specifying particle attributes
			enum class SolverAttrib
			{
				POSITION,
				VELOCITY,
				INDEX,
				PHASE
			};

			//! a struct that holds information about this solver
			struct Format
			{
				Format();

				//! the time interval used to move the simulation forward
				Format& timeStep(float aTimeStep) { mTimeStep = aTimeStep; return *this; }

				//! the number of substeps taken per update
				Format& subStep(float aSubSteps) { mSubSteps = aSubSteps; return *this; }

				//! the parameters used internally by the solver
				Format& params(const FlexParams &aParams) { mParams = aParams; return *this; }

				//! controls whether or not the solver will update
				Format& active(bool aIsActive) { mIsActive = aIsActive; return *this; }

				float mTimeStep;
				int mSubSteps;
				FlexParams mParams;
				bool mIsActive;
			};

			//! factory method for constructing a new solver
			static SolverRef create(uint32_t aMaxParticles, uint32_t aMaxDiffuse = 0, const Format &aFormat = Format());

			//! factory method for constructing a new solver
			static SolverRef create(const SceneRef &aScene, const Format &aFormat = Format());

			//! returns the default parameter set used to initialize a solver
			static FlexParams getDefaultParams();

			//! constructs a new solver with the specified number of particles and format
			Solver(uint32_t aMaxParticles, uint32_t aMaxDiffuse = 0, const Format &aFormat = Format());

			//! constructs a new solver from the specified solver scene
			Solver(const SceneRef &aScene, const Format &aFormat = Format());

			//! destroys the solver and cleans up flex resources
			~Solver();

			//! moves the simulation forward in time
			void update();

			//! pauses or resumes the simulation
			void enable(bool aActive = true);

			//! transfers n particles from the GPU into the specified Vbo
			void transferToVbo(const gl::VboRef &aVbo, int aNumToTransfer, SolverAttrib aAttrib = SolverAttrib::POSITION) const;

			//! sets up a six plane bounding region
			void setBoundingBox(const AxisAlignedBox &aBox, bool aTopOpen = true);

			//! sets up a single bounding plane
			void setCollisionPlane(const vec4 &aPlane);

		private:
			void initFromFormat(const Solver::Format &aFormat);
			void setupDeviceMemory();

			FlexSolver *mSolver;
			float mTimeStep;
			int mSubSteps;
			FlexParams mParams;
			bool mIsActive;
			std::vector<vec4> mPositions;
			std::vector<vec3> mVelocities;
			std::vector<int> mPhases;
			std::vector<int> mActiveIndices;
			uint32_t mMaxParticles;
			uint32_t mMaxDiffuse;
			Format mFormat;
		};
	}
}