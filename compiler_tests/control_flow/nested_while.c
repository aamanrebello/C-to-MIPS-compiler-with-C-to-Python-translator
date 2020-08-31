int nw()
{
	int i = 0, j = 0, k = 0, l = 0;
	while(i < 5)
	{
		while (j < 6)
			while(k < 7)
				while(l < 8)
				{
				       k = l++;
					j = k++;
				}
	i++;
	}
	return i + j + k + l;
}