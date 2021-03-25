Disassembler ={}

OPCODE_TO_COMMAND = {

}



function Disassembler.render( self )
	local memory = GetInstanceCPU():GetMemory()

	for mem_pos = 0x100, 0x110 do
		local value = memory:GetValue( mem_pos )
		local tbl = self.vars.opcode_table['unprefixed']
		local select_tbl = tbl[to_hex_string(value, nil, true)]

		local text = ''
		local mnemonic = ''

		if select_tbl then
			mnemonic = select_tbl.mnemonic

			local op1 = select_tbl.operand1 or ''
			local op2 = select_tbl.operand2 or ''

			if op1 == select_tbl.operand1 then
				op1 = ' ' .. op1
			end

			if op2 == select_tbl.operand2 then
				op2 = ', ' .. op2
			end

			mnemonic = mnemonic .. op1 .. op2
		end

		text = to_hex_string(mem_pos) .. ': ' ..  to_hex_string(value) .. '/' .. mnemonic

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