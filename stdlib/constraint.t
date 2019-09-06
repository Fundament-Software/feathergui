local M = {}
local IsType = terralib.types.istype
local List = require 'stdlib.terralist'

local args
do
    local function iter(invar, state)
        if state < invar[2] then
            return state + 1, invar[1][state+1]
        else
            return nil, nil
        end
    end

    function args(...)
        return iter, {{...}, select("#", ...)}, 0
    end
end

local function call(x, ...)
  return true, x(...)
end

local function PushError(err, errors, context, parent) 
  local e = {unpack(context)}
  if parent ~= nil then
    e[#e + 1] = parent
  end
  e[#e + 1] = err
  errors[#errors + 1] = e
  return errors
end

-- Returns true if a is a subset of b
local function CheckSubset(a, b, context)
  local objs = a:synthesize()
  if #objs == 0 then -- if synthesize returns no types, it's either an invalid constraint or it simply can't be synthesized
    error "Invalid synthesis"
    return false
  end
  return objs:all(function(e)
    local ok, err = call(b.pred, b, e, context)
    return ok
  end)
end

local Constraint_mt = {
  __call = function(self, obj, context)
    if context ~= nil and type(context) ~= "table" then
      error "Invalid context!"
    end
    local ok, err = call(self.pred, self, obj, context or {})
    if not ok then
      if type(err) ~= "table" then
        err = PushError(err, {}, context or {}, tostring(self))
      end
      
      if #err == 0 then
        error "unknown error occured"
      end

      local str = ""
      for _, e in ipairs(err) do
        for i, v in ipairs(e) do
          str = str..string.rep(" ", (i - 1) * 2)..v.."\n"
        end
      end

      error (str)
    end
    return err
  end,
  __add = function(a, b) -- or
    return M.MultiConstraint(false, nil, a, b)
  end,
  __mul = function(a, b) -- and
    return M.MultiConstraint(true, nil, a, b)
  end,
  __tostring = function(self) 
    return "Constraint["..(self.name or tostring(self.pred)).."]"
  end,
  __eq = function(self, b)
    return self.pred == b.pred and self:equal(b)
  end,
  __lt = function(self, b)
    return not self:equal(b) and self:subset(b, {})
  end,
  __le = function(self, b)
    return self:equal(b) or self:subset(b, {})
  end,
}

local function ConstraintName(constraint)
  if getmetatable(constraint) == Constraint_mt then
    return constraint.name or tostring(constraint.pred)
  end
  return tostring(constraint)
end

M.Constraint = function(predicate, synthesis, tag)
  return setmetatable({ pred = predicate, name = tag, synthesize = function(self) return List{synthesis} end }, Constraint_mt)
end

local function IsConstraint(obj)
  return getmetatable(obj) == Constraint_mt
end

-- The basic predicate checks to see if the types match
local BasicPredicate = function(self, obj, context)
  if obj ~= self.type then
      error (tostring(obj).." is not identical to "..tostring(self.type).."! Did you forget to memoize a type?")
  end
  return true
end

M.BasicConstraint = function(metatype)
  local t = { type = metatype, pred = BasicPredicate, name = (metatype == nil) and "nil" or ConstraintName(metatype) }

  function t:equal(b)
    return self.type == b.type
  end
  function t:synthesize()
    return List{self.type or tuple()}
  end
  function t:subset(obj, context)
    if type(obj) ~= self.type then
      error (type(obj).." does not equal "..self.type)
    end
    return {}
  end
  return setmetatable(t, Constraint_mt)
end

local ValuePredicate = function(self, obj, context)
  if self.constraint == nil then
    if obj ~= nil then
      error ("Expected nil, but found "..tostring(obj))
    end
    return {}
  end
  return self.constraint(obj:gettype(), context)
end

M.ValueConstraint = function(constraint)
  local t = { constraint = constraint, pred = ValuePredicate, name = "|"..((constraint == nil) and "nil" or ConstraintName(constraint)).."|" }

  if t.constraint ~= nil then
    t.constraint = M.MakeConstraint(constraint)
  end
  function t:equal(b)
    return self.constraint == b.constraint
  end
  function t:synthesize()
    return self.constraint and self.constraint:synthesize():map(function(e) return quote var v : e in v end end) or List{nil}
  end
  function t:subset(obj, context)
    if t.constraint == nil then
      if obj ~= nil then
        error ("expected nil but got "..tostring(obj))
      end
      return {}
    end
    return t.constraint:subset(obj, context)
  end
  return setmetatable(t, Constraint_mt)
end

local TypePredicate = function(self, obj, context)
  if not IsType(obj) then
    error (tostring(obj).." is not a terra type!")
  end
  if self.constraint ~= nil then
    return self.constraint(obj, context)
  end
  return {}
end

M.TypeConstraint = function(constraint)
  local t = { constraint = constraint, pred = TypePredicate, name = ((constraint == nil) and "{}" or ConstraintName(constraint)), subset = CheckSubset }

  if t.constraint ~= nil then
    t.constraint = M.MakeConstraint(constraint)
  end
  function t:equal(b)
    return self.type == b.type
  end
  function t:synthesize()
    return self.constraint and self.constraint:synthesize() or List{tuple()}
  end
  return setmetatable(t, Constraint_mt)
end

local LuaPredicate = function(self, obj, context)
  if self.type == nil then
    if type(obj) == nil then
      error "Expected lua object of any type, but found nil!"
    end
  else
    if type(obj) ~= self.type then
      error ("Expected lua object of type "..self.type.." but found "..type(obj))
    end
  end
end

M.LuaConstraint = function(luatype)
  local t = { type = luatype, pred = LuaPredicate, name = "Lua: "..(luatype or "Any") }
  function t:equal(b)
    return self.type == b.type
  end
  function t:synthesize()
    if self.type == "number" then
      return List{0}
    elseif self.type == "string" then
      return List{""}
    elseif self.type == "function" then
      return List{function() end }
    elseif self.type == "table" then
      return List{ {} }
    elseif self.type == nil then
      return List{0, "", function() end, {}}
    end
    return List()
  end
  function t:subset(obj, context)
    if type(obj) ~= self.type then
      error (type(obj).." does not equal "..self.type)
    end
    return {}
  end
  return setmetatable(t, Constraint_mt)
end

--[[local CastPredicate = function(self, obj, context)
  if IsType(obj) then
    error ("Type was passed into Cast constraint! Types should use Metatype constraints instead")
  end
  self.type(obj:gettype(), context)

  self.type:synthesize():app(function(e)
    if e:isunit() then
      if obj ~= nil then
        error ("Expected nil, but got "..tostring(obj))
      end
    end

    local val = `[e]([obj])
  end)
  return {}
end

M.CastConstraint = function(typedef)
  local t = { type = typedef, pred = CastPredicate, name = "["..ConstraintName(typedef).."]" }
  if not IsConstraint(t.type) then
    t.type = M.BasicConstraint(t.type, MetatypePredicateAny)
  end
  function t:equal(b)
    return self.type == b.type
  end
  function t:synthesize()
    return self.type:synthesize():map(function(e) return quote var v : e in v end end)
  end
  return setmetatable(t, Constraint_mt)
end--]]

local function MergeStruct(a, b)
  a = a:synthesize()
  b = b:synthesize()
  for k,v in pairs(b.methods) do 
    if a.methods[k] ~= nil and a.methods[k]:gettype() ~= v:gettype() then
      error ("Cannot merge different types "..tostring(a.methods[k]:gettype()).." and "..tostring(v:gettype()))
    end
    a.methods[k] = v
  end

  local fieldmap = {}
  for _,v in ipairs(a.entries) do 
    fieldmap[v.field] = v.type
  end

  for _,v in ipairs(b.entries) do
    if fieldmap[v.field] ~= nil and fieldmap[v.field] ~= v.type then
      error ("Cannot merge different types "..tostring(fieldmap[v.field]).." and "..tostring(v.type))
    end
    fieldmap[v.field] = v.type
    a.entries[#a.entries + 1] = { field = v.field, type = v.type }
  end

  return a
end

local MultiPredicate = function(self, obj, context)
  local errors = {}
  for _, v in ipairs(self.list) do
    local ok, err = call(v, obj, context)
    if not ok then
      PushError(err, errors, context, self.op and "CONSTRAINT[ALL]" or "CONSTRAINT[ANY]")
    elseif not self.op then
      return errors
    end
  end
  if #errors > 0 then
    error(errors)
  end
  return errors
end

-- "true" means use 'and', "false" means use 'or'
M.MultiConstraint = function(switch, id, ...)
  local t = { name = "(", list = List(), pred = MultiPredicate, op = switch, subset = CheckSubset }
  for i, v in args(...) do 
    if IsConstraint(v) and v.pred == t.pred and v.op == t.op then
      for _, value in ipairs(v.list) do
        t.list[#t.list + 1] = value
      end
    else
      t.list[#t.list + 1] = v
    end
  end

  if #t.list < 2 then
    error ("MultiConstraint needs multiple constraints to be valid, but it has "..#t.list)
  end

  for i, v in ipairs(t.list) do
    t.name = t.name .. "(" .. ConstraintName(v) .. ")"
    if i ~= #t.list then
      t.name = t.name .. (switch and " and " or " or ")
    end
  end
  t.name = t.name .. ")"
  
  if id ~= nil then
    t.name = id
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
  function t:synthesize()
    local all = self.list:flatmap(function(e) return e:synthesize() end)
    if not self.op then
      return all
    else
      return List{all:fold(struct {}, MergeStruct)}
    end
  end
  return setmetatable(t, Constraint_mt)
end

local PointerPredicate = function(self, obj, context)
  if not IsType(obj) then
    error (tostring(obj).." is not a terra type!")
  end
  for i = 1, self.count do
    if not obj:ispointer() then
      error ("Expected "..self.count.." indirection(s) on "..tostring(obj)..", but only found "..(i - 1).."!")
    end
    obj = obj.type
  end
  return self.constraint(obj, context)
end

local function nested_pointer(n, t)
    for i = 1, n do
        t = terralib.types.pointer(t)
    end
    return t
end

M.PointerConstraint = function(base, indirection)
  indirection = indirection or 1
  local t = { constraint = M.MakeConstraint(base), count = indirection, pred = PointerPredicate, name = string.rep("&", indirection)..ConstraintName(base), subset = CheckSubset }
  function t:equal(b)
    return self.constraint == b.constraint and self.count == b.count
  end
  function t:synthesize()
    return self.constraint:synthesize():map(function(e)
      return nested_pointer(self.count, e)
    end)
  end
  return setmetatable(t, Constraint_mt)
end

-- Metatable representing typed lua functions
local Metafunc_mt = {
  __call = function(self, ...)
    local params = {...}
    
    -- Clear out any meta-type parameters
    if self.typeparams ~= nil then
      for _, v in ipairs(self.typeparams) do
        v.type = nil
      end
    end

    -- Validate each parameter. If our parameter count exceeds our constraint, immediately fail.
    if select("#", ...) > #self.params and self.vararg == nil then
      error ("Function type signature expects at most "..#self.params.." parameters, but was passed "..select("#", ...).."!")
    end

    -- Otherwise, if our parameter count is lower than our constraint, we simply pass in nil and let the constraint decide if the parameter is optional.
    for i, v in ipairs(self.params) do -- This only works because we're using ipairs on self.parameters, which does not have holes
      v(params[i], {"Parameter #"..i})
    end

    -- If we allow varargs in our constraint and have leftover parameters, verify them all
    if self.vararg ~= nil then
      for i=#self.params + 1,#params do
        self.vararg(params[i], {"Parameter #"..i})
      end
    end
    
    local r = self.raw(...)
    self.result(r, {"Return value"})

    return r
  end
}

local function IsMetafunc(obj)
  --return getmetatable(obj) == Metafunc_mt
  return getmetatable(obj) == terralib.macro and obj.raw ~= nil
end

-- This builds a typed lua function using M.Number, M.String and M.Table (or just M.Value) to represent expected raw lua values, and
-- assumes metatype() is used to wrap types that should be passed in as terra types, not terra values of that type. It exposes this
-- type information so that constraints can query it, then appends the original function with constraint checks on the parameters
-- and return values.
function M.Meta(params, results, obj, varargs)
  if terralib.isfunction(params) then -- if you pass in just a normal terra function, this creates an accurate metatype wrapper around it.
    obj = params
    params = obj:gettype().parameters:map(function(e) return M.ValueConstraint(e) end)
    results = M.ValueConstraint(obj:gettype().returntype)
    varargs = obj:gettype().isvararg
    obj = function(...) return obj(...) end
  end

  results = results or tuple()
  local f = { params = List(), result = M.MakeValue(results), raw = obj, vararg = varargs, _internal = false }
  
  -- Transform all parameters into constraints
  for i, v in ipairs(params) do
    f.params[i] = M.MakeValue(v)
  end
  
  f.fromterra = function(...) return Metafunc_mt.__call(f, ...) end
  f.fromlua = f.fromterra
  return setmetatable(f, terralib.macro)
end

function M.MetaExpression(fn, ...)
  local params = List()

  for i, v in args(...) do 
    params[i] = M.MetaParameter(M.MakeConstraint(v))
  end
  local f = fn(unpack(params))
  f.typeparams = params
  return f
end

local FunctionPredicate = function(self, obj, context, parent)
  if terralib.isfunction(obj) then
    obj = M.Meta(obj)
  end
  if not IsMetafunc(obj) then
    error ("Expected terra function or metafunction but got "..tostring(obj))
  end

  local set = obj.params:map(function(e) return e:synthesize() end)
  local results = obj.result:synthesize()
  local max = set:map(function(e) return #e end):fold(#results, math.max)
  
  if max < 1 then
    error "invalid synthesis occured"
  end

  for i = 1,max do
    local params = set:map(function(e) return e[((i - 1) % #e) + 1] end)
    local result = results[((i - 1) % #results) + 1]
    if parent ~= nil then
      if #params < 1 or params[1].gettype == nil or not params[1]:gettype():ispointer() or params[1]:gettype().type ~= parent then
        error (self.method.." does not have a self parameter with a type matching &"..tostring(parent).." - Instead, found "..tostring(params[1]))
      end 
      table.remove(params, 1)
    end

    -- Validate each parameter. If our parameter count exceeds our constraint, immediately fail.
    if #params > #self.params and self.vararg == nil then
      error ("Constraint accepts up to "..#self.params.." parameters, but "..#params.." were passed in!")
    end

    local errors = {}

    -- Otherwise, if our parameter count is lower than our constraint, we simply pass in nil and let the constraint decide if the parameter is optional.
    for i, v in ipairs(self.params) do
      --local ok, err = call(CheckSubset, params[i], v, context)
      local ok, err = call(v, params[i], context)
      if not ok then
        PushError(err, errors, context)
      end
    end

    -- If we allow varargs in our constraint and have leftover parameters, verify them all
    if self.vararg ~= nil then
      for i=#self.params + 1,#params do
        --local ok, err = call(CheckSubset, params[i], self.vararg, context)
        local ok, err = call(self.vararg, params[i], context)
        if not ok then
          PushError(err, errors, context)
        end
      end
    end
    
    --local ok, err = call(CheckSubset, result, self.result, context)
    local ok, err = call(self.result, result, context)
    if not ok then
      PushError(err, errors, context)
    end

    if #errors > 0 then
      error(errors)
    end
  end
    
  return {}
end

local function FunctionConstraintName(params, result, vararg)
  local str = "{"
  for i, v in ipairs(params) do
    str = str .. ConstraintName(v)
    if i ~= #params then
      str = str..", "
    end
  end

  if vararg ~= nil then
    if #params > 0 then
      str = str..", "
    end

    str = str..tostring(vararg).."..."
  end

  return str .. "} -> " .. ConstraintName(result)
end

M.FunctionConstraint = function(parameters, results, varargs)
  if IsType(parameters) then
    if terralib.types.functype.ispointertofunction(parameters) then
      parameters = functype.type.parameters
      results = functype.type.returntype
    elseif terralib.types.functype.isfunction(parameters) then
      parameters = functype.parameters
      results = functype.returntype
    end
  end

  if results == nil then
    results = tuple()
  end

  local t = { params = List(), pred = FunctionPredicate, result = M.MakeValue(results), subset = CheckSubset }

  if varargs ~= nil then
    t.vararg = M.MakeValue(varargs)
  end

  for i, v in ipairs(parameters) do
    t.params[i] = M.MakeValue(v)
  end
  
  t.name = "Function: "..FunctionConstraintName(t.params, t.result, t.vararg)

  function t:equal(b)
    if #self.params ~= #b.params then
      return false
    end
    return self.params:alli(function(i, e) return e ~= b.params[i] end) and self.result == b.result and self.vararg == b.vararg
  end
  function t:synthesize()
    local set = self.params:map(function(e) return e:synthesize() end)
    local results = self.result:synthesize()
    local max = set:fold(results, function(a, b) return math.max(#a, #b) end)
    
    if max < 1 then
      error "invalid synthesis occured"
    end

    local f = List()
    for i = 1,max do
      f[i] = terralib.types.functype(set:map(function(e) return e[((i - 1) % #e) + 1] end), results[((i - 1) % #results) + 1], self.vararg and true or false)
    end

    return f
  end
  return setmetatable(t, Constraint_mt)
end

local function FieldPredicate(self, obj, context)
  for _, v in ipairs(obj.entries) do
    if v.field == self.field then
      return self.type(v.type, context)
    end
  end
  error ("Could not find field named "..self.field)
end

M.FieldConstriant = function(name, constraint)
  local t = { field = name, type = constraint, pred = FieldPredicate, name = "Field["..name.."]: "..ConstraintName(constraint), subset = CheckSubset }
  function t:equal(b)
    return self.field == b.field and self.type == b.type
  end
  function t:synthesize()
    return self.type:synthesize():map(function(e) 
      local s = struct {}
      s.entries[1] = { field = self.field, type = e:gettype() }
      return s
    end)
  end
  return setmetatable(t, Constraint_mt)
end

local MethodPredicate = function(self, obj, context)
  if not IsType(obj) then
    error (tostring(obj).." is not a terra type!")
  end
  if not obj:isstruct() then
    error (tostring(obj).." is not a terra struct, and therefore cannot have methods")
  end
  local func = obj.methods[self.method]
  if func == nil then
    error ("Could not find method named "..self.method.." in "..tostring(obj))
  end

  return FunctionPredicate(self, func, context, (not self.static) and obj or nil) -- Do not use (self.static and nil or obj), because nil evaluates to false and breaks the psuedo-ternary operator
end

M.MethodConstraint = function(name, parameters, results, static, varargs)
  local t = M.FunctionConstraint(parameters, results, varargs)
  t.name = "Method["..name.."]: "..FunctionConstraintName(t.params, t.result, t.vararg)
  t.pred = MethodPredicate
  t.method = name
  t.static = static or false

  local func_equal = t.equal
  local func_synth = t.synthesize
  function t:equal(b)
    return self.method == b.method and func_equal(self, b)
  end
  function t:synthesize()
    return func_synth(self):map(function(e) 
      local s = struct {}
      s.entries[1] = { field = self.method, type = e:gettype() }
      return s
    end)
  end
  return setmetatable(t, Constraint_mt)
end

local Tautalogy = M.Constraint(function() return true end, tuple(), "nil")

M.Any = function(...)
  if select("#", ...) == 0 then
    return Tautalogy
  end
  return M.MultiConstraint(false, nil, ...)
end

M.All = function(...) return M.MultiConstraint(true, nil, ...) end
M.Number = M.LuaConstraint("number")
M.String = M.LuaConstraint("string")
M.Table = M.LuaConstraint("table")
M.Value = M.LuaConstraint(nil)
M.TerraType = M.TypeConstraint(nil)
M.Field = M.FieldConstriant
M.Method = M.MethodConstraint
M.Integral = M.Constraint(function(obj) return IsType(obj) and obj:isintegral() end, int, "Integral")
M.Float = M.Constraint(function(obj) return IsType(obj) and obj:isfloat() end, float, "Float")
M.Pointer = M.PointerConstraint

-- Transforms an object into a constraint. Understands either a terra type, a terra function, a metafunction object, or a raw lua function.
function M.MakeConstraint(obj)
  if type(obj) == nil then return M.Any end -- nil is treated as accepting any type
  if IsConstraint(obj) then return obj end
  if IsType(obj) then return M.BasicConstraint(obj) end
  if type(obj) == "function" then return M.Constraint(obj) end -- A raw lua function is treated as a custom predicate
  error (tostring(obj).." isn't a valid type to convert to a constraint!")
  return nil
  --if type(obj) == "table"
end

-- Transforms an object into a value constraint.
function M.MakeValue(obj)
  if type(obj) == nil then return nil end
  if IsConstraint(obj) then 
    if obj.pred == ValuePredicate or obj.pred == TypePredicate or obj.pred == LuaPredicate then
      return obj
    end
    return M.ValueConstraint(obj)
  end
  if IsType(obj) then return M.ValueConstraint(obj) end
  if type(obj) == "function" then return M.ValueConstraint(M.Constraint(obj)) end -- A raw lua function is treated as a custom predicate
  error (tostring(obj).." isn't a valid type to convert to a constraint!")
  return nil
end

-- You can pass in a struct as a shortcut to defining a set of constraints
function M.Struct(name, fields)
  local constraints = {}
  if IsType(fields) and fields:isstruct() then
    for _, v in ipairs(fields.entries) do
      constraints[#constraints + 1] = M.FieldConstriant(v.field, M.MakeConstraint(v.type))
    end
    for k, v in ipairs(fields.methods) do
      constraints[#constraints + 1] = M.MethodConstriant(k, M.MakeConstraint(v:gettype()))
    end
  end
  return M.MultiConstraint(true, name, unpack(constraints))
end

local function ParameterPredicate(self, obj, context)
  if self.type ~= nil and self.type ~= obj then
    error ("Type parameter violated! Expected all parameters to have type "..tostring(self.type).." but found type "..tostring(obj).." instead!")
  end

  self.type = obj
  return self.constraint(obj, context)
end

M.MetaParameter = function(constraint)
  local t = { type = nil, constraint = M.MakeConstraint(constraint), pred = ParameterPredicate, name = "MetaParameter: "..ConstraintName(constraint) }

  function t:equal(b)
    return self.constraint == b.constraint
  end
  function t:synthesize()
    return t.constraint:synthesize()
  end
  function t:subset(b, context)
    return t.constraint:subset(b, context)
  end
  return setmetatable(t, Constraint_mt)
end

local function MetaConstraintPredicate(self, obj, context)
  for _, v in ipairs(self.params) do
    v.type = nil
  end
  return self.constraint(obj, context)
  -- If we were tracking additional constraints on the type parameters, we would check them here
end

-- A metaconstraint wraps a more complex constraint statement by applying type relation constraints inside the generated constraint object
function M.MetaConstraint(fn, ...)
  local t = { params = {}, constraint = nil, pred = MetaConstraintPredicate, name = "MetaConstraint[" }
  function t:equal(b)
    for i, v in ipairs(self.params) do
      if v ~= b.params[i] then
        return false
      end
    end
    return self.constraint == b.constraint
  end
  function t:synthesize()
    return t.constraint:synthesize()
  end
  function t:subset(b, context)
    return t.constraint:subset(b, context)
  end

  for i, v in args(...) do 
    if IsConstraint(v) and v.pred == ParameterPredicate then
      t.params[i] = v
    else 
      t.params[i] = M.MetaParameter(v)
    end

    t.name = t.name .. ConstraintName(t.params[i].constraint) -- skip the "MetaParameter" part of the name
    if i ~= select("#", ...) then
      t.name = t.name .. ", "
    end
  end
  t.constraint = fn(unpack(t.params))
  t.name = t.name .. "]: " .. ConstraintName(t.constraint)

  return setmetatable(t, Constraint_mt)
end

return M