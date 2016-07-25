#pragma once

#include "cinder/Cinder.h"
#include "cinder/app/App.h"
#include "cinder/Log.h"
#include "cinder/AxisAlignedBox.h"
#include "cinder/Rand.h"
#include "cinder/gl/Vbo.h"
#include "cinder/Exception.h"

#include "cuda.h"
#include "cuda_runtime_api.h"
#include "cuda_gl_interop.h"

#include "flex.h"

#include "Scene.h"

namespace cinder 
{
	namespace flex
	{
		//! an enumeration for specifying particle attributes
		enum class SolverAttrib
		{
			POSITION,
			VELOCITY,
			INDEX,
			PHASE
		};

		class Solver;
		using SolverRef = std::shared_ptr<Solver>;

		class Solver
		{
		public:
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

				//! the phase of the particles
				Format& phase(FlexPhase aPhase, int aGroup = 0) { mPhase = flexMakePhase(aGroup, aPhase); return *this; }

				//! controls whether or not the solver will update
				Format& active(bool aIsActive) { mIsActive = aIsActive; return *this; }

				float mTimeStep;
				int mSubSteps;
				FlexParams mParams;
				int mPhase;
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

			//! set the particle positions
			void setParticles(const std::vector<vec4> &aParticles);

			//! set the particle velocities
			void setVelocities(const std::vector<vec3> &aVelocities);

			//! set the particle active indices
			void setActive(const std::vector<int> &aActiveIndices);

			//! set the particle phases
			void setPhases(const std::vector<int> &aPhases);

			//! asynchronously transfer particle positions to the cpu
			const std::vector<vec4>& getParticles();

			//! asynchronously transfer particle velocities to the cpu
			const std::vector<vec3>& getVelocities();

			//! asynchronously transfer particle active indices to the cpu
			const std::vector<int>& getActive();

			//! asynchronously transfer particle phases to the cpu
			const std::vector<int>& getPhases();

			//! returns a handle to the solver's vbo 
			const gl::VboRef& getVbo(SolverAttrib aAttrib) const;

			//! move the simulation forward in time
			void update();

			//! pauses or resumes the simulation
			void enable(bool aActive = true);

			//! transfers n particles from the gpu into the specified vbo
			void transferToVbo(const gl::VboRef &aVbo, int aNumToTransfer, SolverAttrib aAttrib = SolverAttrib::POSITION);

			//! sets up a six plane bounding region
			void setCollisionBox(const AxisAlignedBox &aBox, const mat4 aTransform = mat4(1.0f), bool aTopOpen = true);

			//! sets up a single bounding plane
			void setCollisionPlane(const vec4 &aPlane, size_t aIndex);

			//! get the bounding box of the solver
			AxisAlignedBox calcBoundingBox() const;

		private:
			void initFromFormat(const Solver::Format &aFormat);
			void setupDeviceMemory();
			void wait();

			FlexSolver *mSolver;

			float mTimeStep;
			int mSubSteps;
			FlexParams mParams;
			int mPhase;
			bool mIsActive;
			
			cudaGraphicsResource_t mPositionCudaId;
			cudaGraphicsResource_t mVelocityCudaId;
			gl::VboRef mPositionVbo;
			gl::VboRef mVelocityVbo;

			std::vector<vec4> mPositions;
			std::vector<vec3> mVelocities;
			std::vector<int> mPhases;
			std::vector<int> mActiveIndices;
			uint32_t mMaxParticles;
			uint32_t mMaxDiffuse;
		};

		class SolverException : public Exception
		{
		public:
			SolverException() {}
			SolverException(const std::string &aDescription) : Exception(aDescription) {}
		};

		class SolverInteropException : public SolverException
		{
		public:
			virtual const char* what() const throw()
			{
				return "Solver: Failed to register OpenGL buffer object with CUDA";
			}
		};

	} // namespace flex

} // namespace cinder