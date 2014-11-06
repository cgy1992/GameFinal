#ifndef __CGEOMETRY_CREATOR_H_INCLUDE__
#define __CGEOMETRY_CREATOR_H_INCLUDE__

#include "IGeometryCreator.h"

namespace gf
{
	class CGeometryCreator : public IGeometryCreator
	{
	public:
		virtual void createCubeData(
			f32 width,
			f32 height,
			f32 depth,
			SGeometryData& geoData);

		virtual void createPlaneData(
			f32 width,
			f32 depth,
			u32 xsegments,
			u32 ysegments,
			f32 uTiles,
			f32 vTiles,
			SGeometryData& geoData);

		virtual void createSphereData(
			f32 radius,
			u32 sliceCount,
			u32 stackCount,
			SGeometryData& geoData);

		virtual void createQuadData(XMFLOAT3 vertices[], math::SAxisAlignedBox& aabb);
	};

}



#endif
