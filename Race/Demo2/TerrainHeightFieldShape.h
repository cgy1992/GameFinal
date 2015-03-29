#pragma once

using namespace gf;

class TerrainHeightFieldShape : public hkpSampledHeightFieldShape
{
public:
	TerrainHeightFieldShape(const hkpSampledHeightFieldBaseCinfo& ci
		,ITerrainMesh* mesh) 
		: hkpSampledHeightFieldShape(ci)
		, mTerrainMesh(mesh)
	{

	}

	// My simple height lookup.
	HK_FORCE_INLINE hkReal getHeightAtImpl(int x, int z) const
	{
		//return 0.0f; // return your height here...

		//mTerrainMesh->getHeight()

		return mTerrainMesh->getHeightByRowAndCol(m_zRes - z - 1, x);
	}



	// Assuming each height field quad is defined as four points { 00, 01, 11, 10 },
	// this should return true if the two triangles share the edge p00-p11.
	// Otherwise it should return false if the triangles share the edge p01-p10 
	HK_FORCE_INLINE hkBool getTriangleFlipImpl() const
	{
		//return true;
		return false;
	}

	// Forward to collideSpheres Implementation.
	virtual void collideSpheres(const CollideSpheresInput& input, SphereCollisionOutput* outputArray) const
	{
		hkSampledHeightFieldShape_collideSpheres(*this, input, outputArray);
	}


private:

	ITerrainMesh*			mTerrainMesh;

};

