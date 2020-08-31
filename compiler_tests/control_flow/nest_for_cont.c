int nest()
{

int i = 5; int j = 6, k = 7, l = 0;

for(int i = 0; i < 5; i+=1)
	for(int j = 0; j < 27; j+=5)
	{
		if(j > 13)
			continue;
		l++;
	}

return l + i +j + k;

}
