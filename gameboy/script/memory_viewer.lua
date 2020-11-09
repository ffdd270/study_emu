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

function to_hex_string( number, base_num )
	local base = '0x'
	local result_string = ''
	base_num = base_num or 2

	local proc_number = number

	while proc_number ~= 0 do
		local mod_value = proc_number % 16
		local result = HexTable[ mod_value ] ~= nil and HexTable[ mod_value ] or tostring( mod_value )
 		result_string = result .. result_string
		proc_number = math.floor( proc_number / 16 )
	end

	for i = 1, base_num - string.len( result_string ) do
		result_string = '0' .. result_string
	end

	return base .. result_string
end

function MemoryViewer.render( self )
	ImGui.Begin( "Memory Viewer" )

	local len = 8

	for i = 1, len do
		ImGui.Text( to_hex_string( DummyData[ i ] ) )
		ImGui.SameLine()
	end

	ImGui.End()
end

function MemoryViewer.init ( self )
	AddViewer("Memory Viewer", function()
		self:render()
	end)

	self.vars = {}
	self.vars.data = {}

	table.shallow_copy( self.vars, self.vars.data )
end

