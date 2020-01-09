#ifndef EXPRESSION_H
#define EXPRESSION_H

#include <vector>

enum class expr_type
{   
	add,
	sub,
	mul,
	div,

	num,
};

struct expression
{
	expr_type type;

	expression() { }

	expression(int num)
		: type(expr_type::num), num(num)
	{ }

	expression(expr_type type, const expression& a, const expression& b)
		: type(type)
	{   
		expressions.push_back(a);
		expressions.push_back(b);
	}

	int eval() const;

	std::vector<expression> expressions;
	int num = 0;
};



#endif /* EXPRESSION_H */
