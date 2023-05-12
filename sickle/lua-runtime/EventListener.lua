-- EventListener interface.


local EventListener = {}

local function addListener(self, listener)
    table.insert(self.listeners, listener)
end

local function removeListener(self, listener)
    for i,l in ipairs(self.listeners) do
        if l == listener then
            table.remove(self.listeners, i)
        end
    end
end

local function clearListeners(self)
    for i,_ in ipairs(self.listeners) do
        table.remove(self.listeners, i)
    end
end

function doEvent(self, event, args)
    local captured = false
    for _,listener in ipairs(self.listeners) do
        local fn = listener[event]
        if fn(listener, args) then
            captured = true
        end
    end
    return captured
end


function EventListener.construct(self)
    self.listeners = {}
end

function EventListener.inherit(metatable)
    metatable.addListener = addListener
    metatable.removeListener = removeListener
    metatable.clearListeners = clearListeners
    metatable.doEvent = doEvent
end

return EventListener
