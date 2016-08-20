// ���� ifdef ���Ǵ���ʹ�� DLL �������򵥵�
// ��ı�׼�������� DLL �е������ļ��������������϶���� GAMEFINAL_EXPORTS
// ���ű���ġ���ʹ�ô� DLL ��
// �κ�������Ŀ�ϲ�Ӧ����˷��š�������Դ�ļ��а������ļ����κ�������Ŀ���Ὣ
// GAMEFINAL_API ������Ϊ�Ǵ� DLL ����ģ����� DLL ���ô˺궨���
// ������Ϊ�Ǳ������ġ�
#ifdef GAMEFINAL_EXPORTS
#define GAMEFINAL_API __declspec(dllexport)
#else
#define GAMEFINAL_API __declspec(dllimport)
#endif

#include <Windows.h>
#include "xnamath.h"
#include "gfCompileConfig.h"
#include "gfTypes.h"
#include "gfUtil.h"
#include "gfMath.h"
#include "IReferenceCounted.h"
#include "IApplication.h"
#include "IVideoDriver.h"
#include "IShader.h"
#include "IShaderManager.h"
#include "IPipeline.h"
#include "IInputLayoutManager.h"
#include "ISceneManager.h"
#include "IMeshNode.h"
#include "ILightNode.h"
#include "gfEnums.h"
#include "IInputDriver.h"
#include "IThreadPool.h"
#include "IFileSystem.h"
#include "xnacollision/xnacollision.h"

#ifndef GAMEFINAL_H
#define GAMEFINAL_H

#ifndef GF_PLATFORM
// define all platforms
#define GF_PLATFORM_WIN32			(1)
#define GF_PLATFORM_LINUX			(2)
#define GF_PLATFORM_MACOS			(3)
#define GF_PLATFORM_ANDROID			(4)
#define GF_PLATFORM_IOS				(5)
#define GF_PLATFORM_PS4				(6)
#define GF_PLATFORM_XONE			(7)

#ifdef _WIN32
#define GF_PLATFORM		GF_PLATFORM_WIN32
#endif // _WIN32

#endif

namespace gf
{
	extern GAMEFINAL_API int nGameFinal;

	GAMEFINAL_API int fnGameFinal(void);

	/*
	GAMEFINAL_API IApplication* createDevice(E_VIDEO_DRIVER_TYPE driver, u32 width, u32 height,
		u32 style = EWS_NONE, bool vsync = false,
		const SDeviceContextSettings& settings = SDeviceContextSettings());
		*/

	GAMEFINAL_API IApplication* createApp(SAppSettings settings = SAppSettings());
	
}

#endif