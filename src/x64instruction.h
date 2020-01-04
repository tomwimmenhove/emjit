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

	inline constexpr bool is_extended() { return value >= 8; }
};

struct x64_reg_base_normal : public x64_reg_base
{
	using x64_reg_base::x64_reg_base;

	inline constexpr bool is_sp() { return value == 4; }
	inline constexpr bool is_r12() { return value == 8 + 4; }
	inline constexpr bool is_bp() { return value == 5; }
	inline constexpr bool is_r13() { return value == 8 + 5; }
};

struct x64_reg_base8 : public x64_reg_base
{
	using x64_reg_base::x64_reg_base;

	/* 8-bit registers aren't sp/bp :) */
	inline constexpr bool is_sp() { return false; }
	inline constexpr bool is_r12() { return false; }
	inline constexpr bool is_bp() { return false; }
	inline constexpr bool is_r13() { return false; }
};

struct x64_reg64    : public x64_reg_base_normal
{
	using x64_reg_base_normal::x64_reg_base_normal;
	using value_type = uint64_t;
	static const std::string names[16];
};
struct x64_reg64_0 : public x64_reg64 { using x64_reg64::x64_reg64; };

struct x64_reg32   : public x64_reg_base_normal
{
	using x64_reg_base_normal::x64_reg_base_normal;
	using value_type = uint32_t;
	static const std::string names[8];
};
struct x64_reg32_0 : public x64_reg32 { using x64_reg32::x64_reg32; };

struct x64_reg16   : public x64_reg_base_normal
{
	using x64_reg_base_normal::x64_reg_base_normal;
	using value_type = uint16_t;
	static const std::string names[8];
};
struct x64_reg16_0 : public x64_reg16 { using x64_reg16::x64_reg16; };

struct x64_reg8  : public x64_reg_base8
{
	using x64_reg_base8::x64_reg_base8;
	using value_type = uint8_t;
	static const std::string names[8];
};
struct x64_reg8h   : public x64_reg8  { using x64_reg8::x64_reg8; };
struct x64_reg8l   : public x64_reg8  { using x64_reg8::x64_reg8; };
struct x64_reg8l_0 : public x64_reg8l { using x64_reg8l::x64_reg8l; };
struct x64_regs
{
	/* 64 bit registers */
	constexpr static x64_reg64_0 rax = 0;
	constexpr static x64_reg64 rcx = 1;
	constexpr static x64_reg64 rdx = 2;
	constexpr static x64_reg64 rbx = 3;
	constexpr static x64_reg64 rsp = 4;
	constexpr static x64_reg64 rbp = 5;
	constexpr static x64_reg64 rsi = 6;
	constexpr static x64_reg64 rdi = 7;

	/* 64 bit extended registers */
	constexpr static x64_reg64  r8 = 8;
	constexpr static x64_reg64  r9 = 9;
	constexpr static x64_reg64 r10 = 10;
	constexpr static x64_reg64 r11 = 11;
	constexpr static x64_reg64 r12 = 12;
	constexpr static x64_reg64 r13 = 13;
	constexpr static x64_reg64 r14 = 14;
	constexpr static x64_reg64 r15 = 15;

	/* 32 bit registers */
	constexpr static x64_reg32_0 eax = 0;
	constexpr static x64_reg32 ecx = 1;
	constexpr static x64_reg32 edx = 2;
	constexpr static x64_reg32 ebx = 3;
	constexpr static x64_reg32 esp = 4;
	constexpr static x64_reg32 ebp = 5;
	constexpr static x64_reg32 esi = 6;
	constexpr static x64_reg32 edi = 7;

	/* 16 bit registers */
	constexpr static x64_reg16_0 ax = 0;
	constexpr static x64_reg16 cx = 1;
	constexpr static x64_reg16 dx = 2;
	constexpr static x64_reg16 bx = 3;
	constexpr static x64_reg16 sp = 4;
	constexpr static x64_reg16 bp = 5;
	constexpr static x64_reg16 si = 6;
	constexpr static x64_reg16 di = 7;

	/* lower 8 bit registers */
	constexpr static x64_reg8l_0 al = 0;
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

