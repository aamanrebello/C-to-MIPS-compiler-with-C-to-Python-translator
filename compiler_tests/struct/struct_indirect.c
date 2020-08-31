struct x{
    int y;
    int z;
};

int f()
{
    struct x g;
    struct x* h = &g;
    h->y=17;
    h->z=13;
    return h->y + h->z + g.z;
}