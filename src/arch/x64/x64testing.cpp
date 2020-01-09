/*
 * x64testing.cpp
 *
 *  Created on: Dec 27, 2019
 *      Author: tom
 */

#include <regex>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <memory>
#include <alloca.h>
#include <chrono>
#include <algorithm>
#include <vector>
#include <iterator>
#include <cassert>

#include "x64testing.h"
#include "x64disassembler.h"
#include "x64instruction.h"

#include "../../parser/driver.h"

using namespace std::chrono;
using namespace std;

x64_testing::x64_testing()
{
	uintptr_t start = 65536;
	size_t len = 1024*1024*1024;

	allocator = std::make_shared<auto_allocator> (start, len, PROT_READ | PROT_WRITE | PROT_EXEC);
}

void x64_testing::run_tests()
{
	mov_unit_tests();

	cout << "OK!\n";
}

template<typename C, typename T>
void x64_testing::direct_reg_reg(string inst_name, instruction_stream& s, vector<string>& expected_lines, int num_regs)
{
	for (auto i = 0; i < num_regs; i++)
	{
		for (auto j = 0; j < num_regs; j++)
		{
			auto inst = C(T(j), T(i));

			stringstream stream;
			add_to_strinstream(stream, s, inst);
			stream << "\t" << inst_name << " " << T::names[j] << "," << T::names[i];

			expected_lines.push_back(stream.str());

			s << inst;
		}
	}
}

template<typename C, typename T, typename U, typename V>
void x64_testing::regptr_reg(string inst_name, instruction_stream& s, vector<string>& expected_lines, string width_name, int nj, int ni)
{
	for (auto i = 0; i < ni; i++)
	{
		for (auto j = 0; j < nj; j++)
		{
			auto inst = C(V(T(j)), U(i));

			stringstream stream;

			add_to_strinstream(stream, s, inst);
			string name = T::names[j];
			if (T(j).is_bp() || T(j).is_r13())
				name += "+0x0";
			stream << "\t" << inst_name << " " << width_name << " PTR [" << name << "]," << U::names[i];
			stream.flush();

			expected_lines.push_back(stream.str());

			s << inst;
		}
	}
}

template<typename C, typename T, typename U, typename V, typename W>
void x64_testing::regptr_offs_reg(std::string inst_name, instruction_stream& s, vector<string>& expected_lines, string width_name, int nj, int ni)
{
	for (auto i = 0; i < ni; i++)
	{
		for (auto j = 0; j < nj; j++)
		{
			W offs = (j + 1) * 999 + (i + 1) * 333;
			auto inst = C(V(T(j)), offs, U(i));

			stringstream stream;

			add_to_strinstream(stream, s, inst);
			stream << "\t" << inst_name << " " << width_name << " PTR [" << T::names[j];

			if (offs < 0)
				stream << "-0x" << hex << setw(0) << (int(-offs));
			else
				stream << "+0x" << hex << setw(0) << (int(offs));

			stream << "]," << U::names[i];
			stream.flush();

			expected_lines.push_back(stream.str());

			s << inst;
		}
	}
}

template<typename C, typename T, typename U, typename V>
void x64_testing::reg_regptr(std::string inst_name, instruction_stream& s, vector<string>& expected_lines, string width_name, int nj, int ni)
{
	for (auto i = 0; i < ni; i++)
	{
		for (auto j = 0; j < nj; j++)
		{
			auto inst = C(T(j), V(U(i)));

			stringstream stream;

			add_to_strinstream(stream, s, inst);
			string name = U::names[i];
			if (U(i).is_bp() || U(i).is_r13())
				name += "+0x0";
			stream << "\t" << inst_name << " " << T::names[j] << "," << width_name << " PTR [" << name << ']';
			stream.flush();

			expected_lines.push_back(stream.str());

			s << inst;
		}
	}
}

template<typename C, typename T, typename U, typename V, typename W>
void x64_testing::reg_regptr_offs(string inst_name, instruction_stream& s, vector<string>& expected_lines, string width_name, int nj, int ni)
{
	for (auto i = 0; i < ni; i++)
	{
		for (auto j = 0; j < nj; j++)
		{
			W offs = (j + 1) * 999 + (i + 1) * 333;
			auto inst = C(T(j), V(U(i)), offs);

			stringstream stream;

			add_to_strinstream(stream, s, inst);
			string name = U::names[i];
			stream << "\t" << inst_name << " " << T::names[j] << "," << width_name << " PTR [" << name;
			if (offs < 0)
				stream << "-0x" << hex << setw(0) << (int(-offs));
			else
				stream << "+0x" << hex << setw(0) << (int(offs));
			stream  << ']';
			stream.flush();

			expected_lines.push_back(stream.str());

			s << inst;
		}
	}
}

template<typename C, typename T, typename U>
void x64_testing::reg_imm(string inst_name, instruction_stream& s, vector<string>& expected_lines, int n)
{
	for (auto i = 0; i < n; i++)
	{
		U imm = (i + 1) * 0x6732409234657202;
		auto inst = C(T(i), imm);

		stringstream stream;

		add_to_strinstream(stream, s, inst);
		stream << "\t" << inst_name << ' ' << T::names[i] << ",0x" << hex << setw(0) << ((uint64_t)imm);
		stream.flush();

		expected_lines.push_back(stream.str());

		s << inst;
	}
}

template<typename C, typename T, typename U>
void x64_testing::reg_imm_addr(string inst_name, instruction_stream& s, vector<string>& expected_lines, string width_name, int n)
{
	for (auto i = 0; i < n; i++)
	{
		U imm = U((i + 1) * 0x6732409234657202);
		auto inst = C(T(i), x64_addr_ptr<U>(reinterpret_cast<U>(imm)));

		stringstream stream;

		add_to_strinstream(stream, s, inst);
		stream << "\t" << inst_name << " " << T::names[i] << "," << width_name << " PTR ds:0x" << hex << setw(0) << ((int64_t)imm);
		stream.flush();

		expected_lines.push_back(stream.str());

		s << inst;
	}
}

template<typename C, typename T, typename U>
void x64_testing::imm_addr_reg(string inst_name, instruction_stream& s, vector<string>& expected_lines, string width_name, int n)
{
	for (auto i = 0; i < n; i++)
	{
		T imm = (i + 1) * 0x6732409234657202;
		auto inst = C(x64_addr_ptr<T>(reinterpret_cast<T>(imm)), U(i));

		stringstream stream;

		add_to_strinstream(stream, s, inst);
		stream << "\t" << inst_name << " " << width_name << " PTR ds:0x" << hex << setw(0) << ((int64_t)imm) << ',' << U::names[i];
		stream.flush();

		expected_lines.push_back(stream.str());

		s << inst;
	}
}

