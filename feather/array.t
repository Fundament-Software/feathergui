C = terralib.includecstring [[
	#include <stdlib.h>
  #include <string.h>
]]

local Util = require 'feather.util'

local Array = Util.type_template(function(T)
	local struct s {
		data : &T;
		size : uint;
    capacity : uint;
	}

	function s.metamethods.__typename(self)
	    return "Array("..tostring(T)..")"
	end

  s.methods.new = macro(function() return `[&s](C.calloc(1, sizeof(s))) end)

	terra s:reserve(n : uint) : bool
    if n > self.capacity then
      var ptr = C.realloc(self.data, n*sizeof(T))
      if ptr == nil then
        return false
      end
      
      self.data = [&T](ptr)
      self.capacity = n
    end
    return true
  end

  local terra checkcapacity(self : &s, n : uint) : bool 
    if n > self.capacity then return self:reserve(n * 2) end
    return true
  end

	terra s:add(obj : T) : bool
    if not checkcapacity(self, self.size + 1) then 
      return false
    end
    self.data[self.size] = obj
    self.size = self.size + 1
	end

	terra s:insert(obj : T, index : uint) : bool
    if index >= self.size then
      return self:add(obj)
    end
    if not checkcapacity(self, self.size + 1) then 
      return false
    end

    C.memmove(self.data + index + 1, self.data + index, (self.size - index) * sizeof(T))
    self.data[index] = obj
    self.size = self.size + 1
    return true
  end

  terra s:clear() : {}
    self.size = 0
  end

	terra s:free() : {}
    if self.data ~= nil then
		  C.free(self.data)
    end
    C.free(self)
	end

	s.metamethods.__apply = macro(function(self,idx)
		return `self.data[idx]
	end)

	return s
end)

return Array
