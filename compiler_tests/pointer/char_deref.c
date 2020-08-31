char f()
{
    char x;
    char *y=&x;
    x='x';    
    return *y;
}
