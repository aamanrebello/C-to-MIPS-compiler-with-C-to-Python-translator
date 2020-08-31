int main()
{
	int t = 0; int u = 1, v = 2;
	while(t < 3)
		while(u < 4)
			while(v < 5)
			{
				v = v+1;
				u = u + 1;
				t+=2;
			}
	return t + u + v;
}