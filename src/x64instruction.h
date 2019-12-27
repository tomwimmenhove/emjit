/*
 * x64instruction.h
 *
 *  Created on: Dec 19, 2019
 *      Author: tom
 */

#ifndef X64INSTRUCTION_H_
#define X64INSTRUCTION_H_

#include <cstdint>
#include <array>

#include "instruction.h"

struct x64_reg_base
{
	inline constexpr x64_reg_base(int value) : value(value) { }
	const int value;
};

struct x64_reg64    : public x64_reg_base { using x64_reg_base::x64_reg_base; };
struct x64_reg64a   : public x64_reg64    { using x64_reg64::x64_reg64; };
struct x64_reg64agp : public x64_reg64a   { using x64_reg64a::x64_reg64a; };
struct x64_reg64asp : public x64_reg64a   { using x64_reg64a::x64_reg64a; };
struct x64_reg64b   : public x64_reg64    { using x64_reg64::x64_reg64; };

struct x64_reg32   : public x64_reg_base  { using x64_reg_base::x64_reg_base; };
struct x64_reg32gp : public x64_reg32     { using x64_reg32::x64_reg32; };
struct x64_reg32sp : public x64_reg32     { using x64_reg32::x64_reg32; };

struct x64_reg16   : public x64_reg_base  { using x64_reg_base::x64_reg_base; };
struct x64_reg16gp : public x64_reg16     { using x64_reg16::x64_reg16; };
struct x64_reg16sp : public x64_reg16     { using x64_reg16::x64_reg16; };

struct x64_reg8  : public x64_reg_base    { using x64_reg_base::x64_reg_base; };
struct x64_reg8h : public x64_reg8        { using x64_reg8::x64_reg8; };
struct x64_reg8l : public x64_reg8        { using x64_reg8::x64_reg8; };

struct x64_regs
{
	/* 64 bit registers */
	constexpr static x64_reg64agp rax = 0;
	constexpr static x64_reg64agp rcx = 1;
	constexpr static x64_reg64agp rdx = 2;
	constexpr static x64_reg64agp rbx = 3;
	constexpr static x64_reg64asp rsp = 4;
	constexpr static x64_reg64agp rbp = 5;
	constexpr static x64_reg64agp rsi = 6;
	constexpr static x64_reg64agp rdi = 7;

	/* 64 bit extended registers */
	constexpr static x64_reg64b  r8 = 8;
	constexpr static x64_reg64b  r9 = 9;
	constexpr static x64_reg64b r10 = 10;
	constexpr static x64_reg64b r11 = 11;
	constexpr static x64_reg64b r12 = 12;
	constexpr static x64_reg64b r13 = 13;
	constexpr static x64_reg64b r14 = 14;
	constexpr static x64_reg64b r15 = 15;

	/* 32 bit registers */
	constexpr static x64_reg32gp eax = 0;
	constexpr static x64_reg32gp ecx = 1;
	constexpr static x64_reg32gp edx = 2;
	constexpr static x64_reg32gp ebx = 3;
	constexpr static x64_reg32sp esp = 4;
	constexpr static x64_reg32gp ebp = 5;
	constexpr static x64_reg32gp esi = 6;
	constexpr static x64_reg32gp edi = 7;

	/* 16 bit registers */
	constexpr static x64_reg16gp ax = 0;
	constexpr static x64_reg16gp cx = 1;
	constexpr static x64_reg16gp dx = 2;
	constexpr static x64_reg16gp bx = 3;
	constexpr static x64_reg16sp sp = 4;
	constexpr static x64_reg16gp bp = 5;
	constexpr static x64_reg16gp si = 6;
	constexpr static x64_reg16gp di = 7;

	/* lower 8 bit registers */
	constexpr static x64_reg8l al = 0;
	constexpr static x64_reg8l cl = 1;
	constexpr static x64_reg8l dl = 2;
	constexpr static x64_reg8l bl = 3;

	/* upper 8 bit registers */
	constexpr static x64_reg8h ah = 4;
	constexpr static x64_reg8h ch = 5;
	constexpr static x64_reg8h dh = 6;
	constexpr static x64_reg8h bh = 7;
};

struct x64_modrm
{
	unsigned int rm:3;
	unsigned int reg:3;
	unsigned int mod:2;

//	template<typename RM_TYPE, typename REG_TYPE>
//	x64_modrm(RM_TYPE rm, REG_TYPE reg, unsigned int mod)
//	 : rm(static_cast<unsigned int>(rm) & 7), reg(static_cast<unsigned int>(reg) & 7), mod(mod)
//	{ }

	x64_modrm(x64_reg_base rm, x64_reg_base reg, unsigned int mod)
	 //: rm(rm & 7), reg(reg & 7), mod(mod)
	 : rm(static_cast<unsigned int>(rm.value) & 7), reg(static_cast<unsigned int>(reg.value) & 7), mod(mod)
	{ }

} __attribute__((packed));

