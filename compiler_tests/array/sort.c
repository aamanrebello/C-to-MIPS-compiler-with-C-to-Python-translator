double dr[4] = {1.23, 1.22, 2.5, -0.9};

int sort()
{
	for(int j = 0; j < 3; j++)
	{
		for(int i = 0; i < 3; i++)
		{
			if(dr[i] > dr[i+1])
			{
				double t = dr[i];
				dr[i] = dr[i+1];
				dr[i+1] = t;
			}
		}
	}
	int ab = (dr[0] == -0.9)&&(dr[1] == 1.22)&&(dr[2] == 1.23)&&(dr[3] == 2.5);
	return ab;
}
