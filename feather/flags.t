local function Flags(lst)
  local map = {}
  for i, v in ipairs(lst) do
    map[v] = i - 1
  end

  local struct flagset {
    val: uint
  }

  flagset.metamethods.__add = macro(function(a, b)
      return `[flagset]{a.val or b.val}
  end)

  flagset.metamethods.__entrymissing = macro(function(entryname, flag_expr)
      if map[entryname] then
        return `(flag_expr.val and [2^map[entryname]]) ~= 0
      else
        error "No flag of that name is defined."
      end
  end)

  flagset.metamethods.__setentry = macro(function(entryname, flag_expr, value_expr)
      if value_expr:gettype() ~= bool then
        error "Flags can only be true or false."
      end
      if map[entryname] then
        return quote flag_expr.val = (flag_expr.val and not [2^map[entryname]]) or terralib.select(value_expr, [2^map[entryname]], 0) end
      else
        error "No flag of that name is defined."
      end
  end)

  for i, v in ipairs(lst) do
    flagset.methods[v] = constant(`[flagset]{[2^(i - 1)]})
  end

  return flagset
end

return Flags