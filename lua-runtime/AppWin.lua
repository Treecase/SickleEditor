
local moremath = require "../moremath"

function round_to_grid(x)
    return moremath.round_to(x, gAppWin:get_grid_size())
end


function gAppWin:on_key_press_event(keyval)
    -- Increase/decrease grid size with ] and [
    if keyval == LuaGDK.GDK_KEY_bracketleft then
        self:set_grid_size(self:get_grid_size() / 2)
    elseif keyval == LuaGDK.GDK_KEY_bracketright then
        self:set_grid_size(self:get_grid_size() * 2)
    else
        return false
    end
    return true
end