	x64_sib(x64_reg_base base, x64_reg_base index, sib_scale scale)
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

static constexpr x64_reg_ptr32  x64_reg_addr(const x64_reg32& reg) { return x64_reg_ptr32(reg); }
static constexpr x64_reg_ptr64  x64_reg_addr(const x64_reg64& reg) { return x64_reg_ptr64(reg); }

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
	template<typename T, typename U>
	void reg_reg_ptr(const T& a, const x64_addr_ptr<U>& b, uint8_t oc)
	{
		/*
		 * %sp, %r12 and %bp, r13 require some special treatment.
		 * See: https://stackoverflow.com/questions/36529449/why-are-rbp-and-rsp-called-general-purpose-registers
		 */

		/* %bp and %r12 can only be used with a zero-offset */
		if (b.ptr.is_bp() || b.ptr.is_r13())
		{
			reg_reg_ptr_off(a, b, oc, 1, static_cast<uint8_t>(0));
			return;
		}

		add_prefixes(a, b);
		reg_reg_oc_mod(b.ptr, a, oc, 0);

		/* %sp and %r12 always require a 0x24 sib byte. See stackoverflow link above */
		if (b.ptr.is_sp() || b.ptr.is_r12())
			set_sib(0x24);
	}

	template<typename T, typename U, typename W>
	void reg_reg_ptr_off(const T& a, const x64_addr_ptr<U>& b, uint8_t oc, int mod, W imm)
	{
		add_prefixes(a, b);
		reg_reg_oc_mod(b.ptr, a, oc, mod);

		if (b.ptr.is_sp() || b.ptr.is_r12())
			set_sib(0x24);

		set_imm(imm);
	}

	template<typename T>
	void reg_reg(T a, T b, uint8_t oc)
	{
		add_prefixes(a, b);
		reg_reg_oc_mod(a, b, oc, 3);
	}

	template<typename T, typename U>
	void reg_reg_ptr_idx(const T& a, const x64_addr_ptr<U>& b, const U& c, sib_scale scale, uint8_t oc)
	{
		if (c.is_sp())
			throw std::invalid_argument("Stack pointer cannot be used as index register");

		if (b.ptr.is_bp() || b.ptr.is_r13())
		{
			reg_reg_ptr_idx_off(a, b, c, scale, static_cast<int8_t>(0), oc, 1);
			return;
		}

		add_prefixes(a, b, c);
		add_opcode(oc);
		set_modrm(x64_modrm{4, a, 0});
		set_sib(x64_sib(b.ptr, c, scale));
	}

	template<typename T, typename U, typename V>
	void reg_reg_ptr_idx_off(const T& a, const x64_addr_ptr<U>& b, const U& c, sib_scale scale, V off, uint8_t oc, int mod)
	{
		if (c.is_sp())
			throw std::invalid_argument("Stack pointer cannot be used as index register");

		add_prefixes(a, b, c);
		add_opcode(oc);
		set_modrm(x64_modrm{4, a, mod});
		set_sib(x64_sib(b.ptr, c, scale));
		set_imm(off);
	}

	template<typename T, typename U>
	inline void add_prefixes(const T& a, const x64_addr_ptr<U>& b, const U& c = U(0))
	{
		if (sizeof(typename U::value_type) == sizeof(uint32_t))
			add_opcode(x64_override::addr_size);

		if (sizeof(typename T::value_type) == sizeof(uint16_t))
			add_opcode(x64_override::oper_size);

		x64_add_rex(b.ptr, c, a, sizeof(typename T::value_type));
	}

	template<typename T>
	inline void add_prefixes(const T& a, const T& b)
	{
		if (sizeof(typename T::value_type) == sizeof(uint16_t))
			add_opcode(x64_override::oper_size);

		x64_add_rex(a, b, sizeof(typename T::value_type));
	}

	template<typename T, typename U>
	void reg_reg_oc_mod(T a, U b, uint8_t oc, int mod)
	{
		add_opcode(oc);
		set_modrm(x64_modrm{a, b, mod});
	}

	template<typename T, typename U>
	void orred_oc_reg_imm(T reg, U imm, uint8_t oc)
	{
		add_prefixes(reg, T(0));
		add_opcode(oc | reg.value);
		set_imm(imm);
	}

	template<typename T, typename U>
	void eax_imm(T reg, U imm, uint8_t oc)
	{
		add_prefixes(reg, T(0));
		add_opcode(oc);
		set_imm(imm);
	}

	template<typename T, typename U>
	void reg_imm_addr(T reg, x64_addr_ptr<U> addr, uint8_t oc)
	{
		add_prefixes(T(0), reg);
		add_opcode(oc);
		set_modrm(x64_modrm(4, reg, 0));
		set_sib(0x25);
		set_imm(addr.ptr);
	}

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
	template<typename T, typename U>
	inline void x64_add_rex(const T& a, const T& c, const U& b, size_t oper_size)
	{
		uint8_t rex = 0;

		if (a.is_extended())
			rex |= x64_rex::b;
		if (b.is_extended())
			rex |= x64_rex::r;
		if (c.is_extended())
			rex |= x64_rex::x;
		if (oper_size == sizeof(uint64_t))
			rex |= x64_rex::w;

		if (rex)
			add_opcode(rex);
	}

