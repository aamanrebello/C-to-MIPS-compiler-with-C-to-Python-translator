typedef struct {
    int y;
    int z;
} hello;

int f()
{
    hello gh;
    hello* g = &gh;
    g->y=17;
    g->z=13;
    return g->y + 5 + g->z;
}