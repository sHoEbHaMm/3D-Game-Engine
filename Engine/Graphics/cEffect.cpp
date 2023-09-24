#include<Engine/Graphics/cEffect.h>
#include <Engine/Logging/Logging.h>


eae6320::Graphics::cEffect::~cEffect()
{
	EAE6320_ASSERT(m_referenceCount == 0);
	const auto result = CleanUp();
	EAE6320_ASSERT(result);
}

eae6320::cResult eae6320::Graphics::cEffect::Load(cEffect*& o_effect, std::string shaderPath)
{
	{
		auto result = Results::Success;

		cEffect* newEffect = nullptr;

		cScopeGuard scopeGuard([&o_effect, &result, &newEffect]
			{
				if (result)
				{
					EAE6320_ASSERT(newEffect != nullptr);
					o_effect = newEffect;
				}
				else
				{
					if (newEffect)
					{
						newEffect->DecrementReferenceCount();
						newEffect = nullptr;
					}
					o_effect = nullptr;
				}
			});

		// Allocate a new shader
		{
			newEffect = new (std::nothrow) cEffect();

			if (!newEffect)
			{
				result = Results::OutOfMemory;
				EAE6320_ASSERTF(false, "Couldn't allocate memory for the shader %s");
				Logging::OutputError("Failed to allocate memory for the shader %s");
				return result;
			}
		}

		if (!(result = newEffect->InitializeShadingData(shaderPath)))
		{
			EAE6320_ASSERTF(false, "Can't initialize Graphics without the geometry data");
			return result;
		}

		return result;
	}
}