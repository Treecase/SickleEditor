
local moremath = require "moremath"
local EventListener = require "EventListener"


function round_to_grid(x)
    return moremath.round_to(x, gAppWin:get_grid_size())
end


EventListener.inherit(appwin.metatable)

function appwin.metatable:on_key_press_event(keyval)
    -- Increase/decrease grid size with ] and [
    if keyval == LuaGDK.GDK_KEY_bracketleft then
        self:set_grid_size(self:get_grid_size() / 2)
    elseif keyval == LuaGDK.GDK_KEY_bracketright then
        self:set_grid_size(self:get_grid_size() * 2)
    elseif keyval == LuaGDK.GDK_KEY_e then
        function test()
            x = 5
            error("intentional error")
            return x
        end
        print("triggering error")
        print('x=', test())
    else
        return false
    end
    return true
end


function appwin.metatable:on_maptool_changed()
    self:doEvent("on_maptool_changed")
end


EventListener.construct(gAppWin)

gAppWin:add_maptool(
    "Select",
    {
        {"Delete", "Brush.DeleteSelected"},
    },
    function(editor) return not editor:get_selection():is_empty() end
)
gAppWin:add_maptool(
    "Create Brush",
    {
        {"Create Brush", "Brush.CreateFromBox"},
    },
    function(editor) return true end
)
