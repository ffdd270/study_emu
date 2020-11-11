function init_script()
	TestUI:init()
	MemoryViewer:init()
	LoggerViewer:init()
end



function table.shallow_copy( table, copy_target )
	for k, v in pairs( copy_target ) do
		table[k] = v
	end
end


ImGuiListClipperWarp = {}

function ImGuiListClipperWarp.Begin( self, clipper, item_count )
	clipper:Begin( item_count, -1 )
end