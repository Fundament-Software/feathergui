return function(pairs)
  local struct e {
    v: uint
  }

  terra e.metamethods.__eq(a : e, b : e) : bool return a.v == b.v end
  terra e.metamethods.__ne(a : e, b : e) : bool return a.v ~= b.v end

  function build(name, value, ...)
    if value ~= nil then
      e.methods[name] = constant(`[e]{[uint](value)})
      build(...)
    end
  end
  
  build(unpack(pairs))
  return e
end