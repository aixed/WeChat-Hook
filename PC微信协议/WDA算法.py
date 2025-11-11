import hashlib
import time


def calculate_device_id():
    mac_all = "1C1B0D0E93981C1B0D0E9396"
    cpuidtxt = "3219913727" # 需要转有符号 -1077150721





    # 将无符号整数转换为有符号整数
    cpuid_unsigned = int(cpuidtxt)
    if cpuid_unsigned > 0x7FFFFFFF:  # 如果大于最大有符号整数
        cpuid_signed = cpuid_unsigned - 0x100000000
    else:
        cpuid_signed = cpuid_unsigned



    # 使用有符号整数参与运算
    cpuidtxt = str(cpuid_signed)
    # print(cpuidtxt)


    # 第一次MD5
    mac_bytes = bytes.fromhex(mac_all)
    macmd5 = hashlib.md5(mac_bytes).hexdigest().lower().replace(" ", "")

    # 组合字符串
    dev_md5 = macmd5 + cpuidtxt
    # print(dev_md5)


    # 第二次MD5
    dev_bytes = dev_md5.encode('utf-8')
    dev_hex = dev_bytes.hex()
    dev_md5 = hashlib.md5(bytes.fromhex(dev_hex)).hexdigest().lower().replace(" ", "")

    # 添加前缀并截取
    result = "W" + dev_md5
    return result[:17]


# 使用示例
if __name__ == "__main__":
    device_id = calculate_device_id()

    # 获取13位现行时间戳（毫秒级）
    timestamp = str(int(time.time() * 1000))

    # 组合设备ID和时间戳 33-9-141 传入参数固定的 只有部分业务 不同
    final_result = "33-9-141-" + device_id + timestamp

    print("context_id:", final_result)
