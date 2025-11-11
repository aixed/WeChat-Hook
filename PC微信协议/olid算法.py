import base64

def calc_oid(objectId):
    hex_full = hex(int(objectId))[2:]
    if len(hex_full) % 2 != 0:
        hex_full = '0' + hex_full  # 在前面补一个零
    test_data = bytes.fromhex(hex_full)
    standard_result = base64.b64encode(test_data, altchars=b"-_").decode('ascii')
    return standard_result

if __name__ == "__main__":
    objectId = "14363859760702298134"
    ret = calc_oid(objectId)
    print("oid:", ret)