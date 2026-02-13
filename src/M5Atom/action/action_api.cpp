#include "action_api.h"
#include "action.h"

namespace orimer::action
{

namespace
{
    Action g_Action;
}

void Initialize()
{
    g_Action.Initialize();
}

void Finalize()
{
    g_Action.Finalize();
}

void Update()
{
    // 必要ならここで周期制御
    g_Action.LineControl();
}

void ManualMove(float targetV, float targetW)
{
    g_Action.ManualMoving(targetV, targetW);
}

void AutoMove(float targetV)
{
    g_Action.AutoMoving(targetV);
}

void SetTargetAngular(float targetW)
{
    g_Action.SetTargetAngular(targetW);
}

void Stop()
{
    g_Action.ManualMoving(0.0f, 0.0f);
}

}

