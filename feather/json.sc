# TODO;
# * Look at using the Switcher in testing_switch.sc. Like a generator for compile time switch. 
# * Ask Open / the Scopes Discord how to properly use the Match function, could clean up some messy things.

using import struct
using import Array
using import enum
using import String
using import Map
using import Rc

using import format
using import .time

using import .atof

global MAX_DEPTH = 100

enum json
let json-string = String
let json-array = (GrowingArray json)
let json-object = (Map Symbol json)
enum json
    array : json-array
    object : json-object
    string : json-string
    number : f64
    boolean : bool
    null

    inline __rimply (T cls)
        static-match T
        case json-array
            inline (value) (this-type.array value)
        case json-object
            inline (value) (this-type.object value)
        case json-string
            inline (value) (this-type.string value)
        case f64
            inline (value) (this-type.number value)
        case f32
            inline (value) (this-type.number value)
        case i64
            inline (value) (this-type.number (value imply f64))
        case i32
            inline (value) (this-type.number value)
        case bool
            inline (value) (this-type.boolean value)
        case NullType
            inline (value) (this-type.null)
        default ()

# Scopes are namespaces that can get passed around. Namespaces are first class objects.

let ascii =
    do
        let
            qmark = 34:i8 
            
            lparen = 40:i8
            rparen = 41:i8

            lbrace = 91:i8
            rbrace = 93:i8

            lcbrace = 123:i8
            rcbrace = 125:i8

            zero = 48:i8
            negative = 45:i8
            null = 0:i8

            seperator = 58:i8

            comma = 44:i8

            space = 32:i8
            tab = 9:i8
            nl = 10:i8
            cr = 13:i8

            t = 116:i8
            f = 102:i8
            n = 110:i8
        locals;
let ws =
    (tupleof ascii.space ascii.tab ascii.nl ascii.cr)

struct PeekableStream
    source : (Rc String)
    cursor : (Rc (mutable i32))

    fn peek (self)
        # consumes ws
        (self.source @ (self.cursor + 1))
        while ((self.source @ (self.cursor + 1)) in ws)
            self.cursor += 1
        (self.source @ (self.cursor + 1))

    fn next (self)
        ('peek self)
        self.cursor += 1
        (self.source @ self.cursor)

fn eevaluate (stream depth)
    returning (uniqueof json -1)

    let eevaluate = this-function

    if (depth == 0)
        error "nesting depth exceeded unable to continue"

    fn handle-number (stream)
        stream.cursor -= 1

        let valid-number = (tupleof S"0" S"1" S"2" S"3" S"4" S"5" S"6" S"7" S"8" S"9" S"e" S"E" S"-" S"+" S".")
        local str = S""

        while ((String (('peek stream) as string)) in valid-number)
            str ..= (String (('next stream) as string))
  
        (json.number (atof (str as string)))

    fn handle-object (stream depth)
        json.object
            local obj = (json-object)

            while true
                let key-scope = (eevaluate stream (depth - 1))

                dispatch key-scope
                case string (str)
                    let seperator = ('next stream)

                    if (seperator != ascii.seperator)
                        error "expected object seperator : but found something else"

                    ('set obj (Symbol (str as string)) (eevaluate stream (depth - 1)))

                    let branch-char = ('next stream)

                    if (branch-char != ascii.comma and branch-char != ascii.rcbrace)
                        error "expected comma seperator or end of object but found neither"
                    if (branch-char == ascii.rcbrace)
                        break;

                default
                    error "expected string propery in object but found something else"

    fn handle-array (stream depth)
        json.array
            local arr = (json-array)

            while true
                'append arr (eevaluate stream (depth - 1))

                let branch-char = ('next stream)

                if (branch-char != ascii.comma and branch-char != ascii.rbrace)
                    error "expected comma seperator or end of array but found neither"
                if (branch-char == ascii.rbrace)
                    break;
    fn handle-bool (stream text value)
        stream.cursor -= 1 # backtrack to consume first character
        let position = ((stream.cursor as usize) as i32)
        local characters =  S""

        for _ in (range (countof text)) 
            characters ..= (('next stream) as string)

        if (characters == text)
            if (((text as string) == "true") or ((text as string) == "false"))
                return (json.boolean value)
            else
                return (json.null)
        else
            error "invalid character found while attempting to parse literal"

    local value = (json.null)

    let c = ('next stream)

    switch c
    # rules for strings.
    case ascii.qmark
        local str = S""

        while (('peek stream) != ascii.qmark)
            str ..= (String (('next stream) as string))
        ('next stream)

        value = (json.string (json-string str))
    case ascii.negative
        value = (handle-number stream)
    case ascii.zero
        value = (handle-number stream)
    case (ascii.zero + 1)
        value = (handle-number stream)
    case (ascii.zero + 2)
        value = (handle-number stream)
    case (ascii.zero + 3)
        value = (handle-number stream)
    case (ascii.zero + 4)
        value = (handle-number stream)
    case (ascii.zero + 5)
        value = (handle-number stream)
    case (ascii.zero + 6)
        value = (handle-number stream)
    case (ascii.zero + 7)
        value = (handle-number stream)
    case (ascii.zero + 8)
        value = (handle-number stream)
    case (ascii.zero + 9)
        value = (handle-number stream)
    case ascii.lbrace
        value = 
            (handle-array stream depth)
    case ascii.lcbrace
        value =
            (handle-object stream depth)
    case ascii.t
        value =
            (handle-bool stream "true" true)
    case ascii.f
        value = 
            (handle-bool stream "false" false)
    case ascii.n
        value =
            (handle-bool stream "null" true)
    default
        error 
            .. (format "{} <-- invalid character found in input stream" c)

    if (depth == MAX_DEPTH and (('peek stream) != ascii.null))
        error "expected EOF but found more text"

    value

fn pparse (source)
    let stream =
        PeekableStream
            Rc.wrap (String source)
            Rc.wrap -1
    (eevaluate stream MAX_DEPTH)

locals;

