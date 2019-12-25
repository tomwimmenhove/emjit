
//mov    %eax,%eax
//mov    %ax,%ax

mov %rax, (%rax)
mov %rax, (%rcx)
mov %rcx, (%rax)
mov %rcx, (%rcx)

nop

mov (%rax), %rax
nop

mov %rax, (%rax)
mov %r15, (%rax)
mov %rax, (%r15)
mov %r15, (%r15)
nop

mov %ax, (%eax)
mov %cx, (%eax)
mov %ax, (%ecx)
mov %cx, (%ecx)
nop

mov %ah, (%eax)
mov %ch, (%eax)
mov %ah, (%ecx)
mov %ch, (%ecx)
nop

mov %al, (%eax)
mov %cl, (%eax)
mov %al, (%ecx)
mov %cl, (%ecx)
nop

mov %rax, (%rax)
nop

mov %rax, 1(%rax)
mov %rax, 1(%rcx)
mov %rcx, 1(%rax)
mov %rcx, 1(%rcx)


nop
nop
nop

mov %eax, (%eax)
mov %eax, 1(%eax)
mov %ecx, 1(%eax)


nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
mov %r15, (%rax)
mov %rax, (%r15)

mov %rax, (%rax)
mov %eax, (%rax)
mov %ax, (%rax)
mov %ax, (%r15)
mov %ah, (%rax)
mov %al, (%rax)

nop
mov %rax, (%eax)
mov %eax, (%eax)
mov %ax, (%eax)
mov %ah, (%eax)
mov %al, (%eax)



