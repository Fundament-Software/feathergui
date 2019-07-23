

local bindings = {
  ['terra'] = require 'feather.binding.terra',
  cpp_static = require 'feather.binding.cpp_static'
}

setmetatable(bindings, {
               __call = function(_, desc) return bindings['terra'](desc) end
})

return bindings
