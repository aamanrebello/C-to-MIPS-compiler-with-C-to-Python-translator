int ifelif(int a);

int main()
{
	if(ifelif(95) != 1) { return 1; }
	if(ifelif(90) != 2) { return 2; }
	if(ifelif(60+20) != 3) {return 3;}
	if(ifelif(65) != 3) {return 4;}
	if(ifelif(45) != 4) {return 5;}
	if(ifelif(35) != 5) {return 6;}
	return 0;
} 