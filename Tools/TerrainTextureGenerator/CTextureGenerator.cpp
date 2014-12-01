#include "CTextureGenerator.h"


CTextureGenerator::CTextureGenerator():mHeightMapData(NULL)
{
	ilInit();
	/* We want all images to be loaded in a consistent manner */
	ilEnable(IL_ORIGIN_SET);
	ilEnable(IL_FILE_OVERWRITE);
	/* In the next section, we load one image */
	ilGenImages(1, &mHandle);
	ilBindImage(mHandle);
}


CTextureGenerator::~CTextureGenerator()
{
	free(mHeightMapData);
	ilDeleteImages(1, &mHandle);
}

bool CTextureGenerator::readHeightMap(const char* filename)
{
	FILE* fp = fopen(filename, "rb");
	if (!fp)
	{
		printf("File does not exist\n");
		return false;
	}

	fseek(fp, 0, SEEK_END);
	int fileSize = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	mRowVertexNum = static_cast<int>(sqrt((double)fileSize));
	mTotalVertexNum = mRowVertexNum * mRowVertexNum;
	mHeightMapData = (unsigned char*)malloc(fileSize);

	fread(mHeightMapData, fileSize, 1, fp);
	fclose(fp);
	return true;
}

void CTextureGenerator::setHeightRanges(int num, const SHeightRange heightRanges[])
{
	mBaseTextureNum = num;
	if (mBaseTextureNum > 4)
		mBaseTextureNum = 4;

	memcpy(mHeightRanges, heightRanges, mBaseTextureNum * sizeof(SHeightRange));
}

void CTextureGenerator::createLayerRatios()
{
	mRatioPixels.resize(mTotalVertexNum);

	for (int i = 0; i < mTotalVertexNum; i++)
	{
		int height = static_cast<int>(mHeightMapData[i]);
		SPixel currentPixel;
		for (int j = 0; j < mBaseTextureNum; j++)
		{
			currentPixel.Pixels[j] = 0.0f;
			SHeightRange range = mHeightRanges[j];
			if (height > range.MinHeight && height < range.MaxHeight)
			{
				float ratio = 0.0f;
				int mid = (range.MinHeight + range.MaxHeight) / 2;
				if (height < mid)
				{
					ratio = (static_cast<float>(height - range.MinHeight)) / (mid - range.MinHeight);
				}
				else
				{
					ratio = (static_cast<float>(range.MaxHeight - height)) / (range.MaxHeight - mid);
				}
				currentPixel.Pixels[j] = ratio;
			}
		}

		float sum = 0.0f;
		for (int j = 0; j < mBaseTextureNum; j++)
			sum += currentPixel.Pixels[j];

		if (sum != 0.0f)
		{
			for (int j = 0; j < mBaseTextureNum; j++)
				currentPixel.Pixels[j] /= sum;
		}

		mRatioPixels[i] = currentPixel;
	}
}

bool CTextureGenerator::createLayerRatioTexture(const char* filename)
{
	int pixelComponentNum = (mBaseTextureNum == 4) ? 4 : 3;
	int byteNum = mTotalVertexNum * pixelComponentNum;

	ILubyte* data = (ILubyte*)malloc(byteNum);
	memset(data, 0, byteNum);
	ILubyte* dataPtr = data;

	for (int row = 0; row < mRowVertexNum; row++)
	{
		for (int col = 0; col < mRowVertexNum; col++)
		{
			int sourceIndex = row * mRowVertexNum + col;
			int destIndex = (mRowVertexNum - 1 - row) * mRowVertexNum + col;

			for (int j = 0; j < mBaseTextureNum; j++)
			{
				float ratio = mRatioPixels[sourceIndex].Pixels[j];
				float fRatioPixel = ratio * 255;
				int iRatioPixel = (int)(fRatioPixel + 0.5f);
				data[destIndex * pixelComponentNum + j] = static_cast<ILubyte>(iRatioPixel);
			}

		}
	}

	/*
	for (int i = 0; i < mTotalVertexNum; i++)
	{
		for (int j = 0; j < mBaseTextureNum; j++)
		{
			float ratio = mRatioPixels[i].Pixels[j];
			float fRatioPixel = ratio * 255;
			int iRatioPixel = (int)(fRatioPixel + 0.5f);
			data[i * pixelComponentNum + j] = static_cast<ILubyte>(iRatioPixel);
		}
	}
	*/

	ILenum imgFormat = (mBaseTextureNum == 4) ? IL_RGBA : IL_RGB;
	ILboolean ret = ilTexImage(mRowVertexNum, mRowVertexNum, 1, 
		pixelComponentNum, imgFormat, IL_UNSIGNED_BYTE, data);

	if (ret)
	{
		ilSaveImage(filename);
	}

	free(data);
	return ret;
}



