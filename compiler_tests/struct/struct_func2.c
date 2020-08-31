struct x{
    int y;
    int z;
};

void f(struct x* st)
{
    st->y=17;
    st->z=13;
}

int g()
{
	struct x t;
	struct x* fd = &t;
	f(fd);
	t.z = 10;
	return t.y + t.z;
}