/*
 * x64instruction.h
 *
 *  Created on: Dec 19, 2019
 *      Author: tom
 */

#ifndef X64INSTRUCTION_H_
#define X64INSTRUCTION_H_

#include <cstdint>
#include <string>
#include <stdexcept>
#include <initializer_list>

#include "../instruction.h"

struct x64_reg_base
{
	inline constexpr x64_reg_base() : value(0) { }
	inline constexpr x64_reg_base(const x64_reg_base& reg) : value(reg.value) { }
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

	inline constexpr bool add_rex() { return false; }
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

struct x64_reg_base8l : public x64_reg_base8
{
	using x64_reg_base8::x64_reg_base8;
	inline constexpr bool add_rex() { return value >= 4; }
};

struct x64_reg_base8h : public x64_reg_base8
{
	using x64_reg_base8::x64_reg_base8;
	inline constexpr bool add_rex() { return false; }
};

struct x64_reg64    : public x64_reg_base_normal
{
	using x64_reg_base_normal::x64_reg_base_normal;
	using value_type = uint64_t;
	constexpr static int n = 16;
	static const std::string names[n];
};
struct x64_reg64_0 : public x64_reg64 { using x64_reg64::x64_reg64; };

struct x64_reg32   : public x64_reg_base_normal
{
	using x64_reg_base_normal::x64_reg_base_normal;
	using value_type = uint32_t;
	constexpr static int n = 16;
	static const std::string names[n];
};
struct x64_reg32_0 : public x64_reg32 { using x64_reg32::x64_reg32; };

struct x64_reg16   : public x64_reg_base_normal
{
	using x64_reg_base_normal::x64_reg_base_normal;
	using value_type = uint16_t;
	constexpr static int n = 16;
	static const std::string names[n];
};
struct x64_reg16_0 : public x64_reg16 { using x64_reg16::x64_reg16; };

struct x64_reg8   : public x64_reg_base8l
{
	using x64_reg_base8l::x64_reg_base8l;
	using value_type = uint8_t;
	constexpr static int n = 16;
	static const std::string names[n];
};
struct x64_reg8_0 : public x64_reg8 { using x64_reg8::x64_reg8; };

struct x64_reg8h   : public x64_reg_base8h
{
	using x64_reg_base8h::x64_reg_base8h;
	using value_type = uint8_t;
	constexpr static int n = 4;
	static const std::string names[n];

	// XXX: This is a bit of a hack to make
	inline x64_reg8h shift() const { return x64_reg8h(value + 4); }

	inline constexpr x64_reg8h(const x64_reg8& reg) : x64_reg8h(reg.value) { }
};


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

	constexpr static x64_reg32 r8d = 8;
	constexpr static x64_reg32 r9d = 9;
	constexpr static x64_reg32 r10d = 10;
	constexpr static x64_reg32 r11d = 11;
	constexpr static x64_reg32 r12d = 12;
	constexpr static x64_reg32 r13d = 13;
	constexpr static x64_reg32 r14d = 14;
	constexpr static x64_reg32 r15d = 15;

	/* 16 bit registers */
	constexpr static x64_reg16_0 ax = 0;
	constexpr static x64_reg16 cx = 1;
	constexpr static x64_reg16 dx = 2;
	constexpr static x64_reg16 bx = 3;
	constexpr static x64_reg16 sp = 4;
	constexpr static x64_reg16 bp = 5;
	constexpr static x64_reg16 si = 6;
	constexpr static x64_reg16 di = 7;

	constexpr static x64_reg16 r8w = 8;
	constexpr static x64_reg16 r9w = 9;
	constexpr static x64_reg16 r10w = 10;
	constexpr static x64_reg16 r11w = 11;
	constexpr static x64_reg16 r12w = 12;
	constexpr static x64_reg16 r13w = 13;
	constexpr static x64_reg16 r14w = 14;
	constexpr static x64_reg16 r15w = 15;

	/* lower 8 bit registers */
	constexpr static x64_reg8_0 al = 0;
	constexpr static x64_reg8 cl = 1;
	constexpr static x64_reg8 dl = 2;
	constexpr static x64_reg8 bl = 3;
	constexpr static x64_reg8 spl = 4;
	constexpr static x64_reg8 bpl = 5;
	constexpr static x64_reg8 sil = 6;
	constexpr static x64_reg8 dil = 7;

	constexpr static x64_reg8 r8b = 8;
	constexpr static x64_reg8 r9b = 9;
	constexpr static x64_reg8 r10b = 10;
	constexpr static x64_reg8 r11b = 11;
	constexpr static x64_reg8 r12b = 12;
	constexpr static x64_reg8 r13b = 13;
	constexpr static x64_reg8 r14b = 14;
	constexpr static x64_reg8 r15b = 15;

	/* upper 8 bit registers */
	constexpr static x64_reg8h ah = 0;
	constexpr static x64_reg8h ch = 1;
	constexpr static x64_reg8h dh = 2;
	constexpr static x64_reg8h bh = 3;
};

enum class x64_sib_scale
{
	s1 = 0,
	s2,
	s4,
	s8
};

enum x64_rex : uint8_t
{
	b = 0x41,
	x = 0x42,
	r = 0x44,
	w = 0x48,
	rex = 0x40,
};

enum x64_override : uint8_t
{
	oper_size = 0x66,
	addr_size = 0x67,
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

struct x64_sib
{
	unsigned int base:3;
	unsigned int index:3;
	unsigned int scale:2;

	x64_sib() : base(0), index(0), scale(0) { }

	x64_sib(x64_reg_base base, x64_reg_base index, x64_sib_scale scale)
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

struct x64_reg_ptr64    : public x64_addr_ptr<x64_reg64>
{
	using x64_addr_ptr::x64_addr_ptr;
	using reg_type = x64_reg64;
};

static constexpr x64_reg_ptr32  x64_reg_addr(const x64_reg32& reg) { return x64_reg_ptr32(reg); }
static constexpr x64_reg_ptr64  x64_reg_addr(const x64_reg64& reg) { return x64_reg_ptr64(reg); }

class x64_address
{
public:
	x64_address(x64_addr_ptr<int32_t> addr) : address_size(sizeof(addr.ptr)), imm_addr(addr.ptr) { }

	x64_address(x64_reg_ptr64 addr) : addr_reg(addr.ptr.value), address_size(sizeof(addr.ptr)) { }
	x64_address(x64_reg_ptr32 addr) : addr_reg(addr.ptr.value), address_size(sizeof(addr.ptr)) { }

	x64_address(x64_reg_ptr64 addr, int8_t  offset) : addr_reg(addr.ptr.value), address_size(sizeof(addr.ptr)), offset(offset), offset_size(sizeof(offset)) { }
	x64_address(x64_reg_ptr32 addr, int8_t  offset) : addr_reg(addr.ptr.value), address_size(sizeof(addr.ptr)), offset(offset), offset_size(sizeof(offset)) { }
	x64_address(x64_reg_ptr64 addr, int32_t offset) : addr_reg(addr.ptr.value), address_size(sizeof(addr.ptr)), offset(offset), offset_size(sizeof(offset)) { }
	x64_address(x64_reg_ptr32 addr, int32_t offset) : addr_reg(addr.ptr.value), address_size(sizeof(addr.ptr)), offset(offset), offset_size(sizeof(offset)) { }

	x64_address(x64_reg_ptr64 addr, x64_reg64 index, x64_sib_scale scale)                 : addr_reg(addr.ptr.value), address_size(sizeof(addr.ptr)), index_reg(index.value), scale(scale) { }
	x64_address(x64_reg_ptr32 addr, x64_reg32 index, x64_sib_scale scale)                 : addr_reg(addr.ptr.value), address_size(sizeof(addr.ptr)), index_reg(index.value), scale(scale) { }
	x64_address(x64_reg_ptr64 addr, x64_reg64 index, x64_sib_scale scale, int8_t  offset) : addr_reg(addr.ptr.value), address_size(sizeof(addr.ptr)), index_reg(index.value), scale(scale), offset(offset), offset_size(sizeof(offset)) { }
	x64_address(x64_reg_ptr32 addr, x64_reg32 index, x64_sib_scale scale, int8_t  offset) : addr_reg(addr.ptr.value), address_size(sizeof(addr.ptr)), index_reg(index.value), scale(scale), offset(offset), offset_size(sizeof(offset)) { }
	x64_address(x64_reg_ptr64 addr, x64_reg64 index, x64_sib_scale scale, int32_t offset) : addr_reg(addr.ptr.value), address_size(sizeof(addr.ptr)), index_reg(index.value), scale(scale), offset(offset), offset_size(sizeof(offset)) { }
	x64_address(x64_reg_ptr32 addr, x64_reg32 index, x64_sib_scale scale, int32_t offset) : addr_reg(addr.ptr.value), address_size(sizeof(addr.ptr)), index_reg(index.value), scale(scale), offset(offset), offset_size(sizeof(offset)) { }

	inline bool has_index() { return index_reg != -1; }
	//inline

private:
	int addr_reg = 0;
	int address_size;
	int index_reg = -1;
	x64_sib_scale scale = x64_sib_scale::s1;
	int32_t offset = 0;
	int offset_size = 0;
	int32_t imm_addr = 0;
};


enum class x64_cond
{
	overflow				= 0x0,
	not_overflow			= 0x1,
	below					= 0x2,
	not_above_or_equal		= 0x2,
	carry					= 0x2,
	not_below				= 0x3,
	above_or_equal			= 0x3,
	not_carry				= 0x3,
	equal					= 0x4,
	zero					= 0x4,
	not_equal				= 0x5,
	not_zero				= 0x5,
	below_or_equal			= 0x6,
	not_above				= 0x6,
	above					= 0x7,
	not_below_or_equal		= 0x7,
	sign					= 0x8,
	not_sign				= 0x9,
	parity					= 0xa,
	parity_even				= 0xa,
	not_parity				= 0xb,
	parity_odd				= 0xb,
	less					= 0xc,
	not_greater_or_equal	= 0xc,
	greater_or_equal		= 0xd,
	not_less				= 0xd,
	less_or_equal			= 0xe,
	not_greater				= 0xe,
	greater					= 0xf,
	not_less_or_equal		= 0xf,
};

class no_rex_exception: public std::exception
{
public:
	explicit no_rex_exception(x64_reg8h reg)
	{
		msg = "can't encode register '";
		msg += x64_reg8h::names[reg.value];
		msg += "' in an instruction requiring REX prefix.";
	}

	virtual const char* what() const throw()
	{
		return msg.c_str();;
	}

private:
	std::string msg;
};

class x64_instruction : public instruction
{
public:
	x64_instruction() { }

	template<typename IMM_TYPE>
	x64_instruction(
			std::initializer_list<uint8_t> opcode,
			const struct x64_modrm& m,
			const struct x64_sib& s,
			IMM_TYPE imm)
	 : has_modrm(true), modrm(m), has_sib(true), sib(s),
	   imm_size(sizeof(IMM_TYPE)), imm64(static_cast<uint64_t>(from_or_to_little(imm)))
	{ add_opcode(opcode); }

