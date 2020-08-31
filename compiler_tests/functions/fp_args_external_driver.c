double f();

double g(double a, float b, double c, char d, float e)
{
    return a+c+e+b;
}

int main()
{
	int a = (int)(f()*100);
    return (a == 240);
}