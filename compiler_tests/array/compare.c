

char cr[6] = { 'a' , '\\' , '3' , 'G', 'm', ')' };

char compare(char* c, int size)
{
	if(size != 6)
	{
		return 0;
	}
	int i = 0;
	while(i < 6)
	{
		if(c[i] != cr[i])
		{
			return 0;
		}
	i++;
	}
	return 1;
}