int f(int x, int y)
{
	int a = (x^y)&(x << 1);
	int b = ((a-3) < (x-10))&&(y);
	int c = 3*4/2%5;
    return a + b+ c;
}