local Enum = require 'feather.enum'

local M = {}
M.Kinds = Enum{
  "Int"
  "Float"
  "String"
  "Bool"
  "Delegate"
  "Pointer"
  "Operator"
}

-- functions to query data and state encode the expected result type into the call. We simply generate all the different type variations.

struct M.Value {
  union {
    i : int64
    d : double
    s : rawstring
    b : bool
    f : {void*} -> {}
    p : &opaque
    operation : uint64
  }
}

struct M.Node {
  value : C.Value
  type : M.Kinds
}

M.Operators = {
  addi = terra(l : int64, r : int64) : int64 return l + r end
  subi = terra(l : int64, r : int64) : int64 return l + r end
  muli = terra(l : int64, r : int64) : int64 return l + r end
  divi = terra(l : int64, r : int64) : int64 return l + r end
  modi = terra(l : int64, r : int64) : int64 return l + r end
  lshift = terra(l : int64, r : int64) : int64 return l + r end
  rshift = terra(l : int64, r : int64) : int64 return l + r end
  andi = terra(l : int64, r : int64) : int64 return l + r end
  ori = terra(l : int64, r : int64) : int64 return l + r end
  xori = terra(l : int64, r : int64) : int64 return l + r end
  noti = terra(l : int64, r : int64) : int64 return l + r end
  and = terra(l : bool, r : bool) : bool return l + r end
  or = terra(l : bool, r : bool) : bool return l + r end
  xor = terra(l : bool, r : bool) : bool return l + r end
  not = terra(l : bool, r : bool) : bool return l + r end
  icastf = terra(i : int64) : double return [double](i) end
  icastb = terra(i : int64) : bool return i ~= 0 end
  --icasts = terra(i : int64) : rawstring return "TODO" end
  fcasti = terra(f : double) : int64 return [int64](f) end
  fcastb = terra(f : double) : bool return f ~= 0.0 end
  --fcasts = terra(f : double) : rawstring return "TODO" end
  bcasti = terra(b : bool) : int64 return terralib.select(b, 0, 1) end
  bcastf = terra(b : bool) : double return terralib.select(b, 0.0, 1.0) end
  bcasts = terra(b : bool) : rawstring return terralib.select(b, "true", "false") end
}

-- Returns a compiled terra function representation of the calculation
function CompileCalculation(nodes)

end

-- The trick here is that hard-coded operations have the highest bit set, to ensure they will never collide with the dynamically generated
-- hash. Then, we build a switch statement that looks for hardcoded values we recognize, which the falls back to the dynamic hash.

-- Two modes are necessary, one where we build an optimal compilation of the function at compile time, and one that builds a runtime interpreter. This basically amounts to whether or not the switch statement is runtime or compile time.