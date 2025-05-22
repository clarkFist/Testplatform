do
    local p_Name = Proto("VCU_TST","VCU_TST")
    --协议的各个字段
    --这里的base是显示的时候的进制，详细可参考https://www.wireshark.org/docs/wsdg_html_chunked/lua_module_Proto.html#lua_class_ProtoField

    local fields = p_Name.fields
	fields.s1st = ProtoField.uint8("VCU.DstNodeID","DstNodeID", base.HEX)
	fields.s2ed = ProtoField.uint8("VCU.SrcNodeID", "SrcNodeID", base.HEX)
	fields.s3rd = ProtoField.uint8("VCU.Type", "Type", base.HEX)
	fields.s4th = ProtoField.uint8("VCU.Len", "Len", base.DEC)
	fields.VCU_LOG = ProtoField.string("VCU.Log", "Log", base.ASCII)
	fields.LOG_NUM = ProtoField.uint16("VCU.NUMBER", "NUMBER", base.DEC)
	fields.VCU_Cmd1 = ProtoField.uint8("VCU.1", "1", base.HEX)
	fields.VCU_Cmd2 = ProtoField.uint32("VCU.2", "2", base.HEX)
	fields.VCU_Cmd3 = ProtoField.uint32("VCU.3", "3", base.HEX)
	fields.Crc = ProtoField.uint16("VCU.Crc", "Crc", base.HEX)

    --这里是获取data这个解析器
    local data_dis = Dissector.get("data")
    
    local function DT_dissector(buf,pkt,root)
        local buf_len = buf:len();
		local offset = 0
        if buf_len < 8 then return false end

        local tree = root:add(p_Name,buf())
        --在Packet List窗格的Protocol列可以展示出协议的名称
		--local type = tvb(offset, 1)
        --这里是把对应的字段的值填写正确，只有t:add过的才会显示在Packet Details信息里. 所以在之前定义fields的时候要把所有可能出现的都写上，但是实际解析的时候，如果某些字段没出现，就不要在这里add
        tree:add(fields.s1st,buf(offset,1))
		offset = offset  + 1
        tree:add(fields.s2ed,buf(offset,1))
		offset = offset  + 1
        tree:add(fields.s3rd,buf(offset,1))
		local msg_type = buf(offset,1)
		offset = offset  + 1
        tree:add(fields.s4th,buf(offset,1))
		local msg_len = buf(offset,1)
		offset = offset  + 1
		if (msg_type:uint() %16 ) == 0xE then
		    pkt.cols.protocol = "VCU_Log"
			tree:add(fields.LOG_NUM,buf(offset,2))
			offset = offset  + 2
			tree:add(fields.VCU_LOG,buf(offset,buf_len - offset))
		else
			pkt.cols.protocol = "VCU_Cmd"
			tree:add(fields.VCU_Cmd1,buf(offset,1))
			offset = offset  + 1
			tree:add(fields.VCU_Cmd2,buf(offset,4))
			offset = offset  + 4
			tree:add(fields.VCU_Cmd3,buf(offset,4))
			offset = offset  + 4
			if (msg_type:uint() /16 ) == 0x1 then
				--
			else
				tree:add(fields.Crc,buf(offset,2))
			end

		end
        return true
    end
    
    --这段代码是目的Packet符合条件时，被Wireshark自动调用的，是p_Name的成员方法
    function p_Name.dissector(buf,pkt,root) 
        if DT_dissector(buf,pkt,root) then
            --valid DT diagram
        else
            --data这个dissector几乎是必不可少的；当发现不是我的协议时，就应该调用data
            data_dis:call(buf,pkt,root)
        end
    end
    
    local udp_encap_table = DissectorTable.get("udp.port")
    udp_encap_table:add(18125, p_Name)
end
