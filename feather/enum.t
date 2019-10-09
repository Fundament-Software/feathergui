return function(lst, type)
  local struct e {
    v: type or uint
  }

  terra e.metamethods.__eq(a : e, b : e) : bool return a.v == b.v end
  terra e.metamethods.__ne(a : e, b : e) : bool return a.v ~= b.v end

  for i, name in ipairs(lst) do 
    e.methods[name] = constant(`[e]{(i - 1)})
  end
  
  return e
end