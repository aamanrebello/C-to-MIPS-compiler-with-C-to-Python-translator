typedef enum{
    vv = 13,
    ty,
    yy = 10,
    fd = 2
} gret;

int g()
{
    gret a;
    a = ty;
    return a + fd;
}