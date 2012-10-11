#ifndef TR_THREADS_UTILS_H
#define TR_THREADS_UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

// todo: threads accept cdecl or stdcall functions? idk
#ifdef USE_PTHREADS
#pragma comment(lib, "pthreadVC2.lib")
#include <pthreads/pthread.h>
#define thread_id pthread_t
#define THREADRET void*
#define THREAD_ROUTINE THREADRET (*)(void*)
#define THREAD_ROUTINE2 THREADRET (*function)(void*)
#define TMutex CRITICAL_SECTION // pthread_mutex_t
#else
#include <windows.h>
#define thread_id DWORD // use uint32, long?
#define THREADRET DWORD // sint32
#define THREAD_ROUTINE LPTHREAD_START_ROUTINE
#define THREAD_ROUTINE2 LPTHREAD_START_ROUTINE function
#define TMutex CRITICAL_SECTION
#endif

namespace Thread
{
	//static unsigned int ThreadsCount = 1;
	void InitMutex(TMutex* Mutex);
	void DestroyMutex(TMutex* Mutex);
	void LockMutex(TMutex* Mutex);
	void UnlockMutex(TMutex* Mutex);

	// todo: check for open handle leaks
	bool New(thread_id* id, THREAD_ROUTINE2,void* Parameter);
	bool Cancel(thread_id id);
	void Join(thread_id id);
	void Exit();
	//unsigned int  Count();
	//void IncreaseCount();
	//void DecreaseCount();
}
#endif