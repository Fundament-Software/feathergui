-- EVIL HACK to bypass strict mode blocking LuLPeg when the _ENV local isn't present
_G._ENV = nil

local lpeg = require 'LuLPeg.lulpeg'

local M = {}

--[[



]]

local function generator_literal(value)
  return function(buff, i)
    buff[i] = value
    return i + 1
  end
end

local function dispatch_tree(buff, i, rules, tree)
  local typ = type(tree)
  if typ == "string" then
    buff[i] = tree
    return i + 1
  elseif typ == "table" then
    if tree.kind then
      if rules[tree.kind] then
        return rules[tree.kind](buff, i, tree)
      else
        error("no rule for the kind "..tostring(tree.kind))
      end
    else
      error("a tree missing a kind was provided where a kind was required")
    end
  else
    buff[i] = tostring(tree)
    return i + 1
  end
end

local function generator_variable(varname, rules)
  return function(buff, i, data)
    return dispatch_tree(buff, i, rules, data[varname])
  end
end

local function dispatch_treelist(buff, i, rules, sep, tree)
  print(buff, i, rules, sep, tree)
  local typ = type(tree)
  if typ == "string" then
    buff[i] = tree
    buff[i+1] = sep
    return i + 2
  elseif typ == "table" then
    if tree.kind then
      if rules[tree.kind] then
        i = rules[tree.kind](buff, i, tree)
        buff[i] = sep
        return i + 1
      else
        error("no rule for the kind "..tostring(kind))
      end
    else
      if #tree == 0 then return i + 1 end
      for idx, v in ipairs(tree) do
        i = dispatch_treelist(buff, i, rules, sep, v)
      end
      return i
    end
  else
    buff[i] = tostring(tree)
    buff[i+1] = sep
    return i + 2
  end
end

local function generator_list(sep, varname, rules)
  return function(buff, i, data)
    i = dispatch_treelist(buff, i, rules, sep, data[varname])
    buff[i-1] = nil
    return i - 1
  end
end

local function generator_compose(a, b)
  return function(buff, i, data)
    return b(buff, a(buff, i, data), data)
  end
end

local compiler_pat
do
  local P, C, Carg, Cf, Cc, Cs = lpeg.P, lpeg.C, lpeg.Carg, lpeg.Cf, lpeg.Cc, lpeg.Cs
  local lc = lpeg.locale()
  local var_pat = (C(lc.alnum^1) * Carg(1)) / generator_variable
  local varlist_pat = (P"(" * C((1-P")")^0) * ")" * C(lc.alnum^1) * Carg(1)) / generator_list
  local splice_pat = P"$" * ( var_pat + varlist_pat + P"$"*Cc(generator_literal("$")) )
  local literal_pat = C((1-splice_pat)^1) / generator_literal
  compiler_pat = Cf((literal_pat + splice_pat)^0, generator_compose)
end

local generator_mt = {}

function generator_mt:__call(tree)
  local buff = {}
  dispatch_tree(buff, 1, self, tree)
  for k, v in ipairs(buff) do
    print(k, v)
  end
  return table.concat(buff, "")
end

local function make_generator(desc)
  local self = {}
  for k, v in pairs(desc) do
    self[k] = compiler_pat:match(v, 1, self)
  end
  return setmetatable(self, generator_mt)
end

M.generator = make_generator

return M
