int f(int x, int y);

int main()
{
    if( (f(4,32)!=1) ) return 1;
    if( (f(8,256)!=4) ) return 2;
    if( (f(1,0)!=1) ) return 3;
    return 0;
}