template<typename T, typename U>
void x64_testing::reg_ptr64(instruction_stream& s, vector<string>& expected_lines, T reg)
{
	auto inst = x64_mov(reg, x64_addr_ptr<U*>(reinterpret_cast<U*>(0x1122334455667788)));

	stringstream stream;

	add_to_strinstream(stream, s, inst);
	stream << "\tmovabs " << T::names[0] << ",ds:0x1122334455667788";
	stream.flush();

	expected_lines.push_back(stream.str());

	s << inst;
}

template<typename T, typename U>
void x64_testing::ptr64_reg(instruction_stream& s, vector<string>& expected_lines, U reg)
{
	auto inst = x64_mov(x64_addr_ptr<T*>(reinterpret_cast<T*>(0x1122334455667788)), reg);

	stringstream stream;

	add_to_strinstream(stream, s, inst);
	stream << "\tmovabs " << "ds:0x1122334455667788," << U::names[0] ;
	stream.flush();

	expected_lines.push_back(stream.str());

	s << inst;
}

template<typename C, typename T, typename U, typename V>
void x64_testing::reg_reg_ptr_idx(string inst_name, instruction_stream& s, std::vector<std::string>& expected_lines, std::string width_name, int ni, int nj)
{
	for (auto i = 0; i < ni; i++)
	{
		for (auto j = 0; j < nj; j++)
		{
			for (auto k = 0; k < nj; k++)
			{
				auto regidx = V(k);

				if (regidx.is_sp())
					continue;

				auto dst = T(i);
				auto regptr = U(V(j));

				auto shift = (i + j + k) & 3;
				int scale_fact = 1 << shift;
				auto inst = C(dst, regptr, regidx, static_cast<sib_scale>(shift));

				stringstream stream;

				add_to_strinstream(stream, s, inst);
				stream << "\t" << inst_name << " " << T::names[i] << "," << width_name
						<< " PTR [" << V::names[j] << '+'
						<< V::names[k] << '*' << scale_fact;
				if (regptr.ptr.is_bp() || regptr.ptr.is_r13())
					stream << "+0x0";
				stream << ']';
				stream.flush();

				expected_lines.push_back(stream.str());

				s << inst;
			}
		}
	}
}

template<typename C, typename T, typename U, typename V>
void x64_testing::reg_ptr_idx_reg(string inst_name, instruction_stream& s, std::vector<std::string>& expected_lines, std::string width_name, int ni, int nj)
{
	for (auto i = 0; i < ni; i++)
	{
		for (auto j = 0; j < ni; j++)
		{
			for (auto k = 0; k < nj; k++)
			{
				auto regoff = U(j);

				if (regoff.is_sp())
					continue;

				auto regptr = T(U(i));
				auto src = V(k);

				auto shift = (i + j + k) & 3;
				int scale_fact = 1 << shift;
				auto inst = C(regptr, regoff, static_cast<sib_scale>(shift), src);

				stringstream stream;

				add_to_strinstream(stream, s, inst);
				stream << "\t" << inst_name << " " << width_name
						<< " PTR [" << U::names[i] << '+'
						<< U::names[j] << '*' << scale_fact;
				if (regptr.ptr.is_bp() || regptr.ptr.is_r13())
					stream << "+0x0";
				stream << "]," << V::names[k];
				stream.flush();

				expected_lines.push_back(stream.str());

				s << inst;
			}
		}
	}
}

template<typename C, typename T, typename U, typename V, typename W>
void x64_testing::reg_reg_ptr_idx_off(string inst_name, instruction_stream& s, std::vector<std::string>& expected_lines, std::string width_name, int ni, int nj)
{
	for (auto i = 0; i < ni; i++)
	{
		for (auto j = 0; j < nj; j++)
		{
			for (auto k = 0; k < nj; k++)
			{
				auto regidx = V(k);

				if (regidx.is_sp())
					continue;

				auto dst = T(i);
				auto regptr = U(V(j));
				W offs = (j + 1) * 999 + (i + 1) * 333 + (k + 1) * 234;
				auto shift = (i + j + k) & 3;
				int scale_fact = 1 << shift;
				auto inst = C(dst, regptr, regidx, static_cast<sib_scale>(shift), offs);

				stringstream stream;

				add_to_strinstream(stream, s, inst);
				stream << "\t" << inst_name << " " << T::names[i] << "," << width_name
						<< " PTR [" << V::names[j] << '+'
						<< V::names[k] << '*' << scale_fact;
				if (offs < 0)
					stream << "-0x" << hex << setw(0) << (int(-offs));
				else
					stream << "+0x" << hex << setw(0) << (int(offs));
				stream << ']';
				stream.flush();

				expected_lines.push_back(stream.str());

				s << inst;
			}
		}
	}
}

template<typename C, typename T, typename U, typename V, typename W>
void x64_testing::reg_ptr_idx_off_reg(string inst_name, instruction_stream& s, std::vector<std::string>& expected_lines, std::string width_name, int ni, int nj)
{
	for (auto i = 0; i < ni; i++)
	{
		for (auto j = 0; j < ni; j++)
		{
			for (auto k = 0; k < nj; k++)
			{
				auto regoff = U(j);

				if (regoff.is_sp())
					continue;

				auto regptr = T(U(i));
				auto src = W(k);
				V offs = (j + 1) * 999 + (i + 1) * 333 + (k + 1) * 234;
				auto shift = (i + j + k) & 3;
				int scale_fact = 1 << shift;
				auto inst = C(regptr, regoff, static_cast<sib_scale>(shift), offs, src);

				stringstream stream;

				add_to_strinstream(stream, s, inst);
				stream << "\t" << inst_name << " " << width_name
						<< " PTR [" << U::names[i] << '+'
						<< U::names[j] << '*' << scale_fact;
				if (offs < 0)
					stream << "-0x" << hex << setw(0) << (int(-offs));
				else
					stream << "+0x" << hex << setw(0) << (int(offs));
				stream << "]," << W::names[k];
				stream.flush();

				expected_lines.push_back(stream.str());

				s << inst;
			}
		}
	}
}

template<typename C>
void x64_testing::jmpcall_reg(std::string inst_name, instruction_stream& s, std::vector<std::string>& expected_lines)
{
	for (auto i = 0; i < 16; i++)
	{
		auto inst = C(x64_reg64(i));
		stringstream stream;

		add_to_strinstream(stream, s, inst);
		stream << "\t" << inst_name << " " << x64_reg64::names[i];
		stream.flush();

		expected_lines.push_back(stream.str());

		s << inst;
	}
}

