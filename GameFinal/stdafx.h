// stdafx.h : ��׼ϵͳ�����ļ��İ����ļ���
// ���Ǿ���ʹ�õ��������ĵ�
// �ض�����Ŀ�İ����ļ�
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             //  �� Windows ͷ�ļ����ų�����ʹ�õ���Ϣ
// Windows ͷ�ļ�: 
#include <windows.h>


// TODO:  �ڴ˴����ó�����Ҫ������ͷ�ļ�
#pragma warning(push)
#pragma warning(disable: 4005) // possible loss of data
#include <d3dx11.h>
#include <d3d11shader.h>
#include <d3dcompiler.h>
#pragma warning(pop)
#include <xnamath.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <io.h>
#include <iostream>
#include <fstream>
#include <stdexcept>
#include <map>
#include <string>
#include <vector>
#include <deque>
#include <memory>
#include <algorithm>
#include <array>
#include <list>
#include <set>
#include <regex>
#include <functional>
#include <mmsystem.h>
#include <concurrent_unordered_map.h>
#include <unordered_map>
#include <unordered_set>
#include <sstream>

#define DIRECTINPUT_VERSION 0x0800 // avoid the warning
#include <dinput.h>
#include "xnacollision/xnacollision.h"

#include "gfTypes.h"