typedef struct {
    int y;
    int z;
} hello;

int f()
{
    hello g;
    g.y=17;
    g.z=13;
    return g.y+g.z;
}