int func(int a, int b, int c, int d, int e, int f, int g)
{
	return a+b-c*d|e%f&g;
}

int main()
{
int x;
int y;
x = 23;
y = 12;
return func(x, 1, 2, 3, 4, 5, y);
}