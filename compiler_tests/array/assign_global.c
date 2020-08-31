int x[8];

int f()
{
    int i = 2;
    x[i + 2] = i*2*2*2;
    return x[4];
}