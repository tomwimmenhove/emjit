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

	// XXX: This is a bit of a hack to make ah, bh. ch and dh a little easier to encode
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

class x64_address
{
public:
	explicit x64_address(int32_t  addr) : addr_size(sizeof(addr)), imm_addr(addr) { }
	explicit x64_address(x64_reg64 addr) : addr_reg(addr.value), addr_size(sizeof(x64_reg64::value_type)) { }
	explicit x64_address(x64_reg32 addr) : addr_reg(addr.value), addr_size(sizeof(x64_reg32::value_type)) { }
	explicit x64_address(x64_reg64 addr, int8_t  offset) : addr_reg(addr.value), addr_size(sizeof(x64_reg64::value_type)), offs(offset), offs_size(sizeof(offset)) { }
	explicit x64_address(x64_reg32 addr, int8_t  offset) : addr_reg(addr.value), addr_size(sizeof(x64_reg32::value_type)), offs(offset), offs_size(sizeof(offset)) { }
	explicit x64_address(x64_reg64 addr, int32_t offset) : addr_reg(addr.value), addr_size(sizeof(x64_reg64::value_type)), offs(offset), offs_size(sizeof(offset)) { }
	explicit x64_address(x64_reg32 addr, int32_t offset) : addr_reg(addr.value), addr_size(sizeof(x64_reg32::value_type)), offs(offset), offs_size(sizeof(offset)) { }
	explicit x64_address(x64_reg64 addr, x64_reg64 index, int8_t  offset) : addr_reg(addr.value), addr_size(sizeof(x64_reg64::value_type)), idx_reg(index.value), offs(offset), offs_size(sizeof(offset)) { }
	explicit x64_address(x64_reg32 addr, x64_reg32 index, int8_t  offset) : addr_reg(addr.value), addr_size(sizeof(x64_reg32::value_type)), idx_reg(index.value), offs(offset), offs_size(sizeof(offset)) { }
	explicit x64_address(x64_reg64 addr, x64_reg64 index, int32_t offset) : addr_reg(addr.value), addr_size(sizeof(x64_reg64::value_type)), idx_reg(index.value), offs(offset), offs_size(sizeof(offset)) { }
	explicit x64_address(x64_reg32 addr, x64_reg32 index, int32_t offset) : addr_reg(addr.value), addr_size(sizeof(x64_reg32::value_type)), idx_reg(index.value), offs(offset), offs_size(sizeof(offset)) { }
	explicit x64_address(x64_reg64 addr, x64_reg64 index, x64_sib_scale scale = x64_sib_scale::s1) : addr_reg(addr.value), addr_size(sizeof(x64_reg64::value_type)), idx_reg(index.value), scl(scale) { }
	explicit x64_address(x64_reg32 addr, x64_reg32 index, x64_sib_scale scale = x64_sib_scale::s1) : addr_reg(addr.value), addr_size(sizeof(x64_reg32::value_type)), idx_reg(index.value), scl(scale) { }
	explicit x64_address(x64_reg64 addr, x64_reg64 index, x64_sib_scale scale, int8_t  offset) : addr_reg(addr.value), addr_size(sizeof(x64_reg64::value_type)), idx_reg(index.value), scl(scale), offs(offset), offs_size(sizeof(offset)) { }
	explicit x64_address(x64_reg32 addr, x64_reg32 index, x64_sib_scale scale, int8_t  offset) : addr_reg(addr.value), addr_size(sizeof(x64_reg32::value_type)), idx_reg(index.value), scl(scale), offs(offset), offs_size(sizeof(offset)) { }
	explicit x64_address(x64_reg64 addr, x64_reg64 index, x64_sib_scale scale, int32_t offset) : addr_reg(addr.value), addr_size(sizeof(x64_reg64::value_type)), idx_reg(index.value), scl(scale), offs(offset), offs_size(sizeof(offset)) { }
	explicit x64_address(x64_reg32 addr, x64_reg32 index, x64_sib_scale scale, int32_t offset) : addr_reg(addr.value), addr_size(sizeof(x64_reg32::value_type)), idx_reg(index.value), scl(scale), offs(offset), offs_size(sizeof(offset)) { }

