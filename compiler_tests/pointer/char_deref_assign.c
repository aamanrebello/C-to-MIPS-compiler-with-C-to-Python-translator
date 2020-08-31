char f()
{
    char x;
    char *y=&x;
    *y ='x';    
    return x;
}