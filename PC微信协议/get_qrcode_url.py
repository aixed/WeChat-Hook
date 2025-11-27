import re
import time
import requests
from urllib.parse import urlencode
from urllib.parse import unquote
import struct

# ----------------------
# PB 类，支持 1.1 / 1.2
# ----------------------
class PB:
    def __init__(self):
        self.fields = {}  # {field_number: [bytes or PB]}

    def add_field(self, field_number, raw_bytes):
        self.fields.setdefault(field_number, []).append(raw_bytes)

    def setVarint(self, key, value):
        key = str(key)
        if "." in key:
            parent, child = key.split(".")
            parent = int(parent)
            child = int(child)

            if parent not in self.fields or not isinstance(self.fields[parent][0], PB):
                self.fields[parent] = [PB()]

            subpb = self.fields[parent][0]
            subpb.setVarint(child, value)
        else:
            fn = int(key)
            self.add_field(fn, encode_key(fn, 0) + encode_varint(value))

    def setUtf8Str(self, key, value):
        key = str(key)
        if "." in key:
            parent, child = key.split(".")
            parent = int(parent)
            child = int(child)

            if parent not in self.fields or not isinstance(self.fields[parent][0], PB):
                self.fields[parent] = [PB()]

            subpb = self.fields[parent][0]
            subpb.setUtf8Str(child, value)
        else:
            fn = int(key)
            data = value.encode("utf-8")
            self.add_field(fn, encode_key(fn, 2) + encode_length_delimited(data))

    def setHex(self, key, hex_str):
        key = str(key)
        hex_str = hex_str.replace(" ", "")
        data = bytes.fromhex(hex_str)

        if "." in key:
            parent, child = key.split(".")
            parent = int(parent)
            child = int(child)

            if parent not in self.fields or not isinstance(self.fields[parent][0], PB):
                self.fields[parent] = [PB()]

            subpb = self.fields[parent][0]
            subpb.setBin(child, data)
        else:
            self.setBin(key, data)

    def setBin(self, key, data_bytes):
        key = str(key)
        fn = int(key)
        self.add_field(fn, encode_key(fn, 2) + encode_length_delimited(data_bytes))

    def GetAll(self):
        output = b""
        for fn in sorted(self.fields.keys()):
            for v in self.fields[fn]:
                if isinstance(v, PB):
                    raw = v.GetAll()
                    output += encode_key(fn, 2) + encode_length_delimited(raw)
                else:
                    output += v
        return output

# 目标页面 URL
PAGE_URL = "https://cms.slyb.top/WeChat/wxqrcodestudy?token=2392%7C37528267%7C6674E1880DF376D4B5CC0FCFAD58FF5D5C2E66463A2DFF2A61880CC2F34EF0CFC6C3F353E2FC6EB9DE70C012BD13F79469FEBEA12D28A06E"

headers = {
    "User-Agent": "Mozilla/5.0"
}

# 第一步：获取页面源码
resp = requests.get(PAGE_URL, headers=headers)
html = resp.text

# 第二步：用正则提取 WxLogin 的配置项
pattern = re.compile(
    r"new WxLogin\(\s*{\s*([\s\S]*?)\s*}\s*\);",
    re.MULTILINE
)
match = pattern.search(html)
if not match:
    print("WxLogin 配置未找到")
    exit()

config_text = match.group(1)

def extract(key, default=""):
    # 1) 匹配带引号的字符串
    m = re.search(rf'{key}:\s*"(.*?)"', config_text)
    if m:
        return m.group(1)

    # 2) 匹配数字或布尔
    m = re.search(rf'{key}:\s*([0-9]+|true|false)', config_text, re.IGNORECASE)
    if m:
        return m.group(1)

    return default

appid = extract("appid")
scope = extract("scope")
redirect_uri = unquote(extract("redirect_uri"))
# redirect_uri = extract("redirect_uri")
state = extract("state")
style = extract("style")
href = unquote(extract("href"))
# href = extract("href")
fast_login = extract("fast_login")


# 第三步：构造微信二维码登录 URL
ts = int(time.time() * 1000)

params = {
    "appid": appid,
    "scope": scope,
    "redirect_uri": redirect_uri,
    "state": state,
    "login_type": "jssdk",
    "self_redirect": "true",
    "styletype": "",
    "sizetype": "",
    "bgcolor": "",
    "rst": "",
    "ts": ts,
    "style": style,
    "href": href,
    "fast_login": fast_login
}

