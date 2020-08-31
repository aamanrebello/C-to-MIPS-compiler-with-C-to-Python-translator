int compare(char *x, char* y);

int main()
{
    char* s="abcdef";
    char* t= "";
    if(compare(s, t) != 0) {return 1;}
    if(compare(s, s) != 1) {return 2;}
    return 0;
}