	x64_instruction(
			std::initializer_list<uint8_t> opcode,
			const struct x64_modrm m,
			const struct x64_sib s)
	 : has_modrm(true), modrm(m), has_sib(true), sib(s)
	{ add_opcode(opcode); }

	template<typename IMM_TYPE>
	x64_instruction(
			std::initializer_list<uint8_t> opcode,
			const struct x64_modrm m,
			IMM_TYPE imm)
	 : has_modrm(true), modrm(m),
	   imm_size(sizeof(IMM_TYPE)), imm64(static_cast<uint64_t>(from_or_to_little(imm)))
	{ add_opcode(opcode); }

	x64_instruction(
			std::initializer_list<uint8_t> opcode,
			const struct x64_modrm m)
	 : has_modrm(true), modrm(m)
	{ add_opcode(opcode); }

	template<typename IMM_TYPE>
	x64_instruction(
			std::initializer_list<uint8_t> opcode,
			const struct x64_sib s,
			IMM_TYPE imm)
	 : has_modrm(true), has_sib(true), sib(s),
	   imm_size(sizeof(IMM_TYPE)), imm64(static_cast<uint64_t>(from_or_to_little(imm)))
	{ add_opcode(opcode); }

	x64_instruction(
			std::initializer_list<uint8_t> opcode,
			const struct x64_sib s)
	 : has_modrm(true), has_sib(true), sib(s)
	{ add_opcode(opcode); }

	template<typename IMM_TYPE>
	x64_instruction(
			std::initializer_list<uint8_t> opcode,
			IMM_TYPE imm)
	 : imm_size(sizeof(IMM_TYPE)), imm64(static_cast<uint64_t>(from_or_to_little(imm)))
	{ add_opcode(opcode); }

	x64_instruction(std::initializer_list<uint8_t> opcode)
	{ add_opcode(opcode); }

	/* The same as above, but without an opcode parameter in order to add it dynamically later */
	template<typename IMM_TYPE>
	x64_instruction(
			const struct x64_modrm& m,
			const struct x64_sib& s,
			IMM_TYPE imm)
	 : has_modrm(true), modrm(m), has_sib(true), sib(s),
	   imm_size(sizeof(IMM_TYPE)), imm64(static_cast<uint64_t>(from_or_to_little(imm)))
	{ }

	x64_instruction(
			const struct x64_modrm m,
			const struct x64_sib s)
	 : has_modrm(true), modrm(m), has_sib(true), sib(s)
	{ }

	template<typename IMM_TYPE>
	x64_instruction(
			const struct x64_modrm m,
			IMM_TYPE imm)
	 : has_modrm(true), modrm(m),
	   imm_size(sizeof(IMM_TYPE)), imm64(static_cast<uint64_t>(from_or_to_little(imm)))
	{ }

	x64_instruction(const struct x64_modrm m)
	 : has_modrm(true), modrm(m)
	{ }

	template<typename IMM_TYPE>
	x64_instruction(
			const struct x64_sib s,
			IMM_TYPE imm)
	 : has_modrm(true), has_sib(true), sib(s),
	   imm_size(sizeof(IMM_TYPE)), imm64(static_cast<uint64_t>(from_or_to_little(imm)))
	{ }

	x64_instruction(const struct x64_sib s)
	 : has_modrm(true), has_sib(true), sib(s)
	{ }

	template<typename IMM_TYPE>
	x64_instruction(IMM_TYPE imm)
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

	inline void add_opcode(std::initializer_list<uint8_t> oc)
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

		/* For sil, dil, bps and spl 8-bit registers on x64 */
		if (a.add_rex() || b.add_rex() || c.add_rex())
			rex |= x64_rex::rex;

		if (rex)
			add_opcode(rex);
	}
	template<typename T, typename U>
	inline void x64_add_rex(const T& a, const U& b, size_t oper_size = sizeof(int32_t))
	{ x64_add_rex(a, T(0), b, oper_size); }

	template<typename T>
	void single_regptr(const x64_addr_ptr<T>& reg, uint8_t b, std::initializer_list<uint8_t> oc, uint8_t mod)
	{
		x64_add_rex(reg.ptr, x64_reg32(0));
		reg_reg_oc_mod(reg.ptr, x64_reg32(b), oc, mod);
	}

	template<typename T>
	void single_reg(const T& reg, uint8_t b, std::initializer_list<uint8_t> oc, uint8_t mod)
	{
		add_prefixes(reg, T(0));
		reg_reg_oc_mod(reg, x64_reg32(b), oc, mod);
	}

	template<typename T, typename U>
	void reg_reg_ptr(const T& a, const x64_addr_ptr<U>& b, std::initializer_list<uint8_t> oc)
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
	void reg_reg_ptr_off(const T& a, const x64_addr_ptr<U>& b, std::initializer_list<uint8_t> oc, int mod, W imm)
	{
		add_prefixes(a, b);
		reg_reg_oc_mod(b.ptr, a, oc, mod);

		if (b.ptr.is_sp() || b.ptr.is_r12())
			set_sib(0x24);

		set_imm(imm);
	}

	template<typename T>
	void reg_reg(T a, T b, std::initializer_list<uint8_t> oc)
	{
		add_prefixes(a, b);
		reg_reg_oc_mod(a, b, oc, 3);
	}

	template<typename T, typename U>
	void reg_reg_ptr_idx(const T& a, const x64_addr_ptr<U>& b, const U& c, x64_sib_scale scale, std::initializer_list<uint8_t> oc)
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
	void reg_reg_ptr_idx_off(const T& a, const x64_addr_ptr<U>& b, const U& c, x64_sib_scale scale, V off, std::initializer_list<uint8_t> oc, int mod)
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
	void reg_reg_oc_mod(T a, U b, std::initializer_list<uint8_t> oc, int mod)
	{
		add_opcode(oc);
		set_modrm(x64_modrm{a, b, mod});
	}

	template<typename T, typename U>
	void orred_oc_reg_imm(T reg, U imm, std::initializer_list<uint8_t> oc)
	{
		add_prefixes(reg, T(0));
		add_opcode(oc);

		/* or with the last entry in the list */
		opcode[opcode_size - 1] |= reg.value & 0x7;

		set_imm(imm);
	}

	template<typename T, typename U>
	void eax_imm(T reg, U imm, std::initializer_list<uint8_t> oc)
	{
		add_prefixes(reg, T(0));
		add_opcode(oc);
		set_imm(imm);
	}

	template<typename T, typename U>
	void reg_imm_addr(T reg, x64_addr_ptr<U> addr, std::initializer_list<uint8_t> oc)
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

template<uint8_t A, uint8_t B, uint8_t C, uint8_t D, uint8_t... Prefixes>
class x64_src_regdst_oper_base : public x64_instruction
{
public:
	using x64_instruction::x64_instruction;

	/* register into register */
	x64_src_regdst_oper_base(x64_reg64 dst, x64_reg64 src) { reg_reg(dst, src, {std::forward<uint8_t>(Prefixes)..., B}); }
	x64_src_regdst_oper_base(x64_reg32 dst, x64_reg32 src) { reg_reg(dst, src, {std::forward<uint8_t>(Prefixes)..., B}); }
	x64_src_regdst_oper_base(x64_reg16 dst, x64_reg16 src) { reg_reg(dst, src, {std::forward<uint8_t>(Prefixes)..., B}); }
	x64_src_regdst_oper_base(x64_reg8  dst, x64_reg8  src) { reg_reg(dst, src, {std::forward<uint8_t>(Prefixes)..., A}); }
	x64_src_regdst_oper_base(x64_reg8h dst, x64_reg8  src) { if (src.add_rex()) throw no_rex_exception(dst) ; reg_reg(dst.shift(), x64_reg8h(src), {std::forward<uint8_t>(Prefixes)..., A}); }
	x64_src_regdst_oper_base(x64_reg8  dst, x64_reg8h src) { if (dst.add_rex()) throw no_rex_exception(src) ; reg_reg(x64_reg8h(dst), src.shift(), {std::forward<uint8_t>(Prefixes)..., A}); }
	x64_src_regdst_oper_base(x64_reg8h dst, x64_reg8h src) { reg_reg(dst.shift(), src.shift(), {std::forward<uint8_t>(Prefixes)..., A}); }

	/* register pointer into register */
	x64_src_regdst_oper_base(x64_reg64 reg, x64_reg_ptr64 addr) { reg_reg_ptr(reg, addr, {std::forward<uint8_t>(Prefixes)..., D}); }
	x64_src_regdst_oper_base(x64_reg32 reg, x64_reg_ptr64 addr) { reg_reg_ptr(reg, addr, {std::forward<uint8_t>(Prefixes)..., D}); }
	x64_src_regdst_oper_base(x64_reg16 reg, x64_reg_ptr64 addr) { reg_reg_ptr(reg, addr, {std::forward<uint8_t>(Prefixes)..., D}); }
	x64_src_regdst_oper_base(x64_reg8  reg, x64_reg_ptr64 addr) { reg_reg_ptr(reg, addr, {std::forward<uint8_t>(Prefixes)..., C}); }
	x64_src_regdst_oper_base(x64_reg8h reg, x64_reg_ptr64 addr) { reg_reg_ptr(reg.shift(), addr, {std::forward<uint8_t>(Prefixes)..., C}); }

	x64_src_regdst_oper_base(x64_reg64 reg, x64_reg_ptr32 addr) { reg_reg_ptr(reg, addr, {std::forward<uint8_t>(Prefixes)..., D}); }
	x64_src_regdst_oper_base(x64_reg32 reg, x64_reg_ptr32 addr) { reg_reg_ptr(reg, addr, {std::forward<uint8_t>(Prefixes)..., D}); }
	x64_src_regdst_oper_base(x64_reg16 reg, x64_reg_ptr32 addr) { reg_reg_ptr(reg, addr, {std::forward<uint8_t>(Prefixes)..., D}); }
	x64_src_regdst_oper_base(x64_reg8  reg, x64_reg_ptr32 addr) { reg_reg_ptr(reg, addr, {std::forward<uint8_t>(Prefixes)..., C}); }
	x64_src_regdst_oper_base(x64_reg8h reg, x64_reg_ptr32 addr) { reg_reg_ptr(reg.shift(), addr, {std::forward<uint8_t>(Prefixes)..., C}); }

	/* register pointer + 8 bit offset into register */
	x64_src_regdst_oper_base(x64_reg64 reg, x64_reg_ptr64 addr, int8_t off) { reg_reg_ptr_off(reg, addr, {std::forward<uint8_t>(Prefixes)..., D}, 1, off); }
	x64_src_regdst_oper_base(x64_reg32 reg, x64_reg_ptr64 addr, int8_t off) { reg_reg_ptr_off(reg, addr, {std::forward<uint8_t>(Prefixes)..., D}, 1, off); }
	x64_src_regdst_oper_base(x64_reg16 reg, x64_reg_ptr64 addr, int8_t off) { reg_reg_ptr_off(reg, addr, {std::forward<uint8_t>(Prefixes)..., D}, 1, off); }
	x64_src_regdst_oper_base(x64_reg8  reg, x64_reg_ptr64 addr, int8_t off) { reg_reg_ptr_off(reg, addr, {std::forward<uint8_t>(Prefixes)..., C}, 1, off); }
	x64_src_regdst_oper_base(x64_reg8h reg, x64_reg_ptr64 addr, int8_t off) { reg_reg_ptr_off(reg.shift(), addr, {std::forward<uint8_t>(Prefixes)..., C}, 1, off); }

