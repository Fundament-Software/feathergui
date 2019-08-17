local tunpack = unpack or table.unpack

local M = {}

function M.astype(typ)
  if terralib.types.istype(typ) then
    return typ
  elseif terralib.isquote(typ) then
    return typ:astype()
  else
    error "tried to convert something to a type that "
  end
end

function M.type_template(typefn)
  typefn = terralib.memoize(typefn)
  local function fn(...)
    local args = {...}
    for i = 1, select("#", ...) do
      args[i] = M.astype(args[i])
    end
    return typefn(tunpack(args))
  end
  return macro(fn, fn)
end

return M
