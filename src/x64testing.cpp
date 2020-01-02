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

#include "x64testing.h"
#include "x64disassembler.h"
#include "x64instruction.h"

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

template<typename T>
void x64_testing::direct_reg_reg(instruction_stream& s, vector<string>& expected_lines, int num_regs)
{
	for (auto i = 0; i < num_regs; i++)
	{
		for (auto j = 0; j < num_regs; j++)
		{
			auto inst = x64_mov(T(j), T(i));

			stringstream stream;
			add_to_strinstream(stream, s, inst);
			stream << "\tmov " << T::names[j] << "," << T::names[i];

			expected_lines.push_back(stream.str());

			s << inst;
		}
	}
}

template<typename T, typename U, typename V>
void x64_testing::regptr_reg(instruction_stream& s, vector<string>& expected_lines, string width_name, int nj, int ni)
{
	for (auto i = 0; i < ni; i++)
	{
		for (auto j = 0; j < nj; j++)
		{
			auto inst = x64_mov(V(T(j)), U(i));

			stringstream stream;

			add_to_strinstream(stream, s, inst);
			string name = T::names[j];
			if (T(j).is_bp() || T(j).is_r13())
				name += "+0x0";
			stream << "\tmov " << width_name << " PTR [" << name << "]," << U::names[i];
			stream.flush();

			expected_lines.push_back(stream.str());

			s << inst;
		}
	}
}

