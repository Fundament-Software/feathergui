local C = terralib.includecstring [[
#include <stdlib.h>
#include <string.h>
]]

C.printf = terralib.externfunction("printf", terralib.types.funcpointer(rawstring,int,true))
C.snprintf = terralib.externfunction("snprintf", terralib.types.funcpointer({rawstring, intptr, rawstring},int,true))

return C