	template<typename T, typename U>
	inline void x64_add_rex(const T& a, const U& b, size_t oper_size)
	{ x64_add_rex(a, T(0), b, oper_size); }

	size_t opcode_size = 0;
	uint8_t opcode[15]; // 15 is the maximum instruction size on x64

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

template<uint8_t A, uint8_t B, uint8_t C, uint8_t D>
class x64_xxx : public x64_instruction
{
public:
	using x64_instruction::x64_instruction;

	/* register into register */
	x64_xxx(x64_reg64 dst, x64_reg64 src) { reg_reg(dst, src, B); }
	x64_xxx(x64_reg32 dst, x64_reg32 src) { reg_reg(dst, src, B); }
	x64_xxx(x64_reg16 dst, x64_reg16 src) { reg_reg(dst, src, B); }
	x64_xxx(x64_reg8  dst, x64_reg8  src) { reg_reg(dst, src, A); }

	/* register into register pointer */
	x64_xxx(x64_reg_ptr64 addr, x64_reg64 reg) { reg_reg_ptr(reg, addr, B); }
	x64_xxx(x64_reg_ptr64 addr, x64_reg32 reg) { reg_reg_ptr(reg, addr, B); }
	x64_xxx(x64_reg_ptr64 addr, x64_reg16 reg) { reg_reg_ptr(reg, addr, B); }
	x64_xxx(x64_reg_ptr64 addr, x64_reg8l reg) { reg_reg_ptr(reg, addr, A); }

	x64_xxx(x64_reg_ptr32 addr, x64_reg64 reg) { reg_reg_ptr(reg, addr, B); }
	x64_xxx(x64_reg_ptr32 addr, x64_reg32 reg) { reg_reg_ptr(reg, addr, B); }
	x64_xxx(x64_reg_ptr32 addr, x64_reg16 reg) { reg_reg_ptr(reg, addr, B); }
	x64_xxx(x64_reg_ptr32 addr, x64_reg8l reg) { reg_reg_ptr(reg, addr, A); }

	/* register pointer into register */
	x64_xxx(x64_reg64 reg, x64_reg_ptr64 addr) { reg_reg_ptr(reg, addr, D); }
	x64_xxx(x64_reg32 reg, x64_reg_ptr64 addr) { reg_reg_ptr(reg, addr, D); }
	x64_xxx(x64_reg16 reg, x64_reg_ptr64 addr) { reg_reg_ptr(reg, addr, D); }
	x64_xxx(x64_reg8l reg, x64_reg_ptr64 addr) { reg_reg_ptr(reg, addr, C); }

	x64_xxx(x64_reg64 reg, x64_reg_ptr32 addr) { reg_reg_ptr(reg, addr, D); }
	x64_xxx(x64_reg32 reg, x64_reg_ptr32 addr) { reg_reg_ptr(reg, addr, D); }
	x64_xxx(x64_reg16 reg, x64_reg_ptr32 addr) { reg_reg_ptr(reg, addr, D); }
	x64_xxx(x64_reg8l reg, x64_reg_ptr32 addr) { reg_reg_ptr(reg, addr, C); }

	/* register into register pointer + 8 bit offset */
	x64_xxx(x64_reg_ptr64 addr, int8_t off, x64_reg64 reg) { reg_reg_ptr_off(reg, addr, B, 1, off); }
	x64_xxx(x64_reg_ptr64 addr, int8_t off, x64_reg32 reg) { reg_reg_ptr_off(reg, addr, B, 1, off); }
	x64_xxx(x64_reg_ptr64 addr, int8_t off, x64_reg16 reg) { reg_reg_ptr_off(reg, addr, B, 1, off); }
	x64_xxx(x64_reg_ptr64 addr, int8_t off, x64_reg8l reg) { reg_reg_ptr_off(reg, addr, A, 1, off); }

