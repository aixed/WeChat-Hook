# PC微信device uuid 文件名 采用 多项式哈希算法
# 文件路径 C:\Users\Administrator\AppData\Roaming\Tencent\WeChat\All Users\config\c9d52eb5.ini
# c9d52eb5.ini 文件名算法如下
"""
5B20013C | 33C9                         | xor ecx,ecx                                           | device uuid 文件名算法 开始位置
5B20013E | BA 5CD0495D                  | mov edx,wechatwin.5D49D05C                            | edx:L"device_uuid_0"
5B200143 | 8BC1                         | mov eax,ecx                                           |
5B200145 | C1E1 05                      | shl ecx,0x5                                           |
5B200148 | 2BC8                         | sub ecx,eax                                           |
5B20014A | 0FB702                       | movzx eax,word ptr ds:[edx]                           | 把edx的 device_uuid_0 每次赋值一个字符给eax 第一次 d 第二次 v
5B20014D | 83C2 02                      | add edx,0x2                                           | 下一个字符
5B200150 | 03C8                         | add ecx,eax                                           |
5B200152 | 81FA 76D0495D                | cmp edx,wechatwin.5D49D076                            | edx:L"vice_uuid_0"
5B200158 | 7C E9                        | jl 0x5B200143                                         | 循环计算 文件名 算法
"""

def simple_device_uuid_calc():
    input_str = "device_uuid_0"
    result = 0
    for char in input_str:
        result = result * 31 + ord(char)
        result &= 0xFFFFFFFF  # 32位限制
    return result

# 测试
print(f"计算结果: 0x{simple_device_uuid_calc():08X}")

