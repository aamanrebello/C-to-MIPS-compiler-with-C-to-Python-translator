int f(int x, int y)
{
    return ((x+1-3)*(y - (3%2%3))&(0x0000000F)) << 1;
}