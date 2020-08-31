int ifelif(int a)
{
	if((a > 90)&&(a <= 100))
		return 1;
	else if((a > 80)&&(a <= 90))
		return 2;
	else if((a > 60)&&(a <= 80))
		return 3;
	else if((a > 40)&&(a <= 60))
		return 4;
	else
		return 5;
}