	x64_src_regdst_oper_base(x64_reg64 reg, x64_reg_ptr32 addr, int8_t off) { reg_reg_ptr_off(reg, addr, {std::forward<uint8_t>(Prefixes)..., D}, 1, off); }
	x64_src_regdst_oper_base(x64_reg32 reg, x64_reg_ptr32 addr, int8_t off) { reg_reg_ptr_off(reg, addr, {std::forward<uint8_t>(Prefixes)..., D}, 1, off); }
	x64_src_regdst_oper_base(x64_reg16 reg, x64_reg_ptr32 addr, int8_t off) { reg_reg_ptr_off(reg, addr, {std::forward<uint8_t>(Prefixes)..., D}, 1, off); }
	x64_src_regdst_oper_base(x64_reg8  reg, x64_reg_ptr32 addr, int8_t off) { reg_reg_ptr_off(reg, addr, {std::forward<uint8_t>(Prefixes)..., C}, 1, off); }
	x64_src_regdst_oper_base(x64_reg8h reg, x64_reg_ptr32 addr, int8_t off) { reg_reg_ptr_off(reg.shift(), addr, {std::forward<uint8_t>(Prefixes)..., C}, 1, off); }

	/* register pointer + 32 bit offset into register */
	x64_src_regdst_oper_base(x64_reg64 reg, x64_reg_ptr64 addr, int32_t off) { reg_reg_ptr_off(reg, addr, {std::forward<uint8_t>(Prefixes)..., D}, 2, off); }
	x64_src_regdst_oper_base(x64_reg32 reg, x64_reg_ptr64 addr, int32_t off) { reg_reg_ptr_off(reg, addr, {std::forward<uint8_t>(Prefixes)..., D}, 2, off); }
	x64_src_regdst_oper_base(x64_reg16 reg, x64_reg_ptr64 addr, int32_t off) { reg_reg_ptr_off(reg, addr, {std::forward<uint8_t>(Prefixes)..., D}, 2, off); }
	x64_src_regdst_oper_base(x64_reg8  reg, x64_reg_ptr64 addr, int32_t off) { reg_reg_ptr_off(reg, addr, {std::forward<uint8_t>(Prefixes)..., C}, 2, off); }
	x64_src_regdst_oper_base(x64_reg8h reg, x64_reg_ptr64 addr, int32_t off) { reg_reg_ptr_off(reg.shift(), addr, {std::forward<uint8_t>(Prefixes)..., C}, 2, off); }

	x64_src_regdst_oper_base(x64_reg64 reg, x64_reg_ptr32 addr, int32_t off) { reg_reg_ptr_off(reg, addr, {std::forward<uint8_t>(Prefixes)..., D}, 2, off); }
	x64_src_regdst_oper_base(x64_reg32 reg, x64_reg_ptr32 addr, int32_t off) { reg_reg_ptr_off(reg, addr, {std::forward<uint8_t>(Prefixes)..., D}, 2, off); }
	x64_src_regdst_oper_base(x64_reg16 reg, x64_reg_ptr32 addr, int32_t off) { reg_reg_ptr_off(reg, addr, {std::forward<uint8_t>(Prefixes)..., D}, 2, off); }
	x64_src_regdst_oper_base(x64_reg8  reg, x64_reg_ptr32 addr, int32_t off) { reg_reg_ptr_off(reg, addr, {std::forward<uint8_t>(Prefixes)..., C}, 2, off); }
	x64_src_regdst_oper_base(x64_reg8h reg, x64_reg_ptr32 addr, int32_t off) { reg_reg_ptr_off(reg.shift(), addr, {std::forward<uint8_t>(Prefixes)..., C}, 2, off); }

	/* immediate address into register */
	/* 32 bit pointers */
	x64_src_regdst_oper_base(x64_reg64 reg, x64_addr_ptr<int32_t> addr) { reg_imm_addr(reg, addr, {std::forward<uint8_t>(Prefixes)..., D}); }
	x64_src_regdst_oper_base(x64_reg32 reg, x64_addr_ptr<int32_t> addr) { reg_imm_addr(reg, addr, {std::forward<uint8_t>(Prefixes)..., D}); }
	x64_src_regdst_oper_base(x64_reg16 reg, x64_addr_ptr<int32_t> addr) { reg_imm_addr(reg, addr, {std::forward<uint8_t>(Prefixes)..., D}); }
	x64_src_regdst_oper_base(x64_reg8  reg, x64_addr_ptr<int32_t> addr) { reg_imm_addr(reg, addr, {std::forward<uint8_t>(Prefixes)..., C}); }
	x64_src_regdst_oper_base(x64_reg8h reg, x64_addr_ptr<int32_t> addr) { reg_imm_addr(reg.shift(), addr, {std::forward<uint8_t>(Prefixes)..., C}); }

	/* 64 bit base address + index * scale into reg */
	x64_src_regdst_oper_base(x64_reg64 reg, x64_reg_ptr64 addr, x64_reg64 index, x64_sib_scale scale) { reg_reg_ptr_idx(reg, addr, index, scale, {std::forward<uint8_t>(Prefixes)..., D}); }
	x64_src_regdst_oper_base(x64_reg32 reg, x64_reg_ptr64 addr, x64_reg64 index, x64_sib_scale scale) { reg_reg_ptr_idx(reg, addr, index, scale, {std::forward<uint8_t>(Prefixes)..., D}); }
	x64_src_regdst_oper_base(x64_reg16 reg, x64_reg_ptr64 addr, x64_reg64 index, x64_sib_scale scale) { reg_reg_ptr_idx(reg, addr, index, scale, {std::forward<uint8_t>(Prefixes)..., D}); }
	x64_src_regdst_oper_base(x64_reg8  reg, x64_reg_ptr64 addr, x64_reg64 index, x64_sib_scale scale) { reg_reg_ptr_idx(reg, addr, index, scale, {std::forward<uint8_t>(Prefixes)..., C}); }
	x64_src_regdst_oper_base(x64_reg8h reg, x64_reg_ptr64 addr, x64_reg64 index, x64_sib_scale scale) { reg_reg_ptr_idx(reg.shift(), addr, index, scale, {std::forward<uint8_t>(Prefixes)..., C}); }

	/* 32 bit base address + index * scale into reg */
	x64_src_regdst_oper_base(x64_reg64 reg, x64_reg_ptr32 addr, x64_reg32 index, x64_sib_scale scale) { reg_reg_ptr_idx(reg, addr, index, scale, {std::forward<uint8_t>(Prefixes)..., D}); }
	x64_src_regdst_oper_base(x64_reg32 reg, x64_reg_ptr32 addr, x64_reg32 index, x64_sib_scale scale) { reg_reg_ptr_idx(reg, addr, index, scale, {std::forward<uint8_t>(Prefixes)..., D}); }
	x64_src_regdst_oper_base(x64_reg16 reg, x64_reg_ptr32 addr, x64_reg32 index, x64_sib_scale scale) { reg_reg_ptr_idx(reg, addr, index, scale, {std::forward<uint8_t>(Prefixes)..., D}); }
	x64_src_regdst_oper_base(x64_reg8  reg, x64_reg_ptr32 addr, x64_reg32 index, x64_sib_scale scale) { reg_reg_ptr_idx(reg, addr, index, scale, {std::forward<uint8_t>(Prefixes)..., C}); }
	x64_src_regdst_oper_base(x64_reg8h reg, x64_reg_ptr32 addr, x64_reg32 index, x64_sib_scale scale) { reg_reg_ptr_idx(reg.shift(), addr, index, scale, {std::forward<uint8_t>(Prefixes)..., C}); }


	/* 64 bit base address + index * scale + 8 bit offset into reg */
	x64_src_regdst_oper_base(x64_reg64 reg, x64_reg_ptr64 addr, x64_reg64 index, x64_sib_scale scale, int8_t off) { reg_reg_ptr_idx_off(reg, addr, index, scale, off, {std::forward<uint8_t>(Prefixes)..., D}, 1); }
	x64_src_regdst_oper_base(x64_reg32 reg, x64_reg_ptr64 addr, x64_reg64 index, x64_sib_scale scale, int8_t off) { reg_reg_ptr_idx_off(reg, addr, index, scale, off, {std::forward<uint8_t>(Prefixes)..., D}, 1); }
	x64_src_regdst_oper_base(x64_reg16 reg, x64_reg_ptr64 addr, x64_reg64 index, x64_sib_scale scale, int8_t off) { reg_reg_ptr_idx_off(reg, addr, index, scale, off, {std::forward<uint8_t>(Prefixes)..., D}, 1); }
	x64_src_regdst_oper_base(x64_reg8  reg, x64_reg_ptr64 addr, x64_reg64 index, x64_sib_scale scale, int8_t off) { reg_reg_ptr_idx_off(reg, addr, index, scale, off, {std::forward<uint8_t>(Prefixes)..., C}, 1); }
	x64_src_regdst_oper_base(x64_reg8h reg, x64_reg_ptr64 addr, x64_reg64 index, x64_sib_scale scale, int8_t off) { reg_reg_ptr_idx_off(reg.shift(), addr, index, scale, off, {std::forward<uint8_t>(Prefixes)..., C}, 1); }

	/* 32 bit base address + index * scale + 8 bit offset  into reg */
	x64_src_regdst_oper_base(x64_reg64 reg, x64_reg_ptr32 addr, x64_reg32 index, x64_sib_scale scale, int8_t off) { reg_reg_ptr_idx_off(reg, addr, index, scale, off, {std::forward<uint8_t>(Prefixes)..., D}, 1); }
	x64_src_regdst_oper_base(x64_reg32 reg, x64_reg_ptr32 addr, x64_reg32 index, x64_sib_scale scale, int8_t off) { reg_reg_ptr_idx_off(reg, addr, index, scale, off, {std::forward<uint8_t>(Prefixes)..., D}, 1); }
	x64_src_regdst_oper_base(x64_reg16 reg, x64_reg_ptr32 addr, x64_reg32 index, x64_sib_scale scale, int8_t off) { reg_reg_ptr_idx_off(reg, addr, index, scale, off, {std::forward<uint8_t>(Prefixes)..., D}, 1); }
	x64_src_regdst_oper_base(x64_reg8  reg, x64_reg_ptr32 addr, x64_reg32 index, x64_sib_scale scale, int8_t off) { reg_reg_ptr_idx_off(reg, addr, index, scale, off, {std::forward<uint8_t>(Prefixes)..., C}, 1); }
	x64_src_regdst_oper_base(x64_reg8h reg, x64_reg_ptr32 addr, x64_reg32 index, x64_sib_scale scale, int8_t off) { reg_reg_ptr_idx_off(reg.shift(), addr, index, scale, off, {std::forward<uint8_t>(Prefixes)..., C}, 1); }