	x64_xxx(x64_reg_ptr32 addr, int8_t off, x64_reg64 reg) { reg_reg_ptr_off(reg, addr, B, 1, off); }
	x64_xxx(x64_reg_ptr32 addr, int8_t off, x64_reg32 reg) { reg_reg_ptr_off(reg, addr, B, 1, off); }
	x64_xxx(x64_reg_ptr32 addr, int8_t off, x64_reg16 reg) { reg_reg_ptr_off(reg, addr, B, 1, off); }
	x64_xxx(x64_reg_ptr32 addr, int8_t off, x64_reg8l reg) { reg_reg_ptr_off(reg, addr, A, 1, off); }

	/* register pointer + 8 bit offset into register */
	x64_xxx(x64_reg64 reg, x64_reg_ptr64 addr, int8_t off) { reg_reg_ptr_off(reg, addr, D, 1, off); }
	x64_xxx(x64_reg32 reg, x64_reg_ptr64 addr, int8_t off) { reg_reg_ptr_off(reg, addr, D, 1, off); }
	x64_xxx(x64_reg16 reg, x64_reg_ptr64 addr, int8_t off) { reg_reg_ptr_off(reg, addr, D, 1, off); }
	x64_xxx(x64_reg8l reg, x64_reg_ptr64 addr, int8_t off) { reg_reg_ptr_off(reg, addr, C, 1, off); }

	x64_xxx(x64_reg64 reg, x64_reg_ptr32 addr, int8_t off) { reg_reg_ptr_off(reg, addr, D, 1, off); }
	x64_xxx(x64_reg32 reg, x64_reg_ptr32 addr, int8_t off) { reg_reg_ptr_off(reg, addr, D, 1, off); }
	x64_xxx(x64_reg16 reg, x64_reg_ptr32 addr, int8_t off) { reg_reg_ptr_off(reg, addr, D, 1, off); }
	x64_xxx(x64_reg8l reg, x64_reg_ptr32 addr, int8_t off) { reg_reg_ptr_off(reg, addr, C, 1, off); }

	/* register into register pointer + 32 bit offset */
	x64_xxx(x64_reg_ptr64 addr, int32_t off, x64_reg64 reg) { reg_reg_ptr_off(reg, addr, B, 2, off); }
	x64_xxx(x64_reg_ptr64 addr, int32_t off, x64_reg32 reg) { reg_reg_ptr_off(reg, addr, B, 2, off); }
	x64_xxx(x64_reg_ptr64 addr, int32_t off, x64_reg16 reg) { reg_reg_ptr_off(reg, addr, B, 2, off); }
	x64_xxx(x64_reg_ptr64 addr, int32_t off, x64_reg8l reg) { reg_reg_ptr_off(reg, addr, A, 2, off); }

	x64_xxx(x64_reg_ptr32 addr, int32_t off, x64_reg64 reg) { reg_reg_ptr_off(reg, addr, B, 2, off); }
	x64_xxx(x64_reg_ptr32 addr, int32_t off, x64_reg32 reg) { reg_reg_ptr_off(reg, addr, B, 2, off); }
	x64_xxx(x64_reg_ptr32 addr, int32_t off, x64_reg16 reg) { reg_reg_ptr_off(reg, addr, B, 2, off); }
	x64_xxx(x64_reg_ptr32 addr, int32_t off, x64_reg8l reg) { reg_reg_ptr_off(reg, addr, A, 2, off); }

	/* register pointer + 32 bit offset into register */
	x64_xxx(x64_reg64 reg, x64_reg_ptr64 addr, int32_t off) { reg_reg_ptr_off(reg, addr, D, 2, off); }
	x64_xxx(x64_reg32 reg, x64_reg_ptr64 addr, int32_t off) { reg_reg_ptr_off(reg, addr, D, 2, off); }
	x64_xxx(x64_reg16 reg, x64_reg_ptr64 addr, int32_t off) { reg_reg_ptr_off(reg, addr, D, 2, off); }
	x64_xxx(x64_reg8l reg, x64_reg_ptr64 addr, int32_t off) { reg_reg_ptr_off(reg, addr, C, 2, off); }

	x64_xxx(x64_reg64 reg, x64_reg_ptr32 addr, int32_t off) { reg_reg_ptr_off(reg, addr, D, 2, off); }
	x64_xxx(x64_reg32 reg, x64_reg_ptr32 addr, int32_t off) { reg_reg_ptr_off(reg, addr, D, 2, off); }
	x64_xxx(x64_reg16 reg, x64_reg_ptr32 addr, int32_t off) { reg_reg_ptr_off(reg, addr, D, 2, off); }
	x64_xxx(x64_reg8l reg, x64_reg_ptr32 addr, int32_t off) { reg_reg_ptr_off(reg, addr, C, 2, off); }

