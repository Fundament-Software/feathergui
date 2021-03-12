local core = require 'feather.core'
local shared = require 'feather.shared'

return core.basic_template {
  color = `[shared.Color]{0},
  outline = `[shared.Color]{0},
  border = `0.0f,
  innerRadius = `0.0f, -- radius of the cutout inside the circle
  innerBorder = `0.0f, -- radius of the border from the inner circle
  blur = `0.0f
} (function(context, params)
    return quote
        var x, y = context.transform.r.x, context.transform.r.y
        var w, h = context.rtree_node.extent.x, context.rtree_node.extent.y
        var rect = shared.Rect{array(x-w, y-h, x+w, y+h)}
        [context.backend]:DrawCircle([context.window], &rect, [params.color], [params.border], [params.outline], [params.blur], [params.innerRadius], [params.innerBorder], nil, 0, nil)
      end
   end
  )