	inline bool is_immediate() const { return idx_reg == -1; }
	inline int32_t immediate() const { return imm_addr; }

	inline x64_reg_base_normal adddress_reg() const { return x64_reg_base_normal(addr_reg); };
	inline x64_reg32 adddress_reg32() const { return x64_reg32(addr_reg); };
	inline x64_reg64 adddress_reg64() const { return x64_reg64(addr_reg); };

	inline int address_size() const { return addr_size; }

	inline bool has_index() const { return idx_reg != -1; }

	inline x64_reg_base_normal index_reg() const { return x64_reg_base_normal(idx_reg); };
	inline x64_reg32 index_reg32() const { return x64_reg32(idx_reg); };
	inline x64_reg64 index_reg64() const { return x64_reg64(idx_reg); };
	
	inline x64_sib_scale scale() const { return scl; }
	
	inline bool has_offset() const { return offs_size != 0; }
	inline int offset_size() const { return offs_size; }
	inline int32_t offset() const { return offs; }
	
private:
	int addr_reg = -1;
	int addr_size;
	int idx_reg = -1;
	x64_sib_scale scl = x64_sib_scale::s1;
	int32_t offs = 0;
	int offs_size = 0;
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
	x64_instruction(IMM_TYPE  imm)
	: imm_size(sizeof(IMM_TYPE)),  imm64(static_cast<uint64_t>(from_or_to_little(imm))) { }

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