	/* 64 bit base address + index * scale + 32 bit offset into reg */
	x64_src_regdst_oper_base(x64_reg64 reg, x64_reg_ptr64 addr, x64_reg64 index, x64_sib_scale scale, int32_t off) { reg_reg_ptr_idx_off(reg, addr, index, scale, off, {std::forward<uint8_t>(Prefixes)..., D}, 2); }
	x64_src_regdst_oper_base(x64_reg32 reg, x64_reg_ptr64 addr, x64_reg64 index, x64_sib_scale scale, int32_t off) { reg_reg_ptr_idx_off(reg, addr, index, scale, off, {std::forward<uint8_t>(Prefixes)..., D}, 2); }
	x64_src_regdst_oper_base(x64_reg16 reg, x64_reg_ptr64 addr, x64_reg64 index, x64_sib_scale scale, int32_t off) { reg_reg_ptr_idx_off(reg, addr, index, scale, off, {std::forward<uint8_t>(Prefixes)..., D}, 2); }
	x64_src_regdst_oper_base(x64_reg8  reg, x64_reg_ptr64 addr, x64_reg64 index, x64_sib_scale scale, int32_t off) { reg_reg_ptr_idx_off(reg, addr, index, scale, off, {std::forward<uint8_t>(Prefixes)..., C}, 2); }
	x64_src_regdst_oper_base(x64_reg8h reg, x64_reg_ptr64 addr, x64_reg64 index, x64_sib_scale scale, int32_t off) { reg_reg_ptr_idx_off(reg.shift(), addr, index, scale, off, {std::forward<uint8_t>(Prefixes)..., C}, 2); }

	/* 32 bit base address + index * scale + 32 bit offset  into reg */
	x64_src_regdst_oper_base(x64_reg64 reg, x64_reg_ptr32 addr, x64_reg32 index, x64_sib_scale scale, int32_t off) { reg_reg_ptr_idx_off(reg, addr, index, scale, off, {std::forward<uint8_t>(Prefixes)..., D}, 2); }
	x64_src_regdst_oper_base(x64_reg32 reg, x64_reg_ptr32 addr, x64_reg32 index, x64_sib_scale scale, int32_t off) { reg_reg_ptr_idx_off(reg, addr, index, scale, off, {std::forward<uint8_t>(Prefixes)..., D}, 2); }
	x64_src_regdst_oper_base(x64_reg16 reg, x64_reg_ptr32 addr, x64_reg32 index, x64_sib_scale scale, int32_t off) { reg_reg_ptr_idx_off(reg, addr, index, scale, off, {std::forward<uint8_t>(Prefixes)..., D}, 2); }
	x64_src_regdst_oper_base(x64_reg8  reg, x64_reg_ptr32 addr, x64_reg32 index, x64_sib_scale scale, int32_t off) { reg_reg_ptr_idx_off(reg, addr, index, scale, off, {std::forward<uint8_t>(Prefixes)..., C}, 2); }
	x64_src_regdst_oper_base(x64_reg8h reg, x64_reg_ptr32 addr, x64_reg32 index, x64_sib_scale scale, int32_t off) { reg_reg_ptr_idx_off(reg.shift(), addr, index, scale, off, {std::forward<uint8_t>(Prefixes)..., C}, 2); }
};

template<uint8_t A, uint8_t B, uint8_t C, uint8_t D, uint8_t... Prefixes>
class x64_srcdst_oper_base : public x64_src_regdst_oper_base<A, B, C, D, std::forward<uint8_t>(Prefixes)...>
{
public:
	using x64_src_regdst_oper_base<A, B, C, D, std::forward<uint8_t>(Prefixes)...>::x64_src_regdst_oper_base;

	/* register into register pointer */
	x64_srcdst_oper_base(x64_reg_ptr64 addr, x64_reg64 reg) { this->template reg_reg_ptr(reg, addr, {std::forward<uint8_t>(Prefixes)..., B}); }
	x64_srcdst_oper_base(x64_reg_ptr64 addr, x64_reg32 reg) { this->template reg_reg_ptr(reg, addr, {std::forward<uint8_t>(Prefixes)..., B}); }
	x64_srcdst_oper_base(x64_reg_ptr64 addr, x64_reg16 reg) { this->template reg_reg_ptr(reg, addr, {std::forward<uint8_t>(Prefixes)..., B}); }
	x64_srcdst_oper_base(x64_reg_ptr64 addr, x64_reg8  reg) { this->template reg_reg_ptr(reg, addr, {std::forward<uint8_t>(Prefixes)..., A}); }
	x64_srcdst_oper_base(x64_reg_ptr64 addr, x64_reg8h reg) { this->template reg_reg_ptr(reg.shift(), addr, {std::forward<uint8_t>(Prefixes)..., A}); }

	x64_srcdst_oper_base(x64_reg_ptr32 addr, x64_reg64 reg) { this->template reg_reg_ptr(reg, addr, {std::forward<uint8_t>(Prefixes)..., B}); }
	x64_srcdst_oper_base(x64_reg_ptr32 addr, x64_reg32 reg) { this->template reg_reg_ptr(reg, addr, {std::forward<uint8_t>(Prefixes)..., B}); }
	x64_srcdst_oper_base(x64_reg_ptr32 addr, x64_reg16 reg) { this->template reg_reg_ptr(reg, addr, {std::forward<uint8_t>(Prefixes)..., B}); }
	x64_srcdst_oper_base(x64_reg_ptr32 addr, x64_reg8  reg) { this->template reg_reg_ptr(reg, addr, {std::forward<uint8_t>(Prefixes)..., A}); }
	x64_srcdst_oper_base(x64_reg_ptr32 addr, x64_reg8h reg) { this->template reg_reg_ptr(reg.shift(), addr, {std::forward<uint8_t>(Prefixes)..., A}); }

	/* register into register pointer + 8 bit offset */
	x64_srcdst_oper_base(x64_reg_ptr64 addr, int8_t off, x64_reg64 reg) { this->template reg_reg_ptr_off(reg, addr, {std::forward<uint8_t>(Prefixes)..., B}, 1, off); }
	x64_srcdst_oper_base(x64_reg_ptr64 addr, int8_t off, x64_reg32 reg) { this->template reg_reg_ptr_off(reg, addr, {std::forward<uint8_t>(Prefixes)..., B}, 1, off); }
	x64_srcdst_oper_base(x64_reg_ptr64 addr, int8_t off, x64_reg16 reg) { this->template reg_reg_ptr_off(reg, addr, {std::forward<uint8_t>(Prefixes)..., B}, 1, off); }
	x64_srcdst_oper_base(x64_reg_ptr64 addr, int8_t off, x64_reg8  reg) { this->template reg_reg_ptr_off(reg, addr, {std::forward<uint8_t>(Prefixes)..., A}, 1, off); }
	x64_srcdst_oper_base(x64_reg_ptr64 addr, int8_t off, x64_reg8h reg) { this->template reg_reg_ptr_off(reg.shift(), addr, {std::forward<uint8_t>(Prefixes)..., A}, 1, off); }

	x64_srcdst_oper_base(x64_reg_ptr32 addr, int8_t off, x64_reg64 reg) { this->template reg_reg_ptr_off(reg, addr, {std::forward<uint8_t>(Prefixes)..., B}, 1, off); }
	x64_srcdst_oper_base(x64_reg_ptr32 addr, int8_t off, x64_reg32 reg) { this->template reg_reg_ptr_off(reg, addr, {std::forward<uint8_t>(Prefixes)..., B}, 1, off); }
	x64_srcdst_oper_base(x64_reg_ptr32 addr, int8_t off, x64_reg16 reg) { this->template reg_reg_ptr_off(reg, addr, {std::forward<uint8_t>(Prefixes)..., B}, 1, off); }
	x64_srcdst_oper_base(x64_reg_ptr32 addr, int8_t off, x64_reg8  reg) { this->template reg_reg_ptr_off(reg, addr, {std::forward<uint8_t>(Prefixes)..., A}, 1, off); }
	x64_srcdst_oper_base(x64_reg_ptr32 addr, int8_t off, x64_reg8h reg) { this->template reg_reg_ptr_off(reg.shift(), addr, {std::forward<uint8_t>(Prefixes)..., A}, 1, off); }

	/* register into register pointer + 32 bit offset */
	x64_srcdst_oper_base(x64_reg_ptr64 addr, int32_t off, x64_reg64 reg) { this->template reg_reg_ptr_off(reg, addr, {std::forward<uint8_t>(Prefixes)..., B}, 2, off); }
	x64_srcdst_oper_base(x64_reg_ptr64 addr, int32_t off, x64_reg32 reg) { this->template reg_reg_ptr_off(reg, addr, {std::forward<uint8_t>(Prefixes)..., B}, 2, off); }
	x64_srcdst_oper_base(x64_reg_ptr64 addr, int32_t off, x64_reg16 reg) { this->template reg_reg_ptr_off(reg, addr, {std::forward<uint8_t>(Prefixes)..., B}, 2, off); }
	x64_srcdst_oper_base(x64_reg_ptr64 addr, int32_t off, x64_reg8  reg) { this->template reg_reg_ptr_off(reg, addr, {std::forward<uint8_t>(Prefixes)..., A}, 2, off); }
	x64_srcdst_oper_base(x64_reg_ptr64 addr, int32_t off, x64_reg8h reg) { this->template reg_reg_ptr_off(reg.shift(), addr, {std::forward<uint8_t>(Prefixes)..., A}, 2, off); }

	x64_srcdst_oper_base(x64_reg_ptr32 addr, int32_t off, x64_reg64 reg) { this->template reg_reg_ptr_off(reg, addr, {std::forward<uint8_t>(Prefixes)..., B}, 2, off); }
	x64_srcdst_oper_base(x64_reg_ptr32 addr, int32_t off, x64_reg32 reg) { this->template reg_reg_ptr_off(reg, addr, {std::forward<uint8_t>(Prefixes)..., B}, 2, off); }
	x64_srcdst_oper_base(x64_reg_ptr32 addr, int32_t off, x64_reg16 reg) { this->template reg_reg_ptr_off(reg, addr, {std::forward<uint8_t>(Prefixes)..., B}, 2, off); }
	x64_srcdst_oper_base(x64_reg_ptr32 addr, int32_t off, x64_reg8  reg) { this->template reg_reg_ptr_off(reg, addr, {std::forward<uint8_t>(Prefixes)..., A}, 2, off); }
	x64_srcdst_oper_base(x64_reg_ptr32 addr, int32_t off, x64_reg8h reg) { this->template reg_reg_ptr_off(reg.shift(), addr, {std::forward<uint8_t>(Prefixes)..., A}, 2, off); }

	/* register into immediate address */
	/* 32 bit pointers */
	x64_srcdst_oper_base(x64_addr_ptr<int32_t> addr, x64_reg64 reg) { this->template reg_imm_addr(reg, addr, {std::forward<uint8_t>(Prefixes)..., B}); }
	x64_srcdst_oper_base(x64_addr_ptr<int32_t> addr, x64_reg32 reg) { this->template reg_imm_addr(reg, addr, {std::forward<uint8_t>(Prefixes)..., B}); }
	x64_srcdst_oper_base(x64_addr_ptr<int32_t> addr, x64_reg16 reg) { this->template reg_imm_addr(reg, addr, {std::forward<uint8_t>(Prefixes)..., B}); }
	x64_srcdst_oper_base(x64_addr_ptr<int32_t> addr, x64_reg8  reg) { this->template reg_imm_addr(reg, addr, {std::forward<uint8_t>(Prefixes)..., A}); }
	x64_srcdst_oper_base(x64_addr_ptr<int32_t> addr, x64_reg8h reg) { this->template reg_imm_addr(reg.shift(), addr, {std::forward<uint8_t>(Prefixes)..., A}); }