enum class sib_scale
{
	s1 = 0,
	s2,
	s4,
	s8
};

struct x64_sib
{
	unsigned int base:3;
	unsigned int index:3;
	unsigned int scale:2;

	x64_sib() : base(0), index(0), scale(0) { }

	x64_sib(x64_reg64 base, x64_reg64 index, sib_scale scale)
	 : base(base.value), index(index.value), scale(static_cast<unsigned int>(scale))
	{ }

	x64_sib(uint8_t b)
	{
		x64_sib* sib = reinterpret_cast<x64_sib*>(&b);

		base = sib->base;
		index = sib->index;
		scale = sib->scale;
	}

} __attribute__((packed));

enum x64_rex : uint8_t
{
	b = 0x41,
	x = 0x42,
	r = 0x44,
	w = 0x48,
	none = 0x40,
};

enum x64_override : uint8_t
{
	oper_size = 0x66,
	addr_size = 0x67,
};

template<typename T>
struct x64_addr_ptr
{
	constexpr x64_addr_ptr(const T& ptr) : ptr(ptr) { }
	const T ptr;
};

struct x64_reg_ptr32    : public x64_addr_ptr<x64_reg32> { using x64_addr_ptr::x64_addr_ptr; };
struct x64_reg_ptr32gp  : public x64_reg_ptr32 { using x64_reg_ptr32::x64_reg_ptr32; };
struct x64_reg_ptr32sp  : public x64_reg_ptr32 { using x64_reg_ptr32::x64_reg_ptr32; };

struct x64_reg_ptr64    : public x64_addr_ptr<x64_reg64> { using x64_addr_ptr::x64_addr_ptr; };
struct x64_reg_ptr64a   : public x64_reg_ptr64 { using x64_reg_ptr64::x64_reg_ptr64; };
struct x64_reg_ptr64agp : public x64_reg_ptr64a { using x64_reg_ptr64a::x64_reg_ptr64a; };
struct x64_reg_ptr64asp : public x64_reg_ptr64a { using x64_reg_ptr64a::x64_reg_ptr64a; };
struct x64_reg_ptr64b   : public x64_reg_ptr64 { using x64_reg_ptr64::x64_reg_ptr64; };

static constexpr x64_reg_ptr32gp  x64_reg_addr(const x64_reg32gp&  reg) { return x64_reg_ptr32gp(reg); }
static constexpr x64_reg_ptr32sp  x64_reg_addr(const x64_reg32sp&  reg) { return x64_reg_ptr32sp(reg); }
static constexpr x64_reg_ptr64agp x64_reg_addr(const x64_reg64agp& reg) { return x64_reg_ptr64agp(reg); }
static constexpr x64_reg_ptr64asp x64_reg_addr(const x64_reg64asp& reg) { return x64_reg_ptr64asp(reg); }
static constexpr x64_reg_ptr64b   x64_reg_addr(const x64_reg64b& reg)   { return x64_reg_ptr64b(reg); }

class x64_instruction : public instruction
{
public:
	template<std::size_t OPCODE_SIZE, typename IMM_TYPE>
	x64_instruction(
			const std::array<uint8_t, OPCODE_SIZE>& opcode,
			const struct x64_modrm modrm,
			const struct x64_sib sib,
			IMM_TYPE imm,
			uint8_t* data = nullptr)
	 : x64_instruction(opcode, &modrm, &sib, &imm, data)
	{ }

	template<std::size_t OPCODE_SIZE>
	x64_instruction(
			const std::array<uint8_t, OPCODE_SIZE>& opcode,
			const struct x64_modrm modrm,
			const struct x64_sib sib,
			uint8_t* data = nullptr)
	 : x64_instruction(opcode, &modrm, &sib, (uint32_t*) nullptr, data)
	{ }

	template<std::size_t OPCODE_SIZE, typename IMM_TYPE>
	x64_instruction(
			const std::array<uint8_t, OPCODE_SIZE>& opcode,
			const struct x64_modrm modrm,
			IMM_TYPE imm,
			uint8_t* data = nullptr)
	 : x64_instruction(opcode, &modrm, nullptr, &imm, data)
	{ }

	template<std::size_t OPCODE_SIZE>
	x64_instruction(
			const std::array<uint8_t, OPCODE_SIZE>& opcode,
			const struct x64_modrm modrm,
			uint8_t* data = nullptr)
	 : x64_instruction(opcode, &modrm, nullptr, (uint32_t*) nullptr, data)
	{ }