		if (rex) add_opcode(rex);
	}
	template<typename T, typename U>
	inline void x64_add_rex(const T& a, const U& b, size_t oper_size = sizeof(int32_t))
	{ x64_add_rex(a, T(0), b, oper_size); }

	template<typename T>
	void set_x64_reg_address(T reg, const x64_address& address, std::initializer_list<uint8_t> oc)
	{
		/* Add prefixes */
		if (address.address_size() == sizeof(uint32_t))
			add_opcode(x64_override::addr_size);

		if (sizeof(typename T::value_type) == sizeof(uint16_t))
			add_opcode(x64_override::oper_size);

		x64_add_rex(address.adddress_reg(), address.index_reg(), reg, sizeof(typename T::value_type));

		/* Set the opcode */
		add_opcode(oc);

		/* Get the offset and it's size */
		int32_t offset;
		int offset_size;
		bool use_offset = address.has_offset();
		if (use_offset)
		{
			offset = address.offset();
			offset_size = address.offset_size();
		}
		else if (address.adddress_reg().is_bp() || address.adddress_reg().is_r13())
		{
			/* rbp and r13 can't be encoded without an offset */
			use_offset = true;
			offset = 0;
			offset_size = sizeof(int8_t);
		}

		/* Set the Mod-field according to the presence and size of the immediate offset */
		uint8_t mod = 0;
		if (use_offset)
		{
			if (offset_size == sizeof(int8_t))
			{
				mod = 1;
				set_imm(static_cast<int8_t>(offset));
			}
			else
			{
				mod = 2;
				set_imm(offset);
			}
		}

		if (address.has_index())
		{
			if (address.index_reg().is_sp())
				throw std::invalid_argument("Stack pointer cannot be used as index register");

			/* Encode address and index register in the SIB byte */
			set_modrm(x64_modrm{4, reg, mod});
			set_sib(x64_sib(address.adddress_reg(), address.index_reg(), address.scale()));
		}
		else
		{
			/* If the instruction has no index register, we can
			 * encode address and index register in the ModRM byte */
			set_modrm(x64_modrm{address.adddress_reg(), reg, mod});

			/* Mandatory SIB byte for rsp and r12 */
			if (address.adddress_reg().is_sp() || address.adddress_reg().is_r12())
				set_sib(0x24);
		}
	}

	void single_reg64(const x64_reg64& reg, uint8_t b, std::initializer_list<uint8_t> oc, uint8_t mod)
	{
		x64_add_rex(reg, x64_reg32(0));
		reg_reg_oc_mod(reg, x64_reg32(b), oc, mod);
	}

	template<typename T>
	void single_reg(const T& reg, uint8_t b, std::initializer_list<uint8_t> oc, uint8_t mod)
	{
		add_prefixes(reg, T(0));
		reg_reg_oc_mod(reg, x64_reg32(b), oc, mod);
	}

	template<typename T>
	void reg_reg(T a, T b, std::initializer_list<uint8_t> oc)
	{
		add_prefixes(a, b);
		reg_reg_oc_mod(a, b, oc, 3);
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
class x64_srcdst_oper_base : public x64_instruction
{
public:
	using x64_instruction::x64_instruction;

	/* register into register */
	x64_srcdst_oper_base(x64_reg64 dst, x64_reg64 src) { reg_reg(dst, src, {std::forward<uint8_t>(Prefixes)..., B}); }
	x64_srcdst_oper_base(x64_reg32 dst, x64_reg32 src) { reg_reg(dst, src, {std::forward<uint8_t>(Prefixes)..., B}); }
	x64_srcdst_oper_base(x64_reg16 dst, x64_reg16 src) { reg_reg(dst, src, {std::forward<uint8_t>(Prefixes)..., B}); }
	x64_srcdst_oper_base(x64_reg8  dst, x64_reg8  src) { reg_reg(dst, src, {std::forward<uint8_t>(Prefixes)..., A}); }
	x64_srcdst_oper_base(x64_reg8h dst, x64_reg8  src) { if (src.add_rex()) throw no_rex_exception(dst) ; reg_reg(dst.shift(), x64_reg8h(src), {std::forward<uint8_t>(Prefixes)..., A}); }
	x64_srcdst_oper_base(x64_reg8  dst, x64_reg8h src) { if (dst.add_rex()) throw no_rex_exception(src) ; reg_reg(x64_reg8h(dst), src.shift(), {std::forward<uint8_t>(Prefixes)..., A}); }
	x64_srcdst_oper_base(x64_reg8h dst, x64_reg8h src) { reg_reg(dst.shift(), src.shift(), {std::forward<uint8_t>(Prefixes)..., A}); }

	/* memory into register */
	x64_srcdst_oper_base(x64_reg64 dst, const x64_address& src) { set_x64_reg_address(dst, src, {std::forward<uint8_t>(Prefixes)..., D}); }
	x64_srcdst_oper_base(x64_reg32 dst, const x64_address& src) { set_x64_reg_address(dst, src, {std::forward<uint8_t>(Prefixes)..., D}); }
	x64_srcdst_oper_base(x64_reg16 dst, const x64_address& src) { set_x64_reg_address(dst, src, {std::forward<uint8_t>(Prefixes)..., D}); }
	x64_srcdst_oper_base(x64_reg8  dst, const x64_address& src) { set_x64_reg_address(dst, src, {std::forward<uint8_t>(Prefixes)..., C}); }
	x64_srcdst_oper_base(x64_reg8h dst, const x64_address& src) { set_x64_reg_address(dst.shift(), src, {std::forward<uint8_t>(Prefixes)..., C}); }

	/* register into memory */
	x64_srcdst_oper_base(const x64_address& dst, x64_reg64 src) { set_x64_reg_address(src, dst, {std::forward<uint8_t>(Prefixes)..., B}); }
	x64_srcdst_oper_base(const x64_address& dst, x64_reg32 src) { set_x64_reg_address(src, dst, {std::forward<uint8_t>(Prefixes)..., B}); }
	x64_srcdst_oper_base(const x64_address& dst, x64_reg16 src) { set_x64_reg_address(src, dst, {std::forward<uint8_t>(Prefixes)..., B}); }
	x64_srcdst_oper_base(const x64_address& dst, x64_reg8  src) { set_x64_reg_address(src, dst, {std::forward<uint8_t>(Prefixes)..., A}); }
	x64_srcdst_oper_base(const x64_address& dst, x64_reg8h src) { set_x64_reg_address(src.shift(), dst, {std::forward<uint8_t>(Prefixes)..., A}); }
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
	x64_jmpcall_base(const x64_address& src) { set_x64_reg_address(x64_reg32(C), src, {std::forward<uint8_t>(Prefixes)..., A}); }
	x64_jmpcall_base(int32_t off) : x64_instruction( {std::forward<uint8_t>(Prefixes)..., B}, off) { }
	x64_jmpcall_base(x64_reg64 reg) { single_reg64(reg, C, {std::forward<uint8_t>(Prefixes)..., A}, 3); }
};

template<uint8_t A, uint8_t B, uint8_t C, uint8_t... Prefixes>
struct x64_single_op_base : x64_instruction
{
	using x64_instruction::x64_instruction;

	x64_single_op_base(x64_reg64 reg) { single_reg(reg, C, {std::forward<uint8_t>(Prefixes)..., B}, 3); }
	x64_single_op_base(x64_reg32 reg) { single_reg(reg, C, {std::forward<uint8_t>(Prefixes)..., B}, 3); }
	x64_single_op_base(x64_reg16 reg) { single_reg(reg, C, {std::forward<uint8_t>(Prefixes)..., B}, 3); }
	x64_single_op_base(x64_reg8  reg) { single_reg(reg, C, {std::forward<uint8_t>(Prefixes)..., A}, 3); }
	x64_single_op_base(x64_reg8h reg) { single_reg(reg.shift(), C, {std::forward<uint8_t>(Prefixes)..., A}, 3); }
};

/********** ACTUAL INSTRUCTIONS FOLLOW **********/

struct x64_int3	: public x64_instruction{x64_int3()	: x64_instruction({ 0xcc }) {} };
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
struct x64_jne  : x64_jmp_cond { x64_jne(int8_t off)   : x64_jmp_cond(x64_cond::not_equal, off) { } };
struct x64_jnz  : x64_jmp_cond { x64_jnz(int8_t off)   : x64_jmp_cond(x64_cond::not_zero, off) { } };
struct x64_jb   : x64_jmp_cond { x64_jb(int8_t off)   : x64_jmp_cond(x64_cond::below, off) { } };
struct x64_jnae : x64_jmp_cond { x64_jnae(int8_t off) : x64_jmp_cond(x64_cond::not_above_or_equal, off) { } };
struct x64_jc   : x64_jmp_cond { x64_jc(int8_t off)    : x64_jmp_cond(x64_cond::carry, off) { } };
struct x64_jnb  : x64_jmp_cond { x64_jnb(int8_t off)   : x64_jmp_cond(x64_cond::not_below, off) { } };
struct x64_jae  : x64_jmp_cond { x64_jae(int8_t off)   : x64_jmp_cond(x64_cond::above_or_equal, off) { } };
struct x64_jnc  : x64_jmp_cond { x64_jnc(int8_t off)  : x64_jmp_cond(x64_cond::not_carry, off) { } };
struct x64_jbe  : x64_jmp_cond { x64_jbe(int8_t off)   : x64_jmp_cond(x64_cond::below_or_equal, off) { } };
struct x64_jna  : x64_jmp_cond { x64_jna(int8_t off)   : x64_jmp_cond(x64_cond::not_above, off) { } };
struct x64_ja   : x64_jmp_cond { x64_ja(int8_t off)   : x64_jmp_cond(x64_cond::above, off) { } };
struct x64_jnbe : x64_jmp_cond { x64_jnbe(int8_t off)  : x64_jmp_cond(x64_cond::not_below_or_equal, off) { } };
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
	x64_mov(x64_reg64_0 reg, uint64_t* addr) { eax_imm(reg, reinterpret_cast<uint64_t>(addr), {0xa1}); }
	x64_mov(x64_reg32_0 reg, uint32_t* addr) { eax_imm(reg, reinterpret_cast<uint64_t>(addr), {0xa1}); }
	x64_mov(x64_reg16_0 reg, uint16_t* addr) { eax_imm(reg, reinterpret_cast<uint64_t>(addr), {0xa1}); }
	x64_mov(x64_reg8_0  reg, uint8_t*  addr) { eax_imm(reg, reinterpret_cast<uint64_t>(addr), {0xa0}); }
	x64_mov(x64_reg64_0 reg, int64_t*  addr) { eax_imm(reg, reinterpret_cast<uint64_t>(addr), {0xa1}); }
	x64_mov(x64_reg32_0 reg, int32_t*  addr) { eax_imm(reg, reinterpret_cast<uint64_t>(addr), {0xa1}); }
	x64_mov(x64_reg16_0 reg, int16_t*  addr) { eax_imm(reg, reinterpret_cast<uint64_t>(addr), {0xa1}); }
	x64_mov(x64_reg8_0  reg, int8_t*   addr) { eax_imm(reg, reinterpret_cast<uint64_t>(addr), {0xa0}); }

	/* Move register into immediate address */
	/* 64 bit pointers */
	x64_mov(uint64_t* addr, x64_reg64_0 reg) { eax_imm(reg, reinterpret_cast<uint64_t>(addr), {0xa3}); }
	x64_mov(uint32_t* addr, x64_reg32_0 reg) { eax_imm(reg, reinterpret_cast<uint64_t>(addr), {0xa3}); }
	x64_mov(uint16_t* addr, x64_reg16_0 reg) { eax_imm(reg, reinterpret_cast<uint64_t>(addr), {0xa3}); }
	x64_mov(uint8_t*  addr, x64_reg8_0 reg)  { eax_imm(reg, reinterpret_cast<uint64_t>(addr), {0xa2}); }
	x64_mov(int64_t*  addr, x64_reg64_0 reg) { eax_imm(reg, reinterpret_cast<uint64_t>(addr), {0xa3}); }
	x64_mov(int32_t*  addr, x64_reg32_0 reg) { eax_imm(reg, reinterpret_cast<uint64_t>(addr), {0xa3}); }
	x64_mov(int16_t*  addr, x64_reg16_0 reg) { eax_imm(reg, reinterpret_cast<uint64_t>(addr), {0xa3}); }
	x64_mov(int8_t*   addr, x64_reg8_0 reg)  { eax_imm(reg, reinterpret_cast<uint64_t>(addr), {0xa2}); }

	virtual ~x64_mov() { }
};


