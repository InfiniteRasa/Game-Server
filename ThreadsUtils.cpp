#include "ThreadsUtils.h"

#ifdef USE_PTHREADS
void Thread::InitMutex(TMutex* Mutex)			{ pthread_mutex_init(Mutex, NULL); }
	void Thread::DestroyMutex(TMutex* Mutex)	{ pthread_mutex_destroy(Mutex); }
	void Thread::LockMutex(TMutex* Mutex)		{ pthread_mutex_lock(Mutex); }
	void Thread::UnlockMutex(TMutex* Mutex)		{ pthread_mutex_unlock(Mutex); }
#else
void Thread::InitMutex(TMutex* Mutex)			{ InitializeCriticalSection(Mutex); }
	void Thread::DestroyMutex(TMutex* Mutex)	{ DeleteCriticalSection(Mutex); }
	void Thread::LockMutex(TMutex* Mutex)		{ EnterCriticalSection(Mutex); }
	void Thread::UnlockMutex(TMutex* Mutex)		{ LeaveCriticalSection(Mutex); }
#endif

#ifdef USE_PTHREADS
	bool Thread::New(thread_id* id, THREAD_ROUTINE2,void* Parameter)
	{
		thread_id* idp; thread_id idv; // this avoids an error trying to write the id to null
		if (id == NULL) { idp = &idv; } else { idp = id; }
		pthread_attr_t attr; pthread_attr_init(&attr);
		pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
		if (pthread_create(idp, &attr, function, Parameter) != 0)
		{ pthread_attr_destroy(&attr); return false; }
		pthread_attr_destroy(&attr); return true;
	}

	bool Thread::Cancel(thread_id id)
	{
		if (pthread_cancel(id) != 0) { return false; }
		return true;
	}

	void Thread::Join(thread_id id)
	{
		void* status;
		pthread_join(id, &status);
	}

	void Thread::Exit()
	{ pthread_exit((void*) 0); }
#else
	bool Thread::New(thread_id* id, THREAD_ROUTINE2,void* Parameter)
	{
		HANDLE handle = CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)function, Parameter, NULL, id);
		if (handle == NULL) { return false; }
		return true;
	}

	bool Thread::Cancel(thread_id id)
	{
		HANDLE handle = OpenThread(THREAD_TERMINATE, false, id);
		if (TerminateThread(handle, 0) == 0) { return false; }
		return true;
	}

	void Thread::Join(thread_id id)
	{
		HANDLE handle = OpenThread(THREAD_ALL_ACCESS, false, id);
		WaitForSingleObject(handle, INFINITE);
	}

	void Thread::Exit()
	{ ExitThread(0); }
#endif
