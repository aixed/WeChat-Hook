# Python equivalent of C++ algorithm:
# UINT64 ClientCheckData::GetSystemTimeCheckValue(size_t dwTimeStamp)
#
# C++ code analysis:
#   size_t now = dwTimeStamp            # 64-bit on x64
#   BYTE* arr = (BYTE*)&now            # reinterpret as byte array
#   BYTE v3 = ((BYTE)(now >> 32)) ^ 0x66  # high 8 bits of upper 32 bits XOR 0x66
#   BYTE v4 = ((BYTE)(now >> 24)) ^ 0x66  # high 8 bits of 3rd byte XOR 0x66
#   arr[3] = v3                          # modify 4th byte
#   arr[4] = v4                          # modify 5th byte
#   return now                           # return modified 64-bit value
#
# Input:  1774840639
# Expected: 66149018431

def get_system_time_check_value(dw_time_stamp: int) -> int:
    now = dw_time_stamp  # 64-bit unsigned
    # Pack as little-endian 8 bytes
    arr = list(now.to_bytes(8, byteorder='little'))

    # v3 = (now >> 32) lowest byte XOR 0x66
    v3 = ((now >> 32) & 0xFF) ^ 0x66
    # v4 = (now >> 24) lowest byte XOR 0x66
    v4 = ((now >> 24) & 0xFF) ^ 0x66

    arr[3] = v3
    arr[4] = v4

    # Unpack as little-endian 64-bit unsigned
    result = int.from_bytes(bytes(arr), byteorder='little')
    return result


# Input
dw_time_stamp = 1774840639
result = get_system_time_check_value(dw_time_stamp)

print(f"Input (decimal):    {dw_time_stamp}")
print(f"Input (hex):         0x{dw_time_stamp:016X}")
print()
print(f"now >> 32 = 0x{(dw_time_stamp >> 32):08X}, &0xFF = 0x{(dw_time_stamp >> 32) & 0xFF:02X}")
print(f"now >> 24 = 0x{(dw_time_stamp >> 24):08X}, &0xFF = 0x{(dw_time_stamp >> 24) & 0xFF:02X}")
print()
v3 = ((dw_time_stamp >> 32) & 0xFF) ^ 0x66
v4 = ((dw_time_stamp >> 24) & 0xFF) ^ 0x66
print(f"v3 = 0x{v3:02X} ({(dw_time_stamp >> 32) & 0xFF:3d} ^ 0x66)")
print(f"v4 = 0x{v4:02X} ({(dw_time_stamp >> 24) & 0xFF:3d} ^ 0x66)")
print()

# Show byte layout
arr = list(dw_time_stamp.to_bytes(8, byteorder='little'))
print(f"Original bytes (little-endian): {[hex(b) for b in arr]}")
arr[3] = v3
arr[4] = v4
print(f"Modified bytes (little-endian):  {[hex(b) for b in arr]}")
print(f"  byte[0] = 0x{arr[0]:02X} (bits 0-7)")
print(f"  byte[1] = 0x{arr[1]:02X} (bits 8-15)")
print(f"  byte[2] = 0x{arr[2]:02X} (bits 16-23)")
print(f"  byte[3] = 0x{arr[3]:02X} (bits 24-31) <- XORed")
print(f"  byte[4] = 0x{arr[4]:02X} (bits 32-39) <- XORed")
print(f"  byte[5] = 0x{arr[5]:02X} (bits 40-47)")
print(f"  byte[6] = 0x{arr[6]:02X} (bits 48-55)")
print(f"  byte[7] = 0x{arr[7]:02X} (bits 56-63)")
print()

print(f"Result (decimal):   {result}")
print(f"Result (hex):       0x{result:016X}")

# Verify against expected
expected = 66149018431
print(f"\nExpected (decimal): {expected}")
print(f"Expected (hex):     0x{expected:016X}")
print(f"Match: {result == expected}")

# Also try big-endian interpretation
be_result = int.from_bytes(bytes(arr), byteorder='big')
print(f"\nBig-endian interpretation: {be_result} (0x{be_result:016X})")
