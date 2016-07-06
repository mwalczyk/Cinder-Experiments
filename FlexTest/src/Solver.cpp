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
		}

		SolverRef Solver::create(uint32_t aMaxParticles, uint32_t aMaxDiffuse, const Format &aFormat)
		{
			return std::make_shared<Solver>(aMaxParticles, aMaxDiffuse);
		}

		FlexParams Solver::getDefaultParams()
		{
			FlexParams defaultParams;
			
			float defaultRadius = 0.02f;
			defaultParams.mNumIterations = 1;
			defaultParams.mGravity[0] = 0.0f;
			defaultParams.mGravity[1] = -2.0f;
			defaultParams.mGravity[2] = 0.0f;
			defaultParams.mRadius = defaultRadius;
			defaultParams.mSolidRestDistance = defaultParams.mRadius;
			defaultParams.mFluidRestDistance = defaultParams.mRadius * 0.5f;
			defaultParams.mDynamicFriction = 0.0f;
			defaultParams.mStaticFriction = 0.0f;
			defaultParams.mParticleFriction = 0.0f;
			defaultParams.mRestitution = 0.4f;
			defaultParams.mAdhesion = 0.0f;
			defaultParams.mSleepThreshold = 0.0f;
			defaultParams.mMaxVelocity = FLT_MAX;
			defaultParams.mShockPropagation = 0.0f;
			defaultParams.mDissipation = 0.0f;
			defaultParams.mDamping = 0.0f;
			defaultParams.mEnableCCD = false;
			defaultParams.mWind[0] = 0.0f;
			defaultParams.mWind[1] = 0.0f;
			defaultParams.mWind[2] = 0.0f;
			defaultParams.mDrag = 0.0f;
			defaultParams.mLift = 0.0f;
			defaultParams.mFluid = true;
			defaultParams.mCohesion = 0.425f;
			defaultParams.mSurfaceTension = 0.0f;
			defaultParams.mViscosity = 0.0f;
			defaultParams.mVorticityConfinement = 0.0f;
			defaultParams.mAnisotropyScale = 0.0f;
			defaultParams.mSmoothing = 0.0f;
			defaultParams.mSolidPressure = 0.0f;
			defaultParams.mFreeSurfaceDrag = 0.0f;
			defaultParams.mBuoyancy = 1.0f;
			defaultParams.mDiffuseThreshold = 0.0f;
			defaultParams.mDiffuseBuoyancy = 0.0f;
			defaultParams.mDiffuseDrag = 0.0f;
			defaultParams.mDiffuseBallistic = 0;
			defaultParams.mDiffuseSortAxis[0] = 0.0f;
			defaultParams.mDiffuseSortAxis[1] = 0.0f;
			defaultParams.mDiffuseSortAxis[2] = 0.0f;
			defaultParams.mPlasticThreshold = 0.0f;
			defaultParams.mPlasticCreep = 0.0f;
			defaultParams.mCollisionDistance = 0.0f;
			defaultParams.mParticleCollisionMargin = 0.0f;
			defaultParams.mShapeCollisionMargin = 0.0f;
			defaultParams.mNumPlanes = 0;
			defaultParams.mRelaxationMode = eFlexRelaxationLocal;
			defaultParams.mRelaxationFactor = 1.0f;

			// collision boundaries
			(vec4&)defaultParams.mPlanes[0] = vec4(0.0f, 1.0f, 0.0f, 2.0f);
			(vec4&)defaultParams.mPlanes[1] = vec4(1.0f, 0.0f, 0.0f, 2.0f);
			(vec4&)defaultParams.mPlanes[2] = vec4(-1.0f, 0.0f, 0.0f, 2.0f);
			(vec4&)defaultParams.mPlanes[3] = vec4(0.0f, 0.0f, 1.0f, 2.0f);
			(vec4&)defaultParams.mPlanes[4] = vec4(0.0f, 0.0f, -1.0f, 2.0f);
			defaultParams.mNumPlanes = 5;
			
			return defaultParams;
		}

		Solver::Solver(uint32_t aMaxParticles, uint32_t aMaxDiffuse, const Format &aFormat) :
			mMaxParticles(aMaxParticles),
			mMaxDiffuse(aMaxDiffuse),
			mFormat(aFormat),
			mIsActive(true)
		{
			flexInit();

			// create the flex solver
			mSolver = flexCreateSolver(mMaxParticles, mMaxDiffuse);
			flexSetParams(mSolver, &mFormat.mParams);

			// positions
			mPositions.resize(mMaxParticles);
			std::generate(mPositions.begin(), mPositions.end(), []() { return vec4(randVec3(), 1.0f); });

			// velocities
			mVelocities.resize(mMaxParticles, vec3(0.0, 0.0, 0.0));

			// phases
			int defaultPhase = flexMakePhase(0, eFlexPhaseSelfCollide | eFlexPhaseFluid);
			mPhases.resize(mMaxParticles, defaultPhase);
			
			// indices
			int currIndex = 0;
			mActiveIndices.resize(mMaxParticles);
			std::generate(mActiveIndices.begin(), mActiveIndices.end(), [&]() { return currIndex++; });

			// transfer particle information to the GPU
			flexSetParticles(mSolver, (float*)&mPositions[0], mMaxParticles, eFlexMemoryHostAsync);
			flexSetVelocities(mSolver, (float*)&mVelocities[0], mMaxParticles, eFlexMemoryHostAsync);
			flexSetActive(mSolver, &mActiveIndices[0], mMaxParticles, eFlexMemoryHost);
			flexSetPhases(mSolver, &mPhases[0], mMaxParticles, eFlexMemoryHost);
		}

		Solver::~Solver()
		{
			flexDestroySolver(mSolver);
			flexShutdown();
		}
		
		void Solver::update()
		{
			if (mIsActive)
			{
				static const float dt = 1.0f / 60.0f;
				flexUpdateSolver(mSolver, mFormat.mTimeStep, mFormat.mSubSteps, nullptr);
			}
		}

		void Solver::enable(bool aActive)
		{
			mIsActive = aActive;
		}

		void Solver::transferToVbo(const gl::VboRef &aVbo, int aNumToTransfer, Solver::SolverAttrib aAttrib) const
		{
			switch (aAttrib)
			{
			case Solver::SolverAttrib::POSITION:
				break;
			case Solver::SolverAttrib::VELOCITY:
				break;
			case Solver::SolverAttrib::INDEX:
				break;
			case Solver::SolverAttrib::PHASE:
				break;
			default:
				break;
			}

			// kick off async memory reads from device
			flexGetParticles(mSolver, (float*)&mPositions[0], aNumToTransfer, eFlexMemoryHostAsync);

			// wait for GPU to finish working (can perform asynchronous CPU work here)
			flexSetFence();
			flexWaitFence();

			vec4 *vboData = (vec4*)aVbo->mapReplace();
			memcpy(vboData, &mPositions[0], sizeof(vec4) * aNumToTransfer);
			aVbo->unmap();
		}

		void Solver::setBoundingBox(const AxisAlignedBox &aBox, bool aTopOpen)
		{
			auto sz = aBox.getSize();
			(vec4&)mFormat.mParams.mPlanes[0] = vec4(0.0f, -1.0f, 0.0f, sz.y);	// +y
			(vec4&)mFormat.mParams.mPlanes[1] = vec4(0.0f, 1.0f, 0.0f, sz.y);	// -y
			(vec4&)mFormat.mParams.mPlanes[2] = vec4(1.0f, 0.0f, 0.0f, sz.x);	// +x
			(vec4&)mFormat.mParams.mPlanes[3] = vec4(-1.0f, 0.0f, 0.0f, sz.x);	// -x
			(vec4&)mFormat.mParams.mPlanes[4] = vec4(0.0f, 0.0f, 1.0f, sz.z);	// +z
			(vec4&)mFormat.mParams.mPlanes[5] = vec4(0.0f, 0.0f, -1.0f, sz.z);	// -z
			mFormat.mParams.mNumPlanes = 6;

			// update params
			flexSetParams(mSolver, &mFormat.mParams);
		}
	}
}