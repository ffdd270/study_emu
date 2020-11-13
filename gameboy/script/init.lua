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


Util = {}

function Util.LogError( self, str )
	log_error( GetInstanceLogger(), tostring( str ) )
end


function Util.LogWarn( self, str )
	log_warning( GetInstanceLogger(), tostring( str ) )
end


function Util.LogInfo(  self, str )
	log_info( GetInstanceLogger(), tostring( str ) )
end