	/* immediate address into register */
	/* 32 bit pointers */
	x64_xxx(x64_reg64 reg, x64_addr_ptr<int32_t> addr) { reg_imm_addr(reg, addr, D); }
	x64_xxx(x64_reg32 reg, x64_addr_ptr<int32_t> addr) { reg_imm_addr(reg, addr, D); }
	x64_xxx(x64_reg16 reg, x64_addr_ptr<int32_t> addr) { reg_imm_addr(reg, addr, D); }
	x64_xxx(x64_reg8  reg, x64_addr_ptr<int32_t> addr) { reg_imm_addr(reg, addr, C); }

	/* register into immediate address */
	/* 32 bit pointers */
	x64_xxx(x64_addr_ptr<int32_t> addr, x64_reg64 reg) { reg_imm_addr(reg, addr, B); }
	x64_xxx(x64_addr_ptr<int32_t> addr, x64_reg32 reg) { reg_imm_addr(reg, addr, B); }
	x64_xxx(x64_addr_ptr<int32_t> addr, x64_reg16 reg) { reg_imm_addr(reg, addr, B); }
	x64_xxx(x64_addr_ptr<int32_t> addr, x64_reg8  reg) { reg_imm_addr(reg, addr, A); }


	/* 64 bit base address + index * scale into reg */
	x64_xxx(x64_reg64 reg, x64_reg_ptr64 addr, x64_reg64 index, sib_scale scale) { reg_reg_ptr_idx(reg, addr, index, scale, D); }
	x64_xxx(x64_reg32 reg, x64_reg_ptr64 addr, x64_reg64 index, sib_scale scale) { reg_reg_ptr_idx(reg, addr, index, scale, D); }
	x64_xxx(x64_reg16 reg, x64_reg_ptr64 addr, x64_reg64 index, sib_scale scale) { reg_reg_ptr_idx(reg, addr, index, scale, D); }
	x64_xxx(x64_reg8l reg, x64_reg_ptr64 addr, x64_reg64 index, sib_scale scale) { reg_reg_ptr_idx(reg, addr, index, scale, C); }

	/* 32 bit base address + index * scale into reg */
	x64_xxx(x64_reg64 reg, x64_reg_ptr32 addr, x64_reg32 index, sib_scale scale) { reg_reg_ptr_idx(reg, addr, index, scale, D); }
	x64_xxx(x64_reg32 reg, x64_reg_ptr32 addr, x64_reg32 index, sib_scale scale) { reg_reg_ptr_idx(reg, addr, index, scale, D); }
	x64_xxx(x64_reg16 reg, x64_reg_ptr32 addr, x64_reg32 index, sib_scale scale) { reg_reg_ptr_idx(reg, addr, index, scale, D); }
	x64_xxx(x64_reg8l reg, x64_reg_ptr32 addr, x64_reg32 index, sib_scale scale) { reg_reg_ptr_idx(reg, addr, index, scale, C); }

	/* reg into 64 bit base address + index * scale */
	x64_xxx(x64_reg_ptr64 addr, x64_reg64 index, sib_scale scale, x64_reg64 reg) { reg_reg_ptr_idx(reg, addr, index, scale, B); }
	x64_xxx(x64_reg_ptr64 addr, x64_reg64 index, sib_scale scale, x64_reg32 reg) { reg_reg_ptr_idx(reg, addr, index, scale, B); }
	x64_xxx(x64_reg_ptr64 addr, x64_reg64 index, sib_scale scale, x64_reg16 reg) { reg_reg_ptr_idx(reg, addr, index, scale, B); }
	x64_xxx(x64_reg_ptr64 addr, x64_reg64 index, sib_scale scale, x64_reg8l reg) { reg_reg_ptr_idx(reg, addr, index, scale, A); }

	/* reg into 32 bit base address + index * scale */
	x64_xxx(x64_reg_ptr32 addr, x64_reg32 index, sib_scale scale, x64_reg64 reg) { reg_reg_ptr_idx(reg, addr, index, scale, B); }
	x64_xxx(x64_reg_ptr32 addr, x64_reg32 index, sib_scale scale, x64_reg32 reg) { reg_reg_ptr_idx(reg, addr, index, scale, B); }
	x64_xxx(x64_reg_ptr32 addr, x64_reg32 index, sib_scale scale, x64_reg16 reg) { reg_reg_ptr_idx(reg, addr, index, scale, B); }
	x64_xxx(x64_reg_ptr32 addr, x64_reg32 index, sib_scale scale, x64_reg8l reg) { reg_reg_ptr_idx(reg, addr, index, scale, A); }


