Disassembler ={}

function get_op_translate_value( memory, mem_pos, op )
	if op == 'a16' or op == 'd16' then
		local hi = memory:GetValue( mem_pos + 2 )
		local lo = memory:GetValue( mem_pos + 1 )

		op = to_hex_string(  hi << 8 | lo )

		return op, mem_pos + 2
	end

	if op == '(a16)' or op == '(d16)' then
		local hi = memory:GetValue( mem_pos + 2 )
		local lo = memory:GetValue( mem_pos + 1 )

		op = to_hex_string(  hi << 8 | lo )
		op = '(' .. op .. ')'


		return op, mem_pos + 2
	end

	if op == 'r8' then
		local lo = memory:GetValue( mem_pos + 1 )
		local minus = false
		local origin = to_hex_string(lo)

		if (lo & 0x80) == 0x80 then -- 반전 필요함. 그리고 음수임 =ㅁ =
			lo = ( 0xff ~ lo ) + 1
			minus = true
		end

		op = to_hex_string( lo )
		if minus then
			op = '-' .. op
		end

		return op, mem_pos + 1
	end


	if op == 'd8' or op == 'a8' or op == '(d8)' or op == '(a8)' then
		local lo = memory:GetValue( mem_pos + 1 )
		op = to_hex_string( lo )
		return op, mem_pos + 1
	end

	return op, nil
end

function Disassembler.getSelectOpcodeTable( self, value )
	local tbl = self.vars.opcode_table['unprefixed']
	local select_tbl = tbl[to_hex_string(value, nil, true)]

	return select_tbl
end

function Disassembler.renderOpcodes( self, memory_address )
	local memory = GetInstanceCPU():GetMemory()
	local mem_pos = memory_address
	local last_mem_pos = memory_address + 0x50


	for i = 1, 9999 do -- 대충 적음
		if not ( mem_pos >= 0 and mem_pos <= ( 0xffff - 1 ) ) then
			break
		end

		if mem_pos > last_mem_pos then
			break
		end


		local value = memory:GetValue( mem_pos )
		local select_tbl = self:getSelectOpcodeTable( value )

		local text = ''
		local mnemonic = ''
		local next_mem_pos = nil

		if select_tbl then
			mnemonic = select_tbl.mnemonic

			local op1 = select_tbl.operand1 or ''
			local op2 = select_tbl.operand2 or ''

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
		end

		local mem_pos_str = to_hex_string(mem_pos)
		if next_mem_pos then
			mem_pos_str = to_hex_string(mem_pos) .. '-' .. to_hex_string(next_mem_pos)
			mem_pos = next_mem_pos
		end

		text = mem_pos_str .. ': ' ..  to_hex_string(value) .. '/' .. mnemonic
		ImGui.Text( text )

		mem_pos = mem_pos + 1
	end
end

function Disassembler.render( self )
	ImGui.InputTextWithFlags( "",  self.vars.string_buf, ImGuiInputTextFlags_.CharsHexadecimal )
	local memory_address = tonumber( self.vars.string_buf:GetViewString(), 16 )

	ImGui.InputTextWithFlags( "BreakPoint",  self.vars.break_point, ImGuiInputTextFlags_.CharsHexadecimal )
	if ImGui.Button( "BREAK" ) then
		local bp_point = tonumber( self.vars.break_point:GetViewString(), 16 )

		if self.vars.bp_point then
			GetInstanceCPU():ContinueFromBreakPoint()
			GetInstanceCPU():RemoveBreakPoint(self.vars.bp_point)
			self.vars.bp_point = nil
		end

		if bp_point ~= nil and bp_point >= 0 and bp_point <= ( 0xffff - 1 ) then
			self.vars.bp_point = bp_point
			GetInstanceCPU():AddBreakPoint(self.vars.bp_point)
		end
	end

	ImGui.SameLine()

	if ImGui.Button( "CONTINUE" ) and self.vars.bp_point then
		GetInstanceCPU():ContinueFromBreakPoint()
		GetInstanceCPU():RemoveBreakPoint(self.vars.bp_point)
		self.vars.bp_point = nil
	end

	if self.vars.bp_point then
		ImGui.SameLine()
		ImGui.Text(to_hex_string( self.vars.bp_point ))
	end

	ImGui.Text("AF : " .. to_hex_string( GetInstanceCPU():GetRegisterAF().reg_16 ) )

	if memory_address ~= nil and memory_address >= 0 and memory_address <= ( 0xffff - 1 ) then
		self:renderOpcodes( memory_address )
	end
end


function Disassembler.init( self )
	AddViewer("Disassembler", function()
		self:render()
	end)

	self.vars = {}

	local file = io.open("opcode.json", "r")
	local text = file:read("*a")
	local table = json.decode( text )

	self.vars.opcode_table = table
	self.vars.string_buf = StringBuf( 5 )
	self.vars.break_point = StringBuf(5)
	self.vars.bp_point = nil
end