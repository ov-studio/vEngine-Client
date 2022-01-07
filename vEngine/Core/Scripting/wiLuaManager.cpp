#include "Core/Scripting/wiLua.h"
#include "Core/Scripting/wiLuaManager.h"

namespace wi::lua
{
    void AsyncUpdate()
    {
        Signal("vEngine_onAsyncUpdate");
    }

    void Update()
    {
        Signal("vEngine_onUpdate");
    }
    
    void Render()
    {
        Signal("vEngine_onRender");
    }
}