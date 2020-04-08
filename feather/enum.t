return function(lst, type)
  local struct e {
    val: type or uint
  }

  terra e.metamethods.__eq(a : e, b : e) : bool return a.val == b.val end
  terra e.metamethods.__ne(a : e, b : e) : bool return a.val ~= b.val end

  e.enum_values = {}

  for i, name in ipairs(lst) do 
    e.methods[name] = constant(`[e]{(i - 1)})
    e.enum_values[name] = i - 1
  end

  e.convertible = "enum"
  return e
end
