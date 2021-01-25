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