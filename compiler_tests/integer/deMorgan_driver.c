int f(int x, int y);

int main()
{
    if( (f(0x0F,0xF0)!=0) ) return 1;
    if( (f(0x00,0xF0)!=1) ) return 2;
    if( (f(0x0F,0x00)!=1) ) return 3;
    if( (f(0x00,0x00)!=1) ) return 4;
    return 0;
}