	/* reg into 64 bit base address + index * scale */
	x64_srcdst_oper_base(x64_reg_ptr64 addr, x64_reg64 index, x64_sib_scale scale, x64_reg64 reg) { this->template reg_reg_ptr_idx(reg, addr, index, scale, {std::forward<uint8_t>(Prefixes)..., B}); }
	x64_srcdst_oper_base(x64_reg_ptr64 addr, x64_reg64 index, x64_sib_scale scale, x64_reg32 reg) { this->template reg_reg_ptr_idx(reg, addr, index, scale, {std::forward<uint8_t>(Prefixes)..., B}); }
	x64_srcdst_oper_base(x64_reg_ptr64 addr, x64_reg64 index, x64_sib_scale scale, x64_reg16 reg) { this->template reg_reg_ptr_idx(reg, addr, index, scale, {std::forward<uint8_t>(Prefixes)..., B}); }
	x64_srcdst_oper_base(x64_reg_ptr64 addr, x64_reg64 index, x64_sib_scale scale, x64_reg8  reg) { this->template reg_reg_ptr_idx(reg, addr, index, scale, {std::forward<uint8_t>(Prefixes)..., A}); }
	x64_srcdst_oper_base(x64_reg_ptr64 addr, x64_reg64 index, x64_sib_scale scale, x64_reg8h reg) { this->template reg_reg_ptr_idx(reg.shift(), addr, index, scale, {std::forward<uint8_t>(Prefixes)..., A}); }

	/* reg into 32 bit base address + index * scale */
	x64_srcdst_oper_base(x64_reg_ptr32 addr, x64_reg32 index, x64_sib_scale scale, x64_reg64 reg) { this->template reg_reg_ptr_idx(reg, addr, index, scale, {std::forward<uint8_t>(Prefixes)..., B}); }
	x64_srcdst_oper_base(x64_reg_ptr32 addr, x64_reg32 index, x64_sib_scale scale, x64_reg32 reg) { this->template reg_reg_ptr_idx(reg, addr, index, scale, {std::forward<uint8_t>(Prefixes)..., B}); }
	x64_srcdst_oper_base(x64_reg_ptr32 addr, x64_reg32 index, x64_sib_scale scale, x64_reg16 reg) { this->template reg_reg_ptr_idx(reg, addr, index, scale, {std::forward<uint8_t>(Prefixes)..., B}); }
	x64_srcdst_oper_base(x64_reg_ptr32 addr, x64_reg32 index, x64_sib_scale scale, x64_reg8  reg) { this->template reg_reg_ptr_idx(reg, addr, index, scale, {std::forward<uint8_t>(Prefixes)..., A}); }
	x64_srcdst_oper_base(x64_reg_ptr32 addr, x64_reg32 index, x64_sib_scale scale, x64_reg8h reg) { this->template reg_reg_ptr_idx(reg.shift(), addr, index, scale, {std::forward<uint8_t>(Prefixes)..., A}); }

	/* reg into 64 bit base address + index + 8 bit offset  * scale */
	x64_srcdst_oper_base(x64_reg_ptr64 addr, x64_reg64 index, x64_sib_scale scale, int8_t off, x64_reg64 reg) { this->template reg_reg_ptr_idx_off(reg, addr, index, scale, off, {std::forward<uint8_t>(Prefixes)..., B}, 1); }
	x64_srcdst_oper_base(x64_reg_ptr64 addr, x64_reg64 index, x64_sib_scale scale, int8_t off, x64_reg32 reg) { this->template reg_reg_ptr_idx_off(reg, addr, index, scale, off, {std::forward<uint8_t>(Prefixes)..., B}, 1); }
	x64_srcdst_oper_base(x64_reg_ptr64 addr, x64_reg64 index, x64_sib_scale scale, int8_t off, x64_reg16 reg) { this->template reg_reg_ptr_idx_off(reg, addr, index, scale, off, {std::forward<uint8_t>(Prefixes)..., B}, 1); }
	x64_srcdst_oper_base(x64_reg_ptr64 addr, x64_reg64 index, x64_sib_scale scale, int8_t off, x64_reg8  reg) { this->template reg_reg_ptr_idx_off(reg, addr, index, scale, off, {std::forward<uint8_t>(Prefixes)..., A}, 1); }
	x64_srcdst_oper_base(x64_reg_ptr64 addr, x64_reg64 index, x64_sib_scale scale, int8_t off, x64_reg8h reg) { this->template reg_reg_ptr_idx_off(reg.shift(), addr, index, scale, off, {std::forward<uint8_t>(Prefixes)..., A}, 1); }

	/* reg into 32 bit base address + index + 8 bit offset  * scale */
	x64_srcdst_oper_base(x64_reg_ptr32 addr, x64_reg32 index, x64_sib_scale scale, int8_t off, x64_reg64 reg) { this->template reg_reg_ptr_idx_off(reg, addr, index, scale, off, {std::forward<uint8_t>(Prefixes)..., B}, 1); }
	x64_srcdst_oper_base(x64_reg_ptr32 addr, x64_reg32 index, x64_sib_scale scale, int8_t off, x64_reg32 reg) { this->template reg_reg_ptr_idx_off(reg, addr, index, scale, off, {std::forward<uint8_t>(Prefixes)..., B}, 1); }
	x64_srcdst_oper_base(x64_reg_ptr32 addr, x64_reg32 index, x64_sib_scale scale, int8_t off, x64_reg16 reg) { this->template reg_reg_ptr_idx_off(reg, addr, index, scale, off, {std::forward<uint8_t>(Prefixes)..., B}, 1); }
	x64_srcdst_oper_base(x64_reg_ptr32 addr, x64_reg32 index, x64_sib_scale scale, int8_t off, x64_reg8  reg) { this->template reg_reg_ptr_idx_off(reg, addr, index, scale, off, {std::forward<uint8_t>(Prefixes)..., A}, 1); }
	x64_srcdst_oper_base(x64_reg_ptr32 addr, x64_reg32 index, x64_sib_scale scale, int8_t off, x64_reg8h reg) { this->template reg_reg_ptr_idx_off(reg.shift(), addr, index, scale, off, {std::forward<uint8_t>(Prefixes)..., A}, 1); }

	/* reg into 64 bit base address + index + 32 bit offset  * scale */
	x64_srcdst_oper_base(x64_reg_ptr64 addr, x64_reg64 index, x64_sib_scale scale, int32_t off, x64_reg64 reg) { this->template reg_reg_ptr_idx_off(reg, addr, index, scale, off, {std::forward<uint8_t>(Prefixes)..., B}, 2); }
	x64_srcdst_oper_base(x64_reg_ptr64 addr, x64_reg64 index, x64_sib_scale scale, int32_t off, x64_reg32 reg) { this->template reg_reg_ptr_idx_off(reg, addr, index, scale, off, {std::forward<uint8_t>(Prefixes)..., B}, 2); }
	x64_srcdst_oper_base(x64_reg_ptr64 addr, x64_reg64 index, x64_sib_scale scale, int32_t off, x64_reg16 reg) { this->template reg_reg_ptr_idx_off(reg, addr, index, scale, off, {std::forward<uint8_t>(Prefixes)..., B}, 2); }
	x64_srcdst_oper_base(x64_reg_ptr64 addr, x64_reg64 index, x64_sib_scale scale, int32_t off, x64_reg8  reg) { this->template reg_reg_ptr_idx_off(reg, addr, index, scale, off, {std::forward<uint8_t>(Prefixes)..., A}, 2); }
	x64_srcdst_oper_base(x64_reg_ptr64 addr, x64_reg64 index, x64_sib_scale scale, int32_t off, x64_reg8h reg) { this->template reg_reg_ptr_idx_off(reg.shift(), addr, index, scale, off, {std::forward<uint8_t>(Prefixes)..., A}, 2); }

	/* reg into 32 bit base address + index + 32 bit offset  * scale */
	x64_srcdst_oper_base(x64_reg_ptr32 addr, x64_reg32 index, x64_sib_scale scale, int32_t off, x64_reg64 reg) { this->template reg_reg_ptr_idx_off(reg, addr, index, scale, off, {std::forward<uint8_t>(Prefixes)..., B}, 2); }
	x64_srcdst_oper_base(x64_reg_ptr32 addr, x64_reg32 index, x64_sib_scale scale, int32_t off, x64_reg32 reg) { this->template reg_reg_ptr_idx_off(reg, addr, index, scale, off, {std::forward<uint8_t>(Prefixes)..., B}, 2); }
	x64_srcdst_oper_base(x64_reg_ptr32 addr, x64_reg32 index, x64_sib_scale scale, int32_t off, x64_reg16 reg) { this->template reg_reg_ptr_idx_off(reg, addr, index, scale, off, {std::forward<uint8_t>(Prefixes)..., B}, 2); }
	x64_srcdst_oper_base(x64_reg_ptr32 addr, x64_reg32 index, x64_sib_scale scale, int32_t off, x64_reg8  reg) { this->template reg_reg_ptr_idx_off(reg, addr, index, scale, off, {std::forward<uint8_t>(Prefixes)..., A}, 2); }
	x64_srcdst_oper_base(x64_reg_ptr32 addr, x64_reg32 index, x64_sib_scale scale, int32_t off, x64_reg8h reg) { this->template reg_reg_ptr_idx_off(reg.shift(), addr, index, scale, off, {std::forward<uint8_t>(Prefixes)..., A}, 2); }
};

template<uint8_t A, uint8_t B, uint8_t C, uint8_t D, uint8_t E, uint8_t F, uint8_t MOD, uint8_t... Prefixes>
class x64_arith_base : public x64_srcdst_oper_base<A, B, C, D, std::forward<uint8_t>(Prefixes)...>
{
public:
	using x64_srcdst_oper_base<A, B, C, D, std::forward<uint8_t>(Prefixes)...>::x64_srcdst_oper_base;

	/* immediate into register */
	x64_arith_base(x64_reg64 reg, uint8_t  imm) { reg_imm(reg, imm, 				{std::forward<uint8_t>(Prefixes)..., 0x83}); }
	x64_arith_base(x64_reg64 reg, uint32_t imm) { reg_imm_oroc(reg, imm, F, 		{std::forward<uint8_t>(Prefixes)..., 0x81}); }
	x64_arith_base(x64_reg32 reg, uint8_t  imm) { reg_imm(reg, imm,					{std::forward<uint8_t>(Prefixes)..., 0x83}); }
	x64_arith_base(x64_reg32 reg, uint32_t imm) { reg_imm_oroc(reg, imm, F, 		{std::forward<uint8_t>(Prefixes)..., 0x81}); }
	x64_arith_base(x64_reg16 reg, uint8_t  imm) { reg_imm(reg, imm,					{std::forward<uint8_t>(Prefixes)..., 0x83}); }
	x64_arith_base(x64_reg16 reg, uint16_t imm) { reg_imm_oroc(reg, imm, F,			{std::forward<uint8_t>(Prefixes)..., 0x81}); }
	x64_arith_base(x64_reg8  reg, uint8_t  imm) { reg_imm_oroc(reg, imm, E,			{std::forward<uint8_t>(Prefixes)..., 0x80}); }
	x64_arith_base(x64_reg8h reg, uint8_t  imm) { reg_imm_oroc(reg.shift(), imm, E,	{std::forward<uint8_t>(Prefixes)..., 0x80}); }