template<typename C, typename T, typename U>
void x64_testing::jmpcall_regptr(std::string inst_name, instruction_stream& s, std::vector<std::string>& expected_lines, int ni)
{
	for (auto i = 0; i < ni; i++)
	{
		auto inst = C(T(U(i)));
		stringstream stream;

		add_to_strinstream(stream, s, inst);
		string name = U::names[i];
		if (U(i).is_bp() || U(i).is_r13())
			name += "+0x0";
		stream << "\t" << inst_name << " QWORD PTR [" << name << ']';
		stream.flush();

		expected_lines.push_back(stream.str());

		s << inst;
	}
}

template<typename C, typename T, typename U>
void x64_testing::jmpcall_regptr_idx(std::string inst_name, instruction_stream& s, std::vector<std::string>& expected_lines, int ni)
{
	for (auto i = 0; i < ni; i++)
	{
		for (auto j = 0; j < ni; j++)
		{
			if (U(j).is_sp() || U(j).is_r12())
				continue;

			auto shift = (i + j) & 3;
			int scale_fact = 1 << shift;

			auto inst = C(T(U(i)), U(j), (sib_scale) shift);
			stringstream stream;

			add_to_strinstream(stream, s, inst);
			string name = U::names[i];
			stream << "\t" << inst_name << " QWORD PTR [" << name << '+' << U::names[j] << '*';
			stream << scale_fact;
			if (U(i).is_bp() || U(i).is_r13())
				stream << "+0x0";
			stream << ']';
			stream.flush();

			expected_lines.push_back(stream.str());

			s << inst;
		}
	}
}

template<typename C, typename T, typename U, typename V>
void x64_testing::jmpcall_regptr_idx_off(std::string inst_name, instruction_stream& s, std::vector<std::string>& expected_lines, int ni)
{
	for (auto i = 0; i < ni; i++)
	{
		for (auto j = 0; j < ni; j++)
		{
			if (U(j).is_sp() || U(j).is_r12())
				continue;

			V off = i * 234;

			auto shift = (i + j) & 3;
			int scale_fact = 1 << shift;

			auto inst = C(T(U(i)), U(j), (sib_scale) shift, off);
			stringstream stream;

			add_to_strinstream(stream, s, inst);
			string name = U::names[i];
			stream << "\t" << inst_name << " QWORD PTR [" << name << '+' << U::names[j] << '*';
			stream << scale_fact;
			if (off < 0)
				stream  << "-0x" << hex << setw(0) << ((int) -off);
			else
				stream  << "+0x" << hex << setw(0) << ((int) off);
			stream << ']';
			stream.flush();

			expected_lines.push_back(stream.str());

			s << inst;
		}
	}
}

template<typename C, typename T, typename U, typename V>
void x64_testing::jmpcall_regptr_off(std::string inst_name, instruction_stream& s, std::vector<std::string>& expected_lines, int ni)
{
	for (auto i = 0; i < ni; i++)
	{
		V off = i * 234;

		auto inst = C(T(U(i)), off);
		stringstream stream;

		add_to_strinstream(stream, s, inst);
		string name = U::names[i];
		stream << "\t" << inst_name << " QWORD PTR [" << name;
		if (off < 0)
			stream  << "-0x" << hex << setw(0) << ((int) -off);
		else
			stream  << "+0x" << hex << setw(0) << ((int) off);
		stream << ']';
		stream.flush();

		expected_lines.push_back(stream.str());

		s << inst;
	}
}

