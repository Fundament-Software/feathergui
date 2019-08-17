local Vec = require 'feather.vec'

local Rect = terralib.memoize(function(T)
  local struct s {
    data : T[4]
  }

  s.metamethods.type, s.metamethods.N = T, 4
  s.metamethods.__typename = function(self) return ("Rect:%s"):format(tostring(self.metamethods.type)) end
  s.metamethods.__apply = macro(function(self, index) return `self.data[index] end)

  local ops = { "__sub","__add","__mul","__div" }
  for _, op in ipairs(ops) do
    local terra array_op(a : T[4], b : T[4])
      var c : s
      for i = 0,4 do
        c.data[i] = operator(op, a[i], b[i])
      end
      return c
    end
    
    local terra SS_op(a : s, b : s) return array_op(a.data, b.data) end
    local terra ST_op(a : s, b : T) return array_op(a.data, array(b, b, b, b)) end
    local terra TS_op(a : T, b : s) return array_op(array(a, a, a, a), b.data) end
    
    s.metamethods[op] = terralib.overloadedfunction("array_op", { SS_op, ST_op, TS_op })
  end

  local terra array_eq(a : T[4], b : T[4])
    for i = 0,4 do
      if a[i] ~= b[i] then return false end
    end
    return true
  end
    
  local terra SS_eq(a : s, b : s) return array_eq(a.data, b.data) end
  local terra ST_eq(a : s, b : T) return array_eq(a.data, array(b, b, b, b)) end
  local terra TS_eq(a : T, b : s) return array_eq(array(a, a, a, a), b.data) end
  local terra SS_ne(a : s, b : s) return not array_eq(a.data, b.data) end
  local terra ST_ne(a : s, b : T) return not array_eq(a.data, array(b, b, b, b)) end
  local terra TS_ne(a : T, b : s) return not array_eq(array(a, a, a, a), b.data) end
  
  s.metamethods.__eq = terralib.overloadedfunction("array_eq", { SS_eq, ST_eq, TS_eq })
  s.metamethods.__ne = terralib.overloadedfunction("array_ne", { SS_ne, ST_ne, TS_ne })
  
  local lookups = {left = 0, top = 1, right = 2, bottom = 3, l = 0, t = 1, r = 2, b = 3 }
  s.metamethods.__entrymissing = macro(function(entryname, expr)
    if entryname == "topleft" then 
      return Vec(T, 4)
    end
    if lookups[entryname] then
      if lookups[entryname] < n then
        return `expr.v[ [ lookups[entryname] ] ]
      else
        error "Tried to look up letter that doesn't exist in an n-dimensional vector."
      end
    else
      error "That is not a valid field."
    end
  end)
  
  s.metamethods.__setentry = macro(function(entryname, expr, value)
    if value:gettype() ~= t then
      error "tried to set vector element to something other than the vector type"
    end
    if lookups[entryname] then
      if lookups[entryname] < n then
        return quote expr.v[ [ lookups[entryname] ] ] = value end
      else
        error "Tried to look up letter that doesn't exist in an n-dimensional vector."
      end
    else
      error "That is not a valid field."
    end
  end)

  return s
end)

fgRect = Rect(float)

struct URect {
  abs : fgRect 
  rel : fgRect
}

return Rect