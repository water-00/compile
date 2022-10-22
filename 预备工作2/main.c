void fibonacci(int n) {
	int a, b, i, t;
	a = 0, b = 1, i = 1;
	putint(b);
	putchar(10);

	while (i < n) {
		t = a + b;
		putint(t);
		putchar(10);
		a = b;
		b = t;
		i = i + 1;
	}
}

int main()
{
	int n;
	n = getint();

	if (n >= 1) 
		fibonacci(n);

	return 0;
}