template<typename C>
void x64_testing::test_srcdst_oper_base(std::string inst_name, instruction_stream& s, std::vector<std::string>& expected_lines)
{
//	direct_reg_reg<C, x64_reg64>(inst_name, s, expected_lines, 16);
//	direct_reg_reg<C, x64_reg32>(inst_name, s, expected_lines, 16);
//	direct_reg_reg<C, x64_reg16>(inst_name, s, expected_lines, 16);
	direct_reg_reg<C, x64_reg8>(inst_name, s, expected_lines, 16);
//
//	regptr_reg<C, x64_reg64, x64_reg64, x64_reg_ptr64>(inst_name, s, expected_lines, "QWORD", 16, 16);
//	regptr_reg<C, x64_reg64, x64_reg32, x64_reg_ptr64>(inst_name, s, expected_lines, "DWORD", 16, 8);
//	regptr_reg<C, x64_reg64, x64_reg16, x64_reg_ptr64>(inst_name, s, expected_lines, "WORD", 16, 8);
//	regptr_reg<C, x64_reg64, x64_reg8l, x64_reg_ptr64>(inst_name, s, expected_lines, "BYTE", 16, 4);
//
//	regptr_reg<C, x64_reg32, x64_reg64, x64_reg_ptr32>(inst_name, s, expected_lines, "QWORD", 8, 16);
//	regptr_reg<C, x64_reg32, x64_reg32, x64_reg_ptr32>(inst_name, s, expected_lines, "DWORD", 8, 8);
//	regptr_reg<C, x64_reg32, x64_reg16, x64_reg_ptr32>(inst_name, s, expected_lines, "WORD", 8, 8);
//	regptr_reg<C, x64_reg32, x64_reg8l, x64_reg_ptr32>(inst_name, s, expected_lines, "BYTE", 8, 4);
//
//	reg_regptr<C, x64_reg64, x64_reg64, x64_reg_ptr64>(inst_name, s, expected_lines, "QWORD", 16, 16);
//	reg_regptr<C, x64_reg32, x64_reg64, x64_reg_ptr64>(inst_name, s, expected_lines, "DWORD", 8, 16);
//	reg_regptr<C, x64_reg16, x64_reg64, x64_reg_ptr64>(inst_name, s, expected_lines, "WORD", 8, 16);
//	reg_regptr<C, x64_reg8l, x64_reg64, x64_reg_ptr64>(inst_name, s, expected_lines, "BYTE", 4, 16);
//
//	reg_regptr<C, x64_reg64, x64_reg32, x64_reg_ptr32>(inst_name, s, expected_lines, "QWORD", 16, 8);
//	reg_regptr<C, x64_reg32, x64_reg32, x64_reg_ptr32>(inst_name, s, expected_lines, "DWORD", 8, 8);
//	reg_regptr<C, x64_reg16, x64_reg32, x64_reg_ptr32>(inst_name, s, expected_lines, "WORD", 8, 8);
//	reg_regptr<C, x64_reg8l, x64_reg32, x64_reg_ptr32>(inst_name, s, expected_lines, "BYTE", 4, 8);
//
//
//	regptr_offs_reg<C, x64_reg64, x64_reg64, x64_reg_ptr64, int8_t>(inst_name, s, expected_lines, "QWORD", 16, 16);
//	regptr_offs_reg<C, x64_reg64, x64_reg32, x64_reg_ptr64, int8_t>(inst_name, s, expected_lines, "DWORD", 16, 8);
//	regptr_offs_reg<C, x64_reg64, x64_reg16, x64_reg_ptr64, int8_t>(inst_name, s, expected_lines, "WORD", 16, 8);
//	regptr_offs_reg<C, x64_reg64, x64_reg8l, x64_reg_ptr64, int8_t>(inst_name, s, expected_lines, "BYTE", 16, 4);
//
//	regptr_offs_reg<C, x64_reg32, x64_reg64, x64_reg_ptr32, int8_t>(inst_name, s, expected_lines, "QWORD", 8, 16);
//	regptr_offs_reg<C, x64_reg32, x64_reg32, x64_reg_ptr32, int8_t>(inst_name, s, expected_lines, "DWORD", 8, 8);
//	regptr_offs_reg<C, x64_reg32, x64_reg16, x64_reg_ptr32, int8_t>(inst_name, s, expected_lines, "WORD", 8, 8);
//	regptr_offs_reg<C, x64_reg32, x64_reg8l, x64_reg_ptr32, int8_t>(inst_name, s, expected_lines, "BYTE", 8, 4);
//
//
//	reg_regptr_offs<C, x64_reg64, x64_reg64, x64_reg_ptr64, int8_t>(inst_name, s, expected_lines, "QWORD", 16, 16);
//	reg_regptr_offs<C, x64_reg32, x64_reg64, x64_reg_ptr64, int8_t>(inst_name, s, expected_lines, "DWORD", 8, 16);
//	reg_regptr_offs<C, x64_reg16, x64_reg64, x64_reg_ptr64, int8_t>(inst_name, s, expected_lines, "WORD", 8, 16);
//	reg_regptr_offs<C, x64_reg8l, x64_reg64, x64_reg_ptr64, int8_t>(inst_name, s, expected_lines, "BYTE", 4, 16);
//
//	reg_regptr_offs<C, x64_reg64, x64_reg32, x64_reg_ptr32, int8_t>(inst_name, s, expected_lines, "QWORD", 16, 8);
//	reg_regptr_offs<C, x64_reg32, x64_reg32, x64_reg_ptr32, int8_t>(inst_name, s, expected_lines, "DWORD", 8, 8);
//	reg_regptr_offs<C, x64_reg16, x64_reg32, x64_reg_ptr32, int8_t>(inst_name, s, expected_lines, "WORD", 8, 8);
//	reg_regptr_offs<C, x64_reg8l, x64_reg32, x64_reg_ptr32, int8_t>(inst_name, s, expected_lines, "BYTE", 4, 8);
//
//	regptr_offs_reg<C, x64_reg64, x64_reg64, x64_reg_ptr64, int32_t>(inst_name, s, expected_lines, "QWORD", 16, 16);
//	regptr_offs_reg<C, x64_reg64, x64_reg32, x64_reg_ptr64, int32_t>(inst_name, s, expected_lines, "DWORD", 16, 8);
//	regptr_offs_reg<C, x64_reg64, x64_reg16, x64_reg_ptr64, int32_t>(inst_name, s, expected_lines, "WORD", 16, 8);
//	regptr_offs_reg<C, x64_reg64, x64_reg8l, x64_reg_ptr64, int32_t>(inst_name, s, expected_lines, "BYTE", 16, 4);
//
//	regptr_offs_reg<C, x64_reg32, x64_reg64, x64_reg_ptr32, int32_t>(inst_name, s, expected_lines, "QWORD", 8, 16);
//	regptr_offs_reg<C, x64_reg32, x64_reg32, x64_reg_ptr32, int32_t>(inst_name, s, expected_lines, "DWORD", 8, 8);
//	regptr_offs_reg<C, x64_reg32, x64_reg16, x64_reg_ptr32, int32_t>(inst_name, s, expected_lines, "WORD", 8, 8);
//	regptr_offs_reg<C, x64_reg32, x64_reg8l, x64_reg_ptr32, int32_t>(inst_name, s, expected_lines, "BYTE", 8, 4);
//
//
//	reg_regptr_offs<C, x64_reg64, x64_reg64, x64_reg_ptr64, int32_t>(inst_name, s, expected_lines, "QWORD", 16, 16);
//	reg_regptr_offs<C, x64_reg32, x64_reg64, x64_reg_ptr64, int32_t>(inst_name, s, expected_lines, "DWORD", 8, 16);
//	reg_regptr_offs<C, x64_reg16, x64_reg64, x64_reg_ptr64, int32_t>(inst_name, s, expected_lines, "WORD", 8, 16);
//	reg_regptr_offs<C, x64_reg8l, x64_reg64, x64_reg_ptr64, int32_t>(inst_name, s, expected_lines, "BYTE", 4, 16);
//
//	reg_regptr_offs<C, x64_reg64, x64_reg32, x64_reg_ptr32, int32_t>(inst_name, s, expected_lines, "QWORD", 16, 8);
//	reg_regptr_offs<C, x64_reg32, x64_reg32, x64_reg_ptr32, int32_t>(inst_name, s, expected_lines, "DWORD", 8, 8);
//	reg_regptr_offs<C, x64_reg16, x64_reg32, x64_reg_ptr32, int32_t>(inst_name, s, expected_lines, "WORD", 8, 8);
//	reg_regptr_offs<C, x64_reg8l, x64_reg32, x64_reg_ptr32, int32_t>(inst_name, s, expected_lines, "BYTE", 4, 8);
//
//	reg_imm<C, x64_reg32, uint32_t>(inst_name, s, expected_lines, 8);
//	reg_imm<C, x64_reg16, uint16_t>(inst_name, s, expected_lines, 8);
//	reg_imm<C, x64_reg8, uint8_t>(inst_name, s, expected_lines, 8);
//
//	reg_imm_addr<C, x64_reg64, int32_t>(inst_name, s, expected_lines, "QWORD", 16);
//	reg_imm_addr<C, x64_reg32, int32_t>(inst_name, s, expected_lines, "DWORD", 8);
//	reg_imm_addr<C, x64_reg16, int32_t>(inst_name, s, expected_lines, "WORD", 8);
//	reg_imm_addr<C, x64_reg8, int32_t>(inst_name, s, expected_lines, "BYTE", 8);
//
//	imm_addr_reg<C, int32_t, x64_reg64>(inst_name, s, expected_lines, "QWORD", 16);
//	imm_addr_reg<C, int32_t, x64_reg32>(inst_name, s, expected_lines, "DWORD", 8);
//	imm_addr_reg<C, int32_t, x64_reg16>(inst_name, s, expected_lines, "WORD", 8);
//	imm_addr_reg<C, int32_t, x64_reg8>(inst_name, s, expected_lines, "BYTE", 8);
//
//
//
//
//	reg_reg_ptr_idx<C, x64_reg64, x64_reg_ptr64, x64_reg64>(inst_name, s, expected_lines, "QWORD", 16, 16);
//	reg_reg_ptr_idx<C, x64_reg32, x64_reg_ptr64, x64_reg64>(inst_name, s, expected_lines, "DWORD", 8, 16);
//	reg_reg_ptr_idx<C, x64_reg16, x64_reg_ptr64, x64_reg64>(inst_name, s, expected_lines, "WORD", 8, 16);
//	reg_reg_ptr_idx<C, x64_reg8l, x64_reg_ptr64, x64_reg64>(inst_name, s, expected_lines, "BYTE", 4, 16);
//
//	reg_reg_ptr_idx<C, x64_reg64, x64_reg_ptr32, x64_reg32>(inst_name, s, expected_lines, "QWORD", 16, 8);
//	reg_reg_ptr_idx<C, x64_reg32, x64_reg_ptr32, x64_reg32>(inst_name, s, expected_lines, "DWORD", 8, 8);
//	reg_reg_ptr_idx<C, x64_reg16, x64_reg_ptr32, x64_reg32>(inst_name, s, expected_lines, "WORD", 8, 8);
//	reg_reg_ptr_idx<C, x64_reg8l, x64_reg_ptr32, x64_reg32>(inst_name, s, expected_lines, "BYTE", 4, 8);
//
//	reg_ptr_idx_reg<C, x64_reg_ptr64, x64_reg64, x64_reg64>(inst_name, s, expected_lines, "QWORD", 16, 16);
//	reg_ptr_idx_reg<C, x64_reg_ptr64, x64_reg64, x64_reg32>(inst_name, s, expected_lines, "DWORD", 16, 8);
//	reg_ptr_idx_reg<C, x64_reg_ptr64, x64_reg64, x64_reg16>(inst_name, s, expected_lines, "WORD", 16, 8);
//	reg_ptr_idx_reg<C, x64_reg_ptr64, x64_reg64, x64_reg8l>(inst_name, s, expected_lines, "BYTE", 16, 4);
//
//	reg_ptr_idx_reg<C, x64_reg_ptr32, x64_reg32, x64_reg64>(inst_name, s, expected_lines, "QWORD", 8, 16);
//	reg_ptr_idx_reg<C, x64_reg_ptr32, x64_reg32, x64_reg32>(inst_name, s, expected_lines, "DWORD", 8, 8);
//	reg_ptr_idx_reg<C, x64_reg_ptr32, x64_reg32, x64_reg16>(inst_name, s, expected_lines, "WORD", 8, 8);
//	reg_ptr_idx_reg<C, x64_reg_ptr32, x64_reg32, x64_reg8l>(inst_name, s, expected_lines, "BYTE", 8, 4);
//
//
//	reg_reg_ptr_idx_off<C, x64_reg64, x64_reg_ptr64, x64_reg64, int8_t>(inst_name, s, expected_lines, "QWORD", 16, 16);
//	reg_reg_ptr_idx_off<C, x64_reg32, x64_reg_ptr64, x64_reg64, int8_t>(inst_name, s, expected_lines, "DWORD", 8, 16);
//	reg_reg_ptr_idx_off<C, x64_reg16, x64_reg_ptr64, x64_reg64, int8_t>(inst_name, s, expected_lines, "WORD", 8, 16);
//	reg_reg_ptr_idx_off<C, x64_reg8l, x64_reg_ptr64, x64_reg64, int8_t>(inst_name, s, expected_lines, "BYTE", 4, 16);
//
//	reg_reg_ptr_idx_off<C, x64_reg64, x64_reg_ptr32, x64_reg32, int8_t>(inst_name, s, expected_lines, "QWORD", 16, 8);
//	reg_reg_ptr_idx_off<C, x64_reg32, x64_reg_ptr32, x64_reg32, int8_t>(inst_name, s, expected_lines, "DWORD", 8, 8);
//	reg_reg_ptr_idx_off<C, x64_reg16, x64_reg_ptr32, x64_reg32, int8_t>(inst_name, s, expected_lines, "WORD", 8, 8);
//	reg_reg_ptr_idx_off<C, x64_reg8l, x64_reg_ptr32, x64_reg32, int8_t>(inst_name, s, expected_lines, "BYTE", 4, 8);
//
//	reg_ptr_idx_off_reg<C, x64_reg_ptr64, x64_reg64, int8_t, x64_reg64>(inst_name, s, expected_lines, "QWORD", 16, 16);
//	reg_ptr_idx_off_reg<C, x64_reg_ptr64, x64_reg64, int8_t, x64_reg32>(inst_name, s, expected_lines, "DWORD", 16, 8);
//	reg_ptr_idx_off_reg<C, x64_reg_ptr64, x64_reg64, int8_t, x64_reg16>(inst_name, s, expected_lines, "WORD", 16, 8);
//	reg_ptr_idx_off_reg<C, x64_reg_ptr64, x64_reg64, int8_t, x64_reg8l>(inst_name, s, expected_lines, "BYTE", 16, 4);
//
//	reg_ptr_idx_off_reg<C, x64_reg_ptr32, x64_reg32, int8_t, x64_reg64>(inst_name, s, expected_lines, "QWORD", 8, 16);
//	reg_ptr_idx_off_reg<C, x64_reg_ptr32, x64_reg32, int8_t, x64_reg32>(inst_name, s, expected_lines, "DWORD", 8, 8);
//	reg_ptr_idx_off_reg<C, x64_reg_ptr32, x64_reg32, int8_t, x64_reg16>(inst_name, s, expected_lines, "WORD", 8, 8);
//	reg_ptr_idx_off_reg<C, x64_reg_ptr32, x64_reg32, int8_t, x64_reg8l>(inst_name, s, expected_lines, "BYTE", 8, 4);
//
//
//	reg_reg_ptr_idx_off<C, x64_reg64, x64_reg_ptr64, x64_reg64, int32_t>(inst_name, s, expected_lines, "QWORD", 16, 16);
//	reg_reg_ptr_idx_off<C, x64_reg32, x64_reg_ptr64, x64_reg64, int32_t>(inst_name, s, expected_lines, "DWORD", 8, 16);
//	reg_reg_ptr_idx_off<C, x64_reg16, x64_reg_ptr64, x64_reg64, int32_t>(inst_name, s, expected_lines, "WORD", 8, 16);
//	reg_reg_ptr_idx_off<C, x64_reg8l, x64_reg_ptr64, x64_reg64, int32_t>(inst_name, s, expected_lines, "BYTE", 4, 16);
//
//	reg_reg_ptr_idx_off<C, x64_reg64, x64_reg_ptr32, x64_reg32, int32_t>(inst_name, s, expected_lines, "QWORD", 16, 8);
//	reg_reg_ptr_idx_off<C, x64_reg32, x64_reg_ptr32, x64_reg32, int32_t>(inst_name, s, expected_lines, "DWORD", 8, 8);
//	reg_reg_ptr_idx_off<C, x64_reg16, x64_reg_ptr32, x64_reg32, int32_t>(inst_name, s, expected_lines, "WORD", 8, 8);
//	reg_reg_ptr_idx_off<C, x64_reg8l, x64_reg_ptr32, x64_reg32, int32_t>(inst_name, s, expected_lines, "BYTE", 4, 8);
//
//	reg_ptr_idx_off_reg<C, x64_reg_ptr64, x64_reg64, int32_t, x64_reg64>(inst_name, s, expected_lines, "QWORD", 16, 16);
//	reg_ptr_idx_off_reg<C, x64_reg_ptr64, x64_reg64, int32_t, x64_reg32>(inst_name, s, expected_lines, "DWORD", 16, 8);
//	reg_ptr_idx_off_reg<C, x64_reg_ptr64, x64_reg64, int32_t, x64_reg16>(inst_name, s, expected_lines, "WORD", 16, 8);
//	reg_ptr_idx_off_reg<C, x64_reg_ptr64, x64_reg64, int32_t, x64_reg8l>(inst_name, s, expected_lines, "BYTE", 16, 4);
//
//	reg_ptr_idx_off_reg<C, x64_reg_ptr32, x64_reg32, int32_t, x64_reg64>(inst_name, s, expected_lines, "QWORD", 8, 16);
//	reg_ptr_idx_off_reg<C, x64_reg_ptr32, x64_reg32, int32_t, x64_reg32>(inst_name, s, expected_lines, "DWORD", 8, 8);
//	reg_ptr_idx_off_reg<C, x64_reg_ptr32, x64_reg32, int32_t, x64_reg16>(inst_name, s, expected_lines, "WORD", 8, 8);
//	reg_ptr_idx_off_reg<C, x64_reg_ptr32, x64_reg32, int32_t, x64_reg8l>(inst_name, s, expected_lines, "BYTE", 8, 4);
}

