using import struct
using import Array
using import enum
using import String
using import Map

using import .atof

# TODO;
# * On demand lexing.
# * Standardize errors.
# * Implement proper string parsing with solidus / reverse solidus.

enum json
let json-string = string
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

#
# -- Lexer --
#

fn do-error (msg cursor)
    print cursor
    error msg


enum T : i32
    T_NUMBER
    T_STRING
    
    T_ARRAY_START
    T_ARRAY_END
    
    T_OBJECT_START
    T_OBJECT_END

    T_BOOL
    T_NULL

    T_COMMA
    T_COLON

    T_NUMBER_START # utilized as a special production rule as the start of numbers are non-same
    # see https://www.json.org/img/number.png
    T_WHITESPACE # as above but with whitespace
    # see https://www.json.org/img/whitespace.png

    T_EOF


struct Token
    value : string
    kind : T
    position : i32

fn lex (source) 
    struct InputStream
        source : string
        cursor : (mutable@ i32)

        fn peek (self)
            return (self.source @ ((@ self.cursor) + 1))
        
        fn next (self)
            (@ self.cursor) += 1
            return (self.source @ (@ self.cursor))

        fn end (self)
            return (((@ self.cursor) + 1) >= (countof self.source))

        fn get-cursor (self)
            return (@ self.cursor)
    
    fn create-token (value kind position)
        return 
            Token
                value
                kind
                position
    
    fn match(character rule)
        if (rule == T.T_NUMBER_START) 
            return (
            character == "0" or 
            character == "1" or
            character == "2" or
            character == "3" or
            character == "4" or
            character == "5" or
            character == "6" or
            character == "7" or
            character == "8" or
            character == "9" or
            character == "." or
            character == "-"
            )
        
        elseif (rule == T.T_NUMBER)
            return (
            character == "0" or 
            character == "1" or
            character == "2" or
            character == "3" or
            character == "4" or
            character == "5" or
            character == "6" or
            character == "7" or
            character == "8" or
            character == "9" or
            character == "." or
            character == "-" or
            character == "+" or 
            character == "e" or
            character == "E"
            )

        elseif (rule == T.T_STRING)
            return (character == "\"")

        elseif (rule == T.T_ARRAY_START)
            return (character == "[")

        elseif (rule == T.T_ARRAY_END)
            return (character == "]")

        elseif (rule == T.T_OBJECT_START)
            return (character == "{")
        
        elseif (rule == T.T_OBJECT_END)
            return (character == "}")

        elseif (rule == T.T_COMMA)
            return (character == ",")

        elseif (rule == T.T_COLON)
            return (character == ":")

        elseif (rule == T.T_WHITESPACE)
            return (
            character == " "  or
            character == "\n" or
            character == "\r" or
            character == "\t" 
            )

        false

    local cursor = -1
    local tokens = ((Array Token))
    
    let stream =
        InputStream
            source
            &cursor

    while (not ('end stream))
        local c = (('next stream) as string)
        local position = ('get-cursor stream)
        local kind = T.T_NULL

        if (match c T.T_NUMBER_START)
            kind = T.T_NUMBER
            local number = ("" as string)

            while (match c T.T_NUMBER)
                number ..= c

                c = (('next stream) as string)

            # back up 
            (@ stream.cursor) -= 1
            
            c = number
        
        elseif (match c T.T_STRING)
            kind = T.T_STRING

            local str = ("" as string)

            c = (('peek stream) as string)

            while (c != ("\"" as string))
                str ..= (('next stream) as string)
                c = (('peek stream) as string)

            ('next stream)

            c = str

        elseif (c == ("t" as string))
            kind = T.T_BOOL
            if ((('peek stream) as string) != ("r" as string))
                error "unknown literal found in input stream"
            ('next stream)

            if ((('peek stream) as string) != ("u" as string))
                error "unknown literal found in input stream"
            ('next stream)

            if ((('peek stream) as string) != ("e" as string))
                error "unknown literal found in input stream"
            ('next stream)

            c = ("true" as string)

        elseif (c == ("f" as string))
            kind = T.T_BOOL
            if ((('peek stream) as string) != ("a" as string))
                error "unknown literal found in input stream"
            ('next stream)

            if ((('peek stream) as string) != ("l" as string))
                error "unknown literal found in input stream"
            ('next stream)

            if ((('peek stream) as string) != ("s" as string))
                error "unknown literal found in input stream"
            ('next stream)

            if ((('peek stream) as string) != ("e" as string))
                error "unknown literal found in input stream"
            ('next stream)

            c = ("false" as string)

        elseif (c == ("n" as string))
            kind = T.T_NULL
            if ((('peek stream) as string) != ("u" as string))
                error "unknown literal found in input stream"
            ('next stream)

            if ((('peek stream) as string) != ("l" as string))
                error "unknown literal found in input stream"
            ('next stream)

            if ((('peek stream) as string) != ("l" as string))
                error "unknown literal found in input stream"
            ('next stream)

            c = ("null" as string)                      

        elseif (match c T.T_OBJECT_START)
            kind = T.T_OBJECT_START
        elseif (match c T.T_OBJECT_END)
            kind = T.T_OBJECT_END
        elseif (match c T.T_ARRAY_START)
            kind = T.T_ARRAY_START
        elseif (match c T.T_ARRAY_END)
            kind = T.T_ARRAY_END
        elseif (match c T.T_COLON)
            kind = T.T_COLON
        elseif (match c T.T_COMMA)
            kind = T.T_COMMA
        elseif (match c T.T_WHITESPACE)
            continue;
        else
            do-error "non-valid token found in input stream" position
        
        'append tokens (create-token c kind position)

    'append tokens (create-token "EOF" T.T_EOF ('get-cursor stream))

    return &tokens

#
# -- Parser --
#

struct TokenStream
    tokens : (Array Token)
    cursor : (mutable@ i32)

    fn next (self)
        if (((@ self.cursor) + 1) >= (countof self.tokens))
            error "found EOF while parsing"

        ((@ self.cursor) += 1)
        (self.tokens @ (@ self.cursor))

    fn peek (self)
        (self.tokens @ ((@ self.cursor) + 1))

fn evaluate (stream)
    returning (uniqueof json -1)

    let token = ('next stream)

    switch token.kind
    case T.T_NUMBER
        """"
        json.number
            atof token.value
    case T.T_BOOL
        json.boolean
            if (token.value == ("true" as string))
                true
            else
                false
    case T.T_STRING
        # TODO; support solidus / reverse solidus
        json.string (json-string token.value)
    case T.T_NULL
        json.null;
    case T.T_ARRAY_START
        """"
            https://www.json.org/img/array.png
            1. enter the main loop of 
                i. append the parse result of the next token
                ii. if there is a comma repeat
                iii. if there is an array ] terminator token terminate

        json.array
            local arr = (json-array)

            while true
                'append arr (this-function stream)
                
                let final-token = ('next stream)

                if (final-token.kind != T.T_COMMA and final-token.kind != T.T_ARRAY_END)
                    error "expected comma seperator or end of array but found neither"

                if (final-token.kind == T.T_ARRAY_END)
                    break; 

    case T.T_OBJECT_START
        """"
            https://www.json.org/img/object.png
            1. enter the main loop of
                i. assert we find a string token
                ii. find a colon token
                iii. set it's value to the return of a recursive call
                iv. if we see a comma consume it and pass
                v. if we find a object } terminator token terminate

        json.object
            local obj = (json-object)

            while true
                let key-string = ('next stream)

                if (key-string.kind != T.T_STRING)
                    error "expected string as object property but found something else"
                
                let seperator = ('next stream)

                if (seperator.kind != T.T_COLON)
                    error "expected object property seperator but found something else"

                'set obj (Symbol key-string.value) (this-function stream)

                let final-token = ('next stream)

                if (final-token.kind != T.T_OBJECT_END and final-token.kind != T.T_COMMA)
                    error "expected comma seperator or end of object but found neither"

                if (final-token.kind == T.T_OBJECT_END)
                    break;

    default
        error "non-valid token found in token stream"

fn parse (source)
    local cursor = -1

    let stream = 
        TokenStream
            (@ (lex source))
            &cursor

    (evaluate stream)

locals;