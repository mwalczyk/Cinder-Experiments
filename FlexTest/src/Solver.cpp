#include "Solver.h"

namespace cinder
{
	namespace flex
	{
		Solver::Format::Format()
		{
			mTimeStep = 1.0f / 60.0f;
			mSubSteps = 1;
			mParams = getDefaultParams();
			mPhase = flexMakePhase(0, eFlexPhaseSelfCollide | eFlexPhaseFluid);
			mIsActive = true;
		}
		
		SolverRef Solver::create(uint32_t aMaxParticles, uint32_t aMaxDiffuse, const Format &aFormat)
		{
			return std::make_shared<Solver>(aMaxParticles, aMaxDiffuse, aFormat);
		}

	    SolverRef Solver::create(const SceneRef &aScene, const Format &aFormat)
		{
			return std::make_shared<Solver>(aScene, aFormat);
		}

		FlexParams Solver::getDefaultParams()
		{
			static FlexParams defaultParams;
			
			defaultParams.mGravity[0] = 0.0f;
			defaultParams.mGravity[1] = -9.8f;
			defaultParams.mGravity[2] = 0.0f;

			defaultParams.mWind[0] = 0.0f;
			defaultParams.mWind[1] = 0.0f;
			defaultParams.mWind[2] = 0.0f;

			defaultParams.mRadius = 0.15f;
			defaultParams.mViscosity = 0.0f;
			defaultParams.mDynamicFriction = 0.0f;
			defaultParams.mStaticFriction = 0.0f;
			defaultParams.mParticleFriction = 0.0f;		// scale friction between particles by default
			defaultParams.mFreeSurfaceDrag = 0.0f;
			defaultParams.mDrag = 0.0f;
			defaultParams.mLift = 0.0f;
			defaultParams.mNumIterations = 3;
			defaultParams.mFluidRestDistance = defaultParams.mRadius * 0.6f;
			defaultParams.mSolidRestDistance = defaultParams.mRadius;
			defaultParams.mAnisotropyScale = 1.0f;
			defaultParams.mDissipation = 0.0f;
			defaultParams.mDamping = 0.0f;
			defaultParams.mParticleCollisionMargin = 0.0f;
			defaultParams.mShapeCollisionMargin = 0.0f;
			defaultParams.mCollisionDistance = 0.0f;
			defaultParams.mPlasticThreshold = 0.0f;
			defaultParams.mPlasticCreep = 0.0f;
			defaultParams.mFluid = true;
			defaultParams.mSleepThreshold = 0.0f;
			defaultParams.mShockPropagation = 0.0f;
			defaultParams.mRestitution = 0.0f;
			defaultParams.mSmoothing = 1.0f;
			defaultParams.mMaxVelocity = FLT_MAX;
			defaultParams.mRelaxationMode = eFlexRelaxationLocal;
			defaultParams.mRelaxationFactor = 1.0f;
			defaultParams.mSolidPressure = 1.0f;
			defaultParams.mAdhesion = 0.9f;
			defaultParams.mCohesion = 0.225f;
			defaultParams.mSurfaceTension = 0.0f;
			defaultParams.mVorticityConfinement = 0.9f;
			defaultParams.mBuoyancy = 1.0f;
			defaultParams.mDiffuseThreshold = 100.0f;
			defaultParams.mDiffuseBuoyancy = 1.0f;
			defaultParams.mDiffuseDrag = 0.8f;
			defaultParams.mDiffuseBallistic = 16;
			defaultParams.mDiffuseSortAxis[0] = 0.0f;
			defaultParams.mDiffuseSortAxis[1] = 0.0f;
			defaultParams.mDiffuseSortAxis[2] = 0.0f;
			defaultParams.mEnableCCD = false;

			// collision boundaries zzz
			const float boxSize = 3.0f;
			(vec4&)defaultParams.mPlanes[0] = vec4(0.0f, 1.0f, 0.0f, boxSize);
			(vec4&)defaultParams.mPlanes[1] = vec4(1.0f, 0.0f, 0.0f, boxSize);
			(vec4&)defaultParams.mPlanes[2] = vec4(-1.0f, 0.0f, 0.0f, boxSize);
			(vec4&)defaultParams.mPlanes[3] = vec4(0.0f, 0.0f, 1.0f, boxSize);
			(vec4&)defaultParams.mPlanes[4] = vec4(0.0f, 0.0f, -1.0f, boxSize);
			(vec4&)defaultParams.mPlanes[5] = vec4(0.0f, -1.0f, 0.0f, boxSize);
			defaultParams.mNumPlanes = 6;
			
			return defaultParams;
		}