template<typename C>
void x64_testing::test_x64_jmpcall_base(std::string inst_name, instruction_stream& s, std::vector<std::string>& expected_lines)
{
	jmpcall_reg<C>(inst_name, s, expected_lines);
	jmpcall_regptr<C, x64_reg_ptr64, x64_reg64>(inst_name, s, expected_lines, 16);
	jmpcall_regptr<C, x64_reg_ptr32, x64_reg32>(inst_name, s, expected_lines, 8);
	jmpcall_regptr_off<C, x64_reg_ptr64, x64_reg64, int8_t>(inst_name, s, expected_lines, 8);
	jmpcall_regptr_off<C, x64_reg_ptr64, x64_reg64, int32_t>(inst_name, s, expected_lines, 8);
	jmpcall_regptr_off<C, x64_reg_ptr32, x64_reg32, int8_t>(inst_name, s, expected_lines, 8);
	jmpcall_regptr_off<C, x64_reg_ptr32, x64_reg32, int32_t>(inst_name, s, expected_lines, 8);
	jmpcall_regptr_idx<C, x64_reg_ptr64, x64_reg64>(inst_name, s, expected_lines, 8);
	jmpcall_regptr_idx<C, x64_reg_ptr32, x64_reg32>(inst_name, s, expected_lines, 8);
	jmpcall_regptr_idx_off<C, x64_reg_ptr32, x64_reg32, int8_t>(inst_name, s, expected_lines, 8);
	jmpcall_regptr_idx_off<C, x64_reg_ptr32, x64_reg32, int32_t>(inst_name, s, expected_lines, 8);

	auto jmp_pos = s.pos();
	int32_t off = 0x12345678;
	auto inst = C(off);

	stringstream stream;
	add_to_strinstream(stream, s, inst);
	stream << "\t" << inst_name << " 0x" << hex << setw(0) << (uint64_t(jmp_pos) + off + inst.size()) << flush;
	expected_lines.push_back(stream.str());
	s << inst;
}

