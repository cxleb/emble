extern func : void printf(fmt : uint8)

func : int32 dosomething (val1 : uint32, val2 : uint8) 
{
	return val1 + val2
}

func : int32 main (argc : uint32, argv : uint8) 
{
	val1 : int32 = 17
	val2 : int8 = 27
	dosomething(val1, val2)
	printf(val2)
	return 0
}