	template<std::size_t OPCODE_SIZE, typename IMM_TYPE>
	x64_instruction(
			const std::array<uint8_t, OPCODE_SIZE>& opcode,
			const struct x64_sib sib,
			IMM_TYPE imm,
			uint8_t* data = nullptr)
	 : x64_instruction(opcode, nullptr, &sib, &imm, data)
	{ }

	template<std::size_t OPCODE_SIZE>
	x64_instruction(
			const std::array<uint8_t, OPCODE_SIZE>& opcode,
			const struct x64_sib sib,
			uint8_t* data = nullptr)
	 : x64_instruction(opcode, nullptr, &sib, (uint32_t*) nullptr, data)
	{ }

	template<std::size_t OPCODE_SIZE, typename IMM_TYPE>
	x64_instruction(
			const std::array<uint8_t, OPCODE_SIZE>& opcode,
			IMM_TYPE imm,
			uint8_t* data = nullptr)
	 : x64_instruction(opcode, nullptr, nullptr, &imm, data)
	{ }

	template<std::size_t OPCODE_SIZE>
	x64_instruction(
			const std::array<uint8_t, OPCODE_SIZE>& opcode,
			uint8_t* data = nullptr)
	 : x64_instruction( opcode, nullptr, nullptr, (uint32_t*) nullptr, data)
	{ }

	inline std::size_t size() const override { return data_size; }
	inline uint8_t* data() const override { return data_ptr; }

	inline struct x64_modrm &modrm() { return *modrm_ptr; }
	inline struct x64_sib &sib() { return *sib_ptr; }

	/* This gets a reference to either the immediate or the displacement value */
	template<typename IMM_TYPE>
	inline IMM_TYPE& immediate() { return *(IMM_TYPE*)imm_ptr; }

	virtual ~x64_instruction() { }

protected:
	inline uint8_t extend_reg_prefix(const x64_reg64& dst, const x64_reg64& src)
	{
		return static_cast<uint8_t>(
				(dst.value > x64_regs::rdi.value ? x64_rex::w | x64_rex::b : x64_rex::w) |
				(src.value > x64_regs::rdi.value ? x64_rex::w | x64_rex::r : x64_rex::w));
	}

private:
	template<std::size_t OPCODE_SIZE, typename IMM_TYPE>
	x64_instruction(
			const std::array<uint8_t, OPCODE_SIZE>& opcode,
			const struct x64_modrm* modrm_p,
			const struct x64_sib* sib_p,
			IMM_TYPE* imm_p,
			uint8_t* data_p = nullptr)
	{
		data_size = opcode.size() +
				(modrm_p ? 1 : 0) +
				(sib_p ? 1 : 0) +
				(imm_p ? sizeof(IMM_TYPE) : 0);

		if (data_size > 15)
			throw std::overflow_error("Instruction was too long");

		data_ptr = data_p ? data_p : data_alloc;

		/* Get a pointer to the instruction data */
		auto ptr = this->data_ptr;

		/* Add the opcode */
		for(const auto& opcode_byte : opcode)
			*ptr++ = opcode_byte;

		/* Add ModR/M */
		if (modrm_p)
		{
			modrm_ptr = (struct x64_modrm*) ptr;
			*modrm_ptr = *modrm_p;
			ptr++;
		}

		/* Add ModR/M */
		if (sib_p)
		{
			sib_ptr = (struct x64_sib*) ptr;
			*sib_ptr = *sib_p;
			ptr++;
		}

		/* Add immediate/displacement */
		if (imm_p)
		{
			imm_ptr = ptr;
			*(IMM_TYPE*)imm_ptr = *imm_p;
		}
	}

	uint8_t data_alloc[15]; // 15 is the maximum instruction size on x64

	struct x64_modrm* modrm_ptr = nullptr;
	struct x64_sib* sib_ptr = nullptr;
	void* imm_ptr = nullptr;
	uint8_t* data_ptr;
	std::size_t data_size;
};

struct x64_nop1	: public x64_instruction{x64_nop1()	: x64_instruction(std::array<uint8_t, 1> { 0x90 }) {} };
struct x64_ret	: public x64_instruction{x64_ret()	: x64_instruction(std::array<uint8_t, 1> { 0xc3 }) {} };
struct x64_lret	: public x64_instruction{x64_lret()	: x64_instruction(std::array<uint8_t, 1> { 0xcb }) {} };
struct x64_ud2	: public x64_instruction{x64_ud2()	: x64_instruction(std::array<uint8_t, 2> { 0x0f, 0x0b }) {} };

