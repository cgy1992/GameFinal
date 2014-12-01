#ifndef __TEXTURE_GENERATOR_H__
#define __TEXTURE_GENERATOR_H__


#include <stdio.h>
#include <math.h>
#include <windows.h>
#include <vector>
#include "IL/il.h"

class CTextureGenerator
{
public:
	struct SHeightRange
	{
		int MinHeight;
		int MaxHeight;
	};

	struct SPixel
	{
		float Pixels[4];
	};

public:
	CTextureGenerator();
	~CTextureGenerator();

	bool readHeightMap(const char* filename);
	
	void setHeightRanges(int num, const SHeightRange heightRanges[]);

	void createLayerRatios();

	bool createLayerRatioTexture(const char* filename);

private:
	ILuint						mHandle;
	unsigned char*				mHeightMapData;
	int							mRowVertexNum;
	int							mTotalVertexNum;
	int							mBaseTextureNum;
	SHeightRange				mHeightRanges[4];
	std::vector<SPixel>			mRatioPixels;

};

#endif