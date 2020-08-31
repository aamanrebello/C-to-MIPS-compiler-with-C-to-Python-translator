int length(char *x);

int main()
{
    char *s="abcdef";
    char* t= "";
    char* u= "\\\t";
    char* v= "Hi! It's \"me\"";
    if(length(s) != 6) {return 1;}
    if(length(t) != 0) {return 2;}
    if(length(u) != 2) {return 3;}
    if(length(v) != 13) {return 4;}
	return 0;
}