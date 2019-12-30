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

	inline constexpr static bool is_extended(int value) { return value >= 8; }
	inline constexpr static bool is_sp(int value) { return value == 4; }
	inline constexpr static bool is_r12(int value) { return value == 8 + 4; }
	inline constexpr static bool is_bp(int value) { return value == 5; }
	inline constexpr static bool is_r13(int value) { return value == 8 + 5; }

	inline constexpr bool is_extended() { return is_extended(value); }
	inline constexpr bool is_sp() { return is_sp(value); }
	inline constexpr bool is_r12() { return is_r12(value); }
	inline constexpr bool is_bp() { return is_bp(value); }
	inline constexpr bool is_r13() { return is_r13(value); }
};

struct x64_reg64    : public x64_reg_base
{
	using x64_reg_base::x64_reg_base;
	using value_type = uint64_t;
	static const std::string names[16];
};
struct x64_reg64a   : public x64_reg64    { using x64_reg64::x64_reg64; };
struct x64_reg64agp : public x64_reg64a   { using x64_reg64a::x64_reg64a; };
struct x64_reg64asp : public x64_reg64a   { using x64_reg64a::x64_reg64a; };
struct x64_reg64abp : public x64_reg64a   { using x64_reg64a::x64_reg64a; };
struct x64_reg64b   : public x64_reg64    { using x64_reg64::x64_reg64; };

struct x64_reg32   : public x64_reg_base
{
	using x64_reg_base::x64_reg_base;
	using value_type = uint32_t;
	static const std::string names[8];
};
struct x64_reg32gp : public x64_reg32     { using x64_reg32::x64_reg32; };
struct x64_reg32bp : public x64_reg32     { using x64_reg32::x64_reg32; };
struct x64_reg32sp : public x64_reg32     { using x64_reg32::x64_reg32; };

struct x64_reg16   : public x64_reg_base
{
	using x64_reg_base::x64_reg_base;
	using value_type = uint16_t;
	static const std::string names[8];
};
struct x64_reg16gp : public x64_reg16     { using x64_reg16::x64_reg16; };
struct x64_reg16bp : public x64_reg16     { using x64_reg16::x64_reg16; };
struct x64_reg16sp : public x64_reg16     { using x64_reg16::x64_reg16; };

struct x64_reg8  : public x64_reg_base
{
	using x64_reg_base::x64_reg_base;
	using value_type = uint8_t;
	static const std::string names[8];
};
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
	constexpr static x64_reg64abp rbp = 5;
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
	constexpr static x64_reg32bp ebp = 5;
	constexpr static x64_reg32gp esi = 6;
	constexpr static x64_reg32gp edi = 7;

	/* 16 bit registers */
	constexpr static x64_reg16gp ax = 0;
	constexpr static x64_reg16gp cx = 1;
	constexpr static x64_reg16gp dx = 2;
	constexpr static x64_reg16gp bx = 3;
	constexpr static x64_reg16sp sp = 4;
	constexpr static x64_reg16bp bp = 5;
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

	x64_modrm(int rm, int reg, int mod)
	 : rm(rm & 7), reg(reg & 7), mod(mod)
	{ }

	x64_modrm(x64_reg_base rm, x64_reg_base reg, int mod)
	 : x64_modrm(static_cast<int>(rm.value) & 7, static_cast<int>(reg.value) & 7, mod)
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

struct x64_reg_ptr32    : public x64_addr_ptr<x64_reg32>
{
	using x64_addr_ptr::x64_addr_ptr;
	using reg_type = x64_reg32;
};
struct x64_reg_ptr32gp  : public x64_reg_ptr32 { using x64_reg_ptr32::x64_reg_ptr32; };
struct x64_reg_ptr32sp  : public x64_reg_ptr32 { using x64_reg_ptr32::x64_reg_ptr32; };

