# _*_ coding: utf-8 _*_
import shutil
import os, sys
import io
import re
import base64
def replacer1(match):
    global dstdir
    header = match.group(1)
    srcpath = match.group(2)
    full = match.group(0)
    if ':' != srcpath[1]:
        return full

    pictures = dstdir + '\\pictures'
    name = str(os.path.split(srcpath)[1])
    dstpath = pictures + '\\' + name
    if not os.path.exists( pictures):
        os.mkdir(pictures)

    count = 0
    while os.path.exists(dstpath):
        dstpath = pictures + '\\%s_%s' % (count, name)
    shutil.copyfile(srcpath, dstpath)
    return '%s(.\\pictures\\%s)' % (header, name)
def replacer2(match):
    global dstdir
    header = match.group(1)
    srcpath = match.group(2)
    if not srcpath.endswith('.png') and srcpath.endswith('.jpg'):
        return match.group(0)
    if re.search(r'data:image', srcpath) is not None:
        return match.group(0)
    with open(srcpath, 'rb') as f:
        image = f.read()
        image_base64 = base64.b64encode(image).decode('utf8')

    return '%s(data:image/png;base64,%s)' % (header, image_base64)

#dstdir = r'C:\Users\LiuXintian\Desktop\SQLiteSpy'
pattern = re.compile(r'(!\[[^\]]*\])\(([^\)\n]+)\)', re.S | re.DOTALL)
header = 'DOC_'
if len(sys.argv) == 1:
    dstdir = str(os.path.abspath(os.getcwd()))
    _list = os.listdir(dstdir)
elif len(sys.argv) == 2:
    
    dstdir = str(os.path.dirname(sys.argv[1])) 
    _list = [os.path.split(sys.argv[1])[1]]
else:
    raise Exception()
for name in _list:
    name = str(name)
    if name.startswith(header):
        continue
    if name.endswith('.md'):
        try:
            with io.open(dstdir + '/%s' % name, 'r', encoding='gbk') as f:
                content = f.read()
                encode = 'gbk'
        except UnicodeDecodeError:
            with io.open(dstdir + '/%s' % name, 'r', encoding='utf8') as f:
                content = f.read()
                encode = 'utf8'
        content = re.sub(pattern, replacer1, content)
        with io.open(dstdir + '/%s' % name, 'w', encoding=encode) as f:
            f.write(content + '\n')
        content = re.sub(pattern, replacer2, content)
        print(name)
        name = header + name
        content = re.sub(r'\s*([^\[\]\(\)/\\]+\.md)\s*[\)\]]', 
                         lambda _: _.group(0).replace( _.group(1), '%s%s%s' % (header, _.group(1).split('.md')[0], '.html') ), content)
        with io.open(dstdir + '/%s' % name, 'w', encoding='utf8') as f:
            f.write(content)
        
        os.system(r'i5ting_toc -f "%s"' % name)
        os.remove(name)