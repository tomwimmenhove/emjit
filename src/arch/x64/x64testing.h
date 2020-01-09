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

#include "../instructionstream.h"

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
	template<typename C, typename T>
	static void direct_reg_reg(std::string inst_name, instruction_stream& s, std::vector<std::string>& expected_lines, int num_regs);

	template<typename C, typename T, typename U, typename V>
	static void regptr_reg(std::string inst_name, instruction_stream& s, std::vector<std::string>& expected_lines, std::string width_name, int nj, int ni);

	template<typename C, typename T, typename U, typename V, typename W>
	static void regptr_offs_reg(std::string inst_name, instruction_stream& s, std::vector<std::string>& expected_lines, std::string width_name, int nj, int ni);

	template<typename C, typename T, typename U, typename V>
	static void reg_regptr(std::string inst_name, instruction_stream& s, std::vector<std::string>& expected_lines, std::string width_name, int nj, int ni);

	template<typename C, typename T, typename U, typename V, typename W>
	static void reg_regptr_offs(std::string inst_name, instruction_stream& s, std::vector<std::string>& expected_lines, std::string width_name, int nj, int ni);

	template<typename C, typename T, typename U>
	static void reg_imm(std::string inst_name, instruction_stream& s, std::vector<std::string>& expected_lines, int n);

	template<typename C, typename T, typename U>
	static void reg_imm_addr(std::string inst_name, instruction_stream& s, std::vector<std::string>& expected_lines, std::string width_name, int n);

	template<typename C, typename T, typename U>
	static void imm_addr_reg(std::string inst_name, instruction_stream& s, std::vector<std::string>& expected_lines, std::string width_name, int n);

	template<typename T, typename U>
	void reg_ptr64(instruction_stream& s, std::vector<std::string>& expected_lines, T reg);

	template<typename T, typename U>
	void ptr64_reg(instruction_stream& s, std::vector<std::string>& expected_lines, U reg);

	template<typename C, typename T, typename U, typename V>
	static void reg_reg_ptr_idx(std::string inst_name, instruction_stream& s, std::vector<std::string>& expected_lines, std::string width_name, int ni, int nj);

	template<typename C, typename T, typename U, typename V>
	static void reg_ptr_idx_reg(std::string inst_name, instruction_stream& s, std::vector<std::string>& expected_lines, std::string width_name, int ni, int nj);

	template<typename C, typename T, typename U, typename V, typename W>
	static void reg_reg_ptr_idx_off(std::string inst_name, instruction_stream& s, std::vector<std::string>& expected_lines, std::string width_name, int ni, int nj);

	template<typename C, typename T, typename U, typename V, typename W>
	static void reg_ptr_idx_off_reg(std::string inst_name, instruction_stream& s, std::vector<std::string>& expected_lines, std::string width_name, int ni, int nj);

	template<typename C>
	static void jmpcall_reg(std::string inst_name, instruction_stream& s, std::vector<std::string>& expected_lines);

	template<typename C, typename T, typename U>
	static void jmpcall_regptr(std::string inst_name, instruction_stream& s, std::vector<std::string>& expected_lines, int ni);

	template<typename C, typename T, typename U>
	static void jmpcall_regptr_idx(std::string inst_name, instruction_stream& s, std::vector<std::string>& expected_lines, int ni);

	template<typename C, typename T, typename U, typename V>
	static void jmpcall_regptr_idx_off(std::string inst_name, instruction_stream& s, std::vector<std::string>& expected_lines, int ni);

	template<typename C, typename T, typename U, typename V>
	static void jmpcall_regptr_off(std::string inst_name, instruction_stream& s, std::vector<std::string>& expected_lines, int ni);


	template<typename C>
	static void test_srcdst_oper_base(std::string inst_name, instruction_stream& s, std::vector<std::string>& expected_lines);

	template<typename C>
	static void test_x64_jmpcall_base(std::string inst_name, instruction_stream& s, std::vector<std::string>& expected_lines);


	void mov_unit_tests();
	static void compare_assembly(const instruction_stream& s, const std::vector<std::string>& expected_lines);
	static void add_to_strinstream(std::stringstream& sstream, const instruction_stream& inst_stream, const instruction& inst);

	std::shared_ptr<auto_allocator> allocator;
};

#endif /* X64TESTING_H_ */
