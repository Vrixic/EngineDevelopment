#pragma once
#include <string>
#include <vector>
#include <iostream>
#include "Model.h"

struct FileHelper
{
public:
	static bool ReadTerrainFile(const char* inFilePath, uint32& outVertexCount, std::vector<TerrainVertex>& outVertices);

};

