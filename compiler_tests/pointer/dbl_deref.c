double f()
{
    double x;
    double *y=&x;
    x=13.0;    
    return *y + 2.3;
}