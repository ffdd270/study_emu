JPHistory = {}

function JPHistory.render( self )
	for i = 1, #self.vars.history do
		local value = self.vars.history[i]

		if value then
			ImGui.Text( value )
		end
	end
end


function JPHistory.iterFunc( self, tbl, instruction )
	if not instruction.cond then return end
	local memory = GetInstanceCPU():GetMemory()

	if tbl then
		local mnemonic = tbl.mnemonic
		if mnemonic ~= 'JP' then return end

		local operand1 = tbl.operand1 or ''
		local operand2 = tbl.operand2 or ''

		if operand1 ~= 'a16' and operand2 ~= 'a16' then -- TODO : HL 지원 추가
			return
		end

		if operand1 == 'a16' then
			-- 여기서 몇 비트
			operand1 = get_op_translate_value( memory, instruction.instruction_mem_pos, operand1 )
		else
			operand2 = get_op_translate_value( memory, instruction.instruction_mem_pos, operand2 )
		end

		table.insert( self.vars.history,  'From : ' .. to_hex_string(instruction.instruction_mem_pos) .. ' -> ' .. mnemonic .. ' ' .. operand1 .. ' ' .. operand2 )
	end
end

function JPHistory.cleanUp( self )
	local len = #self.vars.history
	if len <= 10 then return end

	local overvalue = len - 10

	for i = 1, overvalue do
		self.vars.history[i] = nil
	end

	for i = overvalue + 1, len do
		self.vars.history[i - overvalue] = self.vars.history[i]
	end
end


function JPHistory.init(self)
	AddViewer("JP History", function()
		self:render()
	end)

	self.vars = {}
	self.vars.history = {}

	Instructions:addEventList( function( list )
		for i = 1, #list do
			local instruction = list[i]
			local opcode = instruction.opcode
			local tbl = Disassembler:getSelectOpcodeTable(opcode)

			self:iterFunc( tbl, instruction )
		end

		self:cleanUp()
	end)
end