

#ifndef ReleaseReferenceCounted
#define ReleaseReferenceCounted(x) { if(x){ x->drop(); x = nullptr; } }
#endif

#ifndef AddReferenceCounted
#define AddReferenceCounted(x) { if(x){ x->grab();} }
#endif

#ifndef ReleaseListElementCounted
#define ReleaseListElementCounted(v) \
{for (auto it = v.begin(); it != v.end(); it++) ReleaseReferenceCounted((*it));}
#endif

#if defined(DEBUG) || defined(_DEBUG) 
#define GF_PRINT_CONSOLE_INFO(format, ...) fprintf(stderr, format, ## __VA_ARGS__)
#else
#define GF_PRINT_CONSOLE_INFO(format, ...)
#endif

#ifndef GF_ASSERT
#define GF_ASSERT(x) assert(x)
#endif


#ifndef _DECLARE_RESOURCE_DESTROY_METHODS
#define _DECLARE_RESOURCE_DESTROY_METHODS(T)			\
	public:												\
	virtual bool destroy(const std::string& name) = 0;	\
	virtual bool destroy(T* object) = 0;				\

#endif

#ifndef _DEFINE_RESOURCE_DESTROY_METHODS
#define _DEFINE_RESOURCE_DESTROY_METHODS(Class, map, T)	\
	public:											\
	bool destroy(const std::string& name)			\
	{												\
		auto it = map.find(name);					\
		if (it == map.end()) return false;			\
			T* object = it->second;					\
		if (object->getReferenceCount() == 1)		\
		{											\
			object->drop();							\
			map.erase(it);							\
			return true;							\
		}											\
		return false;								\
	}												\
													\
	bool destroy(T* object)							\
	{												\
		if (!object)								\
			return false;							\
													\
		auto it = map.find(object->getName());		\
		if (it == map.end())						\
			return false;							\
		if (object->getReferenceCount() == 1)		\
		{											\
		    object->drop();							\
			map.erase(it);							\
			return true;							\
		}											\
		return false;								\
	}												\
													\
	void destroyAll()								\
	{													\
	for (auto it = map.begin(); it != map.end(); it++)	\
		ReleaseReferenceCounted(it->second);				\
	}													
#endif

#ifndef _DEFINE_RESOURCE_MANAGER_DESTRUCTOR
#define _DEFINE_RESOURCE_MANAGER_DESTRUCTOR(Class)	\
public:\
	virtual ~Class() { destroyAll(); }
#endif

#ifdef GAMEFINAL_EXPORTS
#define SINGLETON_API __declspec(dllexport)
#else
#define SINGLETON_API __declspec(dllimport)
#endif

#ifndef _DECLARE_SINGLETON_INSTANCE
#define _DECLARE_SINGLETON_INSTANCE(T)	\
	public:								\
	static SINGLETON_API T* getInstance();			\
	static void _setInstance(T*);		\
	protected:							\
	static T* _instance;				\

#endif

#ifndef _DEFINE_SINGLETON_INSTANCE
#define _DEFINE_SINGLETON_INSTANCE(T)	\
	SINGLETON_API T* T::getInstance() { return _instance; } \
	void T::_setInstance(T* instance) { _instance = instance; } \
	T* T::_instance = nullptr;						\

#endif

#ifndef __MATH_CALCULATION_DEFINE__
#define __MATH_CALCULATION_DEFINE__

//#define Vector3Add(a, b) XMFLOAT3((a).x+(b).x, (a).y+(b).y, (a).z+(b).z)
//#define Vector4Add();

#endif

#ifndef EPSILON
#define EPSILON (0.00001f)
#endif

/*
#ifndef _DEFINE_RESOURCE_DESTORY_FUNCTIONS
#define _DEFINE_RESOURCE_DESTORY_FUNCTIONS(map, T) \
	bool destroy(const std::string& name)			\
	{											\
		auto it = map.find(name);				\	
		if (it == map.end())					\
			return false;						\
		T* object = it->second;					\
		if (object->getReferenceCount() == 1)	\
		{										\
			object->drop();						\
			map.erase(it);						\
			return true;						\
		}										\
		return false;							\
	}											\
#endif
	*/