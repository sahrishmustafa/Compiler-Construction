Language Name: C minus minus (C--) 

This document explains the syntax and usage of a custom programming language, including data types, operations, input/output functions, comments, scoping, and examples.

## Data Types
The language supports the following data types:

1. **digit** - Represents integer values.
   - Example:
     ```
     digit _num_ eq 10;
     ```

2. **decimal** - Represents floating-point numbers.
   - Example:
     ```
     decimal _pi_ eq 3.14159;
     ```

3. **zeroone** - Represents boolean values (true/false).
   - Example:
     ```
     zeroone _flag_ eq true;
     ```

4. **abc** - Represents character values.
   - Example:
     ```
     abc _letter_ eq 'A';
     ```

## Comments
The language supports single-line and multi-line comments:

- **Single-line comment**:
  ```
  @ This is a comment
  ```

- **Multi-line comment**:
  ```
  @many@
     This is a
     multi-line comment.
  @many@
  ```

## Operations
The language supports basic arithmetic and power operations:

1. **Assignment (`eq`)**:
   ```
   digit _x_ eq 5;
   ```

2. **Addition (`add`)**:
   ```
   digit _sum_ eq _x_ add _y_;
   ```

3. **Subtraction (`sub`)**:
   ```
   digit _diff_ eq _x_ sub _y_;
   ```

4. **Multiplication (`mul`)**:
   ```
   digit _product_ eq _x_ mul _y_;
   ```

5. **Division (`div`)**:
   ```
   digit _quotient_ eq _x_ div _y_;
   ```

6. **Modulus (`mod`)**:
   ```
   digit _remainder_ eq _x_ mod _y_;
   ```

7. **Exponentiation (`powow`)**:
   ```
   digit _result_ eq _x_ powow _y_;
   ```

## Input and Output
The language provides functions for input and output:

1. **Output (`come` function):**
   ```
   go._d_;
   ```
   - This function prints the value of `_d_` to the console.

2. **Input (`go` function):**
   ```
   come._y_;
   ```
   - This function takes input and stores it in `_y_`.

## Main Function
Every program must have a `digit main()` function, which acts as the entry point:

``` 
digit main(){
   @ This is the main function
   digit _num1_ eq 5;
   digit _num2_ eq 3;
   digit _sum_ eq _num1_ add _num2_;
   come._sum_;
}
```

## Example Program
Below is a complete example demonstrating the features:

``` 
digit main(){

   @ This program demonstrates basic operations

   digit _num1_ eq 5;
   digit _num2_ eq 3;
   digit _sum_ eq _num1_ add _num2_;
   digit _power_ eq _num1_ powow _num2_;

   come._sum_;
   come._power_;

   zeroone _flag_ eq true;
   abc _letter_ eq 'B';

   go._num1_;
   come._num1_;
}
```



