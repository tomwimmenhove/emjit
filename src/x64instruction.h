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

struct x64_modrm
{
	unsigned int rm:3;
	unsigned int reg:3;
	unsigned int mod:2;

	template<typename RM_TYPE, typename REG_TYPE>
	x64_modrm(RM_TYPE rm, REG_TYPE reg, unsigned int mod)
	 : rm(static_cast<unsigned int>(rm)), reg(static_cast<unsigned int>(reg)), mod(mod)
	{ }
} __attribute__((packed));

struct x64_sib
{
	unsigned int base:3;
	unsigned int index:3;
	unsigned int scale:2;

	x64_sib() : base(0), index(0), scale(0) { }

	x64_sib(unsigned int base, unsigned int index, unsigned int scale)
	 : base(base), index(index), scale(scale)
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

enum class x64_reg64
{
	rax,
	rcx,
	rdx,
	rbx,
	rsp,
	rbp,
	rsi,
	rdi,
};

enum class x64_reg64e
{
	r8 = 8,
	r9,
	r10,
	r11,
	r12,
	r13,
	r14,
	r15,
};

enum class x64_reg32
{
	eax,
	ecx,
	edx,
	ebx,
	esp,
	ebp,
	esi,
	edi,
};

enum class x64_reg16
{
	ax,
	cx,
	dx,
	bx,
	sp,
	bp,
	si,
	di,
};

enum class x64_reg8h
{
	ah,
	ch,
	dh,
	bh,
};

enum class x64_reg8l
{
	al,
	cl,
	dl,
	bl,
};

template<typename T>
struct x64_addr_ptr
{
	x64_addr_ptr(T ptr) : ptr(ptr) { }

	T ptr;
};

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
	inline uint8_t extend_reg_prefix(x64_reg64 dst, x64_reg64 src)
	{
		return static_cast<uint8_t>(
				(dst > x64_reg64::rdi ? x64_rex::w | x64_rex::b : x64_rex::w) |
				(src > x64_reg64::rdi ? x64_rex::w | x64_rex::r : x64_rex::w));
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

template<typename T, T VAL>
class const_operator
{
public:
	inline constexpr operator T() const { return VAL; }
};

template<int VAL> class const_reg64_base : public const_operator<int, VAL> { };
template<int VAL> class const_reg64e : public const_reg64_base<VAL> { };
template<int VAL> class const_reg64 : public const_reg64_base<VAL> { };

class new_x64_regs
{
public:
	static const_reg64<0> rax;
	static const_reg64<1> rcx;
	static const_reg64<2> rdx;
	static const_reg64<3> rbx;
	static const_reg64<4> rsp;
	static const_reg64<5> rbp;
	static const_reg64<6> rsi;
	static const_reg64<7> rdi;

	static const_reg64e<8>  r8;
	static const_reg64e<9>  r9;
	static const_reg64e<10> r10;
	static const_reg64e<11> r11;
	static const_reg64e<12> r12;
	static const_reg64e<13> r13;
	static const_reg64e<14> r14;
	static const_reg64e<15> r15;
};

template<int VAL>
void blabla(const_reg64_base<VAL> value)
{

}

template<int VAL>
void blabla(const_reg64<VAL> value)
{

}

template<int VAL>
void blabla(const_reg64e<VAL> value)
{

}


void test()
{
	blabla(new_x64_regs::rax);
	blabla(new_x64_regs::r15);
}



class x64_mov : public x64_instruction
{
public:
	/* Move register into register */
	x64_mov(x64_reg64 dst, x64_reg64 src) : x64_instruction(std::array<uint8_t, 2> { x64_rex::w, 0x89 }, x64_modrm{dst, src, 3}) {}
	x64_mov(x64_reg64 dst, x64_reg64e src) : x64_instruction(std::array<uint8_t, 2> { x64_rex::w | x64_rex::r, 0x89 }, x64_modrm{dst, src, 3}) {}
	x64_mov(x64_reg64e dst, x64_reg64 src) : x64_instruction(std::array<uint8_t, 2> { x64_rex::w | x64_rex::b, 0x89 }, x64_modrm{dst, src, 3}) {}
	x64_mov(x64_reg64e dst, x64_reg64e src) : x64_instruction(std::array<uint8_t, 2> { x64_rex::w | x64_rex::r | x64_rex::b, 0x89 }, x64_modrm{dst, src, 3}) {}

	x64_mov(x64_reg32 dst, x64_reg32 src) : x64_instruction(std::array<uint8_t, 1> { 0x89 }, x64_modrm{dst, src, 3}) {}
	x64_mov(x64_reg16 dst, x64_reg16 src) : x64_instruction(std::array<uint8_t, 2> { x64_override::oper_size, 0x89 }, x64_modrm{dst, src, 3}) {}
	x64_mov(x64_reg8l dst, x64_reg8l src) : x64_instruction(std::array<uint8_t, 1> { 0x88 }, x64_modrm{dst, src, 3}) {}
	x64_mov(x64_reg8h dst, x64_reg8h src) : x64_instruction(std::array<uint8_t, 1> { 0x88 }, x64_modrm{static_cast<unsigned int>(dst) + 4, static_cast<unsigned int>(src) + 4, 3}) {}

	/* Move register into register pointer */
  //:   48 89 00                mov    %rax,(%rax)
	x64_mov(x64_addr_ptr<x64_reg64> dst, x64_reg64 src) : x64_instruction(std::array<uint8_t, 2> { extend_reg_prefix(dst.ptr, src), 0x89 }, x64_modrm{dst.ptr, src, 0}) { }

  //:   89 00                   mov    %eax,(%rax)
	x64_mov(x64_addr_ptr<x64_reg64> dst, x64_reg32 src) : x64_instruction(std::array<uint8_t, 1> { 0x89 }, x64_modrm{dst.ptr, src, 0}) { }

  //:   66 89 00                mov    %ax,(%rax)
	x64_mov(x64_addr_ptr<x64_reg64> dst, x64_reg16 src) : x64_instruction(std::array<uint8_t, 2> { x64_override::oper_size, 0x89 }, x64_modrm{dst.ptr, src, 0}) { }

  //:   88 00                   mov    %al,(%rax)
	x64_mov(x64_addr_ptr<x64_reg64> dst, x64_reg8l src) : x64_instruction(std::array<uint8_t, 1> { 0x88 }, x64_modrm{dst.ptr, src, 0}) { }

  //:   88 20                   mov    %ah,(%rax)
	x64_mov(x64_addr_ptr<x64_reg64> dst, x64_reg8h src) : x64_instruction(std::array<uint8_t, 1> { 0x88 }, x64_modrm{dst.ptr, static_cast<unsigned int>(src) + 4, 0})
	{
		if (dst.ptr > x64_reg64::rdi)
			throw std::out_of_range("Can't encode extended registers in this instruction");
	}



	//:   90                      nop
  //:   67 48 89 00             mov    %rax,(%eax)
  //:   67 89 00                mov    %eax,(%eax)
  //:   67 66 89 00             mov    %ax,(%eax)
  //:   67 88 20                mov    %ah,(%eax)
  //:   67 88 00                mov    %al,(%eax)
	x64_mov(x64_addr_ptr<x64_reg32> dst, x64_reg32 src) : x64_instruction(std::array<uint8_t, 2> { x64_override::addr_size, 0x89 }, x64_modrm{dst.ptr, src, 0}) { }


	/* Move immediate into register */
	x64_mov(x64_reg32 reg, uint32_t imm) : x64_mov(reg, imm, 0xb8) { };
	x64_mov(x64_reg64 reg, uint64_t imm) : x64_mov(reg, imm, x64_rex::w, 0xb8) { };
	x64_mov(x64_reg64 reg, uint32_t imm) : x64_mov(imm, x64_rex::w, 0xc7, x64_modrm{reg, 0, 3}) { };
	x64_mov(x64_reg16 reg, uint16_t imm) : x64_mov(reg, imm, x64_override::oper_size, 0xb8) { };
	x64_mov(x64_reg8h reg, uint8_t  imm) : x64_mov(reg, imm, 0xb4) { };
	x64_mov(x64_reg8l reg, uint8_t  imm) : x64_mov(reg, imm, 0xb0) { };

	/* Move immediate address into register */
	/* 32 bit pointers */
	x64_mov(x64_reg64 reg, x64_addr_ptr<uint32_t> addr) : x64_mov(addr, x64_rex::w, 0x8b, x64_modrm(4, reg, 0)) { };
	x64_mov(x64_reg32 reg, x64_addr_ptr<uint32_t> addr) : x64_mov(addr, 0x8b, x64_modrm(4, reg, 0)) { };
	x64_mov(x64_reg16 reg, x64_addr_ptr<uint32_t> addr) : x64_mov(addr, x64_override::oper_size, 0x8b, x64_modrm(4, reg, 0)) { };
	x64_mov(x64_reg8l reg, x64_addr_ptr<uint32_t> addr) : x64_mov(addr, 0x8a, x64_modrm(4, reg, 0)) { };
	x64_mov(x64_reg8h reg, x64_addr_ptr<uint32_t> addr) : x64_mov(addr, 0x8a, x64_modrm(4, static_cast<int>(reg) + 4, 0)) { };

	/* 64 bit pointers */
	x64_mov(x64_reg64 reg, x64_addr_ptr<uint64_t*> addr) : x64_mov(reg, addr, x64_rex::w, 0xa1) { };
	x64_mov(x64_reg32 reg, x64_addr_ptr<uint32_t*> addr) : x64_mov(reg, addr, 0xa1) { };
	x64_mov(x64_reg16 reg, x64_addr_ptr<uint16_t*> addr) : x64_mov(reg, addr, x64_override::oper_size, 0xa1) { };
	x64_mov(x64_reg8l reg, x64_addr_ptr<uint8_t*>  addr) : x64_mov(reg, addr, 0xa0) { };


	/* Move register into immediate address */
	/* 32 bit pointers */
	x64_mov(x64_addr_ptr<uint32_t> addr, x64_reg64 reg) : x64_mov(addr, x64_rex::w, 0x89, x64_modrm(4, reg, 0)) { };
	x64_mov(x64_addr_ptr<uint32_t> addr, x64_reg32 reg) : x64_mov(addr, 0x89, x64_modrm(4, reg, 0)) { };
	x64_mov(x64_addr_ptr<uint32_t> addr, x64_reg16 reg) : x64_mov(addr, x64_override::oper_size, 0x89, x64_modrm(4, reg, 0)) { };
	x64_mov(x64_addr_ptr<uint32_t> addr, x64_reg8l reg) : x64_mov(addr, 0x88, x64_modrm(4, reg, 0)) { };
	x64_mov(x64_addr_ptr<uint32_t> addr, x64_reg8h reg) : x64_mov(addr, 0x88, x64_modrm(4, static_cast<int>(reg) + 4, 0)) { };

	/* 64 bit pointers */
	x64_mov(x64_addr_ptr<uint64_t*> addr, x64_reg64 reg) : x64_mov(reg, addr, x64_rex::w, 0xa3) { };
	x64_mov(x64_addr_ptr<uint32_t*> addr, x64_reg32 reg) : x64_mov(reg, addr, 0xa3) { };
	x64_mov(x64_addr_ptr<uint16_t*> addr, x64_reg16 reg) : x64_mov(reg, addr, x64_override::oper_size, 0xa3) { };
	x64_mov(x64_addr_ptr<uint8_t*>  addr, x64_reg8l reg) : x64_mov(reg, addr, 0xa2) { };

private:
	/* Move immediate into register */
	template<typename REG_TYPE, typename IMM_TYPE>
	x64_mov(REG_TYPE reg, IMM_TYPE imm, uint8_t oc)
	 : x64_instruction(
			 std::array<uint8_t, 1> { static_cast<uint8_t>(oc | static_cast<uint8_t>(reg)) },
			 imm)
	{ }

	template<typename REG_TYPE, typename IMM_TYPE>
	x64_mov(REG_TYPE reg, IMM_TYPE imm, uint8_t prefix, uint8_t oc)
	 : x64_instruction(
			std::array<uint8_t, 2> { prefix, static_cast<uint8_t>(oc | static_cast<uint8_t>(reg)) },
			imm)
	{ }

	template<typename IMM_TYPE>
	x64_mov(IMM_TYPE imm, uint8_t prefix, uint8_t oc, const x64_modrm& modrm)
	 : x64_instruction(
			std::array<uint8_t, 2> { prefix, oc },
			modrm, imm)
	{ }

	/* Register <=> immediate address */
	/* 32 bit pointers */
	x64_mov(x64_addr_ptr<uint32_t> addr, uint8_t prefix, uint8_t oc, const x64_modrm& modrm)
	 : x64_instruction(std::array<uint8_t, 2> { prefix, oc }, modrm, x64_sib(0x25), addr.ptr)
	{ }

	x64_mov(x64_addr_ptr<uint32_t> addr, uint8_t oc, const x64_modrm& modrm)
	 : x64_instruction(std::array<uint8_t, 1> { oc }, modrm, x64_sib(0x25), addr.ptr)
	{ }

	/* 64 bit pointers */
	template<typename REG_TYPE, typename IMM_TYPE>
	x64_mov(REG_TYPE reg, x64_addr_ptr<IMM_TYPE*> addr, uint8_t prefix, uint8_t oc)
	 : x64_instruction(
				std::array<uint8_t, 2> { prefix, oc },
				reinterpret_cast<uint64_t>(addr.ptr))
	{
		if (static_cast<int>(reg) != 0)
			throw std::out_of_range("Register not supported");
	}

	template<typename REG_TYPE, typename IMM_TYPE>
	x64_mov(REG_TYPE reg, x64_addr_ptr<IMM_TYPE*> addr, uint8_t oc)
	 : x64_instruction(
				std::array<uint8_t, 1> { oc },
				reinterpret_cast<uint64_t>(addr.ptr))
	{
		if (static_cast<int>(reg) != 0)
			throw std::out_of_range("Register not supported");
	}
};

#endif /* X64INSTRUCTION_H_ */
