           .text
           .text
           .align 2
           .globl f
           .type f, @function
f:
           .set noreorder
           .set nomacro
addiu      $sp, $sp, -32
sw         $31, 28($sp)
sw         $fp, 24($sp)
move       $fp, $sp
sw         $4, 32($fp)
sw         $16, 20($fp)
lw         $8, 32($fp)
nop
li         $9, 1
sll        $9, $9, 3
addu       $8, $8, $9
sw         $8, 32($fp)
nop
lw         $8, 32($fp)
nop
lwc1       $f4, 4($8)
nop
lwc1       $f5, 0($8)
nop
mov.d      $f0, $f4
b          FUN0
nop
FUN0:
lw         $16, 20($fp)
move       $sp, $fp
lw         $31, 28($sp)
lw         $fp, 24($sp)
addiu      $sp, $sp, 32
j          $31
nop

