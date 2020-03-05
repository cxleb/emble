include c_stdio
include c_stdlib

func : uint32 main () 
{
	printf("Hello, World")

	variables()

	return 0
}

func variables()
{
	// comment
	
	var1 : int8 = 1
	var2 : uint8 = 2
	var3 : int16 = 3
	var4 : uint16 = 4
	var5 : int32 = 5
	var6 : uint32 = 6
	var7 : int64 = 7
	var8 : uint64 = 8

	string : cstring = "This is a cstring, in c it's equivelent is a const unsigned char*"

	conditionals()
}

func conditionals()
{
	mystery : uint16 = 10
	if (mystery > 15)
	{
		printf("the mystery number is greater then 15")
	}
	elseif (mystery < 5)
	{
		printf("the mystery number is less then 5")
	}
	else
	{
		printf("the mystery number is somewhere between 5 and 15")
	}

	structs()
}

struct our_struct
{
	name : cstring // this is a uint8* 
	number_variable : uint32
}

func : structs()
{
	stack_defined : our_struct;

	//heap_defined : our_struct* = malloc(sizeof(our_struct))
}


