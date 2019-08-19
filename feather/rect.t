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

  terra s.metamethods.__eq(a : s, b : s)
    for i = 0,4 do
      if a.data[i] ~= b.data[i] then return false end
    end
    return true
  end
  
  terra s.metamethods.__ne(a : s, b : s) : bool
    return not [s.metamethods.__eq](a, b)
  end
  
  local lookups = {left = 0, top = 1, right = 2, bottom = 3, l = 0, t = 1, r = 2, b = 3 }
  s.metamethods.__entrymissing = macro(function(entryname, expr)
    if entryname == "topleft" then 
      return `[Vec(T, 2)]{array(expr.data[0], expr.data[1])}
    end
    if entryname == "bottomright" then 
      return `[Vec(T, 2)]{array(expr.data[2], expr.data[3])}
    end
    if lookups[entryname] then
      if lookups[entryname] < 4 then
        return `expr.data[ [ lookups[entryname] ] ]
      else
        error ("Index "..lookups[entryname].." doesn't exist in a rect.")
      end
    else
      error (entryname.." is not a valid field.")
    end
  end)
  
  s.metamethods.__setentry = macro(function(entryname, expr, value)
    if value:gettype() ~= t then
      error "tried to set rect element to something other than the rect type"
    end
    if lookups[entryname] then
      if lookups[entryname] < 4 then
        return quote expr.data[ [ lookups[entryname] ] ] = value end
      else
        error ("Index "..lookups[entryname].." doesn't exist in a rect.")
      end
    else
      error (entryname.." is not a valid field.")
    end
  end)

  return s
end)

return Rect