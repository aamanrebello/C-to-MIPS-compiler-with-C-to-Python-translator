int check(int a, int b)
{
	if(((a > 23)&&(b-2*4 <= 12))||(a-b == 0))
	{
		return 12;
	}
	else if(!(a^b)&&!(a&b))
	{
		return 10;
	}
	return 13;
}

int main()
{
	return 1 + check(24, 23) + check(1, 1) + check(837, 800);
}