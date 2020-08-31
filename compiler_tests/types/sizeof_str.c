int strsize()
{
	char* c = "HELLO! I am you.";
	return sizeof(c) + sizeof("HELLO");
}