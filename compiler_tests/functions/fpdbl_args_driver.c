int f();

int g(float a, short b, unsigned short c, double d)
{
	if(d!=0)
    		return b*c;
	return b-c;
}

int main()
{
    return !(f()==6);
}