#include <Engine/Graphics/cMesh.h>
#include <Engine/Graphics/cEffect.h>
#include <Engine/Results/Results.h>
#include <Engine/Graphics/sContext.h>
#include <Engine/Asserts/Asserts.h>
#include <Engine/Graphics/Direct3D/Includes.h>
#include <Engine/Logging/Logging.h>
#include <Engine/Graphics/cConstantBuffer.h>
#include <Engine/Graphics/cShader.h>
#include <Engine/Graphics/ConstantBufferFormats.h>
#include <Engine/Concurrency/cEvent.h>
#include <Engine/UserOutput/UserOutput.h>

#include<algorithm>
using namespace std;

eae6320::cResult eae6320::Graphics::cMesh::Initialize(const sInitializationParameters& i_initializationParameters, uint16_t index[], eae6320::Graphics::VertexFormats::sVertex_mesh vertexData[])
{
	auto result = Results::Success;
	{
		if (!(result = InitializeGeometry(index,vertexData)))
		{
			EAE6320_ASSERTF(false, "Can't initialize Graphics without the geometry data");
			return result;
		}
	}

	return result;
}

void eae6320::Graphics::cMesh::Draw()
{
	// Draw the geometry
	{
		// Bind a specific vertex buffer to the device as a data source
		{
			EAE6320_ASSERT(s_vertexArrayId != 0);
			glBindVertexArray(s_vertexArrayId);
			EAE6320_ASSERT(glGetError() == GL_NO_ERROR);
		}
		// Render triangles from the currently-bound vertex buffer
		{
			// The mode defines how to interpret multiple vertices as a single "primitive";
			// a triangle list is defined
			// (meaning that every primitive is a triangle and will be defined by three vertices)
			constexpr GLenum mode = GL_TRIANGLES;
			// As of this comment only a single triangle is drawn
			// (you will have to update this code in future assignments!)
			constexpr unsigned int triangleCount = 2;
			constexpr unsigned int vertexCountPerTriangle = 3;
			constexpr auto indexCountToRender = triangleCount * vertexCountPerTriangle;
			// It's possible to start rendering primitives in the middle of the stream
			const GLvoid* const offset = 0;
			glDrawElements(mode, static_cast<GLsizei>(indexCountToRender), GL_UNSIGNED_SHORT, offset);
			EAE6320_ASSERT(glGetError() == GL_NO_ERROR);
		}
	}

	// Everything has been drawn to the "back buffer", which is just an image in memory.
	// In order to display it the contents of the back buffer must be "presented"
	// (or "swapped" with the "front buffer", which is the image that is actually being displayed)
}

