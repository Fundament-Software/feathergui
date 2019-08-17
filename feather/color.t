local function GetBitMask(low, high)
  return bit.lshift((bit.lshift(2, ((high)-(low)))-1), low)
end

local function GetBitType(count)
  if count > 32 then
    return uint64
  end
  return uint32
end

local Color = terralib.memoize(function(abits, rbits, gbits, bbits)
  local struct s {
    v : GetBitType(abits + rbits + gbits + bbits)
  }

  s.metamethods.type = s.entries[1].type
  s.metamethods.N = {abits, rbits, gbits, bbits}
  s.metamethods.__typename = function(self) return ("Color:%s[%d:%d:%d:%d]"):format(tostring(self.metamethods.type),self.metamethods.N[1],self.metamethods.N[2],self.metamethods.N[3],self.metamethods.N[4]) end
  local lookup = {"b", "g", "r", "a"}

  local lookuplow = {b = 0, g = bbits, r = gbits + bbits, a = rbits + gbits + bbits }
  local lookuphigh = {b = bbits - 1, g = gbits + bbits - 1, r = rbits + gbits + bbits - 1, a = abits + rbits + gbits + bbits - 1 }
  s.metamethods.__entrymissing = macro(function(entryname, expr)
    if lookuplow[entryname] then
        return `(expr.v and [ GetBitMask(lookuplow[entryname], lookuphigh[entryname]) ]) >> [ lookuplow[entryname] ]
    else
      error "That is not a valid color component."
    end
  end)
   
  s.metamethods.__setentry = macro(function(entryname, expr, value)
    if not value:gettype():isintegral() then
      error "tried to set color to something non-integral type"
    end
    if lookuplow[entryname] then
      return quote expr.v = (expr.v and not [ GetBitMask(lookuplow[entryname], lookuphigh[entryname]) ]) or ((value << [ lookuplow[entryname] ]) and [ GetBitMask(lookuplow[entryname], lookuphigh[entryname]) ] ) end
    else
      error "That is not a valid color component."
    end
  end)

  return s
end)

fgColor = Color(8,8,8,8)
fgColor64 = Color(16,16,16,16)

return Color