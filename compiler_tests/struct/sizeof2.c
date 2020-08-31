struct x{
    int y;
    char c;
    double dr;
    unsigned short ty;
    double du;
};

int f()
{
    struct x y;
    return sizeof(y);
}