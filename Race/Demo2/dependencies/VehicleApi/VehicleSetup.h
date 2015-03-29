/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HK_VEHICLE_SETUP_H
#define HK_VEHICLE_SETUP_H

#include <Physics2012/Vehicle/hkpVehicleInstance.h>

#include <Physics2012/Vehicle/AeroDynamics/Default/hkpVehicleDefaultAerodynamics.h>
#include <Physics2012/Vehicle/DriverInput/Default/hkpVehicleDefaultAnalogDriverInput.h>
#include <Physics2012/Vehicle/Brake/Default/hkpVehicleDefaultBrake.h>
#include <Physics2012/Vehicle/Engine/Default/hkpVehicleDefaultEngine.h>
#include <Physics2012/Vehicle/VelocityDamper/Default/hkpVehicleDefaultVelocityDamper.h>
#include <Physics2012/Vehicle/Steering/Default/hkpVehicleDefaultSteering.h>
#include <Physics2012/Vehicle/Suspension/Default/hkpVehicleDefaultSuspension.h>
#include <Physics2012/Vehicle/Transmission/Default/hkpVehicleDefaultTransmission.h>
#include <Physics2012/Vehicle/WheelCollide/RayCast/hkpVehicleRayCastWheelCollide.h>
#include <Physics2012/Vehicle/TyreMarks/hkpTyremarksInfo.h>

	/// This class just holds ALL the vehicle parameters (hardcoded) for an untuned, four
	/// wheeled vehicle and facilitates construction via blueprints on a single call to
	/// buildVehicle().
class VehicleSetup : public hkReferencedObject
{
	public:

		HK_DECLARE_CLASS_ALLOCATOR( HK_MEMORY_CLASS_VEHICLE );

		VehicleSetup(){}

		virtual void buildVehicle( const hkpWorld* world, hkpVehicleInstance& vehicle ); 

	public:

		virtual void setupVehicleData( const hkpWorld* world, hkpVehicleData& data );
		virtual void setupComponent( const hkpVehicleData& data, hkpVehicleDefaultAnalogDriverInput& driverInput );
		virtual void setupComponent( const hkpVehicleData& data, hkpVehicleDefaultSteering& steering );
		virtual void setupComponent( const hkpVehicleData& data, hkpVehicleDefaultEngine& engine );
		virtual void setupComponent( const hkpVehicleData& data, hkpVehicleDefaultTransmission& transmission );
		virtual void setupComponent( const hkpVehicleData& data, hkpVehicleDefaultBrake& brake );
		virtual void setupComponent( const hkpVehicleData& data, hkpVehicleDefaultSuspension& suspension );
		virtual void setupComponent( const hkpVehicleData& data, hkpVehicleDefaultAerodynamics& aerodynamics );
		virtual void setupComponent( const hkpVehicleData& data, hkpVehicleDefaultVelocityDamper& velocityDamper );

		virtual void setupWheelCollide( const hkpWorld* world, const hkpVehicleInstance& vehicle, hkpVehicleRayCastWheelCollide& wheelCollide );
		virtual void setupTyremarks( const hkpVehicleData& data, hkpTyremarksInfo& tyremarkscontroller );
};

#endif // HK_VEHICLE_SETUP_H

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