		Solver::Solver(uint32_t aMaxParticles, uint32_t aMaxDiffuse, const Format &aFormat) :
			mMaxParticles(aMaxParticles),
			mMaxDiffuse(aMaxDiffuse)
		{
			initFromFormat(aFormat);
			flexInit();

			// create the flex solver
			mSolver = flexCreateSolver(mMaxParticles, mMaxDiffuse);
			flexSetParams(mSolver, &mParams);

			// positions
			mPositions.resize(mMaxParticles);
			std::generate(mPositions.begin(), mPositions.end(), []() { return vec4(randVec3(), 1.0f); });

			// velocities
			mVelocities.resize(mMaxParticles, vec3(0.0, 0.0, 0.0));

			setupDeviceMemory();
		}

		Solver::Solver(const SceneRef &aScene, const Format &aFormat) :
			mMaxParticles(aScene->mPositions.size()),
			mMaxDiffuse(0)
		{
			initFromFormat(aFormat);
			flexInit();

			// create the flex solver and transfer ownership of position / velocity data
			mSolver = flexCreateSolver(mMaxParticles, mMaxDiffuse);
			flexSetParams(mSolver, &mParams);
			mPositions = aScene->mPositions;
			mVelocities = aScene->mVelocities;

			setupDeviceMemory();
		}

		void Solver::initFromFormat(const Solver::Format &aFormat)
		{
			mTimeStep = aFormat.mTimeStep;
			mSubSteps = aFormat.mSubSteps;
			mParams = aFormat.mParams;
			mPhase = aFormat.mPhase;
			mIsActive = aFormat.mIsActive;
		}

		void Solver::setupDeviceMemory()
		{
			// generate two vbos: one for position data and one for velocity data
			mPositionVbo = gl::Vbo::create(GL_ARRAY_BUFFER, sizeof(vec4) * mMaxParticles, nullptr, GL_DYNAMIC_DRAW);
			mVelocityVbo = gl::Vbo::create(GL_ARRAY_BUFFER, sizeof(vec3) * mMaxParticles, nullptr, GL_DYNAMIC_DRAW);

			// allow cuda to access the vbos
			auto errPos = cudaGraphicsGLRegisterBuffer(&mPositionCudaId, mPositionVbo->getId(), cudaGraphicsRegisterFlagsWriteDiscard);
			auto errVel = cudaGraphicsGLRegisterBuffer(&mVelocityCudaId, mVelocityVbo->getId(), cudaGraphicsRegisterFlagsWriteDiscard);
			if (errPos != cudaSuccess || errVel != cudaSuccess)
			{
				throw SolverInteropException();
			}

			// setup particle phases: by default, all particles share the same phase
			mPhases.resize(mMaxParticles, mPhase);

			// set up active indices: by default, all particles are made active
			size_t currIndex = 0;
			mActiveIndices.resize(mMaxParticles);
			std::generate(mActiveIndices.begin(), mActiveIndices.end(), [&]() { return currIndex++; });

			// transfer particle information to the gpu
			setParticles(mPositions);
			setVelocities(mVelocities);
			setActive(mActiveIndices);
			setPhases(mPhases);
		}

		Solver::~Solver()
		{
			flexDestroySolver(mSolver);
			flexShutdown();
		}

		void Solver::setParticles(const std::vector<vec4> &aParticles)
		{
			flexSetParticles(mSolver, (float*)&aParticles[0], mMaxParticles, eFlexMemoryHostAsync);
		}

		void Solver::setVelocities(const std::vector<vec3> &aVelocities)
		{
			flexSetVelocities(mSolver, (float*)&aVelocities[0], mMaxParticles, eFlexMemoryHostAsync);
		}

		void Solver::setActive(const std::vector<int> &aActiveIndices)
		{
			flexSetActive(mSolver, &aActiveIndices[0], mMaxParticles, eFlexMemoryHost);
		}