struct x64_add: public x64_arith_base<0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0> { using x64_arith_base::x64_arith_base; };
struct x64_or : public x64_arith_base<0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 1> { using x64_arith_base::x64_arith_base; };
struct x64_adc: public x64_arith_base<0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 2> { using x64_arith_base::x64_arith_base; };
struct x64_sbb: public x64_arith_base<0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 3> { using x64_arith_base::x64_arith_base; };
struct x64_and: public x64_arith_base<0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 4> { using x64_arith_base::x64_arith_base; };
struct x64_sub: public x64_arith_base<0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 5> { using x64_arith_base::x64_arith_base; };
struct x64_xor: public x64_arith_base<0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 6> { using x64_arith_base::x64_arith_base; };
struct x64_cmp: public x64_arith_base<0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 7> { using x64_arith_base::x64_arith_base; };

struct x64_inc   : public x64_single_op_base<0xfe, 0xff, 0> { using x64_single_op_base::x64_single_op_base; };
struct x64_incq  : public x64_inc { using x64_inc::x64_inc; x64_incq(const x64_address& src) { set_x64_reg_address(x64_reg64(0), src, {0xff}); } };
struct x64_incl  : public x64_inc { using x64_inc::x64_inc; x64_incl(const x64_address& src) { set_x64_reg_address(x64_reg32(0), src, {0xff}); } };
struct x64_incw  : public x64_inc { using x64_inc::x64_inc; x64_incw(const x64_address& src) { set_x64_reg_address(x64_reg16(0), src, {0xff}); } };
struct x64_incb  : public x64_inc { using x64_inc::x64_inc; x64_incb(const x64_address& src) { set_x64_reg_address(x64_reg8h(0), src, {0xfe}); } };

