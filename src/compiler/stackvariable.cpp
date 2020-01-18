/*
 * stackvariable.cpp
 *
 *  Created on: Jan 15, 2020
 *      Author: tom
 */

#include "stackvariable.h"

#include <iostream>

using namespace std;

int stack_variable::alloc(int size)
{
	int new_pos = -1;

	if (stack_map.empty() || stack_map.begin()->first >= size)
		new_pos = 0;
	else
	{
		for(auto it = stack_map.begin(); it != stack_map.end(); it++)
		{
			auto nxt = next(it);
			if (nxt == stack_map.end() ||						/* At the end */
				nxt->first - (it->first + it->second >= size))	/* Or between this and the next */
			{
				new_pos = it->first + it->second;
				break;
			}
		}
	}

	stack_map[new_pos] = size;

	int top = new_pos + size;
	if (top > max)
		max = top;

	return new_pos;
}