	/* 64 bit base address + index * scale + 8 bit offset into reg */
	x64_xxx(x64_reg64 reg, x64_reg_ptr64 addr, x64_reg64 index, sib_scale scale, int8_t off) { reg_reg_ptr_idx_off(reg, addr, index, scale, off, D, 1); }
	x64_xxx(x64_reg32 reg, x64_reg_ptr64 addr, x64_reg64 index, sib_scale scale, int8_t off) { reg_reg_ptr_idx_off(reg, addr, index, scale, off, D, 1); }
	x64_xxx(x64_reg16 reg, x64_reg_ptr64 addr, x64_reg64 index, sib_scale scale, int8_t off) { reg_reg_ptr_idx_off(reg, addr, index, scale, off, D, 1); }
	x64_xxx(x64_reg8l reg, x64_reg_ptr64 addr, x64_reg64 index, sib_scale scale, int8_t off) { reg_reg_ptr_idx_off(reg, addr, index, scale, off, C, 1); }

	/* 32 bit base address + index * scale + 8 bit offset  into reg */
	x64_xxx(x64_reg64 reg, x64_reg_ptr32 addr, x64_reg32 index, sib_scale scale, int8_t off) { reg_reg_ptr_idx_off(reg, addr, index, scale, off, D, 1); }
	x64_xxx(x64_reg32 reg, x64_reg_ptr32 addr, x64_reg32 index, sib_scale scale, int8_t off) { reg_reg_ptr_idx_off(reg, addr, index, scale, off, D, 1); }
	x64_xxx(x64_reg16 reg, x64_reg_ptr32 addr, x64_reg32 index, sib_scale scale, int8_t off) { reg_reg_ptr_idx_off(reg, addr, index, scale, off, D, 1); }
	x64_xxx(x64_reg8l reg, x64_reg_ptr32 addr, x64_reg32 index, sib_scale scale, int8_t off) { reg_reg_ptr_idx_off(reg, addr, index, scale, off, C, 1); }

	/* reg into 64 bit base address + index + 8 bit offset  * scale */
	x64_xxx(x64_reg_ptr64 addr, x64_reg64 index, sib_scale scale, int8_t off, x64_reg64 reg) { reg_reg_ptr_idx_off(reg, addr, index, scale, off, B, 1); }
	x64_xxx(x64_reg_ptr64 addr, x64_reg64 index, sib_scale scale, int8_t off, x64_reg32 reg) { reg_reg_ptr_idx_off(reg, addr, index, scale, off, B, 1); }
	x64_xxx(x64_reg_ptr64 addr, x64_reg64 index, sib_scale scale, int8_t off, x64_reg16 reg) { reg_reg_ptr_idx_off(reg, addr, index, scale, off, B, 1); }
	x64_xxx(x64_reg_ptr64 addr, x64_reg64 index, sib_scale scale, int8_t off, x64_reg8l reg) { reg_reg_ptr_idx_off(reg, addr, index, scale, off, A, 1); }

	/* reg into 32 bit base address + index + 8 bit offset  * scale */
	x64_xxx(x64_reg_ptr32 addr, x64_reg32 index, sib_scale scale, int8_t off, x64_reg64 reg) { reg_reg_ptr_idx_off(reg, addr, index, scale, off, B, 1); }
	x64_xxx(x64_reg_ptr32 addr, x64_reg32 index, sib_scale scale, int8_t off, x64_reg32 reg) { reg_reg_ptr_idx_off(reg, addr, index, scale, off, B, 1); }
	x64_xxx(x64_reg_ptr32 addr, x64_reg32 index, sib_scale scale, int8_t off, x64_reg16 reg) { reg_reg_ptr_idx_off(reg, addr, index, scale, off, B, 1); }
	x64_xxx(x64_reg_ptr32 addr, x64_reg32 index, sib_scale scale, int8_t off, x64_reg8l reg) { reg_reg_ptr_idx_off(reg, addr, index, scale, off, A, 1); }


	/* 64 bit base address + index * scale + 32 bit offset into reg */
	x64_xxx(x64_reg64 reg, x64_reg_ptr64 addr, x64_reg64 index, sib_scale scale, int32_t off) { reg_reg_ptr_idx_off(reg, addr, index, scale, off, D, 2); }
	x64_xxx(x64_reg32 reg, x64_reg_ptr64 addr, x64_reg64 index, sib_scale scale, int32_t off) { reg_reg_ptr_idx_off(reg, addr, index, scale, off, D, 2); }
	x64_xxx(x64_reg16 reg, x64_reg_ptr64 addr, x64_reg64 index, sib_scale scale, int32_t off) { reg_reg_ptr_idx_off(reg, addr, index, scale, off, D, 2); }
	x64_xxx(x64_reg8l reg, x64_reg_ptr64 addr, x64_reg64 index, sib_scale scale, int32_t off) { reg_reg_ptr_idx_off(reg, addr, index, scale, off, C, 2); }

