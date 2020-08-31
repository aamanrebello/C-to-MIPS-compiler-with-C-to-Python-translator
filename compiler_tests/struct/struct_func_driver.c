struct x{
    int y;
    int z;
};

void f(struct x* r);

int main()
{
    struct x r;
    struct x* yu = &r;
    f(yu);
    if(r.y + r.z == 30)
	{return 0;}
    else
	{return 1;}
}