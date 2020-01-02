cut -d '{' -f1 | grep -v "/\*" | grep . |
	sed "s/x64_reg64_0 *[a-z|A-Z]*/x64_regs::rax/g" |
       	sed "s/x64_reg32_0 *[a-z|A-Z]*/x64_regs::eax/g" |
       	sed "s/x64_reg16_0 *[a-z|A-Z]*/x64_regs::ax/g" |
       	sed "s/x64_reg8l_0 *[a-z|A-Z]*/x64_regs::al/g" |
       	sed "s/x64_reg8_0 *[a-z|A-Z]*/x64_regs::ah/g" |
       	sed "s/x64_reg64 *[a-z|A-Z]*/x64_regs::rax/g" | 
	sed "s/x64_reg32 *[a-z|A-Z]*/x64_regs::eax/g" |
       	sed "s/x64_reg16 *[a-z|A-Z]*/x64_regs::ax/g" |
       	sed "s/x64_reg8l *[a-z|A-Z]*/x64_regs::al/g" |
       	sed "s/x64_reg8 *[a-z|A-Z]*/x64_regs::ah/g" |
      	sed "s/x64_reg_ptr64 *[a-z|A-Z]*/x64_reg_ptr64(x64_regs::rax)/g" |
       	sed "s/x64_reg_ptr32 *[a-z|A-Z]*/x64_reg_ptr32(x64_regs::eax)/g" |
       	sed "s/sib_scale *[a-z|A-Z]*/sib_scale::s2/g" |
	sed -E "s/, uint8_t *[a-z|A-Z]*/, (uint8_t) 0x42/g" |
	sed -E "s/, uint16_t *[a-z|A-Z]*/, (uint16_t) 0x42/g" |
	sed -E "s/, uint32_t *[a-z|A-Z]*/, (uint32_t) 0x42/g" |
	sed -E "s/, uint64_t *[a-z|A-Z]*/, (uint64_t) 0x42/g" |
	sed -E "s/, int8_t *[a-z|A-Z]*/, (int8_t) 0x42/g" |
	sed -E "s/, int16_t *[a-z|A-Z]*/, (int16_t) 0x42/g" |
	sed -E "s/, int32_t *[a-z|A-Z]*/, (int32_t) 0x42/g" |
	sed -E "s/, int64_t *[a-z|A-Z]*/, (int64_t) 0x42/g" |
        sed "s/x64_addr_ptr<int32_t> *[a-z|A-Z]*/x64_addr_ptr<int32_t>(0x12345678)/g" |
       	sed "s/x64_addr_ptr<uint64_t\*> *[a-z|A-Z]*/x64_addr_ptr<uint64_t\*>((uint64_t\*)0x1234567812345678)/g" |
       	sed "s/x64_addr_ptr<uint32_t\*> *[a-z|A-Z]*/x64_addr_ptr<uint32_t\*>((uint32_t\*)0x1234567812345678)/g" |
       	sed "s/x64_addr_ptr<uint16_t\*> *[a-z|A-Z]*/x64_addr_ptr<uint16_t\*>((uint16_t\*)0x1234567812345678)/g" |
       	sed "s/x64_addr_ptr<uint8_t\*> *[a-z|A-Z]*/x64_addr_ptr<uint8_t\*>((uint8_t\*)0x1234567812345678)/g" |
	while read A ; do
		echo "s << $A;"
	done

