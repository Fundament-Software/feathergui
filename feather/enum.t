local function Enum(lst)
  local struct enumset {
    v: uint
  }

  terra enumset.metamethods.__eq(a : enumset, b : enumset) : bool return a.v == b.v end
  terra enumset.metamethods.__ne(a : enumset, b : enumset) : bool return a.v ~= b.v end

  for i, name in ipairs(lst) do 
    enumset.methods[name] = constant(`[enumset]{(i - 1)})
  end
  
  return enumset
end

return Enum