struct x64_reg_ptr64    : public x64_addr_ptr<x64_reg64>
{
	using x64_addr_ptr::x64_addr_ptr;
	using reg_type = x64_reg64;
};
struct x64_reg_ptr64a   : public x64_reg_ptr64 { using x64_reg_ptr64::x64_reg_ptr64; };
struct x64_reg_ptr64agp : public x64_reg_ptr64a { using x64_reg_ptr64a::x64_reg_ptr64a; };
struct x64_reg_ptr64asp : public x64_reg_ptr64a { using x64_reg_ptr64a::x64_reg_ptr64a; };
struct x64_reg_ptr64abp : public x64_reg_ptr64a { using x64_reg_ptr64a::x64_reg_ptr64a; };
struct x64_reg_ptr64b   : public x64_reg_ptr64 { using x64_reg_ptr64::x64_reg_ptr64; };

static constexpr x64_reg_ptr32gp  x64_reg_addr(const x64_reg32gp&  reg) { return x64_reg_ptr32gp(reg); }
static constexpr x64_reg_ptr32sp  x64_reg_addr(const x64_reg32sp&  reg) { return x64_reg_ptr32sp(reg); }
static constexpr x64_reg_ptr64agp x64_reg_addr(const x64_reg64agp& reg) { return x64_reg_ptr64agp(reg); }
static constexpr x64_reg_ptr64abp x64_reg_addr(const x64_reg64abp& reg) { return x64_reg_ptr64abp(reg); }
static constexpr x64_reg_ptr64asp x64_reg_addr(const x64_reg64asp& reg) { return x64_reg_ptr64asp(reg); }
static constexpr x64_reg_ptr64b   x64_reg_addr(const x64_reg64b& reg)   { return x64_reg_ptr64b(reg); }

class x64_instruction : public instruction
{
public:
	x64_instruction() { }

	template<std::size_t OPCODE_SIZE, typename IMM_TYPE>
	x64_instruction(
			const std::array<uint8_t, OPCODE_SIZE>& opcode,
			const struct x64_modrm& m,
			const struct x64_sib& s,
			IMM_TYPE imm)
	 : has_modrm(true), modrm(m), has_sib(true), sib(s),
	   imm_size(sizeof(IMM_TYPE)), imm64(static_cast<uint64_t>(from_or_to_little(imm)))
	{ add_opcode(opcode); }

	template<std::size_t OPCODE_SIZE>
	x64_instruction(
			const std::array<uint8_t, OPCODE_SIZE>& opcode,
			const struct x64_modrm m,
			const struct x64_sib s)
	 : has_modrm(true), modrm(m), has_sib(true), sib(s)
	{ add_opcode(opcode); }

	template<std::size_t OPCODE_SIZE, typename IMM_TYPE>
	x64_instruction(
			const std::array<uint8_t, OPCODE_SIZE>& opcode,
			const struct x64_modrm m,
			IMM_TYPE imm)
	 : has_modrm(true), modrm(m),
	   imm_size(sizeof(IMM_TYPE)), imm64(static_cast<uint64_t>(from_or_to_little(imm)))
	{ add_opcode(opcode); }

	template<std::size_t OPCODE_SIZE>
	x64_instruction(
			const std::array<uint8_t, OPCODE_SIZE>& opcode,
			const struct x64_modrm m)
	 : has_modrm(true), modrm(m)
	{ add_opcode(opcode); }

	template<std::size_t OPCODE_SIZE, typename IMM_TYPE>
	x64_instruction(
			const std::array<uint8_t, OPCODE_SIZE>& opcode,
			const struct x64_sib s,
			IMM_TYPE imm)
	 : has_modrm(true), has_sib(true), sib(s),
	   imm_size(sizeof(IMM_TYPE)), imm64(static_cast<uint64_t>(from_or_to_little(imm)))
	{ add_opcode(opcode); }

	template<std::size_t OPCODE_SIZE>
	x64_instruction(
			const std::array<uint8_t, OPCODE_SIZE>& opcode,
			const struct x64_sib s)
	 : has_modrm(true), has_sib(true), sib(s)
	{ add_opcode(opcode); }

