enum X{
    vv = 13,
    ty,
    yy = 10
};

int g()
{
    enum X a;
    a = ty;
    return a;
}
