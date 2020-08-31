           .text
           .data
            .comm x,4,4

           .text
           .align 2
           .globl getPtr
           .type getPtr, @function
getPtr:
           .set noreorder
           .set nomacro
addiu      $sp, $sp, -32
sw         $31, 28($sp)
sw         $fp, 24($sp)
move       $fp, $sp
sw         $16, 20($fp)
lui        $8, %hi(x)
addiu      $8, $8, %lo(x)
move       $2, $8
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

           .text
           .align 2
           .globl getVal
           .type getVal, @function
getVal:
           .set noreorder
           .set nomacro
addiu      $sp, $sp, -32
sw         $31, 28($sp)
sw         $fp, 24($sp)
move       $fp, $sp
sw         $16, 20($fp)
lui        $8, %hi(x)
lw         $8, %lo(x)($8)
nop
move       $2, $8
b          FUN1
nop
FUN1:
lw         $16, 20($fp)
move       $sp, $fp
lw         $31, 28($sp)
lw         $fp, 24($sp)
addiu      $sp, $sp, 32
j          $31
nop

