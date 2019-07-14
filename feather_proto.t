
local nuklear = require 'nuklear'


local M = {}

-- object behavior for the body of an element generation.
local body_mt = {}

-- The length operator returns the number of child elements
function body_mt:__len()
  print("getting body length")
  terralib.printraw(self)
  print(self.elems)
  print(#self.elems)
  return #self.elems
end

-- calling it generates the code for rendering the child elements
function body_mt:__call()
  return self.elems:map(function(elem) return elem:generate(self.ctx, self.bind) end)
end

body_mt.__index = {}

function body_mt.__index:length() return #self.elems end

--Create a body generator object capturing the child elements, the drawing context and the bind context
local function make_body(elems, ctx, bind)
  return setmetatable({elems = elems, ctx = ctx, bind = bind}, body_mt)
end

-- Take an element and generate the drawing code for it
local function elem_generate(self, ctx, bind)
  terralib.printraw(self)
  --TODO: resolve bindings against the data in bind
  local bindings = {}
  for k, v in pairs(self.bindings) do
    print("resolving bindngs", k, v)
    bindings[k] = v:bind(bind)
  end
  local body = make_body(self.body, ctx, bind)
  return self.generator(ctx, bindings, body)
end

--Produce the element type specific metatable for a particular element type
local function make_elem_mt(generator)
  return {
    __index = {
      generator = generator,
      generate = elem_generate,
    },
    __call = elem_generate,
  }
end

local literal_mt = { __index = {}}
function literal_mt.__index:bind(data)
  return macro(function() return self.value end)
end
function literal_mt:__tostring()
  return "literal("..tostring(self.value)..")"
end

local function literal(value)
  return setmetatable({value = value}, literal_mt)
end

local function genpath(data, path)
  for _, name in ipairs(path) do
    data = `data.[name]
  end
  return data
end

local event_mt = { __index = {} }
function event_mt.__index:bind(data)
  return macro(function(...)
      return `[genpath(data, self.path)]:[self.name]([{...}])
               end
  )
end
function event_mt:__tostring()
  return "event(."..table.concat(self.path, ".")..":"..self.name..")"
end

local function event(desc)
  local path, eventname = string.match(desc, "([%.a-zA-Z_0-9]*):([a-zA-Z_][a-zA-Z_0-9]*)")
  assert(path, "invalid event specifier")
  local splitpath = {}
  for segment in string.gmatch(path, ".([a-zA-Z_][a-zA-Z_0-9]*)") do
    table.insert(splitpath, segment)
  end
  return setmetatable({path = splitpath, name = eventname}, event_mt)
end

