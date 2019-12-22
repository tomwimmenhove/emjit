/*
 * segvcatcher.h
 *
 *  Created on: Dec 14, 2019
 *      Author: tom
 */

#ifndef SEGVCATCHER_H_
#define SEGVCATCHER_H_

#include <signal.h>
#include <vector>
#include <cstdint>
#include <functional>
#include <cstring>

class segv_catcher
{
private:
	segv_catcher();
	~segv_catcher();

	segv_catcher(segv_catcher const&) = delete;
	segv_catcher& operator=(segv_catcher const&) = delete;

public:
	static segv_catcher& Instance();

	void set_handler(uintptr_t start, size_t len, std::function<bool(uintptr_t)> handler);
	void remove_handler(uintptr_t start);

private:
	void restore();
	struct sigaction saved_action;
	static void handler(int sig, siginfo_t *si, void *context);

	struct segv_handler
	{
		uintptr_t start;
		size_t len;

		std::function<bool(int)> handler;
	};

	std::vector<segv_handler> handlers;
};

#endif /* SEGVCATCHER_H_ */
