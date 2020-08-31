int length(char *x)
{
    int l = 0; 
    while(*x){
        x++;
	l++;
    }
    return l;
}
