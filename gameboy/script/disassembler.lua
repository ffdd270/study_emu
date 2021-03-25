Disassembler ={}

OPCODE_TO_COMMAND = {

}



function Disassembler.render( self )
	for i = 0x100, 0x103 do

		local memory = GetInstanceCPU():GetMemory()
		local value = memory:GetValue( i )

		local tbl = self.vars.opcode_table['unprefixed']
		local select_tbl = tbl[to_hex_string(value)]
		if select_tbl and select_tbl.mnemonic then
			ImGui.Text( select_tbl.mnemonic)
		end


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