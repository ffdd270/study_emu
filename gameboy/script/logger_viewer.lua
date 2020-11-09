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

	for k, v in pairs( self.vars.logs ) do
		ImGui.Text( v.log )
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