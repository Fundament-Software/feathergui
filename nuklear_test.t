
local nuklear = require 'nuklear'

local struct nkcApp {
    nkcHandle: &nuklear.nkc
}

local terra mainloop(arg: &opaque)
    var nkcapp = [&nkcApp](arg)

    var ctx = nuklear.nkc_get_ctx(nkcapp.nkcHandle)

    var e = nuklear.nkc_poll_events(nkcapp.nkcHandle)

    if e.type == nuklear.NKC_EWINDOW and e.window.param == nuklear.NKC_EQUIT then
        nuklear.nkc_stop_main_loop(nkcapp.nkcHandle)
    end

    if nuklear.nk_begin(ctx, "Show", nuklear.nk_rect(50, 50, 220, 220),
                        nuklear.NK_WINDOW_BORDER or nuklear.NK_WINDOW_MOVABLE or nuklear.NK_WINDOW_CLOSABLE) ~= 0 then
        nuklear.nk_layout_row_static(ctx, 30, 80, 1)
        if nuklear.nk_button_label(ctx, "button") ~= 0 then
            nuklear.printf("button pressed\n")
        end

    end
    nuklear.nk_end(ctx)

    nuklear.nkc_render(nkcapp.nkcHandle, nuklear.nk_rgb(40, 40, 40))
end

local terra main()
    var app: nkcApp
    var nkcx: nuklear.nkc
    app.nkcHandle = &nkcx

    if nuklear.nkc_init( app.nkcHandle, "Nuklear+ Example", 640, 480, nuklear.NKC_WIN_NORMAL ) ~= nil then
        nuklear.printf("successfully initialized, entering main loop...\n")
        nuklear.nkc_set_main_loop(app.nkcHandle, mainloop, [&opaque](&app))
    else
        nuklear.printf("failed to init NKC")
    end
    nuklear.printf("exiting")
    nuklear.nkc_shutdown(app.nkcHandle)
    return 0
end

main()

terralib.saveobj("nkc_test", {main = main}, {"-l", "nkc", "-L.", "-L", "/nix/store/zbafyh2j4mw1gd19mr14062kc0jkazyw-user-environment/lib/", "-l", "X11", "-lm"})
