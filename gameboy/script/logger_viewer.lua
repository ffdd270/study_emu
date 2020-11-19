LoggerViewer = {}

local ErrorString = {
	[0] = 'ERROR',
	[1] = 'WARNING',
	[2] = 'INFO'
}

function LoggerViewer.update( self)
	local logs = get_last_logs( GetInstanceLogger() )

	if not logs then return end

	for k, v in pairs( logs ) do
		table.insert( self.vars.logs, v )
	end

end

function LoggerViewer.render( self )
	self:update()

	if ImGui.Button( "CLEAR LOG" ) then
		self.vars.logs = {}
	end

	local clipper = ImGuiListClipper()
	ImGuiListClipperWarp:Begin( clipper, #self.vars.logs )

	while( clipper:Step() ) do
		for i = clipper.DisplayStart + 1, clipper.DisplayEnd do -- lua는 <= 라서
			local log_data = self.vars.logs[i]
			ImGui.Text( ErrorString[ log_data.info ]  .. ' : ' .. log_data.log )
		end
	end
end


function LoggerViewer.init( self )
	AddViewer("Logger", function()
		self:render()
	end )

	self.vars = {}
	self.vars.logs = {}
end