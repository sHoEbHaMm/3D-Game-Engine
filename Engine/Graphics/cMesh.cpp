#include <Engine/Graphics/cMesh.h>
#include <Engine/Logging/Logging.h>
#include<new>

eae6320::Graphics::cMesh::~cMesh()
{
	EAE6320_ASSERT(m_referenceCount == 0);
	const auto result = CleanUp();
	EAE6320_ASSERT(result);
}

eae6320::cResult eae6320::Graphics::cMesh::Load(cMesh*& o_mesh, uint16_t index[], eae6320::Graphics::VertexFormats::sVertex_mesh vertexData[])
{
	auto result = Results::Success;

	cMesh* newMesh = nullptr;

	cScopeGuard scopeGuard([&o_mesh, &result, &newMesh]
		{
			if (result)
			{
				EAE6320_ASSERT(newMesh != nullptr);
				o_mesh = newMesh;
			}
			else
			{
				if (newMesh)
				{
					newMesh->DecrementReferenceCount();
					newMesh = nullptr;
				}
				o_mesh = nullptr;
			}
		});

	// Allocate a new shader
	{
		newMesh = new (std::nothrow) cMesh();
		if (!newMesh)
		{
			result = Results::OutOfMemory;
			EAE6320_ASSERTF(false, "Couldn't allocate memory for the shader %s");
			Logging::OutputError("Failed to allocate memory for the shader %s");
			return result;
		}
	}

	if (!(result = newMesh->InitializeGeometry(index, vertexData)))
	{
		EAE6320_ASSERTF(false, "Can't initialize Graphics without the geometry data");
		return result;
	}

	return result;
}