wx_url = "https://open.weixin.qq.com/connect/qrconnect?" + urlencode(params)

# print("构造出的微信登录 URL：")
# print(wx_url)

# 第四步：请求微信登录页面
wx_resp = requests.get(wx_url, headers=headers)
# print("\n微信返回的页面内容：\n")
# print(wx_resp.text)

# 拿到 uuid
qrcode_key = ""
m = re.search(r'/connect/qrcode/([A-Za-z0-9]+)', wx_resp.text)
if m:
    qrcode_key = m.group(1)
    print("拿到 UUID：")
    print(qrcode_key)
else:
    print("未找到")


# ----------------------
# Protobuf 基础编码函数
# ----------------------

def encode_varint(value):
    """Varint 编码"""
    buf = []
    while True:
        b = value & 0x7F
        value >>= 7
        if value:
            buf.append(b | 0x80)
        else:
            buf.append(b)
            break
    return bytes(buf)

def encode_key(field_number, wire_type):
    """field_number << 3 | wire_type"""
    return encode_varint((field_number << 3) | wire_type)

def encode_length_delimited(data: bytes):
    return encode_varint(len(data)) + data



# ----------------------
# 构造数据
# ----------------------

keys = qrcode_key
url = "https://open.weixin.qq.com/connect/confirm?uuid=" + keys

pb = PB()
pb.setUtf8Str("1.1", "")
pb.setVarint("1.2", 2434220220)
pb.setHex("1.3",  "49 6e 02 63 f3 19 08 1e 52 0b 3d 3d 0d 08 59 ad")
pb.setVarint("1.4", 402669870)
pb.setUtf8Str("1.5",  "iOS14.4.2")
pb.setVarint("1.6",  0)
pb.setVarint("2", 19)
pb.setVarint("3", 4)
pb.setVarint("4", 3273801212)
pb.setUtf8Str("6", url)
pb.setUtf8Str("7", "")
pb.setVarint("8", 0)
pb.setBin("9", bytes.fromhex("302E393634383434"))
pb.setVarint("10", 1)
pb.setBin("11", bytes.fromhex("302E393932313838"))

AllBytes = pb.GetAll()

# 转 HEX 输出
hex_str = AllBytes.hex().upper()
print("待发送 PB HEX:")
print(hex_str)

def diysendpkg(cgiurl, hex):
    # 调用接口
    url = "http://127.0.0.1:30001/DiySendPkg"

    payload = {
        "build_base_head": "0",
        "cgiurl": cgiurl,
        "hex": hex
    }

    headers = {
        "Content-Type": "application/json"
    }

    resp = requests.post(url, json=payload, headers=headers)

    # print("HTTP Status:", resp.status_code)

    # ✅ 尝试解析 JSON
    try:
        data = resp.json()
    except:
        print("返回的不是 JSON：", resp.text)
        raise

    # ✅ 取出 DiySendPkg
    diy_hex = data.get("DiySendPkg", "")

    # print("DiySendPkg HEX:")
    # print(diy_hex)
    return diy_hex


# 调用接口 上报扫码事件
diy_hex = diysendpkg("/cgi-bin/micromsg-bin/scan_qrcode_event_report_cgi", hex_str)
print("返回 PB HEX:")
print(diy_hex)


# 反序列化 pb输出返回数据

def read_varint(data, offset):
    result = 0
    shift = 0
    while True:
        b = data[offset]
        offset += 1
        result |= ((b & 0x7F) << shift)
        if not (b & 0x80):
            break
        shift += 7
    return result, offset
def parse_pb(data, offset=0, depth=0):
    result = []
    indent = "  " * depth

    while offset < len(data):
        key, offset = read_varint(data, offset)
        field_number = key >> 3
        wire_type = key & 0x07

        if wire_type == 0:  # varint
            value, offset = read_varint(data, offset)
            result.append(f"{indent}Field {field_number} (varint): {value}")

        elif wire_type == 1:  # 64-bit
            value = data[offset:offset+8]
            offset += 8
            result.append(f"{indent}Field {field_number} (64-bit): {value.hex()}")

        elif wire_type == 2:  # length-delimited
            length, offset = read_varint(data, offset)
            value = data[offset:offset+length]
            offset += length

            # 尝试 UTF-8
            try:
                txt = value.decode("utf-8")
                result.append(f"{indent}Field {field_number} (string): {txt}")
            except:
                # 可能是嵌套 PB
                sub = parse_pb(value, 0, depth+1)
                if sub:
                    result.append(f"{indent}Field {field_number} (message):")
                    result.extend(sub)
                else:
                    result.append(f"{indent}Field {field_number} (bytes): {value.hex()}")

        elif wire_type == 5:  # 32-bit
            value = data[offset:offset+4]
            offset += 4
            result.append(f"{indent}Field {field_number} (32-bit): {value.hex()}")

        else:
            result.append(f"{indent}Unknown wire type {wire_type}")
            break

    return result