void say_hello()
{
	cout << "Hello world\n";
}

int eval(const expression& exp)
{
	switch(exp.type)
	{
		case expr_type::add: return eval(exp.expressions[0]) + eval(exp.expressions[1]);
		case expr_type::sub: return eval(exp.expressions[0]) - eval(exp.expressions[1]);
		case expr_type::mul: return eval(exp.expressions[0]) * eval(exp.expressions[1]);
		case expr_type::div: return eval(exp.expressions[0]) / eval(exp.expressions[1]);
		case expr_type::num: return exp.num;
	}

	return 0;
}

// XXX: Use some ref-counting mechanism to temporarily disable registers
/* XXX: Improve so that we can RESERVE a register, and take note when it's
 *      actually been written to. This way we don't have to save registers that
 *      have been reserved, but not yet used.
 */

class dirty_register
{
public:
	dirty_register(int n)
	{
		dirty.resize(n);
	}

	dirty_register(int n, std::initializer_list<int> dont_use)
	 : dirty_register(n)
	{
		/* Mark the entries in the dont_use list as used, so we can't use them. */
		for(int x: dont_use)
			use(x);
	}

	inline void use(int reg) { dirty[reg] = true; }
	inline void release(int reg) { dirty[reg] = false; }
	inline bool is_used(int reg) { return dirty[reg]; }

	int get()
	{
		vector<bool>::iterator location = find(dirty.begin(), dirty.end(), false);

		if (location == dirty.end())
			return -1;

		int idx = location - dirty.begin();

		use(idx);
		return idx;
	}

private:
	vector<bool> dirty;
};

