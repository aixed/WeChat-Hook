
<Structures>
  <Structure Name="人物信息结构体" AutoFill="0" AutoCreate="1" DefaultHex="0" AutoDestroy="0" DoNotSaveLocal="0" RLECompression="1" AutoCreateStructsize="4096">
    <Elements>
      <Element Offset="0" Vartype="4 Bytes" Bytesize="4" OffsetHex="00000000" Description="句柄" DisplayMethod="十六进制"/>
      <Element Offset="4" Vartype="4 Bytes" Bytesize="4" OffsetHex="00000004" Description="0" DisplayMethod="无符号整数"/>
      <Element Offset="8" Vartype="4 Bytes" Bytesize="4" OffsetHex="00000008" Description="缓冲区大小" DisplayMethod="无符号整数"/>
      <Element Offset="12" Vartype="4 Bytes" Bytesize="4" OffsetHex="0000000C" Description="0" DisplayMethod="无符号整数"/>
      <Element Offset="16" Vartype="Pointer" Bytesize="4" OffsetHex="00000010" Description="微信ID指针" DisplayMethod="无符号整数">
        <Structure Name="自动创建从 05750400" AutoFill="0" AutoCreate="1" DefaultHex="0" AutoDestroy="0" DoNotSaveLocal="0" RLECompression="1" AutoCreateStructsize="4096">
          <Elements>
            <Element Offset="0" Vartype="Unicode String" Bytesize="100" OffsetHex="00000000" DisplayMethod="无符号整数"/>
          </Elements>
        </Structure>
      </Element>
      <Element Offset="20" Vartype="4 Bytes" Bytesize="4" RLECount="2" OffsetHex="00000014" Description="微信ID字符串大小" DisplayMethod="无符号整数"/>
      <Element Offset="28" Vartype="4 Bytes" Bytesize="4" RLECount="2" OffsetHex="0000001C" Description="置0" DisplayMethod="无符号整数"/>
      <Element Offset="36" Vartype="Pointer" Bytesize="4" OffsetHex="00000024" Description="微信号指针" DisplayMethod="无符号整数">
        <Structure Name="自动创建从 0952FB98" AutoFill="0" AutoCreate="1" DefaultHex="0" AutoDestroy="0" DoNotSaveLocal="0" RLECompression="1" AutoCreateStructsize="4096">
          <Elements>
            <Element Offset="0" Vartype="Unicode String" Bytesize="100" OffsetHex="00000000" Description="微信号" DisplayMethod="无符号整数"/>
          </Elements>
        </Structure>
      </Element>
      <Element Offset="40" Vartype="4 Bytes" Bytesize="4" RLECount="2" OffsetHex="00000028" Description="微信号 长度" DisplayMethod="无符号整数"/>
      <Element Offset="48" Vartype="4 Bytes" Bytesize="4" RLECount="2" OffsetHex="00000030" Description="置0" DisplayMethod="无符号整数"/>
      <Element Offset="56" Vartype="Pointer" Bytesize="4" OffsetHex="00000038" Description="V3 数据指针" DisplayMethod="无符号整数">
        <Structure Name="自动创建从 09763738" AutoFill="0" AutoCreate="1" DefaultHex="0" AutoDestroy="0" DoNotSaveLocal="0" RLECompression="1" AutoCreateStructsize="4096">
          <Elements>
            <Element Offset="0" Vartype="Unicode String" Bytesize="300" OffsetHex="00000000" Description="V3数据" DisplayMethod="无符号整数"/>
          </Elements>
        </Structure>
      </Element>
      <Element Offset="60" Vartype="4 Bytes" Bytesize="4" RLECount="2" OffsetHex="0000003C" Description="V3 数据 长度" DisplayMethod="无符号整数"/>
      <Element Offset="68" Vartype="4 Bytes" Bytesize="4" RLECount="2" OffsetHex="00000044" Description="置0" DisplayMethod="无符号整数"/>
      <Element Offset="76" Vartype="4 Bytes" Bytesize="4" OffsetHex="0000004C" Description="0" DisplayMethod="无符号整数"/>
      <Element Offset="80" Vartype="4 Bytes" Bytesize="4" OffsetHex="00000050" Description="未知 数据变动" DisplayMethod="十六进制"/>
      <Element Offset="84" Vartype="4 Bytes" Bytesize="4" OffsetHex="00000054" Description="0" DisplayMethod="无符号整数"/>
      <Element Offset="88" Vartype="Pointer" Bytesize="4" OffsetHex="00000058" Description="备注指针" DisplayMethod="无符号整数">
        <Structure Name="自动创建从 05750800" AutoFill="0" AutoCreate="1" DefaultHex="0" AutoDestroy="0" DoNotSaveLocal="0" RLECompression="1" AutoCreateStructsize="4096">
          <Elements>
            <Element Offset="0" Vartype="Unicode String" Bytesize="100" OffsetHex="00000000" Description="备注" DisplayMethod="无符号整数"/>
          </Elements>
        </Structure>
      </Element>
      <Element Offset="92" Vartype="4 Bytes" Bytesize="4" RLECount="2" OffsetHex="0000005C" Description="备注字符串大小" DisplayMethod="无符号整数"/>
      <Element Offset="100" Vartype="4 Bytes" Bytesize="4" RLECount="2" OffsetHex="00000064" Description="置0" DisplayMethod="无符号整数"/>
      <Element Offset="108" Vartype="Pointer" Bytesize="4" OffsetHex="0000006C" Description="昵称指针" DisplayMethod="无符号整数">
        <Structure Name="自动创建从 094F3118" AutoFill="0" AutoCreate="1" DefaultHex="0" AutoDestroy="0" DoNotSaveLocal="0" RLECompression="1" AutoCreateStructsize="4096">
          <Elements>
            <Element Offset="0" Vartype="Unicode String" Bytesize="100" OffsetHex="00000000" DisplayMethod="十六进制"/>
          </Elements>
        </Structure>
      </Element>
      <Element Offset="112" Vartype="4 Bytes" Bytesize="4" RLECount="2" OffsetHex="00000070" Description="昵称 长度" DisplayMethod="无符号整数"/>
      <Element Offset="120" Vartype="4 Bytes" Bytesize="4" RLECount="2" OffsetHex="00000078" Description="置0" DisplayMethod="无符号整数"/>
      <Element Offset="128" Vartype="4 Bytes" Bytesize="4" RLECount="11" OffsetHex="00000080" Description="0" DisplayMethod="无符号整数"/>
      <Element Offset="172" Vartype="Pointer" Bytesize="4" OffsetHex="000000AC" Description="昵称首拼大写" DisplayMethod="无符号整数">
        <Structure Name="自动创建从 094F3178" AutoFill="0" AutoCreate="1" DefaultHex="0" AutoDestroy="0" DoNotSaveLocal="0" RLECompression="1" AutoCreateStructsize="4096">
          <Elements>
            <Element Offset="0" Vartype="Unicode String" Bytesize="100" OffsetHex="00000000" Description="昵称首拼" DisplayMethod="十六进制"/>
          </Elements>
        </Structure>
      </Element>
      <Element Offset="176" Vartype="4 Bytes" Bytesize="4" RLECount="2" OffsetHex="000000B0" Description="昵称首拼 长度" DisplayMethod="无符号整数"/>
      <Element Offset="184" Vartype="4 Bytes" Bytesize="4" RLECount="2" OffsetHex="000000B8" Description="置0" DisplayMethod="无符号整数"/>
      <Element Offset="192" Vartype="Pointer" Bytesize="4" OffsetHex="000000C0" Description="昵称全拼小写" DisplayMethod="无符号整数">
        <Structure Name="自动创建从 0952F998" AutoFill="0" AutoCreate="1" DefaultHex="0" AutoDestroy="0" DoNotSaveLocal="0" RLECompression="1" AutoCreateStructsize="4096">
          <Elements>
            <Element Offset="0" Vartype="Unicode String" Bytesize="100" OffsetHex="00000000" DisplayMethod="无符号整数"/>
          </Elements>
        </Structure>
      </Element>
      <Element Offset="196" Vartype="4 Bytes" Bytesize="4" RLECount="2" OffsetHex="000000C4" Description="昵称全拼 size" DisplayMethod="无符号整数"/>
      <Element Offset="204" Vartype="4 Bytes" Bytesize="4" RLECount="2" OffsetHex="000000CC" Description="置0" DisplayMethod="无符号整数"/>
      <Element Offset="212" Vartype="Pointer" Bytesize="4" OffsetHex="000000D4" Description="旧备注信息 如果没有则为0" DisplayMethod="无符号整数">
        <Structure Name="自动创建从 094F3108" AutoFill="0" AutoCreate="1" DefaultHex="0" AutoDestroy="0" DoNotSaveLocal="0" RLECompression="1" AutoCreateStructsize="4096">
          <Elements>
            <Element Offset="0" Vartype="Unicode String" Bytesize="100" OffsetHex="00000000" DisplayMethod="十六进制"/>
          </Elements>
        </Structure>
      </Element>
      <Element Offset="216" Vartype="4 Bytes" Bytesize="4" RLECount="2" OffsetHex="000000D8" Description="size" DisplayMethod="无符号整数"/>
      <Element Offset="224" Vartype="4 Bytes" Bytesize="4" RLECount="2" OffsetHex="000000E0" Description="is 0" DisplayMethod="无符号整数"/>
      <Element Offset="232" Vartype="Pointer" Bytesize="4" OffsetHex="000000E8" Description="旧备注信息 如果没有则为0" DisplayMethod="无符号整数">
        <Structure Name="自动创建从 094F3188" AutoFill="0" AutoCreate="1" DefaultHex="0" AutoDestroy="0" DoNotSaveLocal="0" RLECompression="1" AutoCreateStructsize="4096">
          <Elements>
            <Element Offset="0" Vartype="Unicode String" Bytesize="100" OffsetHex="00000000" DisplayMethod="十六进制"/>
          </Elements>
        </Structure>
      </Element>
      <Element Offset="236" Vartype="4 Bytes" Bytesize="4" RLECount="2" OffsetHex="000000EC" Description="size" DisplayMethod="无符号整数"/>
      <Element Offset="244" Vartype="4 Bytes" Bytesize="4" RLECount="2" OffsetHex="000000F4" Description="is 0" DisplayMethod="无符号整数"/>
      <Element Offset="252" Vartype="Pointer" Bytesize="4" OffsetHex="000000FC" Description="高清头像链接" DisplayMethod="无符号整数">
        <Structure Name="自动创建从 052F6890" AutoFill="0" AutoCreate="1" DefaultHex="0" AutoDestroy="0" DoNotSaveLocal="0" RLECompression="1" AutoCreateStructsize="4096">
          <Elements>
            <Element Offset="0" Vartype="Unicode String" Bytesize="200" OffsetHex="00000000" DisplayMethod="无符号整数"/>
          </Elements>
        </Structure>
      </Element>
      <Element Offset="256" Vartype="4 Bytes" Bytesize="4" RLECount="2" OffsetHex="00000100" Description="size" DisplayMethod="无符号整数"/>
      <Element Offset="264" Vartype="4 Bytes" Bytesize="4" RLECount="2" OffsetHex="00000108" Description="is 0" DisplayMethod="无符号整数"/>
      <Element Offset="272" Vartype="Pointer" Bytesize="4" OffsetHex="00000110" Description="mini头像链接" DisplayMethod="无符号整数">
        <Structure Name="自动创建从 0545E8C0" AutoFill="0" AutoCreate="1" DefaultHex="0" AutoDestroy="0" DoNotSaveLocal="0" RLECompression="1" AutoCreateStructsize="4096">
          <Elements>
            <Element Offset="0" Vartype="Unicode String" Bytesize="200" OffsetHex="00000000" DisplayMethod="无符号整数"/>
          </Elements>
        </Structure>
      </Element>
      <Element Offset="276" Vartype="4 Bytes" Bytesize="4" RLECount="2" OffsetHex="00000114" Description="size" DisplayMethod="无符号整数"/>
      <Element Offset="284" Vartype="4 Bytes" Bytesize="4" RLECount="2" OffsetHex="0000011C" Description="is 0" DisplayMethod="无符号整数"/>
      <Element Offset="292" Vartype="Pointer" Bytesize="4" OffsetHex="00000124" Description="0/或者一个指针" DisplayMethod="无符号整数">
        <Structure Name="自动创建从 09550C28" AutoFill="0" AutoCreate="1" DefaultHex="0" AutoDestroy="0" DoNotSaveLocal="0" RLECompression="1" AutoCreateStructsize="4096">
          <Elements>
            <Element Offset="0" Vartype="4 Bytes" Bytesize="4" OffsetHex="00000000" DisplayMethod="十六进制"/>
          </Elements>
        </Structure>
      </Element>
      <Element Offset="296" Vartype="4 Bytes" Bytesize="4" RLECount="3" OffsetHex="00000128" Description="0" DisplayMethod="无符号整数"/>
      <Element Offset="308" Vartype="4 Bytes" Bytesize="4" OffsetHex="00000134" Description="0/32 变动" DisplayMethod="无符号整数"/>
      <Element Offset="312" Vartype="4 Bytes" Bytesize="4" OffsetHex="00000138" Description="F/2F" DisplayMethod="十六进制"/>
      <Element Offset="316" Vartype="4 Bytes" Bytesize="4" RLECount="2" OffsetHex="0000013C" Description="0" DisplayMethod="无符号整数"/>
      <Element Offset="324" Vartype="4 Bytes" Bytesize="4" RLECount="2" OffsetHex="00000144" Description="1" DisplayMethod="无符号整数"/>
      <Element Offset="332" Vartype="4 Bytes" Bytesize="4" OffsetHex="0000014C" Description="0" DisplayMethod="无符号整数"/>
      <Element Offset="336" Vartype="4 Bytes" Bytesize="4" OffsetHex="00000150" Description="句柄" DisplayMethod="十六进制"/>
      <Element Offset="340" Vartype="4 Bytes" Bytesize="4" OffsetHex="00000154" Description="681/645/721 变动" DisplayMethod="无符号整数"/>
      <Element Offset="344" Vartype="4 Bytes" Bytesize="4" RLECount="2" OffsetHex="00000158" Description="0" DisplayMethod="无符号整数"/>
      <Element Offset="352" Vartype="4 Bytes" Bytesize="4" OffsetHex="00000160" Description="1/3 变动" DisplayMethod="无符号整数"/>
      <Element Offset="356" Vartype="4 Bytes" Bytesize="4" OffsetHex="00000164" Description="1" DisplayMethod="无符号整数"/>
      <Element Offset="360" Vartype="4 Bytes" Bytesize="4" RLECount="6" OffsetHex="00000168" Description="0" DisplayMethod="无符号整数"/>
      <Element Offset="384" Vartype="4 Bytes" Bytesize="4" OffsetHex="00000180" Description="3/0 变动" DisplayMethod="无符号整数"/>
      <Element Offset="388" Vartype="4 Bytes" Bytesize="4" RLECount="5" OffsetHex="00000184" Description="0" DisplayMethod="无符号整数"/>
      <Element Offset="408" Vartype="4 Bytes" Bytesize="4" OffsetHex="00000198" Description="1" DisplayMethod="无符号整数"/>
      <Element Offset="412" Vartype="Pointer" Bytesize="4" OffsetHex="0000019C" Description="签名指针 该指针指向的内容附近有其他用户的信息" DisplayMethod="无符号整数">
        <Structure Name="自动创建从 08AB30D8" AutoFill="0" AutoCreate="1" DefaultHex="0" AutoDestroy="0" DoNotSaveLocal="0" RLECompression="1" AutoCreateStructsize="4096">
          <Elements>
            <Element Offset="0" Vartype="Unicode String" Bytesize="100" OffsetHex="00000000" DisplayMethod="无符号整数"/>
          </Elements>
        </Structure>
      </Element>
      <Element Offset="416" Vartype="4 Bytes" Bytesize="4" RLECount="2" OffsetHex="000001A0" Description="size" DisplayMethod="无符号整数"/>
      <Element Offset="424" Vartype="4 Bytes" Bytesize="4" RLECount="2" OffsetHex="000001A8" Description="is 0" DisplayMethod="无符号整数"/>
      <Element Offset="432" Vartype="Pointer" Bytesize="4" OffsetHex="000001B0" Description="国籍  CN=中国" DisplayMethod="无符号整数">
        <Structure Name="自动创建从 094F3128" AutoFill="0" AutoCreate="1" DefaultHex="0" AutoDestroy="0" DoNotSaveLocal="0" RLECompression="1" AutoCreateStructsize="4096">
          <Elements>
            <Element Offset="0" Vartype="Unicode String" Bytesize="10" OffsetHex="00000000" DisplayMethod="十六进制"/>
          </Elements>
        </Structure>
      </Element>
      <Element Offset="436" Vartype="4 Bytes" Bytesize="4" RLECount="2" OffsetHex="000001B4" Description="size" DisplayMethod="无符号整数"/>
      <Element Offset="444" Vartype="4 Bytes" Bytesize="4" RLECount="2" OffsetHex="000001BC" Description="null" DisplayMethod="无符号整数"/>
      <Element Offset="452" Vartype="Pointer" Bytesize="4" OffsetHex="000001C4" Description="地区" DisplayMethod="无符号整数">
        <Structure Name="自动创建从 0966DFD8" AutoFill="0" AutoCreate="1" DefaultHex="0" AutoDestroy="0" DoNotSaveLocal="0" RLECompression="1" AutoCreateStructsize="4096">
          <Elements>
            <Element Offset="0" Vartype="Unicode String" Bytesize="100" OffsetHex="00000000" DisplayMethod="无符号整数"/>
          </Elements>
        </Structure>
      </Element>
      <Element Offset="456" Vartype="4 Bytes" Bytesize="4" RLECount="2" OffsetHex="000001C8" Description="size" DisplayMethod="无符号整数"/>
      <Element Offset="464" Vartype="4 Bytes" Bytesize="4" RLECount="2" OffsetHex="000001D0" Description="null" DisplayMethod="无符号整数"/>
      <Element Offset="472" Vartype="Pointer" Bytesize="4" OffsetHex="000001D8" Description="城市" DisplayMethod="无符号整数">
        <Structure Name="自动创建从 096AAB70" AutoFill="0" AutoCreate="1" DefaultHex="0" AutoDestroy="0" DoNotSaveLocal="0" RLECompression="1" AutoCreateStructsize="4096">
          <Elements>
            <Element Offset="0" Vartype="Unicode String" Bytesize="100" OffsetHex="00000000" DisplayMethod="无符号整数"/>
          </Elements>
        </Structure>
      </Element>
      <Element Offset="476" Vartype="4 Bytes" Bytesize="4" RLECount="2" OffsetHex="000001DC" Description="size" DisplayMethod="无符号整数"/>
      <Element Offset="484" Vartype="4 Bytes" Bytesize="4" RLECount="2" OffsetHex="000001E4" Description="null" DisplayMethod="无符号整数"/>
      <Element Offset="492" Vartype="4 Bytes" Bytesize="4" OffsetHex="000001EC" Description="14/17" DisplayMethod="无符号整数"/>
      <Element Offset="496" Vartype="4 Bytes" Bytesize="4" RLECount="15" OffsetHex="000001F0" Description="0" DisplayMethod="无符号整数"/>
      <Element Offset="556" Vartype="4 Bytes" Bytesize="4" OffsetHex="0000022C" Description="给一个 -1" DisplayMethod="十六进制"/>
      <Element Offset="560" Vartype="4 Bytes" Bytesize="4" OffsetHex="00000230" Description="必须是3！ 或者 00800001" DisplayMethod="十六进制"/>
      <Element Offset="564" Vartype="4 Bytes" Bytesize="4" OffsetHex="00000234" Description="0" DisplayMethod="无符号整数"/>
      <Element Offset="568" Vartype="4 Bytes" Bytesize="4" OffsetHex="00000238" Description="1" DisplayMethod="无符号整数"/>
      <Element Offset="572" Vartype="4 Bytes" Bytesize="4" OffsetHex="0000023C" Description="8/3/0  变动" DisplayMethod="无符号整数"/>
      <Element Offset="576" Vartype="4 Bytes" Bytesize="4" RLECount="7" OffsetHex="00000240" Description="0" DisplayMethod="无符号整数"/>
      <Element Offset="604" Vartype="Pointer" Bytesize="4" OffsetHex="0000025C" Description="未知key 没有则为0" DisplayMethod="无符号整数">
        <Structure Name="自动创建从 0957FDD0" AutoFill="0" AutoCreate="1" DefaultHex="0" AutoDestroy="0" DoNotSaveLocal="0" RLECompression="1" AutoCreateStructsize="4096">
          <Elements>
            <Element Offset="0" Vartype="Unicode String" Bytesize="100" OffsetHex="00000000" DisplayMethod="无符号整数"/>
          </Elements>
        </Structure>
      </Element>
      <Element Offset="608" Vartype="4 Bytes" Bytesize="4" RLECount="2" OffsetHex="00000260" Description="size" DisplayMethod="无符号整数"/>
      <Element Offset="616" Vartype="4 Bytes" Bytesize="4" RLECount="2" OffsetHex="00000268" Description="null" DisplayMethod="无符号整数"/>
      <Element Offset="624" Vartype="4 Bytes" Bytesize="4" OffsetHex="00000270" Description="1" DisplayMethod="无符号整数"/>
      <Element Offset="628" Vartype="4 Bytes" Bytesize="4" OffsetHex="00000274" Description="0" DisplayMethod="无符号整数"/>
      <Element Offset="632" Vartype="4 Bytes" Bytesize="4" OffsetHex="00000278" Description="1" DisplayMethod="无符号整数"/>
      <Element Offset="636" Vartype="Pointer" Bytesize="4" OffsetHex="0000027C" Description="朋友圈背景图片URL" DisplayMethod="无符号整数">
        <Structure Name="自动创建从 03086940" AutoFill="0" AutoCreate="1" DefaultHex="0" AutoDestroy="0" DoNotSaveLocal="0" RLECompression="1" AutoCreateStructsize="4096">
          <Elements>
            <Element Offset="0" Vartype="Unicode String" Bytesize="200" OffsetHex="00000000" DisplayMethod="无符号整数"/>
          </Elements>
        </Structure>
      </Element>
      <Element Offset="640" Vartype="4 Bytes" Bytesize="4" RLECount="2" OffsetHex="00000280" Description="size" DisplayMethod="无符号整数"/>
      <Element Offset="648" Vartype="4 Bytes" Bytesize="4" RLECount="2" OffsetHex="00000288" Description="null ext" DisplayMethod="无符号整数"/>
      <Element Offset="656" Vartype="4 Bytes" Bytesize="4" OffsetHex="00000290" Description="作用未知 8DXXXXXXX" DisplayMethod="十六进制"/>
      <Element Offset="660" Vartype="4 Bytes" Bytesize="4" OffsetHex="00000294" Description="作用未知 BCXXXXXX" DisplayMethod="十六进制"/>
      <Element Offset="664" Vartype="4 Bytes" Bytesize="4" OffsetHex="00000298" Description="作用未知 00001C81" DisplayMethod="十六进制"/>
      <Element Offset="668" Vartype="4 Bytes" Bytesize="4" RLECount="36" OffsetHex="0000029C" Description="0" DisplayMethod="无符号整数"/>
      <Element Offset="812" Vartype="Pointer" Bytesize="4" OffsetHex="0000032C" Description="换行符" DisplayMethod="无符号整数">
        <Structure Name="自动创建从 094F3198" AutoFill="0" AutoCreate="1" DefaultHex="0" AutoDestroy="0" DoNotSaveLocal="0" RLECompression="1" AutoCreateStructsize="4096">
          <Elements>
            <Element Offset="0" Vartype="String" Bytesize="10" OffsetHex="00000000" DisplayMethod="无符号整数"/>
          </Elements>
        </Structure>
      </Element>
      <Element Offset="816" Vartype="4 Bytes" Bytesize="4" OffsetHex="00000330" Description="size" DisplayMethod="无符号整数"/>
      <Element Offset="820" Vartype="4 Bytes" Bytesize="4" OffsetHex="00000334" Description="null" DisplayMethod="无符号整数"/>
      <Element Offset="824" Vartype="4 Bytes" Bytesize="4" RLECount="27" OffsetHex="00000338" Description="0" DisplayMethod="无符号整数"/>
      <Element Offset="932" Vartype="4 Bytes" Bytesize="4" OffsetHex="000003A4" Description="1" DisplayMethod="无符号整数"/>
      <Element Offset="936" Vartype="4 Bytes" Bytesize="4" RLECount="16" OffsetHex="000003A8" Description="0" DisplayMethod="无符号整数"/>
    </Elements>
  </Structure>
</Structures>