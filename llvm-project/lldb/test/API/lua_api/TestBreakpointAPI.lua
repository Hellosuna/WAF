_T = require('lua_lldb_test').create_test('TestBreakpointAPI')

function _T:TestBreakpointIsValid()
    local target = self:create_target()
    local breakpoint = target:BreakpointCreateByName('AFunction', 'a.out')
    assertTrue(breakpoint:IsValid() and breakpoint:GetNumLocations() == 1)
    local did_delete = target:BreakpointDelete(breakpoint:GetID())
    assertTrue(did_delete)
    local del_bkpt = target:FindBreakpointByID(breakpoint:GetID())
    assertFalse(del_bkpt:IsValid())
    assertFalse(breakpoint:IsValid())
end

function _T:TestTargetDelete()
    local target = self:create_target()
    local breakpoint = target:BreakpointCreateByName('AFunction', 'a.out')
    assertTrue(breakpoint:IsValid() and breakpoint:GetNumLocations() == 1)
    local location = breakpoint:GetLocationAtIndex(0)
    assertTrue(location:IsValid())
    assertEquals(target, breakpoint:GetTarget())
    assertTrue(self.debugger:DeleteTarget(target))
    assertFalse(breakpoint:IsValid())
    assertFalse(location:IsValid())
end

function _T:TestBreakpointHitCount()
    local target = self:create_target()
    local breakpoint = target:BreakpointCreateByName('BFunction', 'a.out')
    assertTrue(breakpoint:IsValid() and breakpoint:GetNumLocations() == 1)
    breakpoint:SetAutoContinue(true)
    target:LaunchSimple(nil, nil, nil)
    assertEquals(breakpoint:GetHitCount(), 100)
end

function _T:TestBreakpointFrame()
    local target = self:create_target()
    local breakpoint = target:BreakpointCreateByName('main', 'a.out')
    assertTrue(breakpoint:IsValid() and breakpoint:GetNumLocations() == 1)
    local process = target:LaunchSimple({ 'arg1', 'arg2' }, nil, nil)
    local thread = get_stopped_thread(process, lldb.eStopReasonBreakpoint)
    assertNotNil(thread)
    assertTrue(thread:IsValid())
    local frame = thread:GetFrameAtIndex(0)
    assertTrue(frame:IsValid())
    local error = lldb.SBError()
    local var_argc = frame:FindVariable('argc')
    local var_argc_value = var_argc:GetValueAsSigned(error, 0)
    assertTrue(error:Success())
    assertEquals(var_argc_value, 3)
end

os.exit(_T:run())