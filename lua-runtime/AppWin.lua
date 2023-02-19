
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
