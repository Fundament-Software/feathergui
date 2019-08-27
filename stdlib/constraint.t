local M = {}
local IsType = terralib.types.istype
local IsFuncPtr = terralib.types.functype.ispointertofunction
local IsFunc = terralib.types.functype.isfunction

local Constraint_mt = {
  __call = function(self, obj)
    return self.pred(obj)
  end,
  __add = function(a, b) -- or
    return M.MultiConstraint(false, a, b)
  end,
  __mul = function(a, b) -- and
    return M.MultiConstraint(true, a, b)
  end,
  __tostring = function(self) 
    return "Constraint["..(self.name or tostring(self.pred)).."]"
  end,
  __eq = function(self, b)
    return self.pred == b.pred and self:equal(b)
  end,
  __lt = function(self, b)
    return not self:equal(b) and self:subset(b)
  end,
  __le = function(self, b)
    return self:equal(b) or self:subset(b)
  end,
   __toterraexpression = function(self) return self end
}

M.Constraint = function(predicate)
  return setmetatable({ pred = predicate }, Constraint_mt)
end

local MultiPredicate = function(self, obj)
  for _, v in ipairs(self.list) do
    if v(obj) ~= self.op then -- an 'and' list short-circuits as soon as something is false, returning false. An 'or' list short-circuits as soon as something is true, returning true.
      return not self.op
    end
  end
  return self.op -- If the whole list was evaluated for 'and', return true. If the whole list was evaluated for 'or', return false.
end