	/* 32 bit base address + index * scale + 32 bit offset  into reg */
	x64_xxx(x64_reg64 reg, x64_reg_ptr32 addr, x64_reg32 index, sib_scale scale, int32_t off) { reg_reg_ptr_idx_off(reg, addr, index, scale, off, D, 2); }
	x64_xxx(x64_reg32 reg, x64_reg_ptr32 addr, x64_reg32 index, sib_scale scale, int32_t off) { reg_reg_ptr_idx_off(reg, addr, index, scale, off, D, 2); }
	x64_xxx(x64_reg16 reg, x64_reg_ptr32 addr, x64_reg32 index, sib_scale scale, int32_t off) { reg_reg_ptr_idx_off(reg, addr, index, scale, off, D, 2); }
	x64_xxx(x64_reg8l reg, x64_reg_ptr32 addr, x64_reg32 index, sib_scale scale, int32_t off) { reg_reg_ptr_idx_off(reg, addr, index, scale, off, C, 2); }

	/* reg into 64 bit base address + index + 32 bit offset  * scale */
	x64_xxx(x64_reg_ptr64 addr, x64_reg64 index, sib_scale scale, int32_t off, x64_reg64 reg) { reg_reg_ptr_idx_off(reg, addr, index, scale, off, B, 2); }
	x64_xxx(x64_reg_ptr64 addr, x64_reg64 index, sib_scale scale, int32_t off, x64_reg32 reg) { reg_reg_ptr_idx_off(reg, addr, index, scale, off, B, 2); }
	x64_xxx(x64_reg_ptr64 addr, x64_reg64 index, sib_scale scale, int32_t off, x64_reg16 reg) { reg_reg_ptr_idx_off(reg, addr, index, scale, off, B, 2); }
	x64_xxx(x64_reg_ptr64 addr, x64_reg64 index, sib_scale scale, int32_t off, x64_reg8l reg) { reg_reg_ptr_idx_off(reg, addr, index, scale, off, A, 2); }

	/* reg into 32 bit base address + index + 32 bit offset  * scale */
	x64_xxx(x64_reg_ptr32 addr, x64_reg32 index, sib_scale scale, int32_t off, x64_reg64 reg) { reg_reg_ptr_idx_off(reg, addr, index, scale, off, B, 2); }
	x64_xxx(x64_reg_ptr32 addr, x64_reg32 index, sib_scale scale, int32_t off, x64_reg32 reg) { reg_reg_ptr_idx_off(reg, addr, index, scale, off, B, 2); }
	x64_xxx(x64_reg_ptr32 addr, x64_reg32 index, sib_scale scale, int32_t off, x64_reg16 reg) { reg_reg_ptr_idx_off(reg, addr, index, scale, off, B, 2); }
	x64_xxx(x64_reg_ptr32 addr, x64_reg32 index, sib_scale scale, int32_t off, x64_reg8l reg) { reg_reg_ptr_idx_off(reg, addr, index, scale, off, A, 2); }
};

class x64_mov : public x64_xxx<0x88, 0x89, 0x8a, 0x8b>
{
public:
	using x64_xxx::x64_xxx;

	/* Move immediate into register */
	x64_mov(x64_reg64 reg, uint64_t imm) { orred_oc_reg_imm(reg, imm, 0xb8); }
	x64_mov(x64_reg32 reg, uint32_t imm) { orred_oc_reg_imm(reg, imm, 0xb8); }
	x64_mov(x64_reg16 reg, uint16_t imm) { orred_oc_reg_imm(reg, imm, 0xb8); }
	x64_mov(x64_reg8  reg, uint8_t  imm) { orred_oc_reg_imm(reg, imm, 0xb0); }
	x64_mov(x64_reg64 reg, uint32_t imm)
	{
		add_prefixes(reg, x64_reg64(0));
		add_opcode(0xc7);
		set_modrm(x64_modrm{reg, 0, 3});
		set_imm(imm);
	}

