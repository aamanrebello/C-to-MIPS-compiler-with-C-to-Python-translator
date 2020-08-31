struct x{
    int yeh;
    char c;
    double dr;
    unsigned short ty;
    double du;
};

int f()
{
    struct x y;
    y.yeh = 12;
    y.c = 'a';
    y.dr = 2.345;
    y.ty = 92;
    y.du = y.dr + 3.45;
    return y.c + y.ty + y.yeh;
}