template<typename T, typename U, typename V, typename W>
void x64_testing::regptr_offs_reg(instruction_stream& s, vector<string>& expected_lines, string width_name, int nj, int ni)
{
	for (auto i = 0; i < ni; i++)
	{
		for (auto j = 0; j < nj; j++)
		{
			W offs = (j + 1) * 999 + (i + 1) * 333;
			auto inst = x64_mov(V(T(j)), offs, U(i));

			stringstream stream;

			add_to_strinstream(stream, s, inst);
			stream << "\tmov " << width_name << " PTR [" << T::names[j];

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

template<typename T, typename U, typename V>
void x64_testing::reg_regptr(instruction_stream& s, vector<string>& expected_lines, string width_name, int nj, int ni)
{
	for (auto i = 0; i < ni; i++)
	{
		for (auto j = 0; j < nj; j++)
		{
			auto inst = x64_mov(T(j), V(U(i)));

			stringstream stream;

			add_to_strinstream(stream, s, inst);
			string name = U::names[i];
			if (U(i).is_bp() || U(i).is_r13())
				name += "+0x0";
			stream << "\tmov " << T::names[j] << "," << width_name << " PTR [" << name << ']';
			stream.flush();

			expected_lines.push_back(stream.str());

			s << inst;
		}
	}
}

template<typename T, typename U, typename V, typename W>
void x64_testing::reg_regptr_offs(instruction_stream& s, vector<string>& expected_lines, string width_name, int nj, int ni)
{
	for (auto i = 0; i < ni; i++)
	{
		for (auto j = 0; j < nj; j++)
		{
			W offs = (j + 1) * 999 + (i + 1) * 333;
			auto inst = x64_mov(T(j), V(U(i)), offs);

			stringstream stream;

			add_to_strinstream(stream, s, inst);
			string name = U::names[i];
			stream << "\tmov " << T::names[j] << "," << width_name << " PTR [" << name;
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

template<typename T, typename U>
void x64_testing::reg_imm(instruction_stream& s, vector<string>& expected_lines, string inst_name, int n)
{
	for (auto i = 0; i < n; i++)
	{
		U imm = (i + 1) * 0x6732409234657202;
		auto inst = x64_mov(T(i), imm);

		stringstream stream;

		add_to_strinstream(stream, s, inst);
		stream << "\t" << inst_name << ' ' << T::names[i] << ",0x" << hex << setw(0) << ((uint64_t)imm);
		stream.flush();

		expected_lines.push_back(stream.str());

		s << inst;
	}
}

template<typename T, typename U>
void x64_testing::reg_imm_addr(instruction_stream& s, vector<string>& expected_lines, string width_name, int n)
{
	for (auto i = 0; i < n; i++)
	{
		U imm = U((i + 1) * 0x6732409234657202);
		auto inst = x64_mov(T(i), x64_addr_ptr<U>(reinterpret_cast<U>(imm)));

		stringstream stream;

		add_to_strinstream(stream, s, inst);
		stream << "\tmov " << T::names[i] << "," << width_name << " PTR ds:0x" << hex << setw(0) << ((int64_t)imm);
		stream.flush();

		expected_lines.push_back(stream.str());

		s << inst;
	}
}

template<typename T, typename U>
void x64_testing::imm_addr_reg(instruction_stream& s, vector<string>& expected_lines, string width_name, int n)
{
	for (auto i = 0; i < n; i++)
	{
		T imm = (i + 1) * 0x6732409234657202;
		auto inst = x64_mov(x64_addr_ptr<T>(reinterpret_cast<T>(imm)), U(i));

		stringstream stream;

		add_to_strinstream(stream, s, inst);
		stream << "\tmov " << width_name << " PTR ds:0x" << hex << setw(0) << ((int64_t)imm) << ',' << U::names[i];
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

template<typename T, typename U, typename V>
void x64_testing::reg_reg_ptr_idx(instruction_stream& s, std::vector<std::string>& expected_lines, std::string width_name, int ni, int nj)
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
				auto inst = x64_mov(dst, regptr, regidx, static_cast<sib_scale>(shift));

				stringstream stream;

				add_to_strinstream(stream, s, inst);
				stream << "\tmov " << T::names[i] << "," << width_name
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

template<typename T, typename U, typename V>
void x64_testing::reg_ptr_idx_reg(instruction_stream& s, std::vector<std::string>& expected_lines, std::string width_name, int ni, int nj)
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
				auto inst = x64_mov(regptr, regoff, static_cast<sib_scale>(shift), src);

				stringstream stream;

				add_to_strinstream(stream, s, inst);
				stream << "\tmov " << width_name
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

template<typename T, typename U, typename V, typename W>
void x64_testing::reg_reg_ptr_idx_off(instruction_stream& s, std::vector<std::string>& expected_lines, std::string width_name, int ni, int nj)
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
				auto inst = x64_mov(dst, regptr, regidx, static_cast<sib_scale>(shift), offs);

				stringstream stream;

				add_to_strinstream(stream, s, inst);
				stream << "\tmov " << T::names[i] << "," << width_name
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

template<typename T, typename U, typename V, typename W>
void x64_testing::reg_ptr_idx_off_reg(instruction_stream& s, std::vector<std::string>& expected_lines, std::string width_name, int ni, int nj)
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
				auto inst = x64_mov(regptr, regoff, static_cast<sib_scale>(shift), offs, src);

				stringstream stream;

				add_to_strinstream(stream, s, inst);
				stream << "\tmov " << width_name
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

//x64_mov(x64_reg_ptr64 addr, x64_reg64 reg)
void x64_testing::mov_unit_tests()
{
	instruction_stream s(allocator);

#if 0
//	s << x64_mov(x64_regs::rax, (uint64_t) 0x1234567812345678);
//	s << x64_mov(x64_regs::rax, (uint32_t) 0x12345678);
//	s << x64_mov(x64_regs::eax, (uint32_t) 0x12345678);
//	s << x64_mov(x64_regs::ax, (uint16_t) 0x1234);
//	s << x64_mov(x64_regs::ah, (uint8_t) 0x12);
//	s << x64_mov(x64_regs::al, (uint8_t) 0x12);
//
//	s << x64_nop1();
//
//	asm("mov %gs,%rax");
//	asm("mov %rax,%gs");
//
//	s << x64_mov(x64_regs::rax, x64_addr_ptr<uint32_t>(0x12345678));
//	s << x64_mov(x64_regs::rbp, x64_addr_ptr<uint32_t>(0x12345678));
//	s << x64_mov(x64_regs::rsp, x64_addr_ptr<uint32_t>(0x12345678));
//	s << x64_mov(x64_regs::r12, x64_addr_ptr<uint32_t>(0x12345678));
//	s << x64_mov(x64_regs::r13, x64_addr_ptr<uint32_t>(0x12345678));
//
//	s << x64_mov(x64_regs::eax, x64_addr_ptr<uint32_t>(0x12345678));
//	s << x64_mov(x64_regs::ax, x64_addr_ptr<uint32_t>(0x12345678));
//	s << x64_mov(x64_regs::ah, x64_addr_ptr<uint32_t>(0x12345678));
//	s << x64_mov(x64_regs::al, x64_addr_ptr<uint32_t>(0x12345678));
//
//	s << x64_nop1();
//
//	s << x64_mov(x64_addr_ptr<uint32_t>(0x12345678), x64_regs::rax);
//	s << x64_mov(x64_addr_ptr<uint32_t>(0x12345678), x64_regs::rbp);
//	s << x64_mov(x64_addr_ptr<uint32_t>(0x12345678), x64_regs::rsp);
//	s << x64_mov(x64_addr_ptr<uint32_t>(0x12345678), x64_regs::r12);
//	s << x64_mov(x64_addr_ptr<uint32_t>(0x12345678), x64_regs::r13);
//
//	s << x64_mov(x64_addr_ptr<uint32_t>(0x12345678), x64_regs::eax);
//	s << x64_mov(x64_addr_ptr<uint32_t>(0x12345678), x64_regs::ax);
//	s << x64_mov(x64_addr_ptr<uint32_t>(0x12345678), x64_regs::ah);
//	s << x64_mov(x64_addr_ptr<uint32_t>(0x12345678), x64_regs::al);
//
//	s << x64_nop1();
//
//	s << x64_mov(x64_regs::rax, x64_addr_ptr<uint64_t*>((uint64_t*)0x1234567812345678));
//	s << x64_mov(x64_regs::eax, x64_addr_ptr<uint32_t*>((uint32_t*)0x1234567812345678));
//	s << x64_mov(x64_regs::ax, x64_addr_ptr<uint16_t*>((uint16_t*)0x1234567812345678));
//	s << x64_mov(x64_regs::al, x64_addr_ptr<uint8_t*>((uint8_t*)0x1234567812345678));
////	s << x64_mov(x64_regs::ah, x64_addr_ptr<uint8_t*>((uint8_t*)0x1234567812345678));
//
//	s << x64_nop1();
//
//	s << x64_mov(x64_addr_ptr<uint64_t*>((uint64_t*)0x1234567812345678), x64_regs::rax);
//	s << x64_mov(x64_addr_ptr<uint32_t*>((uint32_t*)0x1234567812345678), x64_regs::eax);
//	s << x64_mov(x64_addr_ptr<uint16_t*>((uint16_t*)0x1234567812345678), x64_regs::ax);
//	s << x64_mov(x64_addr_ptr<uint8_t*>((uint8_t*)0x1234567812345678), x64_regs::al);

//	s << x64_mov(x64_regs::rax, x64_addr_ptr<uint64_t*>((uint64_t*)0x42));
//	s << x64_mov(x64_addr_ptr<uint64_t*>((uint64_t*)0x42), x64_regs::rax);
//
//	s << x64_mov(x64_addr_ptr<uint32_t*>((uint32_t*)0x42), x64_regs::eax);
//	//s << x64_mov(x64_addr_ptr<uint32_t*>((uint32_t*)0x42), x64_regs::edi);
//
//	s << x64_nop1();
//
//	s << x64_mov(x64_regs::al, x64_addr_ptr<int32_t>(0x12345678));


	s << x64_mov(x64_regs::ax, x64_reg_ptr64(x64_regs::rax), x64_regs::rax, sib_scale::s8);

//	s << x64_mov(x64_reg_ptr64(x64_regs::rax), x64_regs::rax, sib_scale::s8, x64_regs::eax);
//
//	s << x64_mov(x64_regs::rax, x64_reg_ptr64(x64_regs::rax), x64_regs::rbx, sib_scale::s8, (int8_t) 0x42);
//	s << x64_mov(x64_regs::rax, x64_reg_ptr64(x64_regs::rax), x64_regs::rbx, sib_scale::s8, (int32_t) 0x42);
//
//	s << x64_mov(x64_reg_ptr64(x64_regs::rax), x64_regs::rbx, sib_scale::s8, (int8_t) 0x42, x64_regs::rax);
//	s << x64_mov(x64_reg_ptr64(x64_regs::rax), x64_regs::rbx, sib_scale::s8, (int32_t) 0x42, x64_regs::rax);

	//s << x64_mov(x64_reg64 reg, x64_reg_ptr64 addr, x64_reg64 index, sib_scale scale, uint8_t off);

//	s << x64_nop1();
//
//	s << x64_mov(x64_regs::rax, x64_reg_ptr64(x64_regs::rax), x64_regs::rax, sib_scale::s8);
//	s << x64_mov(x64_regs::eax, x64_reg_ptr64(x64_regs::rax), x64_regs::rax, sib_scale::s8);
//	s << x64_mov(x64_regs::ax, x64_reg_ptr64(x64_regs::rax), x64_regs::rax, sib_scale::s8);
//	s << x64_mov(x64_regs::al, x64_reg_ptr64(x64_regs::rax), x64_regs::rax, sib_scale::s8);
//
//	s << x64_mov(x64_regs::rax, x64_reg_ptr32(x64_regs::eax), x64_regs::eax, sib_scale::s8);
//	s << x64_mov(x64_regs::eax, x64_reg_ptr32(x64_regs::eax), x64_regs::eax, sib_scale::s8);
//	s << x64_mov(x64_regs::ax, x64_reg_ptr32(x64_regs::eax), x64_regs::eax, sib_scale::s8);
//	s << x64_mov(x64_regs::al, x64_reg_ptr32(x64_regs::eax), x64_regs::eax, sib_scale::s8);


//	auto blabla = x64_mov(x64_reg_addr(x64_regs::eax), x64_regs::rax);
//
//	blabla.x64_add_prefix(x64_regs::rax, x64_reg_addr(x64_regs::eax));

	cout << x64_disassembler::disassemble(s, "intel", true);

	exit(0);
#endif

	vector<string> expected_lines;

	direct_reg_reg<x64_reg64>(s, expected_lines, 16);
	direct_reg_reg<x64_reg32>(s, expected_lines, 8);
	direct_reg_reg<x64_reg16>(s, expected_lines, 8);
	direct_reg_reg<x64_reg8>(s, expected_lines, 8);

	regptr_reg<x64_reg64, x64_reg64, x64_reg_ptr64>(s, expected_lines, "QWORD", 16, 16);
	regptr_reg<x64_reg64, x64_reg32, x64_reg_ptr64>(s, expected_lines, "DWORD", 16, 8);
	regptr_reg<x64_reg64, x64_reg16, x64_reg_ptr64>(s, expected_lines, "WORD", 16, 8);
	regptr_reg<x64_reg64, x64_reg8l, x64_reg_ptr64>(s, expected_lines, "BYTE", 16, 4);

	regptr_reg<x64_reg32, x64_reg64, x64_reg_ptr32>(s, expected_lines, "QWORD", 8, 16);
	regptr_reg<x64_reg32, x64_reg32, x64_reg_ptr32>(s, expected_lines, "DWORD", 8, 8);
	regptr_reg<x64_reg32, x64_reg16, x64_reg_ptr32>(s, expected_lines, "WORD", 8, 8);
	regptr_reg<x64_reg32, x64_reg8l, x64_reg_ptr32>(s, expected_lines, "BYTE", 8, 4);

	reg_regptr<x64_reg64, x64_reg64, x64_reg_ptr64>(s, expected_lines, "QWORD", 16, 16);
	reg_regptr<x64_reg32, x64_reg64, x64_reg_ptr64>(s, expected_lines, "DWORD", 8, 16);
	reg_regptr<x64_reg16, x64_reg64, x64_reg_ptr64>(s, expected_lines, "WORD", 8, 16);
	reg_regptr<x64_reg8l, x64_reg64, x64_reg_ptr64>(s, expected_lines, "BYTE", 4, 16);

	reg_regptr<x64_reg64, x64_reg32, x64_reg_ptr32>(s, expected_lines, "QWORD", 16, 8);
	reg_regptr<x64_reg32, x64_reg32, x64_reg_ptr32>(s, expected_lines, "DWORD", 8, 8);
	reg_regptr<x64_reg16, x64_reg32, x64_reg_ptr32>(s, expected_lines, "WORD", 8, 8);
	reg_regptr<x64_reg8l, x64_reg32, x64_reg_ptr32>(s, expected_lines, "BYTE", 4, 8);


	regptr_offs_reg<x64_reg64, x64_reg64, x64_reg_ptr64, int8_t>(s, expected_lines, "QWORD", 16, 16);
	regptr_offs_reg<x64_reg64, x64_reg32, x64_reg_ptr64, int8_t>(s, expected_lines, "DWORD", 16, 8);
	regptr_offs_reg<x64_reg64, x64_reg16, x64_reg_ptr64, int8_t>(s, expected_lines, "WORD", 16, 8);
	regptr_offs_reg<x64_reg64, x64_reg8l, x64_reg_ptr64, int8_t>(s, expected_lines, "BYTE", 16, 4);

	regptr_offs_reg<x64_reg32, x64_reg64, x64_reg_ptr32, int8_t>(s, expected_lines, "QWORD", 8, 16);
	regptr_offs_reg<x64_reg32, x64_reg32, x64_reg_ptr32, int8_t>(s, expected_lines, "DWORD", 8, 8);
	regptr_offs_reg<x64_reg32, x64_reg16, x64_reg_ptr32, int8_t>(s, expected_lines, "WORD", 8, 8);
	regptr_offs_reg<x64_reg32, x64_reg8l, x64_reg_ptr32, int8_t>(s, expected_lines, "BYTE", 8, 4);


	reg_regptr_offs<x64_reg64, x64_reg64, x64_reg_ptr64, int8_t>(s, expected_lines, "QWORD", 16, 16);
	reg_regptr_offs<x64_reg32, x64_reg64, x64_reg_ptr64, int8_t>(s, expected_lines, "DWORD", 8, 16);
	reg_regptr_offs<x64_reg16, x64_reg64, x64_reg_ptr64, int8_t>(s, expected_lines, "WORD", 8, 16);
	reg_regptr_offs<x64_reg8l, x64_reg64, x64_reg_ptr64, int8_t>(s, expected_lines, "BYTE", 4, 16);

	reg_regptr_offs<x64_reg64, x64_reg32, x64_reg_ptr32, int8_t>(s, expected_lines, "QWORD", 16, 8);
	reg_regptr_offs<x64_reg32, x64_reg32, x64_reg_ptr32, int8_t>(s, expected_lines, "DWORD", 8, 8);
	reg_regptr_offs<x64_reg16, x64_reg32, x64_reg_ptr32, int8_t>(s, expected_lines, "WORD", 8, 8);
	reg_regptr_offs<x64_reg8l, x64_reg32, x64_reg_ptr32, int8_t>(s, expected_lines, "BYTE", 4, 8);

	regptr_offs_reg<x64_reg64, x64_reg64, x64_reg_ptr64, int32_t>(s, expected_lines, "QWORD", 16, 16);
	regptr_offs_reg<x64_reg64, x64_reg32, x64_reg_ptr64, int32_t>(s, expected_lines, "DWORD", 16, 8);
	regptr_offs_reg<x64_reg64, x64_reg16, x64_reg_ptr64, int32_t>(s, expected_lines, "WORD", 16, 8);
	regptr_offs_reg<x64_reg64, x64_reg8l, x64_reg_ptr64, int32_t>(s, expected_lines, "BYTE", 16, 4);

	regptr_offs_reg<x64_reg32, x64_reg64, x64_reg_ptr32, int32_t>(s, expected_lines, "QWORD", 8, 16);
	regptr_offs_reg<x64_reg32, x64_reg32, x64_reg_ptr32, int32_t>(s, expected_lines, "DWORD", 8, 8);
	regptr_offs_reg<x64_reg32, x64_reg16, x64_reg_ptr32, int32_t>(s, expected_lines, "WORD", 8, 8);
	regptr_offs_reg<x64_reg32, x64_reg8l, x64_reg_ptr32, int32_t>(s, expected_lines, "BYTE", 8, 4);


	reg_regptr_offs<x64_reg64, x64_reg64, x64_reg_ptr64, int32_t>(s, expected_lines, "QWORD", 16, 16);
	reg_regptr_offs<x64_reg32, x64_reg64, x64_reg_ptr64, int32_t>(s, expected_lines, "DWORD", 8, 16);
	reg_regptr_offs<x64_reg16, x64_reg64, x64_reg_ptr64, int32_t>(s, expected_lines, "WORD", 8, 16);
	reg_regptr_offs<x64_reg8l, x64_reg64, x64_reg_ptr64, int32_t>(s, expected_lines, "BYTE", 4, 16);

	reg_regptr_offs<x64_reg64, x64_reg32, x64_reg_ptr32, int32_t>(s, expected_lines, "QWORD", 16, 8);
	reg_regptr_offs<x64_reg32, x64_reg32, x64_reg_ptr32, int32_t>(s, expected_lines, "DWORD", 8, 8);
	reg_regptr_offs<x64_reg16, x64_reg32, x64_reg_ptr32, int32_t>(s, expected_lines, "WORD", 8, 8);
	reg_regptr_offs<x64_reg8l, x64_reg32, x64_reg_ptr32, int32_t>(s, expected_lines, "BYTE", 4, 8);

	reg_imm<x64_reg64, uint64_t>(s, expected_lines, "movabs", 16);
	reg_imm<x64_reg32, uint32_t>(s, expected_lines, "mov", 8);
	reg_imm<x64_reg16, uint16_t>(s, expected_lines, "mov", 8);
	reg_imm<x64_reg8, uint8_t>(s, expected_lines, "mov", 8);

	reg_imm_addr<x64_reg64, int32_t>(s, expected_lines, "QWORD", 16);
	reg_imm_addr<x64_reg32, int32_t>(s, expected_lines, "DWORD", 8);
	reg_imm_addr<x64_reg16, int32_t>(s, expected_lines, "WORD", 8);
	reg_imm_addr<x64_reg8, int32_t>(s, expected_lines, "BYTE", 8);

	imm_addr_reg<int32_t, x64_reg64>(s, expected_lines, "QWORD", 16);
	imm_addr_reg<int32_t, x64_reg32>(s, expected_lines, "DWORD", 8);
	imm_addr_reg<int32_t, x64_reg16>(s, expected_lines, "WORD", 8);
	imm_addr_reg<int32_t, x64_reg8>(s, expected_lines, "BYTE", 8);

	/* 64 bit pointers only support the first register (al, ax, eax, rax) */
	reg_ptr64<x64_reg64_0, uint64_t>(s, expected_lines, x64_regs::rax);
	reg_ptr64<x64_reg32_0, uint32_t>(s, expected_lines, x64_regs::eax);
	reg_ptr64<x64_reg16_0, uint16_t>(s, expected_lines, x64_regs::ax);
	reg_ptr64<x64_reg8l_0, uint8_t>(s, expected_lines, x64_regs::al);

	ptr64_reg<uint64_t, x64_reg64_0>(s, expected_lines, x64_regs::rax);
	ptr64_reg<uint32_t, x64_reg32_0>(s, expected_lines, x64_regs::eax);
	ptr64_reg<uint16_t, x64_reg16_0>(s, expected_lines, x64_regs::ax);
	ptr64_reg<uint8_t, x64_reg8l_0>(s, expected_lines, x64_regs::al);


	reg_reg_ptr_idx<x64_reg64, x64_reg_ptr64, x64_reg64>(s, expected_lines, "QWORD", 16, 16);
	reg_reg_ptr_idx<x64_reg32, x64_reg_ptr64, x64_reg64>(s, expected_lines, "DWORD", 8, 16);
	reg_reg_ptr_idx<x64_reg16, x64_reg_ptr64, x64_reg64>(s, expected_lines, "WORD", 8, 16);
	reg_reg_ptr_idx<x64_reg8l, x64_reg_ptr64, x64_reg64>(s, expected_lines, "BYTE", 4, 16);

	reg_reg_ptr_idx<x64_reg64, x64_reg_ptr32, x64_reg32>(s, expected_lines, "QWORD", 16, 8);
	reg_reg_ptr_idx<x64_reg32, x64_reg_ptr32, x64_reg32>(s, expected_lines, "DWORD", 8, 8);
	reg_reg_ptr_idx<x64_reg16, x64_reg_ptr32, x64_reg32>(s, expected_lines, "WORD", 8, 8);
	reg_reg_ptr_idx<x64_reg8l, x64_reg_ptr32, x64_reg32>(s, expected_lines, "BYTE", 4, 8);

	reg_ptr_idx_reg<x64_reg_ptr64, x64_reg64, x64_reg64>(s, expected_lines, "QWORD", 16, 16);
	reg_ptr_idx_reg<x64_reg_ptr64, x64_reg64, x64_reg32>(s, expected_lines, "DWORD", 16, 8);
	reg_ptr_idx_reg<x64_reg_ptr64, x64_reg64, x64_reg16>(s, expected_lines, "WORD", 16, 8);
	reg_ptr_idx_reg<x64_reg_ptr64, x64_reg64, x64_reg8l>(s, expected_lines, "BYTE", 16, 4);

	reg_ptr_idx_reg<x64_reg_ptr32, x64_reg32, x64_reg64>(s, expected_lines, "QWORD", 8, 16);
	reg_ptr_idx_reg<x64_reg_ptr32, x64_reg32, x64_reg32>(s, expected_lines, "DWORD", 8, 8);
	reg_ptr_idx_reg<x64_reg_ptr32, x64_reg32, x64_reg16>(s, expected_lines, "WORD", 8, 8);
	reg_ptr_idx_reg<x64_reg_ptr32, x64_reg32, x64_reg8l>(s, expected_lines, "BYTE", 8, 4);


	reg_reg_ptr_idx_off<x64_reg64, x64_reg_ptr64, x64_reg64, int8_t>(s, expected_lines, "QWORD", 16, 16);
	reg_reg_ptr_idx_off<x64_reg32, x64_reg_ptr64, x64_reg64, int8_t>(s, expected_lines, "DWORD", 8, 16);
	reg_reg_ptr_idx_off<x64_reg16, x64_reg_ptr64, x64_reg64, int8_t>(s, expected_lines, "WORD", 8, 16);
	reg_reg_ptr_idx_off<x64_reg8l, x64_reg_ptr64, x64_reg64, int8_t>(s, expected_lines, "BYTE", 4, 16);

	reg_reg_ptr_idx_off<x64_reg64, x64_reg_ptr32, x64_reg32, int8_t>(s, expected_lines, "QWORD", 16, 8);
	reg_reg_ptr_idx_off<x64_reg32, x64_reg_ptr32, x64_reg32, int8_t>(s, expected_lines, "DWORD", 8, 8);
	reg_reg_ptr_idx_off<x64_reg16, x64_reg_ptr32, x64_reg32, int8_t>(s, expected_lines, "WORD", 8, 8);
	reg_reg_ptr_idx_off<x64_reg8l, x64_reg_ptr32, x64_reg32, int8_t>(s, expected_lines, "BYTE", 4, 8);

	reg_ptr_idx_off_reg<x64_reg_ptr64, x64_reg64, int8_t, x64_reg64>(s, expected_lines, "QWORD", 16, 16);
	reg_ptr_idx_off_reg<x64_reg_ptr64, x64_reg64, int8_t, x64_reg32>(s, expected_lines, "DWORD", 16, 8);
	reg_ptr_idx_off_reg<x64_reg_ptr64, x64_reg64, int8_t, x64_reg16>(s, expected_lines, "WORD", 16, 8);
	reg_ptr_idx_off_reg<x64_reg_ptr64, x64_reg64, int8_t, x64_reg8l>(s, expected_lines, "BYTE", 16, 4);

	reg_ptr_idx_off_reg<x64_reg_ptr32, x64_reg32, int8_t, x64_reg64>(s, expected_lines, "QWORD", 8, 16);
	reg_ptr_idx_off_reg<x64_reg_ptr32, x64_reg32, int8_t, x64_reg32>(s, expected_lines, "DWORD", 8, 8);
	reg_ptr_idx_off_reg<x64_reg_ptr32, x64_reg32, int8_t, x64_reg16>(s, expected_lines, "WORD", 8, 8);
	reg_ptr_idx_off_reg<x64_reg_ptr32, x64_reg32, int8_t, x64_reg8l>(s, expected_lines, "BYTE", 8, 4);


	reg_reg_ptr_idx_off<x64_reg64, x64_reg_ptr64, x64_reg64, int32_t>(s, expected_lines, "QWORD", 16, 16);
	reg_reg_ptr_idx_off<x64_reg32, x64_reg_ptr64, x64_reg64, int32_t>(s, expected_lines, "DWORD", 8, 16);
	reg_reg_ptr_idx_off<x64_reg16, x64_reg_ptr64, x64_reg64, int32_t>(s, expected_lines, "WORD", 8, 16);
	reg_reg_ptr_idx_off<x64_reg8l, x64_reg_ptr64, x64_reg64, int32_t>(s, expected_lines, "BYTE", 4, 16);

	reg_reg_ptr_idx_off<x64_reg64, x64_reg_ptr32, x64_reg32, int32_t>(s, expected_lines, "QWORD", 16, 8);
	reg_reg_ptr_idx_off<x64_reg32, x64_reg_ptr32, x64_reg32, int32_t>(s, expected_lines, "DWORD", 8, 8);
	reg_reg_ptr_idx_off<x64_reg16, x64_reg_ptr32, x64_reg32, int32_t>(s, expected_lines, "WORD", 8, 8);
	reg_reg_ptr_idx_off<x64_reg8l, x64_reg_ptr32, x64_reg32, int32_t>(s, expected_lines, "BYTE", 4, 8);

	reg_ptr_idx_off_reg<x64_reg_ptr64, x64_reg64, int32_t, x64_reg64>(s, expected_lines, "QWORD", 16, 16);
	reg_ptr_idx_off_reg<x64_reg_ptr64, x64_reg64, int32_t, x64_reg32>(s, expected_lines, "DWORD", 16, 8);
	reg_ptr_idx_off_reg<x64_reg_ptr64, x64_reg64, int32_t, x64_reg16>(s, expected_lines, "WORD", 16, 8);
	reg_ptr_idx_off_reg<x64_reg_ptr64, x64_reg64, int32_t, x64_reg8l>(s, expected_lines, "BYTE", 16, 4);

	reg_ptr_idx_off_reg<x64_reg_ptr32, x64_reg32, int32_t, x64_reg64>(s, expected_lines, "QWORD", 8, 16);
	reg_ptr_idx_off_reg<x64_reg_ptr32, x64_reg32, int32_t, x64_reg32>(s, expected_lines, "DWORD", 8, 8);
	reg_ptr_idx_off_reg<x64_reg_ptr32, x64_reg32, int32_t, x64_reg16>(s, expected_lines, "WORD", 8, 8);
	reg_ptr_idx_off_reg<x64_reg_ptr32, x64_reg32, int32_t, x64_reg8l>(s, expected_lines, "BYTE", 8, 4);

	compare_assembly(s, expected_lines);

	auto start = high_resolution_clock::now();
	int n_loops = 10000;
	for (int i = 0; i < n_loops ; i++)
	{
		s << x64_mov(x64_regs::rax, (uint64_t) 0x1234567812345678);
		s << x64_mov(x64_regs::rax, (uint32_t) 0x12345678);
		s << x64_mov(x64_regs::eax, (uint32_t) 0x12345678);
		s << x64_mov(x64_regs::ax, (uint16_t) 0x1234);
		s << x64_mov(x64_regs::ah, (uint8_t) 0x12);
		s << x64_mov(x64_regs::al, (uint8_t) 0x12);
		s << x64_nop1();
		s << x64_nop1();
		s << x64_mov(x64_regs::rax, x64_addr_ptr<uint64_t*>((uint64_t*)0x1234567812345678));
		s << x64_mov(x64_regs::eax, x64_addr_ptr<uint32_t*>((uint32_t*)0x1234567812345678));
		s << x64_mov(x64_regs::ax, x64_addr_ptr<uint16_t*>((uint16_t*)0x1234567812345678));
		s << x64_mov(x64_regs::al, x64_addr_ptr<uint8_t*>((uint8_t*)0x1234567812345678));
		s << x64_nop1();
		s << x64_mov(x64_addr_ptr<uint64_t*>((uint64_t*)0x1234567812345678), x64_regs::rax);
		s << x64_mov(x64_addr_ptr<uint32_t*>((uint32_t*)0x1234567812345678), x64_regs::eax);
		s << x64_mov(x64_addr_ptr<uint16_t*>((uint16_t*)0x1234567812345678), x64_regs::ax);
		s << x64_mov(x64_addr_ptr<uint8_t*>((uint8_t*)0x1234567812345678), x64_regs::al);
		s << x64_mov(x64_regs::rax, x64_addr_ptr<uint64_t*>((uint64_t*)0x42));
		s << x64_mov(x64_addr_ptr<uint64_t*>((uint64_t*)0x42), x64_regs::rax);
		s << x64_mov(x64_addr_ptr<uint32_t*>((uint32_t*)0x42), x64_regs::eax);
		s << x64_nop1();
		s << x64_mov(x64_regs::al, x64_addr_ptr<int32_t>(0x12345678));
		s << x64_mov(x64_regs::ax, x64_reg_ptr64(x64_regs::rax), x64_regs::rax, sib_scale::s8);
		s << x64_mov(x64_reg_ptr64(x64_regs::rax), x64_regs::rax, sib_scale::s8, x64_regs::eax);
		s << x64_mov(x64_regs::rax, x64_reg_ptr64(x64_regs::rax), x64_regs::rbx, sib_scale::s8, (int8_t) 0x42);
		s << x64_mov(x64_regs::rax, x64_reg_ptr64(x64_regs::rax), x64_regs::rbx, sib_scale::s8, (int32_t) 0x42);
		s << x64_mov(x64_reg_ptr64(x64_regs::rax), x64_regs::rbx, sib_scale::s8, (int8_t) 0x42, x64_regs::rax);
		s << x64_mov(x64_reg_ptr64(x64_regs::rax), x64_regs::rbx, sib_scale::s8, (int32_t) 0x42, x64_regs::rax);
		s << x64_nop1();
		s << x64_mov(x64_regs::rax, x64_reg_ptr64(x64_regs::rax), x64_regs::rax, sib_scale::s8);
		s << x64_mov(x64_regs::eax, x64_reg_ptr64(x64_regs::rax), x64_regs::rax, sib_scale::s8);
		s << x64_mov(x64_regs::ax, x64_reg_ptr64(x64_regs::rax), x64_regs::rax, sib_scale::s8);
		s << x64_mov(x64_regs::al, x64_reg_ptr64(x64_regs::rax), x64_regs::rax, sib_scale::s8);
		s << x64_mov(x64_regs::rax, x64_reg_ptr32(x64_regs::eax), x64_regs::eax, sib_scale::s8);
		s << x64_mov(x64_regs::eax, x64_reg_ptr32(x64_regs::eax), x64_regs::eax, sib_scale::s8);
		s << x64_mov(x64_regs::ax, x64_reg_ptr32(x64_regs::eax), x64_regs::eax, sib_scale::s8);
	}

	auto n = 36 * n_loops;
	auto stop = high_resolution_clock::now();
	auto duration = duration_cast<microseconds>(stop - start);
	auto seconds = ((double) duration.count() / 1000000.0);

	// -O0: Code creation: 3600000 instructions in 0.39589 seconds (9.09344e+06 instructions/second
	// -O3: Code creation: 3600000 instructions in 0.063178 seconds (5.69819e+07 instructions/second
	cout << "Code creation: " << n << " instructions in " << seconds << " seconds (" << ((double) n / seconds) << " instructions/second\n";
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