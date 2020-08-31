int x;
int y;

int main()
{
x = 3-5*8;
y = 4-5;
if((y%2 == 0)&&(x%2 == 0))
{
return 1;
}
else if(y%2 == 0)
{
return 2;
} 
else if(x %2 == 0)
{
return 3;
}
else
{
return 4;
}
}