// SPDX-License-Identifier: Apache-2.0
// SPDX-FileCopyrightText: 2025 Fundament Software SPC <https://fundament.software>

use feather_ui::lua::AppState;
use feather_ui::lua::LuaApp;
use feather_ui::App;
use mlua::prelude::*;
use mlua::Function;

fn wrap_luafunc<'lua>(
    f: Function<'lua>,
) -> impl FnMut(feather_ui::DispatchPair, AppState) -> Result<AppState, AppState> + 'lua {
    // Due to how mlua works, there is no safe way to ensure this lifetime is validated, so we just have
    // to cast the lifetime away and ensure the lua runtime is never closed while we're still using this.
    let f = unsafe { std::mem::transmute::<Function<'lua>, Function<'static>>(f) };
    move |pair, state| Ok(f.call((pair.0, state)).unwrap())
}

fn main() {
    let lua = Lua::new();
    feather_ui::lua::init_environment(&lua).unwrap();
    let alicorn = Box::new(alicorn::Alicorn::new(Some(lua)).unwrap());

    // Load the built-in GLSL prelude from alicorn
    alicorn.load_glsl_prelude().unwrap();

    // Because of constraints on lifetimes, this needs to technically last forever.
    let alicorn = Box::leak(alicorn);
    {
        // This compiles and executes an alicorn program, which then calls back into the lua environment we have created in feather-ui/src/lua.rs
        let (window, init, onclick): (Function, Function, Function) = alicorn
            .execute(include_str!("layout.alc"), "layout.alc")
            .unwrap();

        let onclick = Box::new(wrap_luafunc(onclick));
        let outline = LuaApp { window, init };
        let (mut app, event_loop): (App<AppState, LuaApp>, winit::event_loop::EventLoop<()>) =
            App::new(LuaValue::Integer(0), vec![onclick], outline).unwrap();

        event_loop.run_app(&mut app).unwrap();
    }
    //drop(unsafe { Box::from_raw(alicorn) });
}