dirty_register dr(x64_reg32::n, { x64_regs::eax.value, x64_regs::esp.value, x64_regs::ebp.value });

class steal_register
{
public:
	steal_register(instruction_stream& s, dirty_register& dr)
	 : s(s), dr(dr)
	{ }

	steal_register(instruction_stream& s, dirty_register& dr, x64_reg32 reg)
	 : s(s), dr(dr), reg(reg)
	{
		steal(reg);
	}

	void steal(x64_reg32 reg)
	{
		used = dr.is_used(reg.value);
		if (used)
		{
			/* Can we temporarily store it somewhere else? */
			tmp_reg = dr.get();
			if (tmp_reg != -1)
			{
				/* Yes. Just store it in some temporary register */
				s << x64_mov(x64_reg32(tmp_reg), reg);
			}
			else
			{
				/* Nope. We'll have to push it */
				s << x64_push(x64_reg64(reg.value)); /* Can't push 32-bit registers on x64 */
				pushed = true;
			}
		}
		else
		{
			dr.use(reg.value);
		}

		stolen = 1;
	}

	~steal_register() { give_back(); }

	void give_back()
	{
		if (!stolen)
			return;

		if (used)
		{
			if (pushed)
			{
				s << x64_pop(x64_reg64(reg.value));
				pushed = false;
			}
			else
			{
				s << x64_mov(reg, x64_reg32(tmp_reg));
			}

			used = false;
		}
		else
		{
			dr.release(reg.value);
		}
	}

private:
	instruction_stream& s;
	dirty_register& dr;
	x64_reg32 reg = x64_reg32(0);

	bool stolen = 0;
	int tmp_reg = 0;
	bool used = false;
	bool pushed = false;
};

void compile(instruction_stream& s, const expression& exp, const x64_reg32& target = x64_regs::eax)
{
	cout << "Compiling expr type: ";
	switch(exp.type)
	{
	case expr_type::add: cout << "add\n"; break;
	case expr_type::sub: cout << "sub\n"; break;
	case expr_type::mul: cout << "mul\n"; break;
	case expr_type::div: cout << "div\n"; break;
	case expr_type::num: cout << "num\n"; break;
	}

	if (exp.type == expr_type::num)
	{
		s << x64_mov(target, static_cast<uint32_t>(exp.num));
		return;
	}

	auto reg_idx = dr.get();
	if (reg_idx == -1)
		throw exception();

	auto tmp_reg = x64_reg32(reg_idx);

	cout << "expressions into " << x64_reg32::names[tmp_reg.value] << " and " <<x64_reg32::names[target.value] << '\n';

	compile(s, exp.expressions[0], tmp_reg);

	s << x64_nop1();

	switch(exp.type)
	{
		case expr_type::add:
			compile(s, exp.expressions[1], target);
			s << x64_add(target, tmp_reg);
			break;
		case expr_type::sub:
			compile(s, exp.expressions[1], target);
			s << x64_add(target, tmp_reg);
			break;
		case expr_type::mul:
			compile(s, exp.expressions[1], target);
			if (x64_regs::eax.value != target.value)
				s << x64_mov(x64_regs::eax, target);
			s << x64_mul(tmp_reg);
			if (x64_regs::eax.value != target.value)
				s << x64_mov(target, x64_regs::eax);
			break;
		case expr_type::div:
		{
			// Unsigned divide EDX:EAX by r/m32, with result stored in EAX = Quotient, EDX = Remainder.
			if (x64_regs::eax.value != tmp_reg.value)
				s << x64_mov(x64_regs::eax, tmp_reg);

			/* We're altering EDX below, so make sure that, if it's used, it's restored after this scope */
			steal_register thief(s, dr, x64_regs::edx);

			auto reg_divider = dr.get();
			if (reg_divider == -1)
				throw exception();

			compile(s, exp.expressions[1], reg_divider);

			s << x64_mov(x64_regs::edx, 0);
			s << x64_div(x64_reg32(reg_divider));

			dr.release(reg_divider);
			thief.give_back();

			if (x64_regs::eax.value != target.value)
				s << x64_mov(target, x64_regs::eax);
			break;
		}

		case expr_type::num:
			assert(exp.type != expr_type::num);
			break;
	}

	s << x64_nop1();

	dr.release(reg_idx);
}

__asm__(".global test_asm_fn\n"
		"test_asm_fn:\n"
		"mov    $0xc,%edx\n"
		"mov    %edx,%ebx\n"
		"mov    $0x0,%edx\n"
		"div    %edx\n"
		"retq\n");

extern "C"
{
	int test_asm_fn();
}


