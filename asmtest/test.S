mov (%rax),%r15
mov (%r15),%rax
mov (%r15),%r15
mov (%rax),%ebx
mov (%r15),%ebx
mov (%rax),%cx
mov (%r15),%cx
mov (%rax),%cl
mov (%r15),%cl
mov (%rax),%ch
mov (%eax),%r15
mov (%ebx),%ebx
mov (%ecx),%ebx
mov (%edx),%cx
mov (%edi),%cx
mov (%edi),%cl
mov (%edi),%cl
mov (%edi),%ch

nop
nop


mov 0x7f(%rax),%eax
mov 8(%eax, %eax, 8),%eax
