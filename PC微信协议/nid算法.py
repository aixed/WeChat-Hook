import base64

def string_to_int64_complete(input_str, end_ptr_ptr=None, base=0):

    # 初始化变量
    result_low = 0
    result_high = 0
    current_pos = 0
    str_len = len(input_str)
    flags = 0  # 标志位
    overflow_detected = False

    # 跳过前导空白字符
    while current_pos < str_len and input_str[current_pos].isspace():
        current_pos += 1

    if current_pos >= str_len:
        # 空字符串或只有空白字符
        if end_ptr_ptr is not None:
            end_ptr_ptr[0] = current_pos
        return 0, 0

    # 检查符号
    current_char = input_str[current_pos]
    if current_char == '-':
        flags |= 0x2  # 设置负数标志
        current_pos += 1
    elif current_char == '+':
        current_pos += 1

    # 自动检测进制
    if base == 0:
        if current_pos < str_len and input_str[current_pos] == '0':
            current_pos += 1
            if current_pos < str_len and (input_str[current_pos] == 'x' or input_str[current_pos] == 'X'):
                base = 16
                current_pos += 1
            else:
                base = 8
        else:
            base = 10
    elif base == 16:
        if current_pos < str_len and input_str[current_pos] == '0':
            current_pos += 1
            if current_pos < str_len and (input_str[current_pos] == 'x' or input_str[current_pos] == 'X'):
                current_pos += 1

    # 检查进制有效性
    if base < 2 or base > 36:
        # 无效进制，设置错误
        if end_ptr_ptr is not None:
            end_ptr_ptr[0] = current_pos
        return 0, 0

    # 字符到数字转换函数
    def char_to_digit(c):
        if '0' <= c <= '9':
            return ord(c) - ord('0')
        elif 'a' <= c <= 'z':
            return ord(c) - ord('a') + 10
        elif 'A' <= c <= 'Z':
            return ord(c) - ord('A') + 10
        else:
            return -1

    # 解析数字的主循环
    max_uint64 = (1 << 64) - 1
    any_digits = False

    while current_pos < str_len:
        digit = char_to_digit(input_str[current_pos])
        if digit < 0 or digit >= base:
            break

        any_digits = True

        # 检查乘法溢出
        old_result_low = result_low
        old_result_high = result_high

        # 乘以基数
        temp_low = result_low * base
        temp_high = result_high * base + (temp_low >> 32)
        result_low = temp_low & 0xFFFFFFFF
        result_high = temp_high & 0xFFFFFFFF

        # 检查乘法是否溢出
        if (temp_high >> 32) != 0:
            overflow_detected = True
            flags |= 0x8  # 设置溢出标志
            break

        # 加上新数字
        new_low = result_low + digit
        if new_low < result_low:  # 检查加法进位
            result_high += 1
            if result_high == 0:  # 如果高32位从0xFFFFFFFF变成0
                overflow_detected = True
                flags |= 0x8
                break
        result_low = new_low

        current_pos += 1

    # 如果没有解析到任何数字
    if not any_digits:
        if end_ptr_ptr is not None:
            end_ptr_ptr[0] = current_pos
        return 0, 0

    # 处理溢出
    if overflow_detected:
        if flags & 0x2:  # 负数
            result_low = 0x00000000
            result_high = 0x80000000  # INT64_MIN
        else:
            result_low = 0xFFFFFFFF
            result_high = 0x7FFFFFFF  # INT64_MAX
        flags |= 0x8

    # 处理负数
    if flags & 0x2 and not overflow_detected:
        # 对结果取负（二进制补码）
        result_low = (~result_low + 1) & 0xFFFFFFFF
        carry = 1 if result_low == 0 else 0
        result_high = (~result_high + carry) & 0xFFFFFFFF

        # 检查负数溢出（如果结果是INT64_MIN，这是有效的）
        if result_high == 0x80000000 and result_low == 0:
            # 这是INT64_MIN，有效
            pass
        elif result_high & 0x80000000:
            # 其他负数，有效
            pass
        else:
            # 负数溢出
            result_low = 0x00000000
            result_high = 0x80000000
            flags |= 0x8

    # 设置结束指针
    if end_ptr_ptr is not None:
        end_ptr_ptr[0] = current_pos

    return result_low, result_high

def calc_oid(objectId):
    hex_full = hex(int(objectId))[2:]
    if len(hex_full) % 2 != 0:
        hex_full = '0' + hex_full  # 在前面补一个零
    test_data = bytes.fromhex(hex_full)
    standard_result = base64.b64encode(test_data, altchars=b"-_").decode('ascii')
    return standard_result

def calc_nid(objectNonceId):
    end_pos = [0]
    low, high = string_to_int64_complete(objectNonceId, end_pos, 10)
    hex_low = format(low, '08X')
    hex_high = format(high, '08X')
    hex_full = hex_high + hex_low
    objectId  = int(hex_full, 16)
    ret = calc_oid(objectId)
    return ret

if __name__ == "__main__":
    objectNonceId = "349154408427976371x_15_140_59_32_1714276789860291"
    ret = calc_nid(objectNonceId)
    print("oid:", ret)
