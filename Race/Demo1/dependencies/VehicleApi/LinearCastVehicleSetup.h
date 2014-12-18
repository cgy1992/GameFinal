/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HK_LINEAR_CAST_VEHICLE_SETUP_H
#define HK_LINEAR_CAST_VEHICLE_SETUP_H

//#include <Demos/DemoCommon/Utilities/VehicleApi/VehicleSetup.h>
#include "VehicleSetup.h"
#include <Physics2012/Vehicle/WheelCollide/LinearCast/hkpVehicleLinearCastWheelCollide.h>



	/// This class just holds ALL the vehicle parameters (hardcoded) for an untuned, four
	/// wheeled vehicle and facilitates construction via blueprints on a single call to
	/// buildVehicle().
class LinearCastVehicleSetup : public VehicleSetup
{
	public:

		HK_DECLARE_CLASS_ALLOCATOR( HK_MEMORY_CLASS_VEHICLE );

		LinearCastVehicleSetup(){}

		virtual void buildVehicle( const hkpWorld* world, hkpVehicleInstance& vehicle ); 

	public:
		using VehicleSetup::setupWheelCollide;
		virtual void setupWheelCollide( const hkpWorld* world, const hkpVehicleInstance& vehicle, hkpVehicleLinearCastWheelCollide& wheelCollide );
};

#endif // HK_LINEAR_CAST_VEHICLE_SETUP_H

/*
 * Havok SDK - NO SOURCE PC DOWNLOAD, BUILD(#20140907)
 * 
 * Confidential Information of Havok.  (C) Copyright 1999-2014
 * Telekinesys Research Limited t/a Havok. All Rights Reserved. The Havok
 * Logo, and the Havok buzzsaw logo are trademarks of Havok.  Title, ownership
 * rights, and intellectual property rights in the Havok software remain in
 * Havok and/or its suppliers.
 * 
 * Use of this software for evaluation purposes is subject to and indicates
 * acceptance of the End User licence Agreement for this product. A copy of
 * the license is included with this software and is also available at www.havok.com/tryhavok.
 * 
 */
