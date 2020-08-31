int compare(char *x, char* y)
{
    char* c = x;
    int l1 = 0; 
    while(*x){
        x++;
	l1++;
    }
    x = c;
    c = y;
    int l2 = 0;
    while(*y){
        y++;
	l2++;
    }
    y = c;
    if(l1 != l2)
	{ return 0; }
    while(*x)
    {
	x++;
	y+=1;
	if(*x != *y)
	{ return 0; }
    }
    return 1;
}