-- "true" means use 'and', "false" means use 'or'
M.MultiConstraint = function(switch, ...)
  local t = { list = {}, pred = MultiPredicate, op = switch }
  for _, v in ipairs({...}) do
    if IsConstraint(v) and v.pred == t.pred and v.op == t.op then
      for _, value in ipairs(v.list) do
        list[#list + 1] = value
      end
    else
      list[#list + 1] = v
    end
  end

  if #t.list < 2 then
    error "MultiConstraint needs multiple constraints to be valid"
  end

  for i, v in ipairs(t.list) do
    t.name = t.name .. "(" .. v .. ")"
    if i ~= #t.list then
      t.name = t.name .. (switch and " and " or " or ")
    end
  end

  function t:equal(b)
    if #self.list ~= #b.list or self.op ~= b.op then
      return false
    end
    for i, v in ipairs(self.list) do
      if v ~= b.list[i] then
        return false
      end
    end
    return true
  end
  function t:subset(b)
    if b.op and self.op then -- If we are both minterms, we are a subset of b only if all our terms are a subset of at least one term in b
      return true
    else
      return true
    end
  end
  return setmetatable(t, Constraint_mt)
end

local function attempt_cast(type, obj)
  return `[type]([obj])
end

local CastPredicate = function(self, obj)
  if self.type:isunit() then
    return obj == nil
  end
  return pcall(attempt_cast, self.type, obj)
end

-- `from` is a subset of `to` if we can cast an object of type `from` to type `to`
local function attempt_typecast(from, to)
  return quote
    var a : from 
    return [to](a)
  end
end

M.CastConstraint = function(typedef)
  if not IsType(typedef) then
    error "Tried to build CastConstraint out of something other than a terra type!"
  end

  local t = { type = typedef, pred = CastPredicate, name = "Cast: "..tostring(typedef) }
  function t:equal(b)
    return self.type == b.type
  end
  function t:subset(b)
    return pcall(attempt_typecast, self.type, b)
  end
  return setmetatable(t, Constraint_mt)
end

local MetatypePredicate = function(self, obj)
    if self.type == nil then
      return IsType(obj)
    end
    for i = 1, self.indirection do
      if not obj:ispointer() then
        return false
      end
      obj = obj.type
    end
    return obj == self.type
end

M.MetatypeConstraint = function(metatype, indirection)
  indirection = indirection or 0
  if metatype ~= nil and not IsType(metatype) then
    error "Tried to build MetatypeConstraint out of something other than a terra type!"
  end

  local t = { type = metatype, count = indirection, pred = MetatypePredicate, name = "Metatype: "..string.rep("&", indirection)..(metatype or "Any") }

  function t:equal(b)
    return self.type == b.type
  end
  function t:subset(b)
    return b.type == nil -- is only a subset of the catchall constraint
  end
  return setmetatable(t, Constraint_mt)
end

-- Metatable representing typed lua functions
local Metafunc_mt = {
  __call = function(self, ...)
    local params = {...}
    
    -- Validate each parameter. If our parameter count exceeds our constraint, immediately fail.
    if #params > #self.parameters then
      error ("Function type signature expects at most "..#self.parameters.." parameters, but was passed "..#params.."!")
    end

    -- Otherwise, if our parameter count is lower than our constraint, we simply pass in nil and let the constraint decide if the parameter is optional.
    for i, v in ipairs(self.parameters) do
      if not v(params[i]) then
        error ("Parameter #"..i.." failed to satisfy "..v)
      end
    end

    local r = self.raw(unpack({...}))
    if not self.result(r) then
      error ("Return value failed to satisfy "..v)
    end

    return r
  end,
}

-- This builds a typed lua function using M.Number, M.String and M.Table (or just M.Value) to represent expected raw lua values, and
-- assumes metatype() is used to wrap types that should be passed in as terra types, not terra values of that type. It exposes this
-- type information so that constraints can query it, then appends the original function with constraint checks on the parameters
-- and return values.
function M.Meta(typedef, obj)
  if not is_funcptr(typedef) then
    error "A typed lua function must provide it's type in the form of a function pointer, like { int32 } -> int32"
  end

  local f = { parameters = {}, result = M.Struct(typedef.type.returntype) }

  -- Transform all parameters into constraints
  for i, v in ipairs(typedef.type.parameters) do
    f.parameters[i] = M.MakeConstraint(v)
  end

  return setmetatable(f, Metafunc_mt)
end

local function IsMetafunc(obj)
  return getmetatable(obj) == Metafunc_mt
end

local function IsConstraint(obj)
  return getmetatable(obj) == Constraint_mt
end

local function FunctionSubset(LeftParams, LeftResult, RightParams, RightResult)
  if #LeftParams > #RightParams then
    return false
  end

  -- Otherwise, if our parameter count is lower than our constraint, we simply pass in nil and let the constraint decide if the parameter is optional.
  for i, v in ipairs(RightParams) do
    if not (LeftParams[i] <= v) then
      return false
    end
  end

  return LeftResult <= RightResult
end

local FunctionPredicate = function(self, obj)
  if terralib.isfunction(obj) then
    -- Validate each parameter. If our parameter count exceeds our constraint, immediately fail.
    if #obj:gettype().parameters > #self.params then
      return false
    end

    -- Otherwise, if our parameter count is lower than our constraint, we simply pass in nil and let the constraint decide if the parameter is optional.
    for i, v in ipairs(self.params) do
      if not v(obj:gettype().parameters[i]) then
        return false
      end
    end

    return self.result(obj:gettype().returntype)
  elseif IsMetafunc(obj) then
    return FunctionSubset(self.params, self.result, obj.params, obj.result) 
  else
    return false
  end
end

local function FunctionConstraintName(params, result)
  local str = "{"
  for i, v in ipairs(params) do
    str = str .. v
    if i ~= #params then
      str = str..", "
    end
  end

  str = str .. "} -> " .. result
end

M.FunctionConstraint = function(functype)
  if IsFuncPtr(functype) then
    functype = functype.type
  elseif IsFunc(functype) then
    -- functype already points at true function type
  else
    error "Tried to build FunctionConstraint out of something other than a terra function or terra function pointer!"
  end

  local t = { params = {}, pred = FunctionPredicate, result = M.MakeConstraint(functype.returntype) }
  for i, v in ipairs(functype.parameters) do
    t.params[i] = M.MakeConstraint(v)
  end
  
  t.name = "Function: "..FunctionConstraintName(t.params, t.result)

  function t:equal(b)
    if #self.params ~= #b.params then
      return false
    end
    for i, v in ipairs(self.params) do
      if v ~= b.params[i] then
        return false
      end
    end
    return self.result == b.result
  end
  function t:subset(b)
    return FunctionSubset(self.params, self.result, b.params, b.result) 
  end
  return setmetatable(t, Constraint_mt)
end

local LuaPredicate = function(self, obj)
  if self.type == nil then
    return type(obj) ~= nil
  end
  return type(obj) == self.type
end

M.LuaConstraint = function(luatype)
  local t = { type = luatype, pred = LuaPredicate, name = "Lua: "..(luatype or "Any") }
  function t:equal(b)
    return self.type == b.type
  end
  function t:subset(b)
    return b.type == nil -- is only a subset of the catchall type
  end 
  return setmetatable(t, Constraint_mt)
end

local function FieldPredicate(self, obj)
  for _, v in ipairs(obj.entries) do
    if v.field == self.field then
      return self.type(v.type)
    end
  end
  return false
end

M.FieldConstriant = function(name, constraint)
  local t = { field = name, type = constraint, pred = FieldPredicate, name = "Field: "..constraint }
  function t:equal(b)
    return self.field == b.field and self.type == b.type
  end
  function t:subset(b)
    return self.field == b.field and self.type <= b.type
  end 
  return setmetatable(t, Constraint_mt)
end

local function MethodPredicate(self, obj)
  if not obj.methods[self.method] then
    return false
  end
  return self.type(obj.methods[self.method])
end

M.MethodConstraint = function(name, constraint)
  local t = { method = name, type = constraint, pred = MethodPredicate, name = "Method: "..constraint }
  function t:equal(b)
    return self.method == b.method and self.type == b.type
  end
  function t:subset(b)
    return self.method == b.method and self.type <= b.type
  end 
  return setmetatable(t, Constraint_mt)
end

M.Any = function(...)
  if #{...} == 0 then
    return M.Constraint(function() return true end)
  end
  return M.MultiConstraint(false, unpack({...}))
end

M.All = function(...) return M.MultiConstraint(true, unpack({...})) end
M.Number = M.LuaConstraint("number")
M.String = M.LuaConstraint("string")
M.Table = M.LuaConstraint("table")
M.Value = M.LuaConstraint(nil)
M.TerraType = M.MetatypeConstraint(nil)
M.Optional = function(x) return M.Any + M.MakeConstraint(x) end
M.Field = M.FieldConstriant
M.Method = M.MethodConstraint
M.Integral = M.Constraint(function(obj) return IsType(obj) and obj:isintegral() end)
M.Float = M.Constraint(function(obj) return IsType(obj) and obj:isfloat() end)
M.Pointer = function(a) return M.MetatypeConstraint(a, 1) end

-- Transforms an object into a constraint. Understands either a terra type, a terra function, a metafunction object, or a raw lua function.
function M.MakeConstraint(obj)
  if IsConstraint(obj) then return obj end
  if IsFunc(obj) or IsFuncPtr(obj) then return M.FunctionConstraint(obj) end  -- A function pointer type, such as {int} -> int, is shorthand for a function type constraint
  if IsType(obj) then return M.CastConstraint(obj) end
  if type(obj) == "function" then return M.Constraint(obj) end -- A raw lua function is treated as a custom predicate
  if type(obj) == nil then return M.Any end -- nil is treated as accepting any type
  return nil
  --if type(obj) == "table"
end

-- You can pass in a struct as a shortcut to defining a set of constraints
function M.Struct(fields)
  local constraints = {}
  if IsType(fields) and fields:isstruct() then
    for _, v in ipairs(fields.entries) do
      constraints[#constraints + 1] = M.FieldConstriant(v.field, M.MakeConstraint(v.type))
    end
    for k, v in ipairs(fields.methods) do
      constraints[#constraints + 1] = M.MethodConstriant(k, M.MakeConstraint(v:gettype()))
    end
  end
  return M.MultiConstraint(true, unpack(constraints))
end

local function ParameterPredicate(self, obj)
  if self.type ~= nil and self.type ~= obj then
    return false
  end

  self.type = obj
  return self.constraint(obj)
end

-- Represents a type parameter in a metaconstraint, with an optional constraint that the type must satisfy. Stores the type passed to it.
M.MetaParameter = function(constraint)
  local t = { type = nil, constraint = M.MakeConstraint(constraint), pred = ParameterPredicate, name = "MetaParameter: "..tostring(constraint) }
  function t:equal(b)
    return self.constraint == b.constraint
  end
  return setmetatable(t, Constraint_mt)
end

-- A metaconstraint wraps a more complex constraint statement by applying type relation constraints inside the generated constraint object
function M.MetaConstraint(fn, ...)
  local params = {}
  for i, v in ipairs({...}) do
    params[i] = M.MetaParameter(v)
  end
  return fn(unpack(params))
end

return M