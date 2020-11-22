MemoryWatch = {}

function MemoryWatch.render( self )
	ImGui.InputTextWithFlags( "",  self.vars.string_buf, ImGuiInputTextFlags_.CharsHexadecimal )

	local memory_address = tonumber( self.vars.string_buf:GetViewString(), 16 )
	local cpu = GetInstanceCPU()
	local memory = cpu:GetMemory()

	if memory_address ~= nil and memory_address >= 0 and memory_address <= ( 0xffff - 1 ) then
		ImGui.Text( memory:GetValue( memory_address ) )
	end
end


function MemoryWatch.init( self )
	AddViewer( "Memory Watch", function()
		self:render()
	end)

	self.vars = {}
	self.vars.string_buf = StringBuf( 5 )
end