typedef struct x{
    int y;
    int z;
} hey;

int f()
{
    hey g;
    g.y=17;
    g.z=13;
    return g.y+g.z;
}