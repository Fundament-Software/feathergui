
local nuklear = terralib.includecstring([[
//#include "nuklear.h"
#include "nuklear_cross.h"
]],
  {--[["-DNKC_IMPLEMENATION",]] "-DNKCD=NKC_XLIB"})

terralib.linklibrary "./libnkc.so"
terralib.linklibrary "/nix/store/zbafyh2j4mw1gd19mr14062kc0jkazyw-user-environment/lib/libX11.so"

return nuklear
