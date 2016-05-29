#ifndef __ASYNC_TASK_INTERFACE_H__
#define __ASYNC_TASK_INTERFACE_H__

namespace gf
{
	class IAsyncTask
	{
	public:
		virtual void run() = 0;
		//virtual void onComplete() = 0;
	};
}

#endif
