func : int32 do_something(input : int32)
{
	// do some math jokes
	return input
}

func : int32 main (argc : uint32, argv : uint8) 
{
	test : int32 = do_something(7)
	return test
}