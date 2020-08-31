#include "stdio.h"

int compare(char* c, int size);

int main()
{
char c1[3] = {'a' , '\\' , '3'};
char c2[6] = {'a', 'b', 'c', 'd', 'e', 'f'};
char c3[6] = {'a' , '\\' , '3' , 'G', 'm', ')' };
char c4[6] = {'a' , '\\' , '3' , 'G', 'm', '(' };
char c5[6];
c5[0] = 'a';
c5[1] = '\\';
c5[2] = '3';
c5[3] = 'G';
c5[4] = 'm';
c5[5] = ')';
if(compare(c1, 3) != 0) { printf("1\n"); return 1; }
if(compare(c2, 6) != 0) { printf("2\n"); return 2; }
if(compare(c3, 6) != 1) { printf("3\n"); return 3; }
if(compare(c4, 6) != 0) { printf("4\n"); return 4; }
if(compare(c5, 6) != 1) { printf("5\n"); return 5; }
return 0;
} 