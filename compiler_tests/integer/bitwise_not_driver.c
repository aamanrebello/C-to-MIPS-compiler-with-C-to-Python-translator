int f(int x);

int main()
{
    return !(f(0xFFFF00)==0xFF0000FF);
}