class x64_mov : public x64_instruction
{
public:
	/* Move register into register */
	x64_mov(x64_reg64 dst, x64_reg64 src) : x64_instruction(std::array<uint8_t, 2> { extend_reg_prefix(dst, src), 0x89 }, x64_modrm{dst, src, 3}) {}
	x64_mov(x64_reg32 dst, x64_reg32 src) : x64_instruction(std::array<uint8_t, 1> { 0x89 }, x64_modrm{dst, src, 3}) {}
	x64_mov(x64_reg16 dst, x64_reg16 src) : x64_instruction(std::array<uint8_t, 2> { x64_override::oper_size, 0x89 }, x64_modrm{dst, src, 3}) {}
	x64_mov(x64_reg8  dst, x64_reg8  src) : x64_instruction(std::array<uint8_t, 1> { 0x88 }, x64_modrm{dst, src, 3}) {}

	/* Move register into register pointer */
	x64_mov(x64_reg_ptr64 addr, x64_reg64 reg) : x64_instruction(std::array<uint8_t, 2> { extend_reg_prefix(addr.ptr, reg), 0x89 }, x64_modrm{addr.ptr, reg, 0}) { }
	x64_mov(x64_reg_ptr64a addr, x64_reg32 reg) : x64_instruction(std::array<uint8_t, 1> { 0x89 }, x64_modrm{addr.ptr, reg, 0}) { }
	x64_mov(x64_reg_ptr64b addr, x64_reg32 reg) : x64_instruction(std::array<uint8_t, 2> { x64_rex::b, 0x89 }, x64_modrm{addr.ptr, reg, 0}) { }
	x64_mov(x64_reg_ptr64a addr, x64_reg16 reg) : x64_instruction(std::array<uint8_t, 2> { x64_override::oper_size, 0x89 }, x64_modrm{addr.ptr, reg, 0}) { }
	x64_mov(x64_reg_ptr64b addr, x64_reg16 reg) : x64_instruction(std::array<uint8_t, 3> { x64_override::oper_size, x64_rex::b, 0x89 }, x64_modrm{addr.ptr, reg, 0}) { }
	x64_mov(x64_reg_ptr64a addr, x64_reg8 reg) : x64_instruction(std::array<uint8_t, 1> { 0x88 }, x64_modrm{addr.ptr, reg, 0}) { }
	x64_mov(x64_reg_ptr64b addr, x64_reg8l reg) : x64_instruction(std::array<uint8_t, 2> { x64_rex::b, 0x88 }, x64_modrm{addr.ptr, reg, 0}) { }

	x64_mov(x64_reg_ptr32 addr, x64_reg64 reg) : x64_instruction(std::array<uint8_t, 3> { x64_override::addr_size, extend_reg_prefix(0, reg), 0x89 }, x64_modrm{addr.ptr, reg, 0}) { }
	x64_mov(x64_reg_ptr32 addr, x64_reg32 reg) : x64_instruction(std::array<uint8_t, 2> { x64_override::addr_size, 0x89 }, x64_modrm{addr.ptr, reg, 0}) { }
	x64_mov(x64_reg_ptr32 addr, x64_reg16 reg) : x64_instruction(std::array<uint8_t, 3> { x64_override::addr_size, x64_override::oper_size, 0x89 }, x64_modrm{addr.ptr, reg, 0}) { }
	x64_mov(x64_reg_ptr32 addr, x64_reg8 reg) : x64_instruction(std::array<uint8_t, 2> { x64_override::addr_size, 0x88 }, x64_modrm{addr.ptr, reg, 0}) { }

	/* Move register pointer into register */
	x64_mov(x64_reg64 reg, x64_reg_ptr64 addr) : x64_instruction(std::array<uint8_t, 2> { extend_reg_prefix(addr.ptr, reg), 0x8b }, x64_modrm{addr.ptr, reg, 0}) { }
	x64_mov(x64_reg32 reg, x64_reg_ptr64a addr) : x64_instruction(std::array<uint8_t, 1> { 0x8b }, x64_modrm{addr.ptr, reg, 0}) { }
	x64_mov(x64_reg32 reg, x64_reg_ptr64b addr) : x64_instruction(std::array<uint8_t, 2> { x64_rex::b, 0x8b }, x64_modrm{addr.ptr, reg, 0}) { }
	x64_mov(x64_reg16 reg, x64_reg_ptr64a addr) : x64_instruction(std::array<uint8_t, 2> { x64_override::oper_size, 0x8b }, x64_modrm{addr.ptr, reg, 0}) { }
	x64_mov(x64_reg16 reg, x64_reg_ptr64b addr) : x64_instruction(std::array<uint8_t, 3> { x64_override::oper_size, x64_rex::b, 0x8b }, x64_modrm{addr.ptr, reg, 0}) { }
	x64_mov(x64_reg8 reg, x64_reg_ptr64a addr) : x64_instruction(std::array<uint8_t, 1> { 0x8a }, x64_modrm{addr.ptr, reg, 0}) { }
	x64_mov(x64_reg8l reg, x64_reg_ptr64b addr) : x64_instruction(std::array<uint8_t, 2> { x64_rex::b, 0x8a }, x64_modrm{addr.ptr, reg, 0}) { }

