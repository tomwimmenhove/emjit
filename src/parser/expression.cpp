#include "expression.h"

int expression::eval() const
{   
	switch(type)
	{   
		case expr_type::add: return expressions[0].eval() + expressions[1].eval();
		case expr_type::sub: return expressions[0].eval() - expressions[1].eval();
		case expr_type::mul: return expressions[0].eval() * expressions[1].eval();
		case expr_type::div: return expressions[0].eval() / expressions[1].eval();
		case expr_type::num: return num;
	}

	return 0;
}