	virtual ~x64_arith_base() { }

private:
	template<typename T, typename U>
	void reg_imm(T a, U imm, std::initializer_list<uint8_t> oc)
	{
		this->template add_prefixes(a, T(0));
		this->template add_opcode(oc);
		this->template set_modrm(x64_modrm(a, MOD, 3));
		this->template set_imm(imm);
	}

	template<typename T, typename U>
	void reg_imm_oroc(T a, U imm, uint8_t oc1, std::initializer_list<uint8_t> oc2)
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

template<uint8_t A, uint8_t B, uint8_t C, uint8_t... Prefixes>
struct x64_jmpcall_base : public x64_instruction
{
	using x64_instruction::x64_instruction;

	x64_jmpcall_base(int32_t off) : x64_instruction( {std::forward<uint8_t>(Prefixes)..., B}, off) { }
	x64_jmpcall_base(x64_reg64 reg) { single_regptr(x64_reg_addr(reg), C, {std::forward<uint8_t>(Prefixes)..., A}, 3); }

	x64_jmpcall_base(x64_reg_ptr64 addr) { reg_reg_ptr(x64_reg32(C), addr, {std::forward<uint8_t>(Prefixes)..., A}); }
	x64_jmpcall_base(x64_reg_ptr32 addr) { reg_reg_ptr(x64_reg32(C), addr, {std::forward<uint8_t>(Prefixes)..., A}); }
	x64_jmpcall_base(x64_reg_ptr64 addr, int8_t off) { reg_reg_ptr_off(x64_reg32(C), addr, {std::forward<uint8_t>(Prefixes)..., A}, 1, off); }
	x64_jmpcall_base(x64_reg_ptr32 addr, int8_t off) { reg_reg_ptr_off(x64_reg32(C), addr, {std::forward<uint8_t>(Prefixes)..., A}, 1, off); }
	x64_jmpcall_base(x64_reg_ptr64 addr, int32_t off) { reg_reg_ptr_off(x64_reg32(C), addr, {std::forward<uint8_t>(Prefixes)..., A}, 2, off); }
	x64_jmpcall_base(x64_reg_ptr32 addr, int32_t off) { reg_reg_ptr_off(x64_reg32(C), addr, {std::forward<uint8_t>(Prefixes)..., A}, 2, off); }

	x64_jmpcall_base(x64_reg_ptr64 addr, x64_reg64 index, x64_sib_scale scale) { reg_reg_ptr_idx(x64_reg32(C), addr, index, scale, {std::forward<uint8_t>(Prefixes)..., A}); }
	x64_jmpcall_base(x64_reg_ptr32 addr, x64_reg32 index, x64_sib_scale scale) { reg_reg_ptr_idx(x64_reg32(C), addr, index, scale, {std::forward<uint8_t>(Prefixes)..., A}); }

	x64_jmpcall_base(x64_reg_ptr64 addr, x64_reg64 index, x64_sib_scale scale, int32_t off) { reg_reg_ptr_idx_off(x64_reg32(C), addr, index, scale, off, {std::forward<uint8_t>(Prefixes)..., A}, 2); }
	x64_jmpcall_base(x64_reg_ptr32 addr, x64_reg32 index, x64_sib_scale scale, int32_t off) { reg_reg_ptr_idx_off(x64_reg32(C), addr, index, scale, off, {std::forward<uint8_t>(Prefixes)..., A}, 2); }
	x64_jmpcall_base(x64_reg_ptr64 addr, x64_reg64 index, x64_sib_scale scale, int8_t off) { reg_reg_ptr_idx_off(x64_reg32(C), addr, index, scale, off, {std::forward<uint8_t>(Prefixes)..., A}, 1); }
	x64_jmpcall_base(x64_reg_ptr32 addr, x64_reg32 index, x64_sib_scale scale, int8_t off) { reg_reg_ptr_idx_off(x64_reg32(C), addr, index, scale, off, {std::forward<uint8_t>(Prefixes)..., A}, 1); }
};

template<uint8_t A, uint8_t B, uint8_t C, uint8_t... Prefixes>
struct x64_single_op_base : x64_instruction
{
	x64_single_op_base(x64_reg64 reg) { single_reg(reg, C, {std::forward<uint8_t>(Prefixes)..., B}, 3); }
	x64_single_op_base(x64_reg32 reg) { single_reg(reg, C, {std::forward<uint8_t>(Prefixes)..., B}, 3); }
	x64_single_op_base(x64_reg16 reg) { single_reg(reg, C, {std::forward<uint8_t>(Prefixes)..., B}, 3); }
	x64_single_op_base(x64_reg8  reg) { single_reg(reg, C, {std::forward<uint8_t>(Prefixes)..., A}, 3); }
	x64_single_op_base(x64_reg8h reg) { single_reg(reg.shift(), C, {std::forward<uint8_t>(Prefixes)..., A}, 3); }
};

/********** ACTUAL INSTRUCTIONS FOLLOW **********/

struct x64_nop1	: public x64_instruction{x64_nop1()	: x64_instruction({ 0x90 }) {} };
struct x64_leave: public x64_instruction{x64_leave(): x64_instruction({ 0xc9 }) {} };
struct x64_ret	: public x64_instruction{x64_ret()	: x64_instruction({ 0xc3 }) {} };
struct x64_lret	: public x64_instruction{x64_lret()	: x64_instruction({ 0xcb }) {} };
struct x64_ud2	: public x64_instruction{x64_ud2()	: x64_instruction({ 0x0f, 0x0b }) {} };

struct x64_call  : public x64_jmpcall_base<0xff, 0xe8, 2> { using x64_jmpcall_base::x64_jmpcall_base; };
struct x64_jmp   : public x64_jmpcall_base<0xff, 0xe9, 4> { using x64_jmpcall_base::x64_jmpcall_base; };
struct x64_jecxz : public x64_instruction{x64_jecxz(int8_t off) : x64_instruction({ 0xe3 }, off) {} };

struct x64_jmp_cond : public x64_instruction
{
	x64_jmp_cond(x64_cond cond, int8_t off) : x64_instruction({ static_cast<uint8_t>(0x70 | static_cast<uint8_t>(cond)) }, off) {};

	static x64_cond invert(x64_cond cond)
	{
		switch(cond)
		{
		case x64_cond::overflow: 			 return x64_cond::not_overflow; // 0x0,
		case x64_cond::not_overflow: 		 return x64_cond::overflow; // 0x1,
		case x64_cond::below: 				 return x64_cond::not_below; // 0x2,
		case x64_cond::not_below: 			 return x64_cond::below; // 0x3,
		case x64_cond::equal: 				 return x64_cond::not_equal; // 0x4,
		case x64_cond::not_equal: 			 return x64_cond::equal; // 0x5,
		case x64_cond::below_or_equal:		 return x64_cond::above; // 0x6,
		case x64_cond::above:			 	 return x64_cond::not_above; // 0x7,
		case x64_cond::sign: 				 return x64_cond::not_sign; // 0x8,
		case x64_cond::not_sign: 			 return x64_cond::sign; // 0x9,
		case x64_cond::parity:		 		 return x64_cond::not_parity; // 0xa,
		case x64_cond::not_parity: 			 return x64_cond::parity; // 0xb,
		case x64_cond::less:				 return x64_cond::not_less; // 0xc,
		case x64_cond::not_less: 			 return x64_cond::less; // 0xd,
		case x64_cond::not_greater: 		 return x64_cond::greater; // 0xe,
		case x64_cond::greater:			 	 return x64_cond::not_greater; // 0xf,
		default:
			throw std::invalid_argument("Invalid condition");
		}
	}
};

struct x64_jo   : x64_jmp_cond { x64_jo(int8_t off)   : x64_jmp_cond(x64_cond::overflow, off) { } };
struct x64_jno  : x64_jmp_cond { x64_jno(int8_t off)  : x64_jmp_cond(x64_cond::not_overflow, off) { } };
struct x64_js   : x64_jmp_cond { x64_js(int8_t off)   : x64_jmp_cond(x64_cond::sign, off) { } };
struct x64_jns  : x64_jmp_cond { x64_jns(int8_t off)  : x64_jmp_cond(x64_cond::not_sign, off) { } };
struct x64_je   : x64_jmp_cond { x64_je(int8_t off)   : x64_jmp_cond(x64_cond::equal, off) { } };
struct x64_jz   : x64_jmp_cond { x64_jz(int8_t off)   : x64_jmp_cond(x64_cond::zero, off) { } };
struct x64_ne   : x64_jmp_cond { x64_ne(int8_t off)   : x64_jmp_cond(x64_cond::not_equal, off) { } };
struct x64_nz   : x64_jmp_cond { x64_nz(int8_t off)   : x64_jmp_cond(x64_cond::not_zero, off) { } };
struct x64_jb   : x64_jmp_cond { x64_jb(int8_t off)   : x64_jmp_cond(x64_cond::below, off) { } };
struct x64_jnae : x64_jmp_cond { x64_jnae(int8_t off) : x64_jmp_cond(x64_cond::not_above_or_equal, off) { } };
struct x64_c    : x64_jmp_cond { x64_c(int8_t off)    : x64_jmp_cond(x64_cond::carry, off) { } };
struct x64_nb   : x64_jmp_cond { x64_nb(int8_t off)   : x64_jmp_cond(x64_cond::not_below, off) { } };
struct x64_ae   : x64_jmp_cond { x64_ae(int8_t off)   : x64_jmp_cond(x64_cond::above_or_equal, off) { } };
struct x64_jnc  : x64_jmp_cond { x64_jnc(int8_t off)  : x64_jmp_cond(x64_cond::not_carry, off) { } };
struct x64_be   : x64_jmp_cond { x64_be(int8_t off)   : x64_jmp_cond(x64_cond::below_or_equal, off) { } };
struct x64_na   : x64_jmp_cond { x64_na(int8_t off)   : x64_jmp_cond(x64_cond::not_above, off) { } };
struct x64_ja   : x64_jmp_cond { x64_ja(int8_t off)   : x64_jmp_cond(x64_cond::above, off) { } };
struct x64_nbe  : x64_jmp_cond { x64_nbe(int8_t off)  : x64_jmp_cond(x64_cond::not_below_or_equal, off) { } };
struct x64_jl   : x64_jmp_cond { x64_jl(int8_t off)   : x64_jmp_cond(x64_cond::less, off) { } };
struct x64_jnge : x64_jmp_cond { x64_jnge(int8_t off) : x64_jmp_cond(x64_cond::not_greater_or_equal, off) { } };
struct x64_jge  : x64_jmp_cond { x64_jge(int8_t off)  : x64_jmp_cond(x64_cond::greater_or_equal, off) { } };
struct x64_jnl  : x64_jmp_cond { x64_jnl(int8_t off)  : x64_jmp_cond(x64_cond::not_less, off) { } };
struct x64_jle  : x64_jmp_cond { x64_jle(int8_t off)  : x64_jmp_cond(x64_cond::less_or_equal, off) { } };
struct x64_jng  : x64_jmp_cond { x64_jng(int8_t off)  : x64_jmp_cond(x64_cond::not_greater, off) { } };
struct x64_jg   : x64_jmp_cond { x64_jg(int8_t off)   : x64_jmp_cond(x64_cond::greater, off) { } };
struct x64_jnle : x64_jmp_cond { x64_jnle(int8_t off) : x64_jmp_cond(x64_cond::not_less_or_equal, off) { } };
struct x64_jp   : x64_jmp_cond { x64_jp(int8_t off)   : x64_jmp_cond(x64_cond::parity, off) { } };
struct x64_jpe  : x64_jmp_cond { x64_jpe(int8_t off)  : x64_jmp_cond(x64_cond::parity_even, off) { } };
struct x64_jnp  : x64_jmp_cond { x64_jnp(int8_t off)  : x64_jmp_cond(x64_cond::not_parity, off) { } };
struct x64_jpo  : x64_jmp_cond { x64_jpo(int8_t off)  : x64_jmp_cond(x64_cond::parity_odd, off) { } };

class x64_mov : public x64_srcdst_oper_base<0x88, 0x89, 0x8a, 0x8b>
{
public:
	using x64_srcdst_oper_base::x64_srcdst_oper_base;

