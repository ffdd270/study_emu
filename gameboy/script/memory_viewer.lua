MemoryViewer = {}

DummyData = {
	0x00, 0x32, 0x34, 0x42, 0x31, 0x36, 0x85, 0x92
}


HexTable = {
	[10] = 'A',
	[11] = 'B',
	[12] = 'C',
	[13] = 'D',
	[14] = 'E',
	[15] = 'F',
}

function to_hex_string( number )
	local base = '0x'
	local result_string = ''

	local proc_number = number

	while (proc_number >= 16) do
		local next = proc_number % 16



		result_string = tostring( result_device ) .. result_string
	end


end

function MemoryViewer.render( self )

	ImGui.Text()
end



function MemoryViewer.init ( self )
	AddViewer("Memory Viewer", function()
		self:render()
	end)

	self.vars = {}
	self.vars.data = {}

	table.shallow_copy( self.vars, self.vars.data )
end