struct x64_dec   : public x64_single_op_base<0xfe, 0xff, 1> { using x64_single_op_base::x64_single_op_base; };
struct x64_decq  : public x64_dec { using x64_dec::x64_dec; x64_decq(const x64_address& src) { set_x64_reg_address(x64_reg64(1), src, {0xff}); } };
struct x64_decl  : public x64_dec { using x64_dec::x64_dec; x64_decl(const x64_address& src) { set_x64_reg_address(x64_reg32(1), src, {0xff}); } };
struct x64_decw  : public x64_dec { using x64_dec::x64_dec; x64_decw(const x64_address& src) { set_x64_reg_address(x64_reg16(1), src, {0xff}); } };
struct x64_decb  : public x64_dec { using x64_dec::x64_dec; x64_decb(const x64_address& src) { set_x64_reg_address(x64_reg8h(1), src, {0xfe}); } };

struct x64_neg   : public x64_single_op_base<0xf6, 0xf7, 3> { using x64_single_op_base::x64_single_op_base; };
struct x64_negq  : public x64_neg { using x64_neg::x64_neg; x64_negq(const x64_address& src) { set_x64_reg_address(x64_reg64(3), src, {0xf7}); } };
struct x64_negl  : public x64_neg { using x64_neg::x64_neg; x64_negl(const x64_address& src) { set_x64_reg_address(x64_reg32(3), src, {0xf7}); } };
struct x64_negw  : public x64_neg { using x64_neg::x64_neg; x64_negw(const x64_address& src) { set_x64_reg_address(x64_reg16(3), src, {0xf7}); } };
struct x64_negb  : public x64_neg { using x64_neg::x64_neg; x64_negb(const x64_address& src) { set_x64_reg_address(x64_reg8h(3), src, {0xf6}); } };

