/*
 * x64testing.h
 *
 *  Created on: Dec 27, 2019
 *      Author: tom
 */

#ifndef X64TESTING_H_
#define X64TESTING_H_

#include <exception>
#include <string>
#include <vector>
#include <memory>

#include "instructionstream.h"

class unit_test_exception : std::exception
{
public:
	explicit unit_test_exception(const std::string what_arg) : what_arg(what_arg) { }
	virtual ~unit_test_exception() throw () { }

	virtual const char* what() const throw () { return what_arg.c_str(); }

private:
	const std::string what_arg;
};

class x64_testing
{
public:
	x64_testing();

	void run_tests();
	virtual ~x64_testing() { }

private:
	template<typename T>
	static void direct_reg_reg(instruction_stream& s, std::vector<std::string>& expected_lines, int num_regs);

	template<typename T, typename U, typename V>
	static void regptr_reg(instruction_stream& s, std::vector<std::string>& expected_lines, std::string width_name, int nj, int ni);

	template<typename T, typename U, typename V, typename W>
	static void regptr_offs_reg(instruction_stream& s, std::vector<std::string>& expected_lines, std::string width_name, int nj, int ni);

	template<typename T, typename U, typename V>
	static void reg_regptr(instruction_stream& s, std::vector<std::string>& expected_lines, std::string width_name, int nj, int ni);

	template<typename T, typename U, typename V, typename W>
	static void reg_regptr_offs(instruction_stream& s, std::vector<std::string>& expected_lines, std::string width_name, int nj, int ni);

	template<typename T, typename U>
	static void reg_imm(instruction_stream& s, std::vector<std::string>& expected_lines, std::string inst_name, int n);

	template<typename T, typename U>
	static void reg_imm_addr(instruction_stream& s, std::vector<std::string>& expected_lines, std::string width_name, int n);

	template<typename T, typename U>
	static void imm_addr_reg(instruction_stream& s, std::vector<std::string>& expected_lines, std::string width_name, int n);

	template<typename T, typename U>
	void reg_ptr64(instruction_stream& s, std::vector<std::string>& expected_lines, T reg);

	template<typename T, typename U>
	void ptr64_reg(instruction_stream& s, std::vector<std::string>& expected_lines, U reg);

	template<typename T, typename U, typename V>
	static void reg_reg_ptr_idx(instruction_stream& s, std::vector<std::string>& expected_lines, std::string width_name, int ni, int nj);

	template<typename T, typename U, typename V>
	static void reg_ptr_idx_reg(instruction_stream& s, std::vector<std::string>& expected_lines, std::string width_name, int ni, int nj);

	template<typename T, typename U, typename V, typename W>
	static void reg_reg_ptr_idx_off(instruction_stream& s, std::vector<std::string>& expected_lines, std::string width_name, int ni, int nj);

	template<typename T, typename U, typename V, typename W>
	static void reg_ptr_idx_off_reg(instruction_stream& s, std::vector<std::string>& expected_lines, std::string width_name, int ni, int nj);


	//reg_reg_ptr_idx

	void mov_unit_tests();
	static void compare_assembly(const instruction_stream& s, const std::vector<std::string>& expected_lines);
	static void add_to_strinstream(std::stringstream& sstream, const instruction_stream& inst_stream, const instruction& inst);

	std::shared_ptr<auto_allocator> allocator;
};

#endif /* X64TESTING_H_ */
