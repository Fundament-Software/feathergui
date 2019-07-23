local acg = require 'feather.abstract-codegen'
local formatting = require 'feather.formatting'

local symcount = 0
local function gensym(name)
  name = name or "anon"
  symcount = symcount + 1
  return name .. "_" .. tostring(symcount)
end

local cpp_gen = acg.generator {
  genfile = "$preamble\n\n\n$(\n\n)defns",
  extern_c = "extern \"C\" {\n$(\n\n)defns\n}",
  defun = "$rettype $name($(, )args) {\n\t$(;\n\t)body;\n}",
  declfun = "$rettype $name($(, )args);",
  sym = "$type $name",
  lsh = "($op1 << $op2)",
  assign = "$dest = $expr",
  ret = "return $expr",
  reincast = "reinterpret_cast<$type*>($expr)",
  deref = "(* $expr)",
  field = "$base.$field",
  method = "$base.$name($(, )args)"
}


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
  local pathfun
  local funname = gensym("property_accessor")
  local pathexpr
  if dest_type == "string" then
    pathfun = {
      kind = "defun",
      rettype = "const char *",
      name = funname,
      args = {{kind = "sym", name = "base", type = "void *"}},
      body = {
        {kind = "sym", name = "ss", type = "std::stringstream"},
        {
          kind = "lsh",
          op1 = "ss",
          op2 = {
            kind = "field",
            base = {
              kind = "reincast",
              type = self.base_type,
              expr = "base"
            },
            field = table.concat(path, ".")
          }
        },
        "char * buff = malloc(ss.str.size()+1)",
        "strcpy(buff, ss.str().c_str())",
        {
          kind = "ret",
          expr = "buff"
        }
      }
    }
    local ref = terralib.externfunction(funname, &opaque -> rawstring)
    pathexpr = macro(function()
        return quote
          var buff = ref(self.base)
          defer formatting.free(buff)
        in
          buff
               end
    end)
  else
    error "type conversion not yet implemented"
  end
  self.builder.source_builder:insert(pathfun)
  return pathexpr
end

function binding_mt.__index:event(path, name)
  local eventfun, funname, eventexpr
  funname = gensym("event_callback")
  eventfun = {
    kind = "defun",
    rettype = "void",
    name = funname,
    args = {"void *base"},
    body = {
      { kind = "method",
        base = {
          kind = "deref",
          expr = {
            kind = "reincast",
            type = self.base_type,
            expr = "base",
          }
        },
        name = #path > 0 and table.concat(path, ".").."."..name or name,
        args = {}
      }
    }
  }
  local ref = terralib.externfunction(funname, &opaque -> {})
  eventexpr = macro(function(...)
      if select("#", ...) > 0 then
        error "passing arguments to C++ events is NYI"
      end
      return `ref([self.base])
  end)
  self.builder.source_builder:insert(eventfun)
  return eventexpr
end

local builder_mt = { __index = {} }

function builder_mt.__index:root_type()
  return &opaque
end

function builder_mt.__index:root(data)
  return setmetatable({
      base = data,
      builder = self,
      base_type = self.desc.root_type
                      }, binding_mt)
end

function builder_mt.__index:write_include(fname)
  local f = io.open(fname, "w")
  f:write(cpp_gen {
            kind = "genfile",
            preamble = self.desc.preamble,
            defns = {
              kind = "extern_c",
              defns = self.include_builder
            }
  })
  f:close()
end

function builder_mt.__index:write_source(fname)
  local f = io.open(fname, "w")
  f:write(cpp_gen {
            kind = "genfile",
            preamble = self.desc.preamble,
            defns = {
              kind = "extern_c",
              defns = self.source_builder
            }
  })
  f:close()
end

function builder_mt.__index:export_fun(rettype, name, args)
  self.include_builder:insert {
    kind = "declfun",
    rettype = rettype,
    name = name,
    args = args
  }
end

return function(desc)
  return setmetatable({
      desc = desc,
      include_builder = terralib.newlist{},
      source_builder = terralib.newlist{},
      n_generated = 0,
      compile_cache = {}
                      }, builder_mt)
end
