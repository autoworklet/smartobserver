#ifndef UIOHOOK_WORKER_H_
#define UIOHOOK_WORKER_H_

#include <uiohook.h>

#define UIOHOOK_ERROR_THREAD_CREATE				0x10
#ifdef __cplusplus
extern "C" {
#endif

int uiohook_worker_start(dispatcher_t dispatch_proc);

int uiohook_worker_stop();
#ifdef __cplusplus
}
#endif
#endif // UIOHOOK_WORKER_H_
