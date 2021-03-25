Disassembler ={}

function get_op_translate_value( memory, mem_pos, op )
	if op == 'a16' or op == 'd16' then
		local hi = memory:GetValue( mem_pos + 2 )
		local lo = memory:GetValue( mem_pos + 1 )

		op = to_hex_string(  hi << 8 | lo )

		return op, mem_pos + 2
	end


	if op == 'd8' or op == 'a8' then
		local lo = memory:GetValue( mem_pos + 1 )
		op = to_hex_string( lo )
		return op, mem_pos + 1
	end

	return op, nil
end


function Disassembler.render( self )
	local memory = GetInstanceCPU():GetMemory()

	for mem_pos = 0x100, 0x110 do
		local value = memory:GetValue( mem_pos )
		local tbl = self.vars.opcode_table['unprefixed']
		local select_tbl = tbl[to_hex_string(value, nil, true)]

		local text = ''
		local mnemonic = ''
		local next_mem_pos = nil

		if select_tbl then
			mnemonic = select_tbl.mnemonic

			local op1 = select_tbl.operand1 or ''
			local op2 = select_tbl.operand2 or ''

			if op1 == select_tbl.operand1 then
				op1, next_mem_pos = get_op_translate_value( memory, mem_pos, op1 )

				op1 = ' ' .. op1
			end

			if op2 == select_tbl.operand2 then
				op2, next_mem_pos = get_op_translate_value( memory, mem_pos, op2 )

				op2 = ', ' .. op2
			end

			mnemonic = mnemonic .. op1 .. op2
		end

		local mem_pos_str = to_hex_string(mem_pos)
		if next_mem_pos then
			mem_pos_str = to_hex_string(mem_pos) .. '-' .. to_hex_string(next_mem_pos)
			mem_pos = next_mem_pos - 1
		end

		text = mem_pos_str .. ': ' ..  to_hex_string(value) .. '/' .. mnemonic

		ImGui.Text( text )
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
end