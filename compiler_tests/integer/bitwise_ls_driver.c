int f(int x, int y);

int main()
{
    if( (f(1,2)!=4) ) return 1;
    if( (f(3,3)!=24) ) return 2;
    if( (f(1,0)!=1) ) return 3;
    return 0;
}
