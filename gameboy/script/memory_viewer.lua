MemoryViewer = {}

DummyData = {
	0x00, 0x32, 0x34, 0x42, 0x31, 0x36, 0x85, 0x92,
	0x00, 0x32, 0x34, 0x42, 0x31, 0x36, 0x85, 0x92,
	0x00, 0x32, 0x34, 0x42, 0x31, 0x36, 0x85, 0x92,
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

	local clipper = ImGuiListClipper()
	ImGuiListClipperWarp:Begin( clipper, math.floor( #DummyData / len )  )

	ImGui.Columns( 2 )

	while( clipper:Step() ) do
		local prev_line = 0

		for line = clipper.DisplayStart + 1, clipper.DisplayEnd do -- lua는 <= 라서
			local calc_line = line - 1

			for i = 1, len do
				if DummyData[ i + ( calc_line * len  ) ] then
					if prev_line == line then
						ImGui.SameLine()
					end

					ImGui.Text( to_hex_string( DummyData[ i + ( calc_line * len ) ] ) )
					prev_line = line
				else
					break
				end
			end

			ImGui.NextColumn()

			for i = 1, len  do
				if DummyData[ i + ( calc_line * len ) ] then
					ImGui.SameLine()

					local result = string.char( DummyData[  i + ( calc_line * len )  ] )
					local re_find =  string.find( result, "[%a%d]" )
					if re_find == nil then
						ImGui.Text('.')
					else
						ImGui.Text( result )
					end

				else
					break
				end
			end

			ImGui.NextColumn()
		end
	end

	ImGui.Columns( 1 )


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

