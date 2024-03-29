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
	local memory = GetInstanceCPU():GetMemory()

	for i = 1, 100 do
		local data = self.vars.list[i]

		if data then
			local select_tbl = Disassembler:getSelectOpcodeTable( data.opcode )

			if select_tbl then
				local mnemonic = select_tbl.mnemonic
				local op1 = select_tbl.operand1 or ''
				local op2 = select_tbl.operand2 or ''

				local mem_pos = data.instruction_mem_pos
				local next_mem_pos = nil

				if op1 == select_tbl.operand1 then
					local pos = nil
					op1, pos = get_op_translate_value( memory, mem_pos, op1 )
					if pos then next_mem_pos = pos end

					op1 = ' ' .. op1
				end

				if op2 == select_tbl.operand2 then
					local pos = nil
					op2, pos = get_op_translate_value( memory, mem_pos, op2 )
					if pos then next_mem_pos = pos end

					op2 = ', ' .. op2
				end

				mnemonic = mnemonic .. op1 .. op2
				ImGui.Text( mnemonic .. ' , ' .. to_hex_string( data.opcode ) .. ' , ' ..  to_hex_string( data.instruction_mem_pos ) )
			end
		end
	end
end


function InstructionWatch.init( self )
	AddViewer( "Instruction Watch", function()
		self:render()
	end)

	self.vars = {}
	self.vars.list = {}


	Instructions:addEventList( function( list )
		if #list == 1 then info( list[1]) end

		for k, v in pairs( list ) do
			table.insert( self.vars.list, v )
		end

		local len = #self.vars.list

		if len > 100 then
			local overflow_value = len - 100
			info(overflow_value)
			for i = 1, overflow_value do
				self.vars.list[i] = nil
			end

			for i = overflow_value + 1, len do
				self.vars.list[ i - overflow_value ] = self.vars.list[i]
			end
		end

	end )
end

local OAM_BASE = 0xfe00
local OAM_END = 0xfe9f
OAMWatch = {}

function OAMWatch.render( self )
	local cpu = GetInstanceCPU()
	local memory = cpu:GetMemory()

	ImGui.Text("LCD Control Register : " .. memory:GetValue(0xff40))
	ImGui.Text("LY : " .. memory:GetValue(0xff44))
	ImGui.Text("IS WINDOW ENABLE : " .. tostring((memory:GetValue(0xff40) & 16) >> 4))
	ImGui.Text("WINDOW X : "  .. memory:GetValue( 0xff4b ) )
	ImGui.Text("WINDOW Y : "  .. memory:GetValue( 0xff4a ) )

	local oam_count = ( ( OAM_END - OAM_BASE )  / 4 ) + 1

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