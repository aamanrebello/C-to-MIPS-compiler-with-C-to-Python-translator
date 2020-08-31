float fr[6] = { 1.23f, 3.4F, 2.0f, 2.34f + 6.7F, 1.2f*1.3f, 0.45F};

float summate()
{
	float total = 0.0f;
	float hei[6];
	for(int i = 0; i < 6; i+=1)
		hei[i] = fr[i];

	for(unsigned j = 0u; j < 6; j++)
	{
		total = total + hei[j];
	}
	return total;
} 
	