eae6320::cResult eae6320::Graphics::cMesh::InitializeGeometry(uint16_t indexData[], eae6320::Graphics::VertexFormats::sVertex_mesh vertexData[])
{
	auto result = eae6320::Results::Success;

	// Create a vertex array object and make it active
	{
		constexpr GLsizei arrayCount = 1;
		glGenVertexArrays(arrayCount, &s_vertexArrayId);
		const auto errorCode = glGetError();
		if (errorCode == GL_NO_ERROR)
		{
			glBindVertexArray(s_vertexArrayId);
			const auto errorCode = glGetError();
			if (errorCode != GL_NO_ERROR)
			{
				result = eae6320::Results::Failure;
				EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
				eae6320::Logging::OutputError("OpenGL failed to bind a new vertex array: %s",
					reinterpret_cast<const char*>(gluErrorString(errorCode)));
				return result;
			}
		}
		else
		{
			result = eae6320::Results::Failure;
			EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
			eae6320::Logging::OutputError("OpenGL failed to get an unused vertex array ID: %s",
				reinterpret_cast<const char*>(gluErrorString(errorCode)));
			return result;
		}
	}
	// Create a vertex buffer object and make it active
	{
		constexpr GLsizei bufferCount = 1;
		glGenBuffers(bufferCount, &s_vertexBufferId);
		const auto errorCode = glGetError();
		if (errorCode == GL_NO_ERROR)
		{
			glBindBuffer(GL_ARRAY_BUFFER, s_vertexBufferId);
			const auto errorCode = glGetError();
			if (errorCode != GL_NO_ERROR)
			{
				result = eae6320::Results::Failure;
				EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
				eae6320::Logging::OutputError("OpenGL failed to bind a new vertex buffer: %s",
					reinterpret_cast<const char*>(gluErrorString(errorCode)));
				return result;
			}
		}
		else
		{
			result = eae6320::Results::Failure;
			EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
			eae6320::Logging::OutputError("OpenGL failed to get an unused vertex buffer ID: %s",
				reinterpret_cast<const char*>(gluErrorString(errorCode)));
			return result;
		}
	}

	//Create index buffer
	{
		constexpr GLsizei IndexBufferCount = 1;
		glGenBuffers(IndexBufferCount, &s_indexBufferId);
		const auto errorCode = glGetError();
		if (errorCode == GL_NO_ERROR)
		{
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, s_indexBufferId);
			const auto errorCode = glGetError();
			if (errorCode != GL_NO_ERROR)
			{
				result = eae6320::Results::Failure;
				EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
				eae6320::Logging::OutputError("OpenGL failed to bind a new vertex buffer: %s",
					reinterpret_cast<const char*>(gluErrorString(errorCode)));
				return result;
			}
		}
		else
		{
			result = eae6320::Results::Failure;
			EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
			eae6320::Logging::OutputError("OpenGL failed to get an unused vertex buffer ID: %s",
				reinterpret_cast<const char*>(gluErrorString(errorCode)));
			return result;
		}
	}
	// Assign the data to the buffer
	{
		constexpr unsigned int triangleCount = 2;
		constexpr unsigned int vertexCountPerTriangle = 3;
		//const auto vertexCount = triangleCount * vertexCountPerTriangle;
		//eae6320::Graphics::VertexFormats::sVertex_mesh vertexData[4];
		//{
		//	// OpenGL is right-handed
		//	//1, 2, 3, 4, 1, 3
		//	vertexData[0].x = 0.0f;
		//	vertexData[0].y = 0.0f;
		//	vertexData[0].z = 0.0f;

		//	vertexData[1].x = 1.0f;
		//	vertexData[1].y = 0.0f;
		//	vertexData[1].z = 0.0f;

		//	vertexData[2].x = 1.0f;
		//	vertexData[2].y = 1.0f;
		//	vertexData[2].z = 0.0f;

		//	vertexData[3].x = 0.0f;
		//	vertexData[3].y = 1.0f;
		//	vertexData[3].z = 0.0f;
		//}

		//uint16_t indexData[] = { 1,2,3,4,1,3 };
		indexData = adjustWinding(indexData);
		constexpr auto bufferSize = sizeof(vertexData[0]) * 4;
		EAE6320_ASSERT(bufferSize <= std::numeric_limits<GLsizeiptr>::max());
		glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(bufferSize), reinterpret_cast<GLvoid*>(vertexData),
			// In our class we won't ever read from the buffer
			GL_STATIC_DRAW);
		const auto errorCode = glGetError();
		if (errorCode != GL_NO_ERROR)
		{
			result = eae6320::Results::Failure;
			EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
			eae6320::Logging::OutputError("OpenGL failed to allocate the vertex buffer: %s",
				reinterpret_cast<const char*>(gluErrorString(errorCode)));
			return result;
		}

		constexpr auto IndexbufferSize = sizeof(indexData[0]) * 6;
		EAE6320_ASSERT(IndexbufferSize <= std::numeric_limits<GLsizeiptr>::max());
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<GLsizeiptr>(IndexbufferSize), reinterpret_cast<GLvoid*>(indexData),
			// In our class we won't ever read from the buffer
			GL_STATIC_DRAW);
		const auto IndexerrorCode = glGetError();
		if (errorCode != GL_NO_ERROR)
		{
			result = eae6320::Results::Failure;
			EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(IndexerrorCode)));
			eae6320::Logging::OutputError("OpenGL failed to allocate the vertex buffer: %s",
				reinterpret_cast<const char*>(gluErrorString(IndexerrorCode)));
			return result;
		}
	}
	// Initialize vertex format
	{
		// The "stride" defines how large a single vertex is in the stream of data
		// (or, said another way, how far apart each position element is)
		constexpr auto stride = static_cast<GLsizei>(sizeof(eae6320::Graphics::VertexFormats::sVertex_mesh));

		// Position (0)
		// 3 floats == 12 bytes
		// Offset = 0
		{
			constexpr GLuint vertexElementLocation = 0;
			constexpr GLint elementCount = 3;
			constexpr GLboolean notNormalized = GL_FALSE;	// The given floats should be used as-is
			glVertexAttribPointer(vertexElementLocation, elementCount, GL_FLOAT, notNormalized, stride,
				reinterpret_cast<GLvoid*>(offsetof(eae6320::Graphics::VertexFormats::sVertex_mesh, x)));
			const auto errorCode = glGetError();
			if (errorCode == GL_NO_ERROR)
			{
				glEnableVertexAttribArray(vertexElementLocation);
				const GLenum errorCode = glGetError();
				if (errorCode != GL_NO_ERROR)
				{
					result = eae6320::Results::Failure;
					EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
					eae6320::Logging::OutputError("OpenGL failed to enable the POSITION vertex attribute at location %u: %s",
						vertexElementLocation, reinterpret_cast<const char*>(gluErrorString(errorCode)));
					return result;
				}
			}
			else
			{
				result = eae6320::Results::Failure;
				EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
				eae6320::Logging::OutputError("OpenGL failed to set the POSITION vertex attribute at location %u: %s",
					vertexElementLocation, reinterpret_cast<const char*>(gluErrorString(errorCode)));
				return result;
			}
		}
	}

	return result;
}