local bind_mt = { __index = {} }
function bind_mt.__index:bind(data)
  return macro(function()
      return `[genpath(data, self.path)]
               end
  )
end
function bind_mt:__tostring()
  return "bind(."..table.concat(self.path, ".")..")"
end

local function bind(desc)
  local path = string.match(desc, "((.[a-zA-Z_][a-zA-Z_0-9]*)*)")
  assert(path, ("invalid bind specifier %q"):format(desc))
  local splitpath = {}
  for segment in string.gmatch(path, ".([a-zA-Z_][a-zA-Z_0-9]*)") do
    table.insert(splitpath, segment)
  end
  return setmetatable({path = splitpath, name = eventname}, event_mt)
end

local function model(desc)
  --TODO: implement model binding types
  error "model bindings are not yet implemented"
end

--metatable for template objects.
local template_mt = {}

local binding_handlers = {
  on_ = event,
  bind_ = bind,
  model_ = model,
  [""] = literal
}
local function binding_dispatch(name, value)
  print("dispatching binding", name, value)
  local prefix, basename
  if name:sub(1, 3) == "on_" then prefix, basename = "on_", name:sub(4)
  elseif name:sub(1, 5) == "bind_" then prefix, basename = "bind_", name:sub(6)
  elseif name:sub(1, 6) == "model_" then prefix, basename = "model_", name:sub(7)
  else prefix, basename = "", name
  end
  -- be sad about not having lpeg and lua patterns being limited.
  -- local prefix, basename = string.match(name, "(on_|bind_|model_|)([a-zA-Z_][a-zA-Z_0-9]*)")
  print("name breakdown", prefix, basename)
  assert(prefix, ("invalid binding name %q"):format(name))
  return basename, binding_handlers[prefix](value)
end

-- when a template is called with a table descriptor, it produces the element described
function template_mt:__call(desc)
  local res = {body = terralib.newlist{}, generator = nil, bindings = {}}
  --split the descriptor into string key and numeric key parts
  --string keys describe the bindings and attributes
  --while numeric keys form the body
  for k, v in pairs(desc) do
    if type(k) == "number" then
      res.body[k] = v
    elseif type(k) == "string" then
      local name, binding = binding_dispatch(k, v)
      print("binding", name, binding)
      res.bindings[name] = binding
    else
      error "invalid key type in element description"
    end
  end

  -- resolve the descriptor attributes and bindings with the slots of the template
  for k, v in pairs(self.binds) do
    if res.bindings[k] == nil then
      if v[2] == nil then -- if there is no default for a slot and no binding for it, there is an error
        error("Missing value for non-defaultable binding " .. k)
      end
      res.bindings[k] = literal(v[2])
    end
  end

  --turn the data into an element.
  setmetatable(res, self.elem_mt)

  return res
end

--create a template from a description of its slots and a generator function
--this is a low level UI used for creating elements that call drawing and layout code directly
--a higher level function will allow creation from other UI elements
local function template_build(desc)
  local self = {
    binds = desc.binds,
    generator = desc.generator,
    elem_mt = make_elem_mt(desc.generator)
  }
  setmetatable(self, template_mt)
  return self
end

-- string value and length
local terra strlen_count(str: &uint8): int
  var len: int = 0
  while str[len] ~= 0 do len = len + 1 end
  return len
end
local strlen = macro(function(str_expr)
  if terralib.isconstant(str_expr) then
    return {str_expr, #str_expr:asvalue()}
  else
    return quote
        var str = [str_expr]
      in
        str, strlen_count(str)
           end
  end
end)

--describe a button
M.button = template_build {
  binds = { --it has text and a click event
    text = {"string", "Button"},
    click = {"event", quote end}
  },
  generator = function(ctx, desc, body)
    return quote -- and is rendered like so
      if nuklear.nk_button_label(ctx, desc.text()) ~= 0 then
        desc.click()
      end
           end
  end
}

--describe a window
M.window = template_build {
  binds = { -- it has rectangles and a title
    x = {"number", 50},
    y = {"number", 50},
    w = {"number", 220},
    h = {"number", 220},
    title = {"string", "feather window"}
  },
  generator = function(ctx, desc, body)
    return quote -- and is rendered like so
      if nuklear.nk_begin(ctx, desc.title(), nuklear.nk_rect(desc.x(), desc.y(), desc.w(), desc.h()),
                          nuklear.NK_WINDOW_BORDER
                            or nuklear.NK_WINDOW_MOVABLE
                            or nuklear.NK_WINDOW_SCALABLE
                            or nuklear.NK_WINDOW_CLOSABLE
                         ) ~= 0 then
        [body()] -- The body of the window is the contents
      end
           end
  end
}

--describe a static layout row
M.layout_row_static = template_build {
  binds = { --it has the height and width of each cell
    height = {"number", 30},
    width = {"number", 80}
  },
  generator = function(ctx, desc, body)
    return quote -- and is rendered like so
        nuklear.nk_layout_row_static(ctx, desc.height(), desc.width(), [body:length()]) -- #body is how many elements are in the row
      [body()] --the body is the elements across the row
           end
  end
}
M.layout_row_dynamic = template_build {
  binds = {
    height = {"number", 30}
  },
  generator = function(ctx, desc, body)
    return quote
        nuklear.nk_layout_row_dynamic(ctx, desc.height(), [body:length()])
      [body()]
           end
  end
}

M.text = template_build {
  binds = {
    text = {},
    align = {"number", 0x11}
  },
  generator = function(ctx, desc, body)
    return quote
      nuklear.nk_text(ctx, strlen(desc.text()), desc.align)
           end
  end
}

M.text_wrap = template_build {
  binds = {
    text = {},
    --align = {"number", 0x11}
  },
  generator = function(ctx, desc, body)
    return quote nuklear.nk_text_wrap(ctx, strlen(desc.text())) end
  end
}

local ui_mt = {}

--create a datatype backing the UI.
--contains both the user specified data binding and the UI's internal state
local function make_root_type(datatype)
  local struct FeatherRoot {
    nkc: &nuklear.nkc
    data: datatype
                           }
  return FeatherRoot
end
make_root_type = terralib.memoize(make_root_type)

--when the ui template is called, rather than producing an element, it produces a UI outline
--the UI Outline can be called like a function with a specific datatype which will cause it
--to populate the bindings from that datatype and generate the specialized render code.
function ui_mt:__call(datatype)
  local root_type = make_root_type(datatype)
  local res = {}

  -- tag the resulting UI object with the types it uses
  res.bind_type = datatype
  res.app_type = root_type

  -- The generated event loop to perform the rendering and events of the UI
  terra res.mainfunc(data: &root_type)

    var ctx = nuklear.nkc_get_ctx(data.nkc)

    var e = nuklear.nkc_poll_events(data.nkc)

    if e.type == nuklear.NKC_EWINDOW and e.window.param == nuklear.NKC_EQUIT then
      nuklear.nkc_stop_main_loop(data.nkc)
    end

    [
      self.body:map(function(elem)
          return elem:generate(ctx, `data.data)
      end)
    ]

    nuklear.nk_end(ctx)

    nuklear.nkc_render(data.nkc, nuklear.nk_rgb(40, 40, 40))
  end

  return res
end

--The UI template function to build a UI outline
function M.ui(desc)
  local self = {
    body = terralib.newlist{}
  }
  for _, v in ipairs(desc) do
    self.body:insert(v)
  end

  setmetatable(self, ui_mt)
  return self
end

--TODO: build a way to declare templates from existing elements
function M.template(desc)
end

--Generate the code to launch the UI from a specific outline and root binding type
function M.launchUI(ui, datatype)
  local ui_compiled = ui(datatype)

  --DEBUG: show the code for the resulting UI event loop
  ui_compiled.mainfunc:printpretty()
  --The generated code to start the UI
  return terra(data: datatype)
    var nkcx: nuklear.nkc
    var root: ui_compiled.app_type
    root.nkc = &nkcx
    root.data = data

    if nuklear.nkc_init( root.nkc, "Nuklear+ Example", 640, 480, nuklear.NKC_WIN_NORMAL ) ~= nil then
      nuklear.printf("successfully initialized, entering main loop...\n")
      nuklear.nkc_set_main_loop(root.nkc, [&opaque->{}](ui_compiled.mainfunc), [&opaque](&root))
    else
      nuklear.printf("failed to init NKC\n")
    end
    nuklear.printf("exiting\n")
    nuklear.nkc_shutdown(root.nkc)
  end
end

return M