	x64_mov(x64_reg64 reg, x64_reg_ptr32 addr) : x64_instruction(std::array<uint8_t, 3> { x64_override::addr_size, extend_reg_prefix(0, reg), 0x8b }, x64_modrm{addr.ptr, reg, 0}) { }
	x64_mov(x64_reg32 reg, x64_reg_ptr32 addr) : x64_instruction(std::array<uint8_t, 2> { x64_override::addr_size, 0x8b }, x64_modrm{addr.ptr, reg, 0}) { }
	x64_mov(x64_reg16 reg, x64_reg_ptr32 addr) : x64_instruction(std::array<uint8_t, 3> { x64_override::addr_size, x64_override::oper_size, 0x8b }, x64_modrm{addr.ptr, reg, 0}) { }
	x64_mov(x64_reg8 reg, x64_reg_ptr32 addr) : x64_instruction(std::array<uint8_t, 2> { x64_override::addr_size, 0x8a }, x64_modrm{addr.ptr, reg, 0}) { }

	/* Move register into register pointer + 8 bit offset */
	x64_mov(x64_reg_ptr64 addr, x64_reg64 reg, int8_t off) : x64_instruction(std::array<uint8_t, 2> { extend_reg_prefix(addr.ptr, reg), 0x89 }, x64_modrm{addr.ptr, reg, 1}, off) { }
	x64_mov(x64_reg_ptr64a addr, x64_reg32 reg, int8_t off) : x64_instruction(std::array<uint8_t, 1> { 0x89 }, x64_modrm{addr.ptr, reg, 1}, off) { }
	x64_mov(x64_reg_ptr64b addr, x64_reg32 reg, int8_t off) : x64_instruction(std::array<uint8_t, 2> { x64_rex::b, 0x89 }, x64_modrm{addr.ptr, reg, 1}, off) { }
	x64_mov(x64_reg_ptr64a addr, x64_reg16 reg, int8_t off) : x64_instruction(std::array<uint8_t, 2> { x64_override::oper_size, 0x89 }, x64_modrm{addr.ptr, reg, 1}, off) { }
	x64_mov(x64_reg_ptr64b addr, x64_reg16 reg, int8_t off) : x64_instruction(std::array<uint8_t, 3> { x64_override::oper_size, x64_rex::b, 0x89 }, x64_modrm{addr.ptr, reg, 1}, off) { }
	x64_mov(x64_reg_ptr64a addr, x64_reg8 reg, int8_t off) : x64_instruction(std::array<uint8_t, 1> { 0x88 }, x64_modrm{addr.ptr, reg, 1}, off) { }
	x64_mov(x64_reg_ptr64b addr, x64_reg8l reg, int8_t off) : x64_instruction(std::array<uint8_t, 2> { x64_rex::b, 0x88 }, x64_modrm{addr.ptr, reg, 1}, off) { }

	x64_mov(x64_reg_ptr32 addr, x64_reg64 reg, int8_t off) : x64_instruction(std::array<uint8_t, 3> { x64_override::addr_size, extend_reg_prefix(0, reg), 0x89 }, x64_modrm{addr.ptr, reg, 1}, off) { }
	x64_mov(x64_reg_ptr32 addr, x64_reg32 reg, int8_t off) : x64_instruction(std::array<uint8_t, 2> { x64_override::addr_size, 0x89 }, x64_modrm{addr.ptr, reg, 1}, off) { }
	x64_mov(x64_reg_ptr32 addr, x64_reg16 reg, int8_t off) : x64_instruction(std::array<uint8_t, 3> { x64_override::addr_size, x64_override::oper_size, 0x89 }, x64_modrm{addr.ptr, reg, 1}, off) { }
	x64_mov(x64_reg_ptr32 addr, x64_reg8 reg, int8_t off) : x64_instruction(std::array<uint8_t, 2> { x64_override::addr_size, 0x88 }, x64_modrm{addr.ptr, reg, 1}, off) { }

