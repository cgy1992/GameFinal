#include <windows.h>
#include <stdio.h>
#include <math.h>
#include "CTextureGenerator.h"

#pragma comment(lib, "DevIL.lib")
#pragma comment(lib, "ILU.lib")
#pragma comment(lib, "ILUT.lib")

int main(int argc, const char * argv[])
{
	CTextureGenerator gen;

	char filename[256];
	printf("Enter terrain height map file path:");
	scanf("%s", filename);

	if (!gen.readHeightMap(filename))
	{
		printf("Can not load height map.\n");
		goto fail;
	}

	int baseTextureNum;
	printf("Enter the number of base textures:");
	scanf("%d", &baseTextureNum);
	if (baseTextureNum > 4) baseTextureNum = 4;
	
	CTextureGenerator::SHeightRange ranges[4];
	for (int i = 0; i < baseTextureNum; i++)
	{
		printf("Enter NO.%d range:", i + 1);
		scanf("%d%d", &ranges[i].MinHeight, &ranges[i].MaxHeight);
	}

	gen.setHeightRanges(baseTextureNum, ranges);
	gen.createLayerRatios();

	printf("Enter generated file saving path:");
	scanf("%s", filename);
	if (!gen.createLayerRatioTexture(filename))
	{
		printf("Filed to save to file.\n");
		goto fail;
	}

	system("pause");
	return 0;

fail:
	system("pause");
	return 1;
}