	/* Move immediate into register */
	x64_mov(x64_reg64 reg, uint64_t imm) { orred_oc_reg_imm(reg, imm, {0xb8}); }
	x64_mov(x64_reg32 reg, uint32_t imm) { orred_oc_reg_imm(reg, imm, {0xb8}); }
	x64_mov(x64_reg16 reg, uint16_t imm) { orred_oc_reg_imm(reg, imm, {0xb8}); }
	x64_mov(x64_reg8  reg, uint8_t  imm) { orred_oc_reg_imm(reg, imm, {0xb0}); }
	x64_mov(x64_reg8h reg, uint8_t  imm) { orred_oc_reg_imm(reg, imm, {0xb4}); }
	x64_mov(x64_reg64 reg, uint32_t imm)
	{
		add_prefixes(reg, x64_reg64(0));
		add_opcode(0xc7);
		set_modrm(x64_modrm{reg, 0, 3});
		set_imm(imm);
	}

	/* Move immediate address into register */
	/* 64 bit pointers */
	x64_mov(x64_reg64_0 reg, x64_addr_ptr<uint64_t*> addr) { eax_imm(reg, reinterpret_cast<uint64_t>(addr.ptr), {0xa1}); }
	x64_mov(x64_reg32_0 reg, x64_addr_ptr<uint32_t*> addr) { eax_imm(reg, reinterpret_cast<uint64_t>(addr.ptr), {0xa1}); }
	x64_mov(x64_reg16_0 reg, x64_addr_ptr<uint16_t*> addr) { eax_imm(reg, reinterpret_cast<uint64_t>(addr.ptr), {0xa1}); }
	x64_mov(x64_reg8_0  reg, x64_addr_ptr<uint8_t*>  addr) { eax_imm(reg, reinterpret_cast<uint64_t>(addr.ptr), {0xa0}); }

	/* Move register into immediate address */
	/* 64 bit pointers */
	x64_mov(x64_addr_ptr<uint64_t*> addr, x64_reg64_0 reg) { eax_imm(reg, reinterpret_cast<uint64_t>(addr.ptr), {0xa3}); }
	x64_mov(x64_addr_ptr<uint32_t*> addr, x64_reg32_0 reg) { eax_imm(reg, reinterpret_cast<uint64_t>(addr.ptr), {0xa3}); }
	x64_mov(x64_addr_ptr<uint16_t*> addr, x64_reg16_0 reg) { eax_imm(reg, reinterpret_cast<uint64_t>(addr.ptr), {0xa3}); }
	x64_mov(x64_addr_ptr<uint8_t*>  addr, x64_reg8_0 reg) { eax_imm(reg, reinterpret_cast<uint64_t>(addr.ptr), {0xa2}); }

	virtual ~x64_mov() { }
};


struct x64_add: public x64_arith_base<0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0>
{ using x64_arith_base::x64_arith_base; };

struct x64_or: public x64_arith_base<0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 1>
{ using x64_arith_base::x64_arith_base; };

struct x64_adc: public x64_arith_base<0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 2>
{ using x64_arith_base::x64_arith_base; };

struct x64_sbb: public x64_arith_base<0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 3>
{ using x64_arith_base::x64_arith_base; };

struct x64_and: public x64_arith_base<0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 4>
{ using x64_arith_base::x64_arith_base; };

struct x64_sub: public x64_arith_base<0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 5>
{ using x64_arith_base::x64_arith_base; };

struct x64_xor: public x64_arith_base<0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 6>
{ using x64_arith_base::x64_arith_base; };

struct x64_cmp: public x64_arith_base<0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 7>
{ using x64_arith_base::x64_arith_base; };


struct x64_inc  : public x64_single_op_base<0xfe, 0xff, 0> { using x64_single_op_base::x64_single_op_base; };
struct x64_dec  : public x64_single_op_base<0xfe, 0xff, 1> { using x64_single_op_base::x64_single_op_base; };
struct x64_neg  : public x64_single_op_base<0xf6, 0xf7, 3> { using x64_single_op_base::x64_single_op_base; };
struct x64_mul  : public x64_single_op_base<0xf6, 0xf7, 4> { using x64_single_op_base::x64_single_op_base; };
struct x64_div  : public x64_single_op_base<0xf6, 0xf7, 6> { using x64_single_op_base::x64_single_op_base; };
struct x64_idiv : public x64_single_op_base<0xf6, 0xf7, 7> { using x64_single_op_base::x64_single_op_base; };

struct x64_imul : public x64_src_regdst_oper_base<0xae, 0xaf, 0xae, 0xaf, 0x0f>
{
	using x64_src_regdst_oper_base<0xae, 0xaf, 0xae, 0xaf, 0x0f>::x64_src_regdst_oper_base;

	x64_imul(x64_reg64 reg) { single_reg(reg, 5, {0xf7}, 3); }
	x64_imul(x64_reg32 reg) { single_reg(reg, 5, {0xf7}, 3); }
	x64_imul(x64_reg16 reg) { single_reg(reg, 5, {0xf7}, 3); }
	x64_imul(x64_reg8  reg) { single_reg(reg, 5, {0xf6}, 3); }
	x64_imul(x64_reg8h reg) { single_reg(reg.shift(), 5, {0xf6}, 3); }

	// XXX: What is this reversed register madness?!
	x64_imul(x64_reg64 dst, x64_reg64 src) { reg_reg(src, dst, {0xaf}); }
	x64_imul(x64_reg32 dst, x64_reg32 src) { reg_reg(src, dst, {0xaf}); }
	x64_imul(x64_reg16 dst, x64_reg16 src) { reg_reg(src, dst, {0xaf}); }

	x64_imul(x64_reg64 dst, x64_reg64 src, int8_t n) { reg_reg(src, dst, {0x6b}); set_imm(n); }
	x64_imul(x64_reg32 dst, x64_reg32 src, int8_t n) { reg_reg(src, dst, {0x6b}); set_imm(n); }
	x64_imul(x64_reg16 dst, x64_reg16 src, int8_t n) { reg_reg(src, dst, {0x6b}); set_imm(n); }

	x64_imul(x64_reg64 dst, x64_reg64 src, int32_t n) { reg_reg(src, dst, {0x69}); set_imm(n); }
	x64_imul(x64_reg32 dst, x64_reg32 src, int32_t n) { reg_reg(src, dst, {0x69}); set_imm(n); }
	x64_imul(x64_reg16 dst, x64_reg16 src, int16_t n) { reg_reg(src, dst, {0x69}); set_imm(n); }





	/* register pointer + 8 bit offset into register */
	x64_imul(x64_reg64 reg, x64_reg_ptr64 addr, int8_t off, int8_t n) { reg_reg_ptr_off(reg, addr, {0x6b}, 1, off);  set_imm(n); }
	x64_imul(x64_reg32 reg, x64_reg_ptr64 addr, int8_t off, int8_t n) { reg_reg_ptr_off(reg, addr, {0x6b}, 1, off);  set_imm(n); }
	x64_imul(x64_reg16 reg, x64_reg_ptr64 addr, int8_t off, int8_t n) { reg_reg_ptr_off(reg, addr, {0x6b}, 1, off);  set_imm(n); }

	x64_imul(x64_reg64 reg, x64_reg_ptr32 addr, int8_t off, int8_t n) { reg_reg_ptr_off(reg, addr, {0x6b}, 1, off);  set_imm(n); }
	x64_imul(x64_reg32 reg, x64_reg_ptr32 addr, int8_t off, int8_t n) { reg_reg_ptr_off(reg, addr, {0x6b}, 1, off);  set_imm(n); }
	x64_imul(x64_reg16 reg, x64_reg_ptr32 addr, int8_t off, int8_t n) { reg_reg_ptr_off(reg, addr, {0x6b}, 1, off);  set_imm(n); }

	x64_imul(x64_reg64 reg, x64_reg_ptr64 addr, int8_t off, int32_t n) { reg_reg_ptr_off(reg, addr, {0x69}, 1, off);  set_imm(n); }
	x64_imul(x64_reg32 reg, x64_reg_ptr64 addr, int8_t off, int32_t n) { reg_reg_ptr_off(reg, addr, {0x69}, 1, off);  set_imm(n); }
	x64_imul(x64_reg16 reg, x64_reg_ptr64 addr, int8_t off, int32_t n) { reg_reg_ptr_off(reg, addr, {0x69}, 1, off);  set_imm(n); }

