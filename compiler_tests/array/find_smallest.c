int smallest(int* a, int size)
{
	int min = a[0];
	for(int i = 0; i < size; i++)
	{
		if(a[i] < min)
		{ min = a[i]; }
	}
	return min;
}
	