	/* Move register pointer + 8 bit  offset into register */
	x64_mov(x64_reg64 reg, x64_reg_ptr64 addr, int8_t off) : x64_instruction(std::array<uint8_t, 2> { extend_reg_prefix(addr.ptr, reg), 0x8b }, x64_modrm{addr.ptr, reg, 1}, off) { }
	x64_mov(x64_reg32 reg, x64_reg_ptr64agp addr, int8_t off) : x64_instruction(std::array<uint8_t, 1> { 0x8b }, x64_modrm{addr.ptr, reg, 1}, off) { }
	x64_mov(x64_reg32 reg, x64_reg_ptr64b addr, int8_t off) : x64_instruction(std::array<uint8_t, 2> { x64_rex::b, 0x8b }, x64_modrm{addr.ptr, reg, 1}, off) { }
	x64_mov(x64_reg16 reg, x64_reg_ptr64a addr, int8_t off) : x64_instruction(std::array<uint8_t, 2> { x64_override::oper_size, 0x8b }, x64_modrm{addr.ptr, reg, 1}, off) { }
	x64_mov(x64_reg16 reg, x64_reg_ptr64b addr, int8_t off) : x64_instruction(std::array<uint8_t, 3> { x64_override::oper_size, x64_rex::b, 0x8b }, x64_modrm{addr.ptr, reg, 1}, off) { }
	x64_mov(x64_reg8 reg, x64_reg_ptr64a addr, int8_t off) : x64_instruction(std::array<uint8_t, 1> { 0x8a }, x64_modrm{addr.ptr, reg, 1}, off) { }
	x64_mov(x64_reg8l reg, x64_reg_ptr64b addr, int8_t off) : x64_instruction(std::array<uint8_t, 2> { x64_rex::b, 0x8a }, x64_modrm{addr.ptr, reg, 1}, off) { }

	x64_mov(x64_reg64 reg, x64_reg_ptr32 addr, int8_t off) : x64_instruction(std::array<uint8_t, 3> { x64_override::addr_size, extend_reg_prefix(0, reg), 0x8b }, x64_modrm{addr.ptr, reg, 1}, off) { }
	x64_mov(x64_reg32 reg, x64_reg_ptr32 addr, int8_t off) : x64_instruction(std::array<uint8_t, 2> { x64_override::addr_size, 0x8b }, x64_modrm{addr.ptr, reg, 1}, off) { }
	x64_mov(x64_reg16 reg, x64_reg_ptr32 addr, int8_t off) : x64_instruction(std::array<uint8_t, 3> { x64_override::addr_size, x64_override::oper_size, 0x8b }, x64_modrm{addr.ptr, reg, 1}, off) { }
	x64_mov(x64_reg8 reg, x64_reg_ptr32 addr, int8_t off) : x64_instruction(std::array<uint8_t, 2> { x64_override::addr_size, 0x8a }, x64_modrm{addr.ptr, reg, 1}, off) { }


	/* Move register into register pointer + 32 bit offset */
	x64_mov(x64_reg_ptr64 addr, x64_reg64 reg, int32_t off) : x64_instruction(std::array<uint8_t, 2> { extend_reg_prefix(addr.ptr, reg), 0x89 }, x64_modrm{addr.ptr, reg, 2}, off) { }
	x64_mov(x64_reg_ptr64a addr, x64_reg32 reg, int32_t off) : x64_instruction(std::array<uint8_t, 1> { 0x89 }, x64_modrm{addr.ptr, reg, 2}, off) { }
	x64_mov(x64_reg_ptr64b addr, x64_reg32 reg, int32_t off) : x64_instruction(std::array<uint8_t, 2> { x64_rex::b, 0x89 }, x64_modrm{addr.ptr, reg, 2}, off) { }
	x64_mov(x64_reg_ptr64a addr, x64_reg16 reg, int32_t off) : x64_instruction(std::array<uint8_t, 2> { x64_override::oper_size, 0x89 }, x64_modrm{addr.ptr, reg, 2}, off) { }
	x64_mov(x64_reg_ptr64b addr, x64_reg16 reg, int32_t off) : x64_instruction(std::array<uint8_t, 3> { x64_override::oper_size, x64_rex::b, 0x89 }, x64_modrm{addr.ptr, reg, 2}, off) { }
	x64_mov(x64_reg_ptr64a addr, x64_reg8 reg, int32_t off) : x64_instruction(std::array<uint8_t, 1> { 0x88 }, x64_modrm{addr.ptr, reg, 2}, off) { }
	x64_mov(x64_reg_ptr64b addr, x64_reg8l reg, int32_t off) : x64_instruction(std::array<uint8_t, 2> { x64_rex::b, 0x88 }, x64_modrm{addr.ptr, reg, 2}, off) { }

	x64_mov(x64_reg_ptr32 addr, x64_reg64 reg, int32_t off) : x64_instruction(std::array<uint8_t, 3> { x64_override::addr_size, extend_reg_prefix(0, reg), 0x89 }, x64_modrm{addr.ptr, reg, 2}, off) { }
	x64_mov(x64_reg_ptr32 addr, x64_reg32 reg, int32_t off) : x64_instruction(std::array<uint8_t, 2> { x64_override::addr_size, 0x89 }, x64_modrm{addr.ptr, reg, 2}, off) { }
	x64_mov(x64_reg_ptr32 addr, x64_reg16 reg, int32_t off) : x64_instruction(std::array<uint8_t, 3> { x64_override::addr_size, x64_override::oper_size, 0x89 }, x64_modrm{addr.ptr, reg, 2}, off) { }
	x64_mov(x64_reg_ptr32 addr, x64_reg8 reg, int32_t off) : x64_instruction(std::array<uint8_t, 2> { x64_override::addr_size, 0x88 }, x64_modrm{addr.ptr, reg, 2}, off) { }

