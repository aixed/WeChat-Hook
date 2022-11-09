from ctypes import *

dll = CDLL('G:\新建文件夹 (2)\wechatrun.dll')

a='filehelper'.encode('gbk')

b='这是写好的给你们调用dll'.encode('gbk')

c = dll.send_text_msg(a, b)
