StackTraceback = { }

function StackTraceback.render( self )
	for i = 1, #self.vars.stack do
		local stack_value = self.vars.stack[i]

		if stack_value then
			ImGui.Text( to_hex_string( stack_value.instruction_mem_pos ) )
		end
	end
end

function StackTraceback.iterFunc( self, tbl, instruction )
	if not instruction.cond then return end

	if tbl then
		local mnemonic = tbl.mnemonic

		if mnemonic == 'RET' then
			info("YES. IT'S RET!")
			self.vars.stack[#self.vars.stack] = nil
		elseif mnemonic == 'CALL' then
			table.insert( self.vars.stack, instruction )
		end
	end
end

function StackTraceback.init( self )
	AddViewer("Stack Traceback", function()
		self:render()
	end)

	self.vars = { }
	self.vars.stack = {}

	Instructions:addEventList( function( list )
		for i = 1, #list do
			local instruction = list[i]
			local opcode = instruction.opcode
			local tbl = Disassembler:getSelectOpcodeTable(opcode)

			self:iterFunc( tbl, instruction )
		end
	end)
end