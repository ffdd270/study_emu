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

local OAM_BASE = 0xfe00
local OAM_END = 0xfe9f
OAMWatch = {}

function OAMWatch.render( self )
	local cpu = GetInstanceCPU()
	local memory = cpu:GetMemory()

	ImGui.Text("LCD Control Register " .. memory:GetValue(0xff40));

	local oam_count = ( OAM_END - OAM_BASE )  / 4

	for i = 0, oam_count - 1 do
		local values = {}

		for cnt = 0, 3 do
			table.insert( values, memory:GetValue( OAM_BASE + ( i * 4 ) + cnt ) )
		end

		local y_pos = values[1]
		local x_pos = values[2]
		local pallet = ( values[4] & 16 ) >> 4
		local horizontal_flip = ( values[4] & 32 ) >> 5
		local vertical_flip = ( values[4] & 64 ) >> 6
		local bg_to_oam_priority = ( values[4] & 128 ) >> 7

		ImGui.Text( i  .. " : Y " .. y_pos .. " X " .. x_pos .. " / ATTR. P " .. pallet .. ' H-F ' .. horizontal_flip .. ' V-F ' .. vertical_flip .. ' PRI ' .. bg_to_oam_priority)
	end
end

function OAMWatch.init( self )
	AddViewer( "OAMWatch", function()
		self:render()
	end)
end


OAMSetter = { }
function OAMSetter.render( self )
	ImGui.Text( "OAM NUMBER" ); ImGui.SameLine(); ImGui.InputTextWithFlags( " ",  self.vars.oam_buf, 0 );
	ImGui.Text( "Y POSITION" ); ImGui.SameLine(); ImGui.InputTextWithFlags( "  ",  self.vars.y_buf, 0 );
	ImGui.Text( "X POSITION" ); ImGui.SameLine(); ImGui.InputTextWithFlags( "   ",  self.vars.x_buf, 0 );

	if ImGui.Button("SET!") then
		local cpu = GetInstanceCPU()
		local memory = cpu:GetMemory()

		local oam_number = tonumber( self.vars.oam_buf:GetViewString() )
		local y_pos = tonumber( self.vars.y_buf:GetViewString() )
		local x_pos = tonumber( self.vars.x_buf:GetViewString() )

		memory:SetValue( OAM_BASE + ( oam_number * 4 ),   y_pos ) -- Y
		memory:SetValue( OAM_BASE + ( oam_number * 4 ) + 1,   x_pos ) -- Y
	end
end


function OAMSetter.init( self )
	self.vars = {}
	self.vars.oam_buf = StringBuf( 3 )
	self.vars.y_buf = StringBuf( 4 )
	self.vars.x_buf = StringBuf( 4 )

	AddViewer( "OAMSetter", function()
		self:render()
	end)
end