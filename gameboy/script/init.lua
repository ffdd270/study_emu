function init_script()
	TestUI:init()
	MemoryViewer:init()
	LoggerViewer:init()
	MemoryWatch:init()
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

function show_op_code( code )
	Util:LogInfo( GetInstanceCPU():TestOpCode( code ) )
end


ImGuiInputTextFlags_ = {
	None = 0,
	CharsHexadecimal = 1 << 1
}

local function _test_op_code_err( err  )
	Util:LogError( err ) -- TODO : 맨 마지막 글자만 따서 Hex로 뿌리기
end
function _test_op_code( code )
	GetInstanceCPU():TestOpCode( code )
end

function test_op_code( )
	for i = 0, 0xff do
		xpcall( _test_op_code, _test_op_code_err, i )
	end
end