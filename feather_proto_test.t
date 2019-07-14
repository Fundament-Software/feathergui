
local feather = require 'feather_proto'

local test_ui = feather.ui {
  feather.window {
    title = "Feather Test!",
    feather.layout_row_dynamic {
      height = 30,
      --width = 60,
      feather.button {
        text = "Hello",
        on_click = ":inc"
      },
      feather.button {
        text = "Goodbye",
        on_click = ":dec"
      }
    },
    feather.layout_row_dynamic {
      --width = 160,
      height = 50,

      feather.text_wrap {
        text = "test text that is much longer and might overflow the box"
      }
    }
  }
}

local struct count_data {
  count: int
                        }

local C = terralib.includec "stdio.h"

terra count_data:inc()
  C.printf("inc clicked\n")
  self.count = self.count + 1
end

terra count_data:dec()
  C.printf("dec clicked\n")
  self.count = self.count - 1
end

local test_launch = feather.launchUI(test_ui, &count_data)

terra main()
  var data: count_data
  data.count = 0
  test_launch(&data)
  C.printf("final count %d\n", data.count)
  return 0
end

print(main())


terralib.saveobj("feather_test.o", {main = main}, {"-l", "nkc", "-L.", "-L", "/nix/store/zbafyh2j4mw1gd19mr14062kc0jkazyw-user-environment/lib/", "-l", "X11", "-lm"})
