#include "FileHelper.h"
#include <fstream>

bool FileHelper::ReadTerrainFile(const char* inFilePath, uint32& outVertexCount, std::vector<TerrainVertex>& outVertices)
{
	std::ifstream FileHandle(inFilePath, std::ios::binary);

	if (!FileHandle.is_open())
	{
		return false;
	}

	// Read Vertex Count
	FileHandle.read((char*)&outVertexCount, sizeof(uint32));

	// Resize the vector
	outVertices.resize(outVertexCount);

	// Read Vertices

	/* Read Vertex Positions */
	for (uint32 i = 0; i < outVertexCount; ++i)
	{
		FileHandle.read((char*)&outVertices[i].Position.X, sizeof(Vector3D));
	}

	/* Read Vertex Normals */
	for (uint32 i = 0; i < outVertexCount; ++i)
	{
		FileHandle.read((char*)&outVertices[i].Normal.X, sizeof(Vector3D));
	}

	/* Read Vertex UVs */
	for (uint32 i = 0; i < outVertexCount; ++i)
	{
		FileHandle.read((char*)&outVertices[i].TexCoord.X, sizeof(Vector2D));
	}

	return true;
}