	/* Move register pointer + 32 bit offset into register */
	x64_mov(x64_reg64 reg, x64_reg_ptr64 addr, int32_t off) : x64_instruction(std::array<uint8_t, 2> { extend_reg_prefix(addr.ptr, reg), 0x8b }, x64_modrm{addr.ptr, reg, 2}, off) { }
	x64_mov(x64_reg64 reg, x64_reg_ptr64asp addr, int32_t off) = delete; // To be implemented
	x64_mov(x64_reg32 reg, x64_reg_ptr64agp addr, int32_t off) : x64_instruction(std::array<uint8_t, 1> { 0x8b }, x64_modrm{addr.ptr, reg, 2}, off) { }
	x64_mov(x64_reg32 reg, x64_reg_ptr64b addr, int32_t off) : x64_instruction(std::array<uint8_t, 2> { x64_rex::b, 0x8b }, x64_modrm{addr.ptr, reg, 2}, off) { }
	x64_mov(x64_reg16 reg, x64_reg_ptr64agp addr, int32_t off) : x64_instruction(std::array<uint8_t, 2> { x64_override::oper_size, 0x8b }, x64_modrm{addr.ptr, reg, 2}, off) { }
	x64_mov(x64_reg16 reg, x64_reg_ptr64b addr, int32_t off) : x64_instruction(std::array<uint8_t, 3> { x64_override::oper_size, x64_rex::b, 0x8b }, x64_modrm{addr.ptr, reg, 2}, off) { }
	x64_mov(x64_reg8 reg, x64_reg_ptr64agp addr, int32_t off) : x64_instruction(std::array<uint8_t, 1> { 0x8a }, x64_modrm{addr.ptr, reg, 2}, off) { }
	x64_mov(x64_reg8l reg, x64_reg_ptr64b addr, int32_t off) : x64_instruction(std::array<uint8_t, 2> { x64_rex::b, 0x8a }, x64_modrm{addr.ptr, reg, 2}, off) { }

	x64_mov(x64_reg64 reg, x64_reg_ptr32gp addr, int32_t off) : x64_instruction(std::array<uint8_t, 3> { x64_override::addr_size, extend_reg_prefix(0, reg), 0x8b }, x64_modrm{addr.ptr, reg, 2}, off) { }
	x64_mov(x64_reg32 reg, x64_reg_ptr32gp addr, int32_t off) : x64_instruction(std::array<uint8_t, 2> { x64_override::addr_size, 0x8b }, x64_modrm{addr.ptr, reg, 2}, off) { }
	x64_mov(x64_reg16 reg, x64_reg_ptr32gp addr, int32_t off) : x64_instruction(std::array<uint8_t, 3> { x64_override::addr_size, x64_override::oper_size, 0x8b }, x64_modrm{addr.ptr, reg, 2}, off) { }
	x64_mov(x64_reg8 reg, x64_reg_ptr32gp addr, int32_t off) : x64_instruction(std::array<uint8_t, 2> { x64_override::addr_size, 0x8a }, x64_modrm{addr.ptr, reg, 2}, off) { }

	// XXX


//	x64_mov(x64_reg32 reg, x64_reg_ptr64a addr, int32_t off) : x64_instruction(std::array<uint8_t, 1> { 0x8b }, x64_modrm{addr.ptr, reg, 2}, off) { }
	//x64_mov(x64_reg32 dst, x64_reg64a base, x64_reg64 idx, sib_scale scale, int8_t off) : x64_instruction(std::array<uint8_t, 1> { 0x8b }, x64_modrm{base, dst, 1}, off) { }

	/* Move immediate into register */
	x64_mov(x64_reg32 reg, uint32_t imm) : x64_instruction(std::array<uint8_t, 1> { static_cast<uint8_t>(0xb8 | reg.value) }, imm) { }
	x64_mov(x64_reg64 reg, uint64_t imm) : x64_instruction(std::array<uint8_t, 2> { extend_reg_prefix(reg, 0), static_cast<uint8_t>(0xb8 | reg.value) }, imm) { }
	x64_mov(x64_reg64 reg, uint32_t imm) : x64_instruction(std::array<uint8_t, 2> { extend_reg_prefix(reg, 0), 0xc7 }, x64_modrm{reg, 0, 3}, imm) { }
	x64_mov(x64_reg16 reg, uint16_t imm) : x64_instruction(std::array<uint8_t, 2> { x64_override::oper_size, static_cast<uint8_t>(0xb8 | reg.value) }, imm) { }
	x64_mov(x64_reg8  reg, uint8_t  imm) : x64_instruction(std::array<uint8_t, 1> { static_cast<uint8_t>(0xb0 | reg.value) }, imm) { }


