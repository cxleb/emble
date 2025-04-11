func foo() u32 {
    return 1
}

func bar() u32 {
    return 0
}

export func main(args: []string) u32 {
    let a = 10

    if a != 10 { 
        return foo()
    } else { 
        return bar()   
    }
}