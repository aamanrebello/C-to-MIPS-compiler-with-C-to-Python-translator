#!/bin/bash

if [[ "$1" != "" ]] ; then
    compiler="$1"
else
    compiler="bin/c_compiler"
fi

have_compiler=0
if [[ ! -f bin/c_compiler ]] ; then
    >&2 echo "Warning : cannot find compiler at path ${compiler}. Only checking C reference against python reference."
    have_compiler=1
fi

echo "Enter Name of Subdirectory in compiler_tests you wish to test:"

read varname

input_dir="compiler_tests/${varname}"

working="tmp/compile"
mkdir -p ${working}

for i in ${input_dir}/*.c ; do
    base=$(echo $i | sed -E -e "s|${input_dir}/([^.]+)[.]c|\1|g");

    if [[ -f "${input_dir}/${base}_driver.c" ]] ; then

	echo "${input_dir}/${base}_driver.c"
    	if [[ ${have_compiler} -eq 0 ]] ; then
        
        	# Create the executable by invoking the compiler with -S flags
        	$compiler -S ${input_dir}/${base}.c -o ${working}/${base}.s
		mips-linux-gnu-gcc -mfp32 -o ${working}/${base}.o -c ${working}/${base}.s
		mips-linux-gnu-gcc -mfp32 -static -o ${working}/${base} ${working}/${base}.o ${input_dir}/${base}_driver.c
        
        	# Run the executable
        	qemu-mips ${working}/${base}
        	GOT_MIP_OUT=$?
    	fi
    
    	if [[ ${have_compiler} -ne 0 ]] ; then
        	echo "$base, Fail, No C compiler/translator"
    	elif [[ $GOT_MIP_OUT -ne 0 ]] ; then
        	echo "$base, Fail, Expected 0, got ${GOT_MIP_OUT}"
    	else
        	echo "$base, Pass"
    	fi
    fi
done
