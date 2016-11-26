func add(a: int, b: int): int return a + b;
func sub(a: int, b: int): int return a - b;

func factorial(n: int): int
{
	if n {
		return 1;
	} else {
		return n*factorial(n-1);
	}
}

func mul(a: int, b: int): int;

func add(a: int, b: int): int
{
	result :=  a + b;
}

func malloc(bytes: u32): *u8;

func add(a: int, b: int): int
{
	return a + b;
}

// Inferred to be of return void
func main()
{
	arr: *int = malloc(100);
	//arr[10] = 123;
    add(arr[10][12], 12);
	return;
}