	x64_imul(x64_reg64 reg, x64_reg_ptr32 addr, int8_t off, int32_t n) { reg_reg_ptr_off(reg, addr, {0x69}, 1, off);  set_imm(n); }
	x64_imul(x64_reg32 reg, x64_reg_ptr32 addr, int8_t off, int32_t n) { reg_reg_ptr_off(reg, addr, {0x69}, 1, off);  set_imm(n); }
	x64_imul(x64_reg16 reg, x64_reg_ptr32 addr, int8_t off, int32_t n) { reg_reg_ptr_off(reg, addr, {0x69}, 1, off);  set_imm(n); }
//
//	/* register pointer + 32 bit offset into register */
//	x64_imul(x64_reg64 reg, x64_reg_ptr64 addr, int32_t off) { reg_reg_ptr_off(reg, addr, {std::forward<uint8_t>(Prefixes)..., D}, 2, off); }
//	x64_imul(x64_reg32 reg, x64_reg_ptr64 addr, int32_t off) { reg_reg_ptr_off(reg, addr, {std::forward<uint8_t>(Prefixes)..., D}, 2, off); }
//	x64_imul(x64_reg16 reg, x64_reg_ptr64 addr, int32_t off) { reg_reg_ptr_off(reg, addr, {std::forward<uint8_t>(Prefixes)..., D}, 2, off); }
//
//	x64_imul(x64_reg64 reg, x64_reg_ptr32 addr, int32_t off) { reg_reg_ptr_off(reg, addr, {std::forward<uint8_t>(Prefixes)..., D}, 2, off); }
//	x64_imul(x64_reg32 reg, x64_reg_ptr32 addr, int32_t off) { reg_reg_ptr_off(reg, addr, {std::forward<uint8_t>(Prefixes)..., D}, 2, off); }
//	x64_imul(x64_reg16 reg, x64_reg_ptr32 addr, int32_t off) { reg_reg_ptr_off(reg, addr, {std::forward<uint8_t>(Prefixes)..., D}, 2, off); }
//
//	/* immediate address into register */
//	/* 32 bit pointers */
//	x64_imul(x64_reg64 reg, x64_addr_ptr<int32_t> addr) { reg_imm_addr(reg, addr, {std::forward<uint8_t>(Prefixes)..., D}); }
//	x64_imul(x64_reg32 reg, x64_addr_ptr<int32_t> addr) { reg_imm_addr(reg, addr, {std::forward<uint8_t>(Prefixes)..., D}); }
//	x64_imul(x64_reg16 reg, x64_addr_ptr<int32_t> addr) { reg_imm_addr(reg, addr, {std::forward<uint8_t>(Prefixes)..., D}); }
//
//	/* 64 bit base address + index * scale into reg */
//	x64_imul(x64_reg64 reg, x64_reg_ptr64 addr, x64_reg64 index, x64_sib_scale scale) { reg_reg_ptr_idx(reg, addr, index, scale, {std::forward<uint8_t>(Prefixes)..., D}); }
//	x64_imul(x64_reg32 reg, x64_reg_ptr64 addr, x64_reg64 index, x64_sib_scale scale) { reg_reg_ptr_idx(reg, addr, index, scale, {std::forward<uint8_t>(Prefixes)..., D}); }
//	x64_imul(x64_reg16 reg, x64_reg_ptr64 addr, x64_reg64 index, x64_sib_scale scale) { reg_reg_ptr_idx(reg, addr, index, scale, {std::forward<uint8_t>(Prefixes)..., D}); }
//
//	/* 32 bit base address + index * scale into reg */
//	x64_imul(x64_reg64 reg, x64_reg_ptr32 addr, x64_reg32 index, x64_sib_scale scale) { reg_reg_ptr_idx(reg, addr, index, scale, {std::forward<uint8_t>(Prefixes)..., D}); }
//	x64_imul(x64_reg32 reg, x64_reg_ptr32 addr, x64_reg32 index, x64_sib_scale scale) { reg_reg_ptr_idx(reg, addr, index, scale, {std::forward<uint8_t>(Prefixes)..., D}); }
//	x64_imul(x64_reg16 reg, x64_reg_ptr32 addr, x64_reg32 index, x64_sib_scale scale) { reg_reg_ptr_idx(reg, addr, index, scale, {std::forward<uint8_t>(Prefixes)..., D}); }
//
//
//	/* 64 bit base address + index * scale + 8 bit offset into reg */
//	x64_imul(x64_reg64 reg, x64_reg_ptr64 addr, x64_reg64 index, x64_sib_scale scale, int8_t off) { reg_reg_ptr_idx_off(reg, addr, index, scale, off, {std::forward<uint8_t>(Prefixes)..., D}, 1); }
//	x64_imul(x64_reg32 reg, x64_reg_ptr64 addr, x64_reg64 index, x64_sib_scale scale, int8_t off) { reg_reg_ptr_idx_off(reg, addr, index, scale, off, {std::forward<uint8_t>(Prefixes)..., D}, 1); }
//	x64_imul(x64_reg16 reg, x64_reg_ptr64 addr, x64_reg64 index, x64_sib_scale scale, int8_t off) { reg_reg_ptr_idx_off(reg, addr, index, scale, off, {std::forward<uint8_t>(Prefixes)..., D}, 1); }
//
//	/* 32 bit base address + index * scale + 8 bit offset  into reg */
//	x64_imul(x64_reg64 reg, x64_reg_ptr32 addr, x64_reg32 index, x64_sib_scale scale, int8_t off) { reg_reg_ptr_idx_off(reg, addr, index, scale, off, {std::forward<uint8_t>(Prefixes)..., D}, 1); }
//	x64_imul(x64_reg32 reg, x64_reg_ptr32 addr, x64_reg32 index, x64_sib_scale scale, int8_t off) { reg_reg_ptr_idx_off(reg, addr, index, scale, off, {std::forward<uint8_t>(Prefixes)..., D}, 1); }
//	x64_imul(x64_reg16 reg, x64_reg_ptr32 addr, x64_reg32 index, x64_sib_scale scale, int8_t off) { reg_reg_ptr_idx_off(reg, addr, index, scale, off, {std::forward<uint8_t>(Prefixes)..., D}, 1); }
//
//	/* 64 bit base address + index * scale + 32 bit offset into reg */
//	x64_imul(x64_reg64 reg, x64_reg_ptr64 addr, x64_reg64 index, x64_sib_scale scale, int32_t off) { reg_reg_ptr_idx_off(reg, addr, index, scale, off, {std::forward<uint8_t>(Prefixes)..., D}, 2); }
//	x64_imul(x64_reg32 reg, x64_reg_ptr64 addr, x64_reg64 index, x64_sib_scale scale, int32_t off) { reg_reg_ptr_idx_off(reg, addr, index, scale, off, {std::forward<uint8_t>(Prefixes)..., D}, 2); }
//	x64_imul(x64_reg16 reg, x64_reg_ptr64 addr, x64_reg64 index, x64_sib_scale scale, int32_t off) { reg_reg_ptr_idx_off(reg, addr, index, scale, off, {std::forward<uint8_t>(Prefixes)..., D}, 2); }
//
//	/* 32 bit base address + index * scale + 32 bit offset  into reg */
//	x64_imul(x64_reg64 reg, x64_reg_ptr32 addr, x64_reg32 index, x64_sib_scale scale, int32_t off) { reg_reg_ptr_idx_off(reg, addr, index, scale, off, {std::forward<uint8_t>(Prefixes)..., D}, 2); }
//	x64_imul(x64_reg32 reg, x64_reg_ptr32 addr, x64_reg32 index, x64_sib_scale scale, int32_t off) { reg_reg_ptr_idx_off(reg, addr, index, scale, off, {std::forward<uint8_t>(Prefixes)..., D}, 2); }
//	x64_imul(x64_reg16 reg, x64_reg_ptr32 addr, x64_reg32 index, x64_sib_scale scale, int32_t off) { reg_reg_ptr_idx_off(reg, addr, index, scale, off, {std::forward<uint8_t>(Prefixes)..., D}, 2); }
//
//
	/* Imul doesn't support these 8-bit operations */
    x64_imul(x64_reg8  dst, x64_reg8  src) = delete;
    x64_imul(x64_reg8h dst, x64_reg8  src) = delete;
    x64_imul(x64_reg8  dst, x64_reg8h src) = delete;
    x64_imul(x64_reg8h dst, x64_reg8h src) = delete;
    x64_imul(x64_reg8  reg, x64_reg_ptr64 addr) = delete;
    x64_imul(x64_reg8h reg, x64_reg_ptr64 addr) = delete;
    x64_imul(x64_reg8  reg, x64_reg_ptr32 addr) = delete;
    x64_imul(x64_reg8h reg, x64_reg_ptr32 addr) = delete;
    x64_imul(x64_reg8  reg, x64_reg_ptr64 addr, int8_t off) = delete;
    x64_imul(x64_reg8h reg, x64_reg_ptr64 addr, int8_t off) = delete;
    x64_imul(x64_reg8  reg, x64_reg_ptr32 addr, int8_t off) = delete;
    x64_imul(x64_reg8h reg, x64_reg_ptr32 addr, int8_t off) = delete;
    x64_imul(x64_reg8  reg, x64_reg_ptr64 addr, int32_t off) = delete;
    x64_imul(x64_reg8h reg, x64_reg_ptr64 addr, int32_t off) = delete;
    x64_imul(x64_reg8  reg, x64_reg_ptr32 addr, int32_t off) = delete;
    x64_imul(x64_reg8h reg, x64_reg_ptr32 addr, int32_t off) = delete;
    x64_imul(x64_reg8  reg, x64_addr_ptr<int32_t> addr) = delete;
    x64_imul(x64_reg8h reg, x64_addr_ptr<int32_t> addr) = delete;
    x64_imul(x64_reg8  reg, x64_reg_ptr64 addr, x64_reg64 index, x64_sib_scale scale) = delete;
    x64_imul(x64_reg8h reg, x64_reg_ptr64 addr, x64_reg64 index, x64_sib_scale scale) = delete;
    x64_imul(x64_reg8  reg, x64_reg_ptr32 addr, x64_reg32 index, x64_sib_scale scale) = delete;
    x64_imul(x64_reg8h reg, x64_reg_ptr32 addr, x64_reg32 index, x64_sib_scale scale) = delete;
    x64_imul(x64_reg8  reg, x64_reg_ptr64 addr, x64_reg64 index, x64_sib_scale scale, int8_t off) = delete;
    x64_imul(x64_reg8h reg, x64_reg_ptr64 addr, x64_reg64 index, x64_sib_scale scale, int8_t off) = delete;
    x64_imul(x64_reg8  reg, x64_reg_ptr32 addr, x64_reg32 index, x64_sib_scale scale, int8_t off) = delete;
    x64_imul(x64_reg8h reg, x64_reg_ptr32 addr, x64_reg32 index, x64_sib_scale scale, int8_t off) = delete;
    x64_imul(x64_reg8  reg, x64_reg_ptr64 addr, x64_reg64 index, x64_sib_scale scale, int32_t off) = delete;
    x64_imul(x64_reg8h reg, x64_reg_ptr64 addr, x64_reg64 index, x64_sib_scale scale, int32_t off) = delete;
    x64_imul(x64_reg8  reg, x64_reg_ptr32 addr, x64_reg32 index, x64_sib_scale scale, int32_t off) = delete;
    x64_imul(x64_reg8h reg, x64_reg_ptr32 addr, x64_reg32 index, x64_sib_scale scale, int32_t off) = delete;
};

template<uint8_t A>
struct x64_pushpop : x64_instruction
{
	x64_pushpop(x64_reg64 reg) { x64_add_rex(reg, x64_reg64(0)); add_opcode((reg.value & 7) | A); }
	x64_pushpop(x64_reg16 reg) : x64_instruction({ x64_override::oper_size, static_cast<uint8_t>(reg.value | A) } ) { }
};

struct x64_push : public x64_pushpop<0x50> { using x64_pushpop::x64_pushpop; };
struct x64_pop : public x64_pushpop<0x58> { using x64_pushpop::x64_pushpop; };

#endif /* X64INSTRUCTION_H_ */
