/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

//HK_REFLECTION_PARSER_EXCLUDE_FILE

#ifndef HK_DEMO_PSEUDOPAD_H
#define HK_DEMO_PSEUDOPAD_H
// not standalone : included from hkdemoframework.cpp

class hkPseudoPad : public hkgPad
{
	public:

		HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_DEMO, hkPseudoPad);

		hkPseudoPad () {}

		void updatePad(int padIndex, hkgWindow* window, hkReal deltaTime, hkBool fakePads, hkBool desiredCtrlState);

			/// Try to find the corresponding pad value for a key. Returns 0 if there is none.
		static HKG_PAD_BUTTON HK_CALL keyToPad(HKG_KEYBOARD_VKEY k);

		static int s_keyToPadMapping[][2];
};

#endif //HK_DEMO_PSEUDOPAD_H

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
