int f(int c);
int main(){
    int a=0;

    for(int i=6;i>=0;i--){
        a=a+f(i);
    }
    
    if(a==35){
        return 0;
    }
    else{
        return 100;
    }
}