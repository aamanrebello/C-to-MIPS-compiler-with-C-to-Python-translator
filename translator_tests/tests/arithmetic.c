int arith(int d)
{
	int x;
	x = 0;
	x = x + d + d + d + d + d + 2 + 2;
	x = x - 3 - 3- 4 -5 - 6;
	x = x*3*d/d;
	return x%d;
}

int main()
{
	int a = 2*3*4;
	return 5 + !(arith(10) + a == 24);
}