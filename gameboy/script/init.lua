function init_script()
	TestUI:init()
	MemoryViewer:init()
end



function table.shallow_copy( table, copy_target )
	for k, v in pairs( copy_target ) do
		table[k] = v
	end
end