	template<std::size_t OPCODE_SIZE, typename IMM_TYPE>
	x64_instruction(
			const std::array<uint8_t, OPCODE_SIZE>& opcode,
			IMM_TYPE imm)
	 : imm_size(sizeof(IMM_TYPE)), imm64(static_cast<uint64_t>(from_or_to_little(imm)))
	{ add_opcode(opcode); }

	template<std::size_t OPCODE_SIZE>
	x64_instruction(const std::array<uint8_t, OPCODE_SIZE>& opcode)
	{ add_opcode(opcode); }

	/* The same as above, but without an opcode parameter in order to add it dynamically later */
	template<std::size_t OPCODE_SIZE, typename IMM_TYPE>
	x64_instruction(
			const struct x64_modrm& m,
			const struct x64_sib& s,
			IMM_TYPE imm)
	 : has_modrm(true), modrm(m), has_sib(true), sib(s),
	   imm_size(sizeof(IMM_TYPE)), imm64(static_cast<uint64_t>(from_or_to_little(imm)))
	{ }

	template<std::size_t OPCODE_SIZE>
	x64_instruction(
			const struct x64_modrm m,
			const struct x64_sib s)
	 : has_modrm(true), modrm(m), has_sib(true), sib(s)
	{ }

	template<std::size_t OPCODE_SIZE, typename IMM_TYPE>
	x64_instruction(
			const struct x64_modrm m,
			IMM_TYPE imm)
	 : has_modrm(true), modrm(m),
	   imm_size(sizeof(IMM_TYPE)), imm64(static_cast<uint64_t>(from_or_to_little(imm)))
	{ }

	template<std::size_t OPCODE_SIZE>
	x64_instruction(
			const struct x64_modrm m)
	 : has_modrm(true), modrm(m)
	{ }

	template<std::size_t OPCODE_SIZE, typename IMM_TYPE>
	x64_instruction(
			const struct x64_sib s,
			IMM_TYPE imm)
	 : has_modrm(true), has_sib(true), sib(s),
	   imm_size(sizeof(IMM_TYPE)), imm64(static_cast<uint64_t>(from_or_to_little(imm)))
	{ }

	template<std::size_t OPCODE_SIZE>
	x64_instruction(
			const struct x64_sib s)
	 : has_modrm(true), has_sib(true), sib(s)
	{ }

	template<std::size_t OPCODE_SIZE, typename IMM_TYPE>
	x64_instruction(
			IMM_TYPE imm)
	 : imm_size(sizeof(IMM_TYPE)), imm64(static_cast<uint64_t>(from_or_to_little(imm)))
	{ }

	inline std::size_t size() const override
	{
		return  opcode_size +
				(has_modrm ? 1 : 0) +
				(has_sib ? 1 : 0) +
				imm_size;
	}

	bool emit_to(uint8_t* p) const override
	{
		for (size_t i = 0; i < opcode_size; i++)
			*p++ = opcode[i];

		if (has_modrm)
			*p++ = modrm_byte;

		if (has_sib)
			*p++ = sib_byte;

		for (size_t i = 0; i < imm_size; i++)
			*p++ = imm_data[i];

		return true;
	}

	virtual ~x64_instruction() { }

	template<std::size_t OPCODE_SIZE>
	inline void add_opcode(const std::array<uint8_t, OPCODE_SIZE>& oc)
	{
		if (oc.size() + opcode_size > 15)
			throw std::overflow_error("instruction was too long");
		/* Add the opcode */

		for(const auto& b : oc)
			opcode[opcode_size++] = b;
	}

	inline void add_opcode(uint8_t oc)
	{
		if (opcode_size >= sizeof(opcode))
			throw std::overflow_error("X86/64 opcode longer than 15 bytes");

		opcode[opcode_size++] = oc;
	}
	inline void reset_opcode() { opcode_size = 0; }

	inline void set_modrm(const x64_modrm& m)
	{
		has_modrm = true;
		modrm = m;
	}
	inline void set_modrm(uint8_t m)
	{
		has_modrm = true;
		modrm_byte = m;
	}
	inline const struct x64_modrm& get_modrm() { return modrm; }
	inline void reset_modrm() { has_modrm = false; }

