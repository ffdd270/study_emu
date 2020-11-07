TestUI = {}

function TestUI.render( self )
	ImGui.Begin("Test UI")

	ImGui.Text("But With Live Hot Reload,")
	ImGui.Text("Not Borad!")

	ImGui.End()
end


function debug()
	ImGui.Begin("Test Debug")

	ImGui.Text("Just Debug.")

	ImGui.End() 
end

function TestUI.init( self )
	AddViewer( "TestUI", function()
		self:render()
	end)

	AddViewer( "Can Add Another", function()
		debug()
	end)

end