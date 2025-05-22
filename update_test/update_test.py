#!/usr/bin/python
# _*_ coding: utf-8 _*_
import os
import re
import io
import shutil
 
def getPath_TestPlatform():
    ''''''
    import winreg
    _key = winreg.OpenKey(winreg.HKEY_CURRENT_USER, r'Environment')
    value, ty = winreg.QueryValueEx(_key, 'VCU_TEST')
    return value
TestPlatform = getPath_TestPlatform()

def find_dir(path):
    rt_path = []
    for p in os.listdir(path):
        p = path + '\\' + p
        if os.path.isdir(p):
            #rt_path.append(p)
            rt_path += find_dir(p)
        else:
            rt_path.append(p)
    return rt_path

comment_char_dict = {'.py': '#', '.c': '//', '.h': '//', '.bat': '::', '.lua': '--', '.pyw': '#'}
to_update_list = find_dir(os.path.dirname(__file__) + '\\new_Test_folder')

dst_update_list = []
src_update_list = []
for f in to_update_list:
    dst_update_list.append(f.replace(os.path.dirname(__file__) + '\\new_Test_folder', TestPlatform))
    src_update_list.append(
                           f.replace(
                                     os.path.dirname(__file__) + '\\new_Test_folder', os.path.join(os.path.dirname(os.path.dirname(__file__)), 'Master', 'testplatform')
                                    )
                           )

for src, dst in zip(src_update_list, dst_update_list):
    if os.path.exists(dst) and os.path.splitext(src)[1] in comment_char_dict:
        content_temp = []
        for filename in [src, dst]:
            try:
                with io.open(filename, 'r', encoding='utf8') as f:
                    content_temp.append( f.read() )
                    _encode = 'utf8'

            except UnicodeDecodeError as e:
                with io.open(filename, 'r') as f:
                    content_temp.append( f.read() )
                    _encode = 'gbk'

        src_content, dst_content = content_temp
        new_content = src_content
        
        _chars = comment_char_dict[os.path.splitext(src)[1]]
        config_re = re.compile(r'%sCONFIG_START\s.*?\s%sCONFIG_END' % (_chars, _chars), re.S)

        dst_config = re.findall(config_re, dst_content)
        src_config = re.findall(config_re, src_content)
        if len(dst_config) != len(src_config):
            raise Exception(src, dst)
        for s_con, d_con in zip(src_config, dst_config):
            new_content = new_content.replace(s_con, d_con)
        if new_content == dst_content:
            continue

        with io.open(dst, 'w', encoding=_encode) as f:
            f.write(new_content)
            print('update %s' % dst)
    else:
        assert(os.path.exists(os.path.dirname(dst) ) )
        shutil.copyfile(src, dst) 
        print('update %s' % dst)