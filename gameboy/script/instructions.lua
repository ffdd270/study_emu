Instructions = {}

function Instructions.sendEvent( self, data )
	for k, v in pairs( self.vars.event_list ) do
		v( data )
	end
end

function Instructions.addEventList( self,  callback )
	table.insert( self.vars.event_list, callback )
end

function Instructions.render( self )
	local list = get_last_instructions()
	-- 이벤트 리스너들에게 전파함
	if list[1] then
		self:sendEvent( list )
	end


	ImGui.Text("This is Just Load Instructions Class... Not Need Window.")
end

function Instructions.init( self )
	AddViewer("Instructions", function()
		self:render()
	end)

	self.vars = {}
	self.vars.list  = {}
	self.vars.event_list = {}
end