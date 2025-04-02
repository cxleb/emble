# Emble
This is an introductory guide and imformal specification to the emble language

Guiding principles, in no particular order:
- n-1 ways of doing doing something is better then n, where n >= 2(aka reduce the number of ways of going about something)
- simple, readable syntax
- expressive
- type safety
- memory safety

### Basic Example

```go
import print from "std:io"

export func main() {
    print("Hello, World!\n")
}
```

### Variables

```go
let a = 10
let a : int32 = 10
const a = 10
const a : int32 = 10
```

### Pointers, References, Memory
Pointers "own" the memory they are pointing too and they have move semantics.

To allocate memory you call the builtin `new<T>() *T`, `e` will have the type `*Vector` 
```go
let vector = new<Vector>()
```

`a` has been moved to `b`, using `a` will result in a use-after-move error
```go
let a
let f = e
``` 

References are automatically created based on an explicit reference type, trying to create another reference of `f` will result in an multiple-reference error
```go
let g: &Vector = f
``` 

`free` is defined as `free<T>(*T)`, it takes a pointer because it will own the memory, thus accessing f and g is invalid 
```go
free(vector)
``` 

You can also create references to stack-based variables too
```go
let a = 10
let b: &int32 = a
``` 

### Move and Copy Semantics
Only pointers have move semantics since their lifetimes need to be tightly controlled

accessing h or i is not an error since it is a copy
```go
let a = new<thing>()
let b: &thing = a
let c = b

```
All other variable types can be copied
```go
let a = 10
let b = a
a += 10
// b is 10, a is 20
```

### Types
built in types
```go
int8    int16   int32   int64
uint8   uint16  uint32  uint64
float32 float64 

usize   size

void
```

Arrays are fixed in size, an array type can specified by prefixing with square brackets.

`a` will have type `[4]int32`
```go
let a = [ 10, 20, 30, 40 ]

```

Pointers and References
```go
// pointer
*type
// references
&type
```

Pointer deferencing??

### Control Flow
```go
if cond {

}

for element in array {

}

while cond {

}
```

### Functions

Functions can have 0 or more parameters and an optional return type when the return type is left out it is inferred from the return value. Parameters are also const, they cannot be mutated
```go
func add(a: int32, b: int32) int32 {
	return a + b;
} 
```

Variadic functions, the last arguement can be a variadic, it will go into an array, you cannot mix types
```go
func add_up_ints(ints: int32...) int32 {
	let accumulator: int32 = 0
	for i in ints {
		accumulator += i
	}
	return accumulator
}
```

if you want to mix types, to do this safely, you need to use an interface
```go
func print(args: &Printable...) {
	for arg in args {
		args.print()
	}
}
```

### Error handling
Errors as values, how what do they look like??

### Struct, Interfaces and Implementations
Structs are the way to build structures, they are defined as a list of fields and functions
```go
struct Vector {
	// field definitions
	x: float32,
	y: float32,

	// This is a function which is name spaced to the struct, effectively like 
	// a free function.
	func create(name: float32, age: float32) {
		// this is the syntax for initialising a struct
		return vector {
			x: name, 
			y: age
		}
	}

	// when the first arguement is `self` the function, you require an instance 
	// of the struct to call the function like so `vector.add(other)`
	func add(self, other: vector) {
		return Vector {
			x: self.x + other.x,
			y: self.y + other.y
		}
	}
}
```

Interfaces allow you to implement common functionality for dynamic dispatch
```go
interface Printable {
	func print(self)
}
```

`impl` is to create an implementation of an interface for a struct
```go
impl Printable on Vector {
	func print(self) {
		//
	}
}
```

### Module, Packages, Imports and Exports
Modules are single translation unit of Emble code, effectively a single file.

Packages are multiple translation units, example being `std`

You can import all exports from another module or cherry pick certain definitions.
```go
import "module" // import all definitions
import something from "module" // only imports `something`
import something, another_thing from "module" // imports multiple definitions
```

Modules imports are relative to the location of the file
```go
import "directory\module"
```

Modules from other packages are prefixed with their package name, like so:
```go
import print from "std:io"
import something from "package:module"
```

You can export any definition by marking it as `export`. 
```go
export func exported_func () {}
export struct exported_struct {}
export interface exported_interface {}
```
Note: If a struct has an impl on an interface, and both are exportable(either imported or exported) the impl is also exported.

You can also export an imported symbol
```
export "module"
export something from "module"
```

### Generics
```go
func generic<T>() {}
struct generic<T> {}
interface generic<T> {}
```

### Bultins
Get array size
```go
len<T>([]T) usize
```

Allocate and free memory using the global general purpose allocator
```go
new<T>() *T
free<T>(*T)
```

Allocate and free memory using a custom allocator
```go
new<T>(allocator) *T
free<T>(*T, allocator) *T
```

unsafe builtins which allow for specialist behaviour, they will be prefixed with `unsafe_`

Gets the pointer of a reference, this allows you to copy a pointer, use case will be for creating memory allocators and interacting with external code which uses pointers
```go
unsafe_ref_as_ptr(&T) *T
```

### TODO
- decide if generics should be scoped to an interface(like rust) or should it be simple substitution(like c++/zig), im leaning towards simple substitution
- nullables?
- deferencing pointers?
- error handling model, this will be errors as values, like go, zig or rust, the goal here would be a simple model which leads,exceptions are a terrible mechanism for errors
	- Zig has too many keywords for handling errors, i do like how it has this concept of global error{} and return values are marked with an error
	- Go has the opposite problem and lacks syntax sugar for handling them nicely and it banks on multiple return values which can lead them to be ignored, but its very simple syntax
	- Rust has a nice mechanism with `Result` and `Option` but it can be clunky if errors arent the same and there are too many options for handling them
- decide if `unsafe_ref_as_ptr` is a good idea or should there be a `unsafe_copy_ptr<T>(*T) *T` and `unsafe_offset_ptr<T>(*T, offset, align) *T`, or all of them, i suspect ill only know in the distant future when its being used in anger
- How should custom allocators be implemented