	/* Move immediate address into register */
	/* 32 bit pointers */
	x64_mov(x64_reg64 reg, x64_addr_ptr<uint32_t> addr) : x64_instruction(std::array<uint8_t, 2> { extend_reg_prefix(0, reg), 0x8b }, x64_modrm(4, reg, 0), x64_sib(0x25), addr.ptr) { }
	x64_mov(x64_reg32 reg, x64_addr_ptr<uint32_t> addr) : x64_instruction(std::array<uint8_t, 1> { 0x8b }, x64_modrm(4, reg, 0), x64_sib(0x25), addr.ptr) { }
	x64_mov(x64_reg16 reg, x64_addr_ptr<uint32_t> addr) : x64_instruction(std::array<uint8_t, 2> { x64_override::oper_size, 0x8b }, x64_modrm(4, reg, 0), x64_sib(0x25), addr.ptr) { }
	x64_mov(x64_reg8  reg, x64_addr_ptr<uint32_t> addr) : x64_instruction(std::array<uint8_t, 1> { 0x8a }, x64_modrm(4, reg, 0), x64_sib(0x25), addr.ptr) { }

	/* 64 bit pointers */
	x64_mov(x64_reg64 reg, x64_addr_ptr<uint64_t*> addr) : x64_mov(reg, addr, x64_rex::w, 0xa1) { };
	x64_mov(x64_reg32 reg, x64_addr_ptr<uint32_t*> addr) : x64_mov(reg, addr, 0xa1) { };
	x64_mov(x64_reg16 reg, x64_addr_ptr<uint16_t*> addr) : x64_mov(reg, addr, x64_override::oper_size, 0xa1) { };
	x64_mov(x64_reg8l reg, x64_addr_ptr<uint8_t*>  addr) : x64_mov(reg, addr, 0xa0) { };


	/* Move register into immediate address */
	/* 32 bit pointers */
	x64_mov(x64_addr_ptr<uint32_t> addr, x64_reg64 reg) : x64_instruction(std::array<uint8_t, 2> { extend_reg_prefix(0, reg), 0x89 }, x64_modrm(4, reg, 0), x64_sib(0x25), addr.ptr) { }
	x64_mov(x64_addr_ptr<uint32_t> addr, x64_reg32 reg) : x64_instruction(std::array<uint8_t, 1> { 0x89 }, x64_modrm(4, reg, 0), x64_sib(0x25), addr.ptr) { }
	x64_mov(x64_addr_ptr<uint32_t> addr, x64_reg16 reg) : x64_instruction(std::array<uint8_t, 2> { x64_override::oper_size, 0x89 }, x64_modrm(4, reg, 0), x64_sib(0x25), addr.ptr) { }
	x64_mov(x64_addr_ptr<uint32_t> addr, x64_reg8  reg) : x64_instruction(std::array<uint8_t, 1> { 0x88 }, x64_modrm(4, reg, 0), x64_sib(0x25), addr.ptr) { }

	/* 64 bit pointers */
	x64_mov(x64_addr_ptr<uint64_t*> addr, x64_reg64 reg) : x64_mov(reg, addr, x64_rex::w, 0xa3) { };
	x64_mov(x64_addr_ptr<uint32_t*> addr, x64_reg32 reg) : x64_mov(reg, addr, 0xa3) { };
	x64_mov(x64_addr_ptr<uint16_t*> addr, x64_reg16 reg) : x64_mov(reg, addr, x64_override::oper_size, 0xa3) { };
	x64_mov(x64_addr_ptr<uint8_t*>  addr, x64_reg8l reg) : x64_mov(reg, addr, 0xa2) { };

private:
	/* Register <=> immediate address */
	/* 64 bit pointers */
	template<typename REG_TYPE, typename IMM_TYPE>
	x64_mov(REG_TYPE reg, x64_addr_ptr<IMM_TYPE*> addr, uint8_t prefix, uint8_t oc)
	 : x64_instruction(
				std::array<uint8_t, 2> { prefix, oc },
				reinterpret_cast<uint64_t>(addr.ptr))
	{
		if (static_cast<int>(reg.value) != 0)
			throw std::out_of_range("Register not supported");
	}

	template<typename REG_TYPE, typename IMM_TYPE>
	x64_mov(REG_TYPE reg, x64_addr_ptr<IMM_TYPE*> addr, uint8_t oc)
	 : x64_instruction(
				std::array<uint8_t, 1> { oc },
				reinterpret_cast<uint64_t>(addr.ptr))
	{
		if (static_cast<int>(reg.value) != 0)
			throw std::out_of_range("Register not supported");
	}
};

#endif /* X64INSTRUCTION_H_ */
