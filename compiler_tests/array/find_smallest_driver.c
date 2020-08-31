int smallest(int* a, int size);

int main()
{
	int arr[10] = { 23, 12, 25, 13, 0, 2, -3, -4, 10, -1};
	return !(smallest(arr, 10) == -4);
}  