struct x64_mul   : public x64_single_op_base<0xf6, 0xf7, 4> { using x64_single_op_base::x64_single_op_base; };
struct x64_mulq  : public x64_mul { using x64_mul::x64_mul; x64_mulq(const x64_address& src) { set_x64_reg_address(x64_reg64(4), src, {0xf7}); } };
struct x64_mull  : public x64_mul { using x64_mul::x64_mul; x64_mull(const x64_address& src) { set_x64_reg_address(x64_reg32(4), src, {0xf7}); } };
struct x64_mulw  : public x64_mul { using x64_mul::x64_mul; x64_mulw(const x64_address& src) { set_x64_reg_address(x64_reg16(4), src, {0xf7}); } };
struct x64_mulb  : public x64_mul { using x64_mul::x64_mul; x64_mulb(const x64_address& src) { set_x64_reg_address(x64_reg8h(4), src, {0xf6}); } };

struct x64_div   : public x64_single_op_base<0xf6, 0xf7, 6> { using x64_single_op_base::x64_single_op_base; };
struct x64_divq  : public x64_div { using x64_div::x64_div; x64_divq(const x64_address& src) { set_x64_reg_address(x64_reg64(6), src, {0xf7}); } };
struct x64_divl  : public x64_div { using x64_div::x64_div; x64_divl(const x64_address& src) { set_x64_reg_address(x64_reg32(6), src, {0xf7}); } };
struct x64_divw  : public x64_div { using x64_div::x64_div; x64_divw(const x64_address& src) { set_x64_reg_address(x64_reg16(6), src, {0xf7}); } };
struct x64_divb  : public x64_div { using x64_div::x64_div; x64_divb(const x64_address& src) { set_x64_reg_address(x64_reg8h(6), src, {0xf6}); } };

struct x64_idiv  : public x64_single_op_base<0xf6, 0xf7, 7> { using x64_single_op_base::x64_single_op_base; };
struct x64_idivq : public x64_idiv { using x64_idiv::x64_idiv; x64_idivq(const x64_address& src) { set_x64_reg_address(x64_reg64(7), src, {0xf7}); } };
struct x64_idivl : public x64_idiv { using x64_idiv::x64_idiv; x64_idivl(const x64_address& src) { set_x64_reg_address(x64_reg32(7), src, {0xf7}); } };
struct x64_idivw : public x64_idiv { using x64_idiv::x64_idiv; x64_idivw(const x64_address& src) { set_x64_reg_address(x64_reg16(7), src, {0xf7}); } };
struct x64_idivb : public x64_idiv { using x64_idiv::x64_idiv; x64_idivb(const x64_address& src) { set_x64_reg_address(x64_reg8h(7), src, {0xf6}); } };

