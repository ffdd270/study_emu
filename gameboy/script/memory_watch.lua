MemoryWatch = {}

function MemoryWatch.render( self )
	ImGui.InputTextWithFlags( "",  self.vars.string_buf, ImGuiInputTextFlags_.CharsHexadecimal )

	local memory_address = tonumber( self.vars.string_buf:GetViewString(), 16 )
	local cpu = GetInstanceCPU()
	local memory = cpu:GetMemory()

	if memory_address ~= nil and memory_address >= 0 and memory_address <= ( 0xffff - 1 ) then
		ImGui.Text( to_hex_string( memory:GetValue( memory_address ) ) )
	end
end


function MemoryWatch.init( self )
	AddViewer( "Memory Watch", function()
		self:render()
	end)

	self.vars = {}
	self.vars.string_buf = StringBuf( 5 )
end

InstructionWatch = {}


function InstructionWatch.render( self )
	local list = get_last_instructions()
	for k, v in pairs( list ) do
		table.insert( self.vars.list, v )
	end

	local len = #self.vars.list

	if len > 100 then
		local overflow_value = len - 100

		for i = 1, overflow_value do
			self.vars.list[i] = nil
		end

		for i = overflow_value, len do
			self.vars.list[ i - overflow_value ] = self.vars.list[i]
		end
	end


	for i = 1, 100 do
		local index = math.max( len - 100, 0 ) + i
		local data = self.vars.list[index]

		if data then
			ImGui.Text( data.instruction_name .. ' , ' .. to_hex_string( data.opcode ) .. ' , ' ..  to_hex_string( data.instruction_mem_pos ) )
		end
	end
end


function InstructionWatch.init( self )
	AddViewer( "Instruction Watch", function()
		self:render()
	end)

	self.vars = {}
	self.vars.list = {}
end

OAMWatch = {}

function OAMWatch.render( self )
	local cpu = GetInstanceCPU()
	local memory = cpu:GetMemory()

	local oam_base = 0xfe00
	local oam_end = 0xfe9f
	local oam_count = ( oam_end - oam_base )  / 4

	for i = 0, oam_count - 1 do
		local values = {}

		for cnt = 0, 3 do
			table.insert( values, memory:GetValue( oam_base + ( i * 4 ) + cnt ) )
		end

		local y_pos = values[1]
		local x_pos = values[2]
		local pallet = ( values[4] & 8 ) >> 3
		local horizontal_flip = ( values[4] & 16 ) >> 4
		local vertical_flip = ( values[4] & 32 ) >> 5
		local bg_to_oam_priority = ( values[4] & 64 ) >> 6

		ImGui.Text( i  .. " : Y " .. y_pos .. " X " .. x_pos .. " / ATTR. P " .. pallet .. ' H-F ' .. horizontal_flip .. ' V-F ' .. vertical_flip .. ' PRI ' .. bg_to_oam_priority)
	end
end

function OAMWatch.init( self )
	AddViewer( "OAMWatch", function()
		self:render()
	end)
end