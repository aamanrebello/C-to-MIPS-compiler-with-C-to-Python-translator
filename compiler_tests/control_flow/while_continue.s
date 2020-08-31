           .text
           .text
           .align 2
           .globl while_continue
           .type while_continue, @function
while_continue:
           .set noreorder
           .set nomacro
addiu      $sp, $sp, -40
sw         $31, 36($sp)
sw         $fp, 32($sp)
move       $fp, $sp
sw         $16, 20($fp)
sw         $0, 24($fp)
li         $8, 0
sw         $8, 24($fp)
nop
sw         $0, 28($fp)
li         $8, 0
sw         $8, 28($fp)
nop
b          L2
nop
L0:
lw         $8, 24($fp)
nop
addiu      $8, $8, 1
sw         $8, 24($fp)
nop
lw         $8, 24($fp)
nop
li         $9, 15
slt        $8, $9, $8
andi       $8, $8, 0x00ff
beq        $8, $0, L3
nop
b          L0
nop
L3:
lw         $8, 28($fp)
nop
addiu      $8, $8, 1
sw         $8, 28($fp)
nop
L2:
lw         $8, 24($fp)
nop
li         $9, 30
slt        $8, $8, $9
andi       $8, $8, 0x00ff
bne        $8, $0, L0
nop
L1:
lw         $8, 24($fp)
nop
lw         $9, 28($fp)
nop
addu       $8, $8, $9
move       $2, $8
b          FUN0
nop
FUN0:
lw         $16, 20($fp)
move       $sp, $fp
lw         $31, 36($sp)
lw         $fp, 32($sp)
addiu      $sp, $sp, 40
j          $31
nop

