

local bindings = {
  ['terra'] = require 'feather.binding.terra',
}

setmetatable(bindings, {
               __call = function(_, desc) return bindings['terra'](desc) end
})

return bindings
