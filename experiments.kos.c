func putchar(c: u8): void;

NODE_NUM := 1;
NODE_OP  := 2;

struct KOSNum
{
	val: s32;
}

struct KOSOp
{
	op: u8;
	lhs: *KOSNode;
	rhs: *KOSNode;
}

struct KOSNode
{
	type: u32;

	shared {
		num: KOSNum;
		op: KOSOp;
	};
}

func make_node(type: u32): *KOSNode
{
	n: *KOSNode = malloc(sizeof(KOSNode);
	n.type = type;
	return n;
}

func kos_main()
{
	v1 := make_node(NODE_NUM);
	v1.num.val = 123;
	v2 := make_node(NODE_NUM);
	v2.num.val = 321;

	op := make_node(NODE_OP);
	op.op.op = '+';
	op.op.lhs = v1;
	op.op.rhs = v2;

	res := kos_main(op);
	if res != 444 {
		printf("Failed to do add!\n");
	} else {
		printf("Yay!\n");
	}
}

func kos_test(n: *KOSNode): u32
{
	switch n.type {
		case NODE_NUM {
			return n.num.val;
		}

		case NODE_OP {
			u32 lhs = kos_test(n.op.lhs);
			u32 rhs = kos_test(n.op.rhs);
			switch n.op.op {
				case '+' {
					return lhs + rhs;
				}
				case '-' {
					return lhs - rhs;
				}
				case '/' {
					return lhs / rhs;
				}
				case '*' {
					return lhs * rhs;
				}
			}
		}
	}
}

func fac(n: int): int
{
	if n == 0 {
		return 1;
	} else {
		return fac(i-1);
	}
}

func main(): u8
{
    putchar(72);  // H
    putchar(101); // e
    putchar(108); // l
    putchar(108); // l
    putchar(111); // o
    putchar(44);  // ,
    putchar(32);  // 
    putchar(119); // w
    putchar(111); // o
    putchar(114); // r
    putchar(108); // l
    putchar(100); // d
    putchar(3*11);  // !
    putchar(10);  // \n
    main();
    return 123;
}
