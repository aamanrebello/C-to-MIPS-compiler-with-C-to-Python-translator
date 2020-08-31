
int f()
{
    if(0){
        return 11;
    }else{
        return 10;
    }
}

int main()
{
    return 4 + !(f()==10);
}
