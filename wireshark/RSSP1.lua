do
    local p_Name = Proto("RSSP1","RSSP1")
    --协议的各个字段
    --这里的base是显示的时候的进制，详细可参考https://www.wireshark.org/docs/wsdg_html_chunked/lua_module_Proto.html#lua_class_ProtoField

    fields = p_Name.fields
    local data_dis = Dissector.get("data",size)
	function default_field()
		local rt = 0
		
		if g_size <= 0 then
			rt = ProtoField.bytes(g_headername .. g_name, g_name, base.NONE)
		elseif g_size == 1 then
			rt = ProtoField.uint8(g_headername .. g_name, g_name, base.HEX)
		elseif g_size == 2 then
			rt = ProtoField.uint16(g_headername .. g_name, g_name, base.HEX)
		elseif g_size == 4 then
			rt = ProtoField.uint32(g_headername .. g_name, g_name, base.HEX)
		end
		return rt
	end
	local function sortdict(tab,key,value) 
		table.insert(tab,{key,value})
	end
	local function sortdict_pairs(dict)
		local index = 0
		local count = #dict
		return function() 
			index = index + 1
			if index <= count then
				return dict[index][1],dict[index][2]
			end
		end
	end
	---------------------------
	local my_fields = {}
	----sortdict(my_fields,{"RMS",11},{{"IfVer",1},{"Length",2},{"SrcSSTy",1},{"SrcSSI",1},{"DestSS",1},{"DestSSID",1},{"SN",4}} )
	--sortdict(my_fields,{"GAPP",19},{{"IfVer",1},{"Length",2},{"SrcSSTy",1},{"SrcLogID",1},{"SrcSSID",1},{"DestSSTy",1},{"DestLogID",1},
	--			{"DestSID",1},{"NTPTime1",4},{"NTPTime2",4},{"MsgID",1},{"AppType",1}} )
	sortdict(my_fields,{"RSSP1",6},{{"App_categort",1},{"MsgType",1},{"SrcAddr",2},{"DstAddr",2}})
	local sub_fields = {}
	local key1 = {"BSD",0}
	local key2 = {"SSE",14}
	local key3 = {"SSR",19}
	local match_dict = {}
	match_dict[0x81] = key1
	match_dict[0x80] = key1
	match_dict[0x90] = key2
	match_dict[0x91] = key3
	sub_fields[key1] =  { {"TimeCounter",4},{"DataSize",2},{"CRC1",4},{"CRC2",4},{"Data",-2},{"Crc16",2} }  
	sub_fields[key2] =  {{"TimeCounter",4},{"SEQENQ_1",4},{"SEQENQ_2",4},{"Crc16",2}}
	sub_fields[key3] =  {{"Time_replier",4},{"Time_enquirer",4},{"SEQENQ_1",4},{"SEQENQ_2",4},{"NUM_DATAVER",1},{"Crc16",2}}
	---------------------------------
	local fields_dict ={}
	for key1,val1 in sortdict_pairs(my_fields) do
		g_headername = key1[1] .. "."
		g_Fileldname = "fields." .. key1[1] .. "_"
		for key2,val2 in pairs(val1) do
			g_name = val2[1]
			g_size = val2[2]
			loadstring(g_Fileldname .. g_name .. " = default_field()")()
			fields_dict[g_Fileldname .. g_name] = loadstring("return " .. g_Fileldname .. g_name)()
		end
	end
	for key1,val1 in pairs(sub_fields) do
		g_headername = key1[1] .. "."
		g_Fileldname = "fields." .. key1[1] .. "_"
		for key2,val2 in pairs(val1) do
			g_name = val2[1]
			g_size = val2[2]
			loadstring(g_Fileldname .. g_name .. " = default_field()")()
			fields_dict[g_Fileldname .. g_name] = loadstring("return " .. g_Fileldname .. g_name)()
		end
	end
	local function set_next(name,size)
		
		if size == 0 then
			size = g_len - g_offset
		end
		local tmp = g_buf(g_offset,size)
		g_tree:add( fields_dict[g_Fileldname .. name],tmp)
		g_offset = g_offset  + size
	end
    local function My_dissector(buf,pkt,root)
        local l_len = 0
		local tmp_val = 0
		local buf_len = buf:len();
		local jud
		local str = string.sub(tostring(pkt.src),4,4)
		if str == "2" or str == "1" then return false end
		g_pos = 0
		g_len = 0
		g_buf = buf
		
		pkt.cols.protocol = "Fail"
		----------------------------------------
		for key1,val1 in sortdict_pairs(my_fields) do
			g_Fileldname = "fields." .. key1[1] .. "_"
			g_pos = g_pos + g_len
			if key1[2] <= 0 then
				g_len = buf_len - g_pos + key1[2]
			else
				g_len = key1[2] 
			end
			g_buf = buf:range(g_pos,g_len)
			g_tree = root:add(p_Name,g_buf)
			g_tree:set_text( key1[1])
			g_offset = 0
			for key2,val2 in pairs(val1) do
				if val2[2] <= 0 then
					l_len = g_buf:len() - g_offset + val2[2]
				else
					l_len = val2[2]
				end
				if key1[1] == "RSSP1" and val2[1] == "MsgType" then
					jud = g_buf(g_offset,l_len):uint()
				end
				set_next(val2[1],l_len)
			end
		end
		---------------------------------------
		if match_dict[jud] ~= nil then
			key1 = match_dict[jud]
			val1 = sub_fields[key1]
			pkt.cols.protocol = p_Name.name .. "_" .. key1[1]
			
			g_Fileldname = "fields." .. key1[1] .. "_"
			g_pos = g_pos + g_len
			if key1[2] <= 0 then
				g_len = buf_len - g_pos + key1[2]
			else
				g_len = key1[2] 
			end
			g_buf = buf:range(g_pos,g_len)
			g_tree = root:add(p_Name,g_buf)
			g_tree:set_text( key1[1])
			g_offset = 0
			for key2,val2 in pairs(val1) do
				if val2[2] <= 0 then
					l_len = g_buf:len() - g_offset + val2[2]
				else
					l_len = val2[2]
				end
				tmp_val = set_next(val2[1],l_len)
			end
		end
		-----------------------------------------
        return true
    end
    
    --这段代码是目的Packet符合条件时，被Wireshark自动调用的，是p_Name的成员方法
    function p_Name.dissector(buf,pkt,root) 
        if My_dissector(buf,pkt,root) then
            --valid DT diagram
        else
            --data这个dissector几乎是必不可少的；当发现不是我的协议时，就应该调用data
            data_dis:call(buf,pkt,root)
        end
    end
    
    local udp_encap_table = DissectorTable.get("udp.port")
    udp_encap_table:add(50001, p_Name)
    udp_encap_table:add(50000, p_Name)
end