	inline void set_sib(const x64_sib& s)
	{
		has_sib = true;
		sib = s;
	}
	inline void set_sib(uint8_t s)
	{
		has_sib = true;
		sib_byte = s;
	}
	inline const struct x64_sib& get_sib() { return sib; }
	inline void reset_sib() { has_sib = false; }

	template<typename T>
	inline void set_imm(T x)
	{
		imm_size = sizeof(x);
		imm64 = static_cast<uint64_t>(from_or_to_little(x));
	}

	template<typename T>
	inline T get_imm()
	{
		 return static_cast<T>(from_or_to_little(imm64));
	}

	inline void reset_imm() { imm_size = 0; }

protected:
	inline uint8_t extend_reg_prefix(x64_reg_base dst, x64_reg_base src)
	{
		return static_cast<uint8_t>(
				(dst.is_extended() ? x64_rex::w | x64_rex::b : x64_rex::w) |
				(src.is_extended() ? x64_rex::w | x64_rex::r : x64_rex::w));
	}

public:
#if __BYTE_ORDER == __LITTLE_ENDIAN
	inline static uint64_t from_or_to_little(uint64_t x) { return x; }
	inline static uint32_t from_or_to_little(uint32_t x) { return x; }
	inline static uint16_t from_or_to_little(uint16_t x) { return x; }
	inline static int64_t from_or_to_little(int64_t x) { return x; }
	inline static int32_t from_or_to_little(int32_t x) { return x; }
	inline static int16_t from_or_to_little(int16_t x) { return x; }
#elif __BYTE_ORDER == __BIG_ENDIAN
	inline static uint64_t from_or_to_little(uint64_t x) { return __builtin_bswap64(x); }
	inline static uint32_t from_or_to_little(uint32_t x) { return __builtin_bswap32(x); }
	inline static uint16_t from_or_to_little(uint16_t x) { return __builtin_bswap16(x); }
	inline static int64_t from_or_to_little(int64_t x) { return __builtin_bswap64(x); }
	inline static int32_t from_or_to_little(int32_t x) { return __builtin_bswap32(x); }
	inline static int16_t from_or_to_little(int16_t x) { return __builtin_bswap16(x); }
#else
#error Cannot determine endianness
#endif

private:
	size_t opcode_size = 0;
	uint8_t opcode[15]; // 15 is the maximum instructionuction size on x64

	bool has_modrm = false;
	union
	{
		struct x64_modrm modrm;
		uint8_t modrm_byte;
	};

	bool has_sib = false;
	union
	{
		struct x64_sib sib;
		uint8_t sib_byte;
	};

	size_t imm_size = 0;
	union
	{
		uint64_t imm64;
		uint32_t imm32;
		uint16_t imm16;
		uint8_t imm8;
		uint8_t imm_data[8];
	};
};

struct x64_nop1	: public x64_instruction{x64_nop1()	: x64_instruction(std::array<uint8_t, 1> { 0x90 }) {} };
struct x64_ret	: public x64_instruction{x64_ret()	: x64_instruction(std::array<uint8_t, 1> { 0xc3 }) {} };
struct x64_lret	: public x64_instruction{x64_lret()	: x64_instruction(std::array<uint8_t, 1> { 0xcb }) {} };
struct x64_ud2	: public x64_instruction{x64_ud2()	: x64_instruction(std::array<uint8_t, 2> { 0x0f, 0x0b }) {} };

class x64_mov : public x64_instruction
{
public:
	/* For 8-bit reg-reg moves, the opcode is 0x88, otherwise it's 0x89 */
	template<typename OPER_TYPE, typename ADDR_TYPE>
	void x64_add_overrides()
	{
		if (sizeof(typename ADDR_TYPE::value_type) == sizeof(uint32_t))
			add_opcode(x64_override::addr_size);

		if (sizeof(typename OPER_TYPE::value_type) == sizeof(uint16_t))
			add_opcode(x64_override::oper_size);
	}

