
int f()
{
    if(1){
        return 11;
    }
    return 10;
}

int main()
{
    return 3 + (f()==11);
}