//x64_mov(x64_reg_ptr64 addr, x64_reg64 reg)
void x64_testing::mov_unit_tests()
{
	instruction_stream s(allocator);



	s << x64_mov(x64_regs::al, x64_regs::bl);
	s << x64_mov(x64_regs::al, x64_regs::bh);
	s << x64_mov(x64_regs::ah, x64_regs::bl);
	s << x64_mov(x64_regs::ah, x64_regs::bh);

	s << x64_mov(x64_regs::spl, x64_regs::bh);



	cout << x64_disassembler::disassemble(s, "intel", true);

	exit(0);

	vector<string> expected_lines;

//	/* 64-bit only move shit */
//	reg_imm<x64_mov, x64_reg64, uint64_t>("movabs", s, expected_lines, 16);
//
//	/* 64 bit pointers only support the first register (al, ax, eax, rax) */
//	reg_ptr64<x64_reg64_0, uint64_t>(s, expected_lines, x64_regs::rax);
//	reg_ptr64<x64_reg32_0, uint32_t>(s, expected_lines, x64_regs::eax);
//	reg_ptr64<x64_reg16_0, uint16_t>(s, expected_lines, x64_regs::ax);
//	reg_ptr64<x64_reg8l_0, uint8_t>(s, expected_lines, x64_regs::al);
//
//	ptr64_reg<uint64_t, x64_reg64_0>(s, expected_lines, x64_regs::rax);
//	ptr64_reg<uint32_t, x64_reg32_0>(s, expected_lines, x64_regs::eax);
//	ptr64_reg<uint16_t, x64_reg16_0>(s, expected_lines, x64_regs::ax);
//	ptr64_reg<uint8_t, x64_reg8l_0>(s, expected_lines, x64_regs::al);
//
//	/* General stuff */
//	test_srcdst_oper_base<x64_mov>("mov", s, expected_lines);
//	test_srcdst_oper_base<x64_add>("add", s, expected_lines);
//	test_srcdst_oper_base<x64_sub>("sub", s, expected_lines);
//	test_srcdst_oper_base<x64_sbb>("sbb", s, expected_lines);
//	test_srcdst_oper_base<x64_adc>("adc", s, expected_lines);
//	test_srcdst_oper_base<x64_and>("and", s, expected_lines);
//	test_srcdst_oper_base<x64_or>("or", s, expected_lines);
//	test_srcdst_oper_base<x64_cmp>("cmp", s, expected_lines);
//	test_srcdst_oper_base<x64_xor>("xor", s, expected_lines);

//	test_x64_jmpcall_base<x64_jmp>("jmp", s, expected_lines);
//	test_x64_jmpcall_base<x64_call>("call", s, expected_lines);

	compare_assembly(s, expected_lines);

	auto start = high_resolution_clock::now();
	int n_loops = 10000;
	int n_per_loop = 6;//36 - 5;
	for (int i = 0; i < n_loops ; i++)
	{
		s << x64_mov(x64_regs::rax, (uint64_t) 0x1234567812345678);
		s << x64_mov(x64_regs::rax, (uint32_t) 0x12345678);
		s << x64_mov(x64_regs::eax, (uint32_t) 0x12345678);
		s << x64_mov(x64_regs::ax, (uint16_t) 0x1234);
		//s << x64_mov(x64_regs::ah, (uint8_t) 0x12);
		//s << x64_mov(x64_regs::sil, (uint8_t) 0x12);
		s << x64_mov(x64_regs::al, (uint8_t) 0x12);
	}

	auto n = n_per_loop * n_loops;
	auto stop = high_resolution_clock::now();
	auto duration = duration_cast<microseconds>(stop - start);
	auto seconds = ((double) duration.count() / 1000000.0);

	// -O0: Code creation: 3600000 instructions in 0.39589 seconds (9.09344e+06 instructions/second
	// -O3: Code creation: 3600000 instructions in 0.063178 seconds (5.69819e+07 instructions/second
	cout << "Code creation: " << n << " instructions in " << seconds << " seconds (" << ((double) n / seconds) << " instructions/second\n";

	exit(0);

//	for (int i = 0; i < 1024; i++)
//		s << x64_nop1();

#if 1
//	auto fn = s.entry_point<void()>();
//
//	uint64_t p = (uint64_t) &say_hello;
//
//	s << x64_mov(x64_regs::rax, (uint64_t) &say_hello);
//	s << x64_instruction(array<uint8_t, 2> { 0xff, 0xd0 });
//
//	s << x64_mov(x64_regs::rax, (uint64_t) &p);
//	s << x64_instruction(array<uint8_t, 2> { 0xff, 0x10 });
//
//	s << x64_mov(x64_regs::rax, (uint64_t) &p);
//	s << x64_instruction(array<uint8_t, 2> { 0xff, 0xa0 }, (uint32_t) 0);
//
//
//	//s << x64_instruction(array<uint8_t, 1> { 0xe9 }, (int32_t) 0);
//	//s << x64_instruction(array<uint8_t, 2> { x64_override::oper_size, 0xe9 }, (uint16_t) 0x0);

//	s << x64_ret();
//
//	s << x64_jmp((int32_t) 0x12);
//
//
//	s << x64_jecxz(0x7f);
//
//	s << x64_jo((int8_t) -12);
//
//	s << x64_neg(x64_regs::bl);
//	s << x64_neg(x64_regs::bh);
//	s << x64_neg(x64_regs::bx);
//	s << x64_neg(x64_regs::ebx);
//	s << x64_neg(x64_regs::rax);
//	s << x64_neg(x64_regs::r12);
//
//
//	fn();



	driver drv;
	//drv.trace_parsing = true;
	//drv.trace_scanning = true;
	drv.parse ("src/parser/parseme.txt");

	auto program = s.entry_point<int()>();
	compile(s, drv.expression_result);

	s << x64_ret();

	s << x64_nop1();
	s << x64_nop1();
	s << x64_mov(x64_reg8(0),x64_reg8(8));


	cout << x64_disassembler::disassemble(s, "intel", true);

	auto res = program();
	//auto res = test_asm_fn();

	std::cout << "expression.eval() : " << drv.expression_result.eval() << '\n';
	std::cout << "eval(expression)  : " << eval(drv.expression_result) << '\n';
	std::cout << "machine code      : " << res << '\n';

	exit(0);
#endif
}

void x64_testing::add_to_strinstream(stringstream& sstream, const instruction_stream& inst_stream, const instruction& inst)
{
	sstream << setfill('0');
	sstream << setw(0) << " " << hex << (intptr_t(inst_stream.pos())) << ":\t";

	uint8_t* data = static_cast<uint8_t*>(alloca(inst.size()));
	inst.emit_to(data);

	for (size_t x = 0; x < inst.size(); x++)
		sstream << setw(2) << ((int) data[x]) << " ";
}

void x64_testing::compare_assembly(const instruction_stream& s, const vector<string>& expected_lines)
{
	static regex spaces_regex("  *");

	/* Disassemble the instruction stream */
	auto disassembly = x64_disassembler::disassemble(s, "intel", true);

	/* Remove multiple spaces */
	disassembly = regex_replace(disassembly, spaces_regex, " ");

//	cout << "Expected output:\n";
//	for (const auto& line: expected_lines)
//		cout << line << '\n';

	cout << "Disassembled output:\n" << disassembly;

	/* Split into lines */
	vector<string> split_disassembly;
	regex pattern("(\\n)");
	copy( sregex_token_iterator(disassembly.begin(), disassembly.end(), pattern, -1),
			sregex_token_iterator(), back_inserter(split_disassembly));

	/* remove the 'header' */
	split_disassembly.erase(split_disassembly.begin(), split_disassembly.begin() + 7);

	if (expected_lines.size() != split_disassembly.size())
	{
		std::stringstream stream;

		stream << "Expected number of instructions does not match:\n";
		stream << "Expected: " << expected_lines.size() << '\n';
		stream << "Result  : " << split_disassembly.size() << '\n' << flush;

		throw unit_test_exception(stream.str());
	}

	for (size_t i = 0; i < expected_lines.size(); i++)
	{
		if (expected_lines[i] != split_disassembly[i])
		{
			std::stringstream stream;

			stream << "Instruction mismatch:\n";
			stream << "Expected: " << expected_lines[i] << '\n';
			stream << "Result  : " << split_disassembly[i] << '\n' << flush;

			throw unit_test_exception(stream.str());
		}
	}
}
