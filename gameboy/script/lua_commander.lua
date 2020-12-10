LuaCommander = { }

function LuaCommander.render( self )
	ImGui.InputTextMultiline( self.vars.string_buf )

	if ImGui.Button( "EXECUTE" ) then
		local execute_func = load( self.vars.string_buf:GetViewString() )

		if type(execute_func) == 'function' then
			xpcall( execute_func, TRACE_BACK )
		end
	end
end

function LuaCommander.init( self )
	AddViewer("Lua Commander", function()
		self:render()
	end)

	self.vars = {}
	self.vars.string_buf = StringBuf( 200 * 100 ) -- 1000 바이트 100라인 지원.
end