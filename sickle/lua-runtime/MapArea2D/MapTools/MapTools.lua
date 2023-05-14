
local MapTools = {}
MapTools[appwin.MapTools.SELECT] = require "MapArea2D/MapTools/Select"
MapTools[appwin.MapTools.CREATE_BRUSH] = (
    require "MapArea2D/MapTools/CreateBrush")

return MapTools
