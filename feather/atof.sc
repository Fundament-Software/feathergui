fn atof (str)
    """"
        1. cut off the sign if there is one provided. not possible except by recursion.
            a. hold on to this value as a multiplicand in the end.
        2. split the string into two two values (number) (exponent)
        3. return the value gotten by multiplying the (sign) (number) (exponent)
    
    returning f64
    local number = str
    local number-sign = 1:f64

    if (((number @ 0) as string) == ("-" as string))
        number-sign *= -1
        number = (rslice number 1)

    if (((number @ 0) as string) == ("+" as string))
        number = (rslice number 1)

    let initial-size = (countof number)

    local value-string = ("" as string)
    local exponent-string = ("" as string)

    local decimal-position = -1

    loop (idx = 0)
        if (idx < initial-size)
            let c = ((number @ idx) - 48)

            if (c < 0)
                if (decimal-position > 0)
                    error "found too many decimals in numeric literal"
                decimal-position = idx
            elseif (c > 9)
                exponent-string = (rslice number (idx + 1))
                number = (lslice number idx)
                break idx

            repeat (idx + 1)
        else
            break idx

    local number-literal = 0:f64
    local exponent-literal = 1:f64

    let number-size = (countof number)
    
    if (decimal-position < 0)
        decimal-position = (number-size as i32)

    loop (idx = 0)
        if (idx < number-size)
            let num = (((number @ idx) - 48) as f64)

            if (num >= 0)
                let exponent = (pow 10:f64 (((decimal-position - 1) - idx) as f64))

                number-literal += (num * exponent)
            else
                decimal-position += 1

            repeat (idx + 1)
        else
            break idx

    let exponent-size = ((countof exponent-string) as i32)

    if (exponent-size > 0)
        exponent-literal = (pow 10:f64 (this-function exponent-string))

    return (number-sign * number-literal * exponent-literal)

locals;