		void Solver::setPhases(const std::vector<int> &aPhases)
		{
			flexSetPhases(mSolver, &aPhases[0], mMaxParticles, eFlexMemoryHost);
		}
		
		void Solver::wait()
		{
			flexSetFence();
			flexWaitFence();
		}

		const std::vector<vec4>& Solver::getParticles()
		{
			flexGetParticles(mSolver, (float*)&mPositions[0], mMaxParticles, eFlexMemoryHostAsync);
			wait();
			return mPositions;
		}

		const std::vector<vec3>& Solver::getVelocities()
		{
			flexGetVelocities(mSolver, (float*)&mVelocities[0], mMaxParticles, eFlexMemoryHostAsync);
			wait();
			return mVelocities;
		}

		const std::vector<int>& Solver::getActive()
		{
			flexGetActive(mSolver, &mActiveIndices[0], eFlexMemoryHostAsync);
			wait();
			return mActiveIndices;
		}

		const std::vector<int>& Solver::getPhases()
		{
			flexGetPhases(mSolver, &mPhases[0], mMaxParticles, eFlexMemoryDeviceAsync);
			wait();
			return mPhases;
		}

		const gl::VboRef& Solver::getVbo(SolverAttrib aAttrib) const
		{
			switch (aAttrib)
			{
			case SolverAttrib::POSITION:
				return mPositionVbo;
			case SolverAttrib::VELOCITY:
				return mVelocityVbo;
			default:
				break;
			}
		}

		void Solver::update()
		{
			if (mIsActive)
			{
				cudaGraphicsResource* resources[2];
				resources[0] = mPositionCudaId;
				resources[1] = mVelocityCudaId;
				cudaGraphicsMapResources(2, resources);

				flexUpdateSolver(mSolver, mTimeStep, mSubSteps, nullptr);

				// don't know why we have to perform this transfer to the cpu...
				int numParticles = flexGetActiveCount(mSolver);
				void *positionPtr;
				void *velocityPtr;
				size_t s;

				cudaGraphicsResourceGetMappedPointer(&positionPtr, &s, mPositionCudaId);
				flexGetParticles(mSolver, (float*)positionPtr, numParticles, eFlexMemoryDeviceAsync);
				
				cudaGraphicsResourceGetMappedPointer(&velocityPtr, &s, mVelocityCudaId);
				flexGetVelocities(mSolver, (float*)velocityPtr, numParticles, eFlexMemoryDeviceAsync);

				cudaGraphicsUnmapResources(2, resources);
			}
		}

		void Solver::enable(bool aActive)
		{
			mIsActive = aActive;
		}

		void Solver::transferToVbo(const gl::VboRef &aVbo, int aNumToTransfer, SolverAttrib aAttrib) 
		{
			flexGetParticles(mSolver, (float*)&mPositions[0], aNumToTransfer, eFlexMemoryHostAsync);
			wait();

			vec4 *vboData = (vec4*)aVbo->mapReplace();
			memcpy(vboData, &mPositions[0], sizeof(vec4) * aNumToTransfer);
			aVbo->unmap();
		}

		void Solver::setCollisionBox(const AxisAlignedBox &aBox, const mat4 aTransform, bool aTopOpen)
		{
			auto sz = aBox.getSize() * 0.5f;
			(vec4&)mParams.mPlanes[0] = vec4(0.0f, -1.0f, 0.0f, sz.y);	// +y
			(vec4&)mParams.mPlanes[1] = vec4(0.0f, 1.0f, 0.0f, sz.y);	// -y
			(vec4&)mParams.mPlanes[2] = vec4(1.0f, 0.0f, 0.0f, sz.x);	// +x
			(vec4&)mParams.mPlanes[3] = vec4(-1.0f, 0.0f, 0.0f, sz.x);	// -x
			(vec4&)mParams.mPlanes[4] = vec4(0.0f, 0.0f, 1.0f, sz.z);	// +z
			(vec4&)mParams.mPlanes[5] = vec4(0.0f, 0.0f, -1.0f, sz.z);	// -z
			mParams.mNumPlanes = 6;

			// update params
			flexSetParams(mSolver, &mParams);
		}

		AxisAlignedBox Solver::calcBoundingBox() const
		{
			vec3 upper, lower;
			flexGetBounds(mSolver, &upper[0], &lower[0]);
			return AxisAlignedBox(lower, upper);
		}
	}
}