# 格式化显示
hex_str = diy_hex
data = bytes.fromhex(hex_str)
parsed = parse_pb(data)
print("\n".join(parsed))

# 2.获取 mp-a8key
url = "https://open.weixin.qq.com/connect/confirm?uuid=" + keys

pb = PB()

# 字段组 1.x (子 message)
pb.setUtf8Str("1.1",  "")
pb.setVarint("1.2", 2434220220)
pb.setHex("1.3",  "49 6e 02 63 f3 19 08 1e 52 0b 3d 3d 0d 08 59 ad")
pb.setVarint("1.4", 402669870)
pb.setUtf8Str("1.5",  "iOS14.4.2")
pb.setVarint("1.6",  0)


pb.setVarint("2", 2)
pb.setVarint("3.1",  0)
pb.setUtf8Str("3.2", "")
pb.setUtf8Str("4",  "")
pb.setUtf8Str("5",  "")
pb.setUtf8Str("6",  "")
pb.setUtf8Str("7.1", url)
pb.setVarint("10", 4)
pb.setVarint("15", 100)
pb.setUtf8Str("17", "WIFI")
pb.setVarint("18", 19)
pb.setVarint("19", 6)
pb.setVarint("20", 3273801212)
pb.setVarint("25", 1)

AllBytes = pb.GetAll()

# 转 HEX 输出
hex_str = AllBytes.hex().upper()
print("待发送 PB HEX:")
print(hex_str)

# 调用接口 获取mp-a8key
diy_hex = diysendpkg("/cgi-bin/micromsg-bin/mp-geta8key", hex_str)
print("返回 PB HEX:")
print(diy_hex)



# 3.扫码
url = "https://open.weixin.qq.com/connect/confirm?uuid=" + keys

pb = PB()

# 字段组 1.x (子 message)
pb.setUtf8Str("1.1",  "")
pb.setVarint("1.2", 2434220220)
pb.setHex("1.3",  "49 6e 02 63 f3 19 08 1e 52 0b 3d 3d 0d 08 59 ad")
pb.setVarint("1.4", 402669870)
pb.setUtf8Str("1.5",  "iOS14.4.2")
pb.setVarint("1.6",  0)

# 顶层字段
pb.setUtf8Str("2", url)
pb.setVarint("3.1", 0)
pb.setVarint("3.2", 0)

AllBytes = pb.GetAll()

# 转 HEX 输出
hex_str = AllBytes.hex().upper()
print("待发送 PB HEX:")
print(hex_str)

# 调用接口 获取mp-a8key
diy_hex = diysendpkg("/cgi-bin/mmbiz-bin/qrconnect_authorize", hex_str)
print("返回 PB HEX:")
print(diy_hex)


# 4.授权第三方登录
url = "https://open.weixin.qq.com/connect/confirm?uuid=" + keys

pb = PB()

# 字段组 1.x (子 message)
pb.setUtf8Str("1.1",  "")
pb.setVarint("1.2", 2434220220)
pb.setHex("1.3",  "49 6e 02 63 f3 19 08 1e 52 0b 3d 3d 0d 08 59 ad")
pb.setVarint("1.4", 402669870)
pb.setUtf8Str("1.5",  "iOS14.4.2")
pb.setVarint("1.6",  0)

# 顶层字段
pb.setUtf8Str("2", url)
pb.setVarint("3", 1)
pb.setUtf8Str("4", "snsapi_login")
pb.setVarint("5", 0)
pb.setVarint("7", 0)
AllBytes = pb.GetAll()

# 转 HEX 输出
hex_str = AllBytes.hex().upper()
print("待发送 PB HEX:")
print(hex_str)

# 调用接口 获取mp-a8key
diy_hex = diysendpkg("/cgi-bin/mmbiz-bin/qrconnect_authorize_confirm", hex_str)
print("返回 PB HEX:")
print(diy_hex)
