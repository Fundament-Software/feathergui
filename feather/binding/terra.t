local formatting = require 'feather.formatting'

local binding_mt = { __index = {} }

local function genpath(data, path)
  for _, name in ipairs(path) do
    data = `data.[name]
  end
  return data
end

local function gen_fmt(fmt)
  return function(val)
    return quote
      var len = formatting.snprintf(nil, 0, fmt, val)
      var buff = [&int8](formatting.malloc(len+1))
      defer formatting.free(buff)
      len = formatting.snprintf(buff, len+1, fmt, val)
      in
        buff
           end
  end
end

local function id(x) return x end

local baserules = {
  string = {
    [int] = gen_fmt "%d",
    [float] = gen_fmt "%f",
  },
  number = {
    [int] = id,
    [float] = id
  }
}

local function typeconversion(src, dest)
  local cvt = baserules[dest] and baserules[dest][src]
  if not cvt then
    error(("no known type conversion %s to %s"):format(src, dest))
  end
  return cvt
end

local property_mt = { __index = {} }

function property_mt:__toterraexpression()
  local pathexpr = genpath(self.base, self.path)
  return typeconversion(pathexpr:gettype(), self.dest_type)(pathexpr)
end

function binding_mt.__index:property(path, dest_type)
  local pathexpr = genpath(self.base, path)
  return macro(function()
      return typeconversion(pathexpr:gettype(), dest_type)(pathexpr)
  end)
end

function binding_mt.__index:event(path, name)
  local pathexpr = genpath(self.base, path)
  return macro(function(...)
      return `[pathexpr]:[name]([{...}])
  end)
end

local builder_mt = { __index = {} }

function builder_mt.__index:root_type()
  return self.basetype
end

function builder_mt.__index:root(data)
  return setmetatable({ base = data }, binding_mt)
end

return function(desc)
  return setmetatable({ basetype = desc }, builder_mt)
end