eae6320::cResult eae6320::Graphics::cMesh::CleanUp()
{
	auto result = Results::Success;

	{
		if (s_vertexArrayId != 0)
		{
			// Make sure that the vertex array isn't bound
			{
				// Unbind the vertex array
				glBindVertexArray(0);
				const auto errorCode = glGetError();
				if (errorCode != GL_NO_ERROR)
				{
					if (result)
					{
						result = Results::Failure;
					}
					EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
					Logging::OutputError("OpenGL failed to unbind all vertex arrays before cleaning up geometry: %s",
						reinterpret_cast<const char*>(gluErrorString(errorCode)));
				}
			}
			constexpr GLsizei arrayCount = 1;
			glDeleteVertexArrays(arrayCount, &s_vertexArrayId);
			const auto errorCode = glGetError();
			if (errorCode != GL_NO_ERROR)
			{
				if (result)
				{
					result = Results::Failure;
				}
				EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
				Logging::OutputError("OpenGL failed to delete the vertex array: %s",
					reinterpret_cast<const char*>(gluErrorString(errorCode)));
			}
			s_vertexArrayId = 0;
		}
		if (s_vertexBufferId != 0)
		{
			constexpr GLsizei bufferCount = 1;
			glDeleteBuffers(bufferCount, &s_vertexBufferId);
			const auto errorCode = glGetError();
			if (errorCode != GL_NO_ERROR)
			{
				if (result)
				{
					result = Results::Failure;
				}
				EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
				Logging::OutputError("OpenGL failed to delete the vertex buffer: %s",
					reinterpret_cast<const char*>(gluErrorString(errorCode)));
			}
			s_vertexBufferId = 0;
		}
	}
	return result;
}

uint16_t* eae6320::Graphics::cMesh::adjustWinding(uint16_t arr[])
{
	int size = sizeof(arr) / sizeof(arr[0]);

	for (int i = 0; i < size; i++)
	{
		if (i == size - 1)
		{
			swap(arr[i], arr[i - 1]);
		}
	}

	return arr;
}
