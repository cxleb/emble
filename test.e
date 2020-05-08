extern func : void printf(fmt : cstring, int : int32)

func : int32 add_two_numbers(num1 : int32, num2 : int32)
{
	return num1 + num2
}

func : int32 main(argc : int32, argv : uint8)
{
	//string : cstring = "hey mate its going good\n"
	math : int32 = add_two_numbers(17, 13)
	printf("%d\n", add_two_numbers(17, 13))
	return 0
}



