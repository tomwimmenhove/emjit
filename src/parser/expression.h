#ifndef EXPRESSION_H
#define EXPRESSION_H

#include <vector>
#include <string>

enum class expr_type
{   
	invalid = -1,

	add,
	sub,
	mul,
	div,

	num,

	var,
};

struct expression
{
	expr_type type = expr_type::invalid;

	expression() { }

	expression(expr_type type, int num)
		: type(type), num(num)
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

struct declaration
{
	int result_var;

	expression exp;
};

enum class statement_type
{
	decl,
	ret,
};

struct statement
{
	//statement() { }
	statement(const declaration& decl) : type(statement_type::decl), decl(decl) { }
	statement(const expression& ret_exp) : type(statement_type::ret), ret_exp(ret_exp) { }

	statement_type type;

	//union
	//{
		declaration decl;
		expression ret_exp;
	//};
};


#endif /* EXPRESSION_H */
