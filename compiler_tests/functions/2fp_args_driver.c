int f();

int g(float a, int b, char c, unsigned d, float e)
{
	if((a > 0)&&(b > 0))
    	{ return b+c+d; }
	else 
	{ return b-c-d; }
}

int main()
{
    return !(f()==9);
}