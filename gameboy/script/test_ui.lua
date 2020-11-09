TestUI = {}

function TestUI.render( self )
	ImGui.Begin("Test UI")

	ImGui.Text("But With Live Hot Reload,")
	ImGui.Text("Not Borad!")

	local draw_list = ImGui.GetWindowDrawList()
	draw_list:AddText( ImVec2( 0, 0 ), 0xffeeffff, "Draw List Test." )

	ImGui.End()
end

function TestUI.init( self )
	AddViewer( "TestUI", function()
		self:render()
	end)
end