struct x{
    int yeh;
    char c;
    double dr;
    unsigned short ty;
    float fu;
    double du;
};

double f()
{
    struct x y;
    struct x* hg = &y;
    y.yeh = 12;
    y.c = 'a';
    hg->dr = 2.0;
    y.ty = 92;
    hg->fu = 3.2f;
    hg->du = y.dr + 3.0;
    return y.dr + hg->du;
}