struct x64_imul : public x64_instruction
{
	using x64_instruction::x64_instruction;

	x64_imul(x64_reg64 reg) { single_reg(reg, 5, {0xf7}, 3); }
	x64_imul(x64_reg32 reg) { single_reg(reg, 5, {0xf7}, 3); }
	x64_imul(x64_reg16 reg) { single_reg(reg, 5, {0xf7}, 3); }
	x64_imul(x64_reg8  reg) { single_reg(reg, 5, {0xf6}, 3); }
	x64_imul(x64_reg8h reg) { single_reg(reg.shift(), 5, {0xf6}, 3); }

	// XXX: What is this reversed register madness?!
	x64_imul(x64_reg64 dst, x64_reg64 src) { reg_reg(src, dst, {0x0f, 0xaf}); }
	x64_imul(x64_reg32 dst, x64_reg32 src) { reg_reg(src, dst, {0x0f, 0xaf}); }
	x64_imul(x64_reg16 dst, x64_reg16 src) { reg_reg(src, dst, {0x0f, 0xaf}); }

	x64_imul(x64_reg64 dst, x64_reg64 src, int8_t n) { reg_reg(src, dst, {0x6b}); set_imm(n); }
	x64_imul(x64_reg32 dst, x64_reg32 src, int8_t n) { reg_reg(src, dst, {0x6b}); set_imm(n); }
	x64_imul(x64_reg16 dst, x64_reg16 src, int8_t n) { reg_reg(src, dst, {0x6b}); set_imm(n); }

	x64_imul(x64_reg64 dst, x64_reg64 src, int32_t n) { reg_reg(src, dst, {0x69}); set_imm(n); }
	x64_imul(x64_reg32 dst, x64_reg32 src, int32_t n) { reg_reg(src, dst, {0x69}); set_imm(n); }
	x64_imul(x64_reg16 dst, x64_reg16 src, int16_t n) { reg_reg(src, dst, {0x69}); set_imm(n); }

	x64_imul(x64_reg64 dst, const x64_address& src) { set_x64_reg_address(dst, src, {0x0f, 0xaf}); }
	x64_imul(x64_reg32 dst, const x64_address& src) { set_x64_reg_address(dst, src, {0x0f, 0xaf}); }
	x64_imul(x64_reg16 dst, const x64_address& src) { set_x64_reg_address(dst, src, {0x0f, 0xaf}); }
};

struct x64_imulq : public x64_imul { using x64_imul::x64_imul; x64_imulq(const x64_address& src) { set_x64_reg_address(x64_reg64(5), src, {0xf7}); } };
struct x64_imull : public x64_imul { using x64_imul::x64_imul; x64_imull(const x64_address& src) { set_x64_reg_address(x64_reg32(5), src, {0xf7}); } };
struct x64_imulw : public x64_imul { using x64_imul::x64_imul; x64_imulw(const x64_address& src) { set_x64_reg_address(x64_reg16(5), src, {0xf7}); } };
struct x64_imulb : public x64_imul { using x64_imul::x64_imul; x64_imulb(const x64_address& src) { set_x64_reg_address(x64_reg8h(5), src, {0xf6}); } }; // x64_reg8h because it won't add a REX prefix


template<uint8_t A>
struct x64_pushpop : x64_instruction
{
	x64_pushpop(x64_reg64 reg) { x64_add_rex(reg, x64_reg64(0)); add_opcode((reg.value & 7) | A); }
	x64_pushpop(x64_reg16 reg) : x64_instruction({ x64_override::oper_size, static_cast<uint8_t>(reg.value | A) } ) { }
};

struct x64_push : public x64_pushpop<0x50> { using x64_pushpop::x64_pushpop; };
struct x64_pop  : public x64_pushpop<0x58> { using x64_pushpop::x64_pushpop; };

#endif /* X64INSTRUCTION_H_ */
