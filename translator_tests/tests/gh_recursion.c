int f(int x)
{
  if(x>1){
    return x*f(x-1);
  }else{
    return x;
  }
}

int main()
{
return f(5)*f(2);
}