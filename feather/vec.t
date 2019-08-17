local Vec = terralib.memoize(function(t, n)
  local struct s {
    v : t[n]
  }

  s.metamethods.type, s.metamethods.N = t, n
  s.metamethods.__typename = function(self) return ("Vec:%s[%d]"):format(tostring(self.metamethods.type),self.metamethods.N) end
  s.metamethods.__apply = macro(function(self, index) return `self.v[index] end)

  local ops = { "__sub","__add","__mul","__div" }
  for _, op in ipairs(ops) do
    --[[local function operation(a, b)
      return quote
        var c : s
        for i = 0,n do
          c.v[i] = operator(op, a, b)
        end
        return c
      end
    end--]]

    local terra SS_op(a : s, b : s)
      var c : s
      for i = 0,n do
        c.v[i] = operator(op, a.v[i], b.v[i])
      end
      return c
    end
    s.metamethods[op] = SS_op
  end

  terra s.metamethods.__eq(a : s, b : s) : bool
    for i = 0,n do
      if a.v[i] ~= b.v[i] then return false end
    end
    return true
  end

  terra s.metamethods.__ne(a : s, b : s) : bool
    return not [s.metamethods.__eq](a, b)
  end

  local lookups = {x = 0, y = 1, z = 2, w = 3 }
  s.metamethods.__entrymissing = macro(function(entryname, expr)
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

fgVec = Vec(float, 2)
fgVec3D = Vec(float, 3)
fgVeci = Vec(int, 2)

struct UVec {
  abs : fgVec
  rel : fgVec
}

return Vec