int tellme(int a, int b)
{
	return (a*b)^(a-b);
}
int k;

int main()
{
    int x = -(2<<5&8*(2%9)+tellme(4, 2)-12*3%2-8|5^6);
    return x;
}