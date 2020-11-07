TestUI = {}

function TestUI.render( self )
	ImGui.Begin("Test UI")

	ImGui.Text("Hello, With TestUI.")

	ImGui.End()
end

function TestUI.init( self )
	AddViewer( "TestUI", function()
		self:render()
	end)
end