	template<typename T, typename U>
	void x64_add_prefix(const T& a, const x64_addr_ptr<U>& b)
	{
		x64_add_overrides<T, U>();

		uint8_t rex = 0;

		if (a.is_extended())
			rex |= x64_rex::r;
		if (b.ptr.is_extended())
			rex |= x64_rex::b;
		if (sizeof(typename T::value_type) == sizeof(uint64_t))
			rex |= x64_rex::w;

		if (rex)
			add_opcode(rex);
	}

	template<typename T>
	void x64_add_prefix(const T& a, const T& b)
	{
		if (sizeof(typename T::value_type) == sizeof(uint16_t))
			add_opcode(x64_override::oper_size);

		uint8_t rex = 0;

		if (a.is_extended())
			rex |= x64_rex::b;
		if (b.is_extended())
			rex |= x64_rex::r;
		if (sizeof(typename T::value_type) == sizeof(uint64_t))
			rex |= x64_rex::w;

		if (rex)
			add_opcode(rex);
	}

	template<typename T, typename U, int MOD, uint8_t OC>
	void x64_reg_reg(T a, U b)
	{
		add_opcode(OC);
		set_modrm(x64_modrm{a, b, MOD});
	}

	template<typename T, typename U, uint8_t OC>
	void x64_mov_reg_regptr(const T& a, const x64_addr_ptr<U>& b)
	{
		x64_add_prefix(a, b);
		x64_reg_reg_ptr<T, U, OC>(a, b);
	}

	template<typename T, typename U, uint8_t OC>
	void x64_mov_regptr_reg(const x64_addr_ptr<T>& a, const U& b)
	{
		x64_add_prefix(b, a);
		x64_reg_reg_ptr<U, T, OC>(b, a);
	}

	template<typename T, typename U, uint8_t OC>
	void x64_reg_reg_ptr(const T& a, const x64_addr_ptr<U>& b)
	{
		/*
		 * %sp, %r12 and %bp, r13 require some special treatment.
		 * See: https://stackoverflow.com/questions/36529449/why-are-rbp-and-rsp-called-general-purpose-registers
		 */

		if (b.ptr.is_bp() || b.ptr.is_r13())
		{
			x64_mov_reg_reg_ptr_off<T, U, uint8_t, 1, OC>(a, b, static_cast<uint8_t>(0));

			return;
		}

		x64_reg_reg<U, T, 0, OC>(b.ptr, a);

		/* %sp and %r12 always require a 0x24 sib byte. See stackoverflow link above */
		if (b.ptr.is_sp() || b.ptr.is_r12())
		{
			set_sib(x64_sib(0x24));
		}
	}

	template<typename T, typename U, typename W, int MOD, uint8_t OC>
	void x64_mov_reg_reg_ptr_off(const T& a, const x64_addr_ptr<U>& b, W imm)
	{
		x64_reg_reg<U, T, MOD, OC>(b.ptr, a);

		if ((a.is_sp() || a.is_r12()) && !(b.ptr.is_bp() || b.ptr.is_r13()))
		{
			set_sib(x64_sib(0x24));
		}

		set_imm(imm);
	}

	template<typename T, uint8_t OC>
	void x64_mov_reg_reg(T a, T b)
	{
		x64_add_prefix(a, b);
		x64_reg_reg<T, T, 3, OC>(a, b);
	}


	/* Move register into register */
	x64_mov(x64_reg64 dst, x64_reg64 src) { x64_mov_reg_reg<x64_reg64, 0x89>(dst, src); }
	x64_mov(x64_reg32 dst, x64_reg32 src) { x64_mov_reg_reg<x64_reg32, 0x89>(dst, src); }
	x64_mov(x64_reg16 dst, x64_reg16 src) { x64_mov_reg_reg<x64_reg16, 0x89>(dst, src); }
	x64_mov(x64_reg8  dst, x64_reg8  src) { x64_mov_reg_reg<x64_reg8, 0x88>(dst, src); }

	/* Move register into register pointer */
	x64_mov(x64_reg_ptr64 addr, x64_reg64 reg) { x64_mov_regptr_reg<x64_reg64, x64_reg64, 0x89>(addr, reg); }
	x64_mov(x64_reg_ptr64 addr, x64_reg32 reg) { x64_mov_regptr_reg<x64_reg64, x64_reg32, 0x89>(addr, reg); }
	x64_mov(x64_reg_ptr64 addr, x64_reg16 reg) { x64_mov_regptr_reg<x64_reg64, x64_reg16, 0x89>(addr, reg); }
	x64_mov(x64_reg_ptr64 addr, x64_reg8l reg) { x64_mov_regptr_reg<x64_reg64, x64_reg8, 0x88>(addr, reg); }

	x64_mov(x64_reg_ptr32 addr, x64_reg64 reg) { x64_mov_regptr_reg<x64_reg32, x64_reg64, 0x89>(addr, reg); }
	x64_mov(x64_reg_ptr32 addr, x64_reg32 reg) { x64_mov_regptr_reg<x64_reg32, x64_reg32, 0x89>(addr, reg); }
	x64_mov(x64_reg_ptr32 addr, x64_reg16 reg) { x64_mov_regptr_reg<x64_reg32, x64_reg16, 0x89>(addr, reg); }
	x64_mov(x64_reg_ptr32 addr, x64_reg8l reg) { x64_mov_regptr_reg<x64_reg32, x64_reg8, 0x88>(addr, reg); }

	/* Move register pointer into register */
	x64_mov(x64_reg64 reg, x64_reg_ptr64 addr) { x64_mov_reg_regptr<x64_reg64, x64_reg64, 0x8b>(reg, addr); }
	x64_mov(x64_reg32 reg, x64_reg_ptr64 addr) { x64_mov_reg_regptr<x64_reg32, x64_reg64, 0x8b>(reg, addr); }
	x64_mov(x64_reg16 reg, x64_reg_ptr64 addr) { x64_mov_reg_regptr<x64_reg16, x64_reg64, 0x8b>(reg, addr); }
	x64_mov(x64_reg8l reg, x64_reg_ptr64 addr) { x64_mov_reg_regptr<x64_reg8, x64_reg64, 0x8a>(reg, addr); }

	x64_mov(x64_reg64 reg, x64_reg_ptr32 addr) { x64_mov_reg_regptr<x64_reg64, x64_reg32, 0x8b>(reg, addr); }
	x64_mov(x64_reg32 reg, x64_reg_ptr32 addr) { x64_mov_reg_regptr<x64_reg32, x64_reg32, 0x8b>(reg, addr); }
	x64_mov(x64_reg16 reg, x64_reg_ptr32 addr) { x64_mov_reg_regptr<x64_reg16, x64_reg32, 0x8b>(reg, addr); }
	x64_mov(x64_reg8l reg, x64_reg_ptr32 addr) { x64_mov_reg_regptr<x64_reg8, x64_reg32, 0x8a>(reg, addr); }

	/* Move register into register pointer + 8 bit offset */
	//x64_mov(x64_reg_ptr64 addr, x64_reg64 reg, int8_t off) { x64_mov_reg_reg_ptr_off<x64_reg64, x64_reg64, int8_t, 1>(addr.ptr, reg, off); } //: x64_instruction(std::array<uint8_t, 2> { extend_reg_prefix(addr.ptr, reg), 0x89 }, x64_modrm{addr.ptr, reg, 1}, off) { }
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
	//x64_mov(x64_reg64 reg, x64_reg_ptr64 addr, int8_t off) { x64_mov_reg_reg_ptr_off<x64_reg64, x64_reg64, int8_t, 1>(addr.ptr, reg, off); } //: x64_instruction(std::array<uint8_t, 2> { extend_reg_prefix(addr.ptr, reg), 0x8b }, x64_modrm{addr.ptr, reg, 1}, off) { }
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

	virtual ~x64_mov() { }

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
