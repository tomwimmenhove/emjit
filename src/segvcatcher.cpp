/*
 * segvcatcher.cpp
 *
 *  Created on: Dec 14, 2019
 *      Author: tom
 */

#include "segvcatcher.h"
#include <cstring>
#include <algorithm>
#include <ucontext.h>

using namespace std;

segv_catcher::segv_catcher()
{
	struct sigaction sa;

	memset(&sa, 0, sizeof(sa));
	sa.sa_sigaction = &handler;
	sa.sa_flags = SA_SIGINFO;
	sigemptyset(&sa.sa_mask);

	sigaction(SIGSEGV, &sa, &saved_action);
}

segv_catcher::~segv_catcher()
{
	restore();
}

segv_catcher& segv_catcher::Instance()
{
	static segv_catcher instance;

	return instance;
}


void segv_catcher::restore()
{
	sigaction(SIGSEGV, &saved_action, nullptr);
}

void segv_catcher::handler(int sig, siginfo_t *si, void *context)
{
	auto addr = (uintptr_t) si->si_addr;

	for(auto handler: Instance().handlers)
	{
		if (addr >= handler.start && addr < handler.start + handler.len)
		{
			if (!handler.handler((uintptr_t) si->si_addr))
			{
				/* Handler failed -> crash */
				Instance().restore();
			}

			return;
		}
	}

	/* No handler found -> crash */
	Instance().restore();
}

void segv_catcher::set_handler(uintptr_t start, size_t len, std::function<bool(uintptr_t)> handler)
{
	handlers.push_back(segv_handler { start, len, handler });
}

void segv_catcher::remove_handler(uintptr_t start)
{
	for(auto i = handlers.begin(); i != handlers.end(); i++)
	{
		if (i->start == start)
		{
			handlers.erase(i);
			return;
		}
	}
}