	/* Move immediate address into register */
	/* 64 bit pointers */
	x64_mov(x64_reg64_0 reg, x64_addr_ptr<uint64_t*> addr) { eax_imm(reg, reinterpret_cast<uint64_t>(addr.ptr), 0xa1); }
	x64_mov(x64_reg32_0 reg, x64_addr_ptr<uint32_t*> addr) { eax_imm(reg, reinterpret_cast<uint64_t>(addr.ptr), 0xa1); }
	x64_mov(x64_reg16_0 reg, x64_addr_ptr<uint16_t*> addr) { eax_imm(reg, reinterpret_cast<uint64_t>(addr.ptr), 0xa1); }
	x64_mov(x64_reg8l_0 reg, x64_addr_ptr<uint8_t*>  addr) { eax_imm(reg, reinterpret_cast<uint64_t>(addr.ptr), 0xa0); }

	/* Move register into immediate address */
	/* 32 bit pointers */
	/* 64 bit pointers */
	x64_mov(x64_addr_ptr<uint64_t*> addr, x64_reg64_0 reg) { eax_imm(reg, reinterpret_cast<uint64_t>(addr.ptr), 0xa3); }
	x64_mov(x64_addr_ptr<uint32_t*> addr, x64_reg32_0 reg) { eax_imm(reg, reinterpret_cast<uint64_t>(addr.ptr), 0xa3); }
	x64_mov(x64_addr_ptr<uint16_t*> addr, x64_reg16_0 reg) { eax_imm(reg, reinterpret_cast<uint64_t>(addr.ptr), 0xa3); }
	x64_mov(x64_addr_ptr<uint8_t*>  addr, x64_reg8l_0 reg) { eax_imm(reg, reinterpret_cast<uint64_t>(addr.ptr), 0xa2); }

	virtual ~x64_mov() { }
};

template<uint8_t A, uint8_t B, uint8_t C, uint8_t D, uint8_t E, uint8_t F, uint8_t MOD>
class x64_arith_base : public x64_xxx<A, B, C, D>
{
public:
	using x64_xxx<A, B, C, D>::x64_xxx;

	/* Add immediate into register */
	x64_arith_base(x64_reg64 reg, uint8_t imm) { reg_imm(reg, imm, 0x83); }
	x64_arith_base(x64_reg64 reg, uint32_t imm) { reg_imm_oroc(reg, imm, F, 0x81); }

	x64_arith_base(x64_reg32 reg, uint8_t imm) { reg_imm(reg, imm, 0x83); }
	x64_arith_base(x64_reg32 reg, uint32_t imm) { reg_imm_oroc(reg, imm, F, 0x81); }

	x64_arith_base(x64_reg16 reg, uint8_t imm) { reg_imm(reg, imm, 0x83); }
	x64_arith_base(x64_reg16 reg, uint16_t imm) { reg_imm_oroc(reg, imm, F, 0x81); }

	x64_arith_base(x64_reg8  reg, uint8_t  imm) { reg_imm_oroc(reg, imm, E, 0x80); }

	virtual ~x64_arith_base() { }

private:
	template<typename T, typename U>
	void reg_imm(T a, U imm, uint8_t oc)
	{
		this->template add_prefixes(a, T(0));
		this->template add_opcode(oc);
		this->template set_modrm(x64_modrm(a, MOD, 3));
		this->template set_imm(imm);
	}

	template<typename T, typename U>
	void reg_imm_oroc(T a, U imm, uint8_t oc1, uint8_t oc2)
	{
		if (a.value != 0)
		{
			reg_imm(a, imm, oc2);
			return;
		}

		this->template add_prefixes(a, T(0));
		this->template add_opcode(oc1);
		this->template set_imm(imm);
	}
};

class x64_add: public x64_arith_base<0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0>
{ using x64_arith_base::x64_arith_base; };

class x64_or: public x64_arith_base<0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 1>
{ using x64_arith_base::x64_arith_base; };

class x64_adc: public x64_arith_base<0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 2>
{ using x64_arith_base::x64_arith_base; };

class x64_sbb: public x64_arith_base<0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 3>
{ using x64_arith_base::x64_arith_base; };

class x64_and: public x64_arith_base<0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 4>
{ using x64_arith_base::x64_arith_base; };

class x64_sub: public x64_arith_base<0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 5>
{ using x64_arith_base::x64_arith_base; };

class x64_xor: public x64_arith_base<0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 6>
{ using x64_arith_base::x64_arith_base; };

class x64_cmp: public x64_arith_base<0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 7>
{ using x64_arith_base::x64_arith_base; };

#endif /* X64INSTRUCTION_H_ */
