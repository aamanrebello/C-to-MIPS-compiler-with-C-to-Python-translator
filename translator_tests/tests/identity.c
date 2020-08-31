
int g(int x)
{
    return x;
}

int f()
{
    return g(10);
}

int main()
{
    return !(f()==10);
}
