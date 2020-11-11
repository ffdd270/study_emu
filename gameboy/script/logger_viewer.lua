LoggerViewer = {}

function LoggerViewer.update( self)
	local logs = get_last_logs( GetInstanceLogger() )

	if not logs then return end

	for k, v in pairs( logs ) do
		table.insert( self.vars.logs, v )
	end

end

function LoggerViewer.render( self )
	self:update()

	ImGui.Begin("Logger Viewer")


	if ImGui.Button( "CLEAR LOG" ) then
		self.vars.logs = {}
	end

	local clipper = ImGuiListClipper()
	ImGuiListClipperWarp:Begin( clipper, #self.vars.logs )

	while( clipper:Step() ) do
		for i = clipper.DisplayStart + 1, clipper.DisplayEnd do -- lua는 <= 라서..
			ImGui.Text( self.vars.logs[i].log )
		end
	end

	ImGui.End()
end


function LoggerViewer.init( self )
	AddViewer("Logger", function()
		self:render()
	end )

	self.vars = {}
	self.vars.logs = {}
end