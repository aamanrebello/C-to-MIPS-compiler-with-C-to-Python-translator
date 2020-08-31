
int bsqrt(int lo, int hi, int val)
{
    while(lo+1 < hi){
        int mid;
	mid = (lo+hi)>>1;
        int sqr;
	sqr =mid*mid;
        if(sqr <= val){
            lo=mid;
        }else{
            hi=mid;
        }
    }
    if( lo*lo < val ) {
        return hi;
    }else{
        return lo;
    }
}

int main()
{
    int x;
    return bsqrt(1,1000,64);
}
