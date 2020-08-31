int globArray[3]={5,2,15};

int recursive(int a, int b, int c, int d){
    if(a>=b){
        b++;
        d--;
        c/=d+800;
        return recursive(a,b,c,d);
    }else{
        return b;
    }
}
int func(){
    int var=17;
    int output= recursive(10,5,var,-20);
    output-=globArray[1];
    int test=1;
    for(int i=1;i<10;i++){
        test*=i;
        if (i==5){
            break;
        }
    }
    output=output+test;
    if(output==129){
        return 0;
    }else{
        return 10;
    }
}
