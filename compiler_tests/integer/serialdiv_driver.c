int f(int x, int y);

int main()
{
    if(f(1000,5)!=8) {return 1;}
    if(f(729, f(243, 3)) != 1) { return 2;}
     return 0;
}