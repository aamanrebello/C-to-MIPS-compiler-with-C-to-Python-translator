unsigned long f();

int main()
{
    unsigned long x[2];
    x[1]=13;
    return !(f(x)==13);
}