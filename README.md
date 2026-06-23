# WeChat-Hook

Windows 微信 Hook DLL，当前版本以 `version.dll` 代理方式加载，在微信主进程中启动本地 HTTP 服务，提供消息发送、XML 转发、图片解码、个人资料读取和数据库查询接口。

### Hook 3.9.10.16 接口文档
- E语言版 https://www.showdoc.com.cn/WeChatProject/8929480485871668
- C++版 https://www.showdoc.com.cn/2598853379229303/11559060627222881

### Hook 4.1.10.27 接口文档
- https://www.showdoc.com.cn/PCWeixinHook/11559060626558382

### go 协议版 3.9.10.16
- https://www.showdoc.com.cn/go391016/11559060627219544

### c++ 协议版 3.9.10.16
- https://www.showdoc.com.cn/vx391016protocol/11559060626578602

## 交流群
- https://t.me/WeChat_Hook

## QQ 364831018

## 版本说明

- 目标微信版本：`4.1.10.27`
- 架构：Windows x64
- DLL 名称：`version.dll`
- 默认监听地址：`0.0.0.0:30001`
- 当前 main 分支已移除 VMP 保护、远程授权校验、PB/NetSceneSendPB、CDN、WcProbe/CCD/NtQuery 相关 Hook 安装与处理代码。

## 使用方式

1. 编译或从 Release 下载生成后的 `version.dll`。
2. 将 `version.dll` 放到微信安装目录下，我这里是：

```text
C:\Program Files\Tencent\Weixin
```

3. 启动微信。DLL 加载后会自动启动 HTTP 服务，默认端口是 `30001`。
4. 使用 Postman 或其他 HTTP 客户端调用接口，默认基地址：

```text
http://127.0.0.1:30001
```

5. Postman 可直接导入接口集合：

```text
postman/WeChat-Hook.postman_collection.json
```

## 项目结构

- `dllmain.cpp`：DLL 入口，解析启动参数，加载真实系统 `version.dll`，只在微信主进程初始化。
- `src/version_proxy.cpp`：代理系统 `version.dll` 导出函数。
- `src/inline_weixin_dll_load.cpp`：等待并初始化 `Weixin.dll` 相关逻辑，启动 HTTP 服务。
- `src/http_routes.cpp`：HTTP 路由注册入口。
- `src/SendTextMsg.cpp`：文本发送和图片解码接口。
- `src/SendImageMsg.cpp`：图片发送接口。
- `src/ForwardXMLMsg.cpp`：XML 消息转发接口。
- `src/GetSelfProfile.cpp`：当前账号资料接口。
- `src/QueryDB.cpp`、`xdb/`：微信进程内 SQLite 数据库查询接口。

## 编译

使用 Visual Studio / MSBuild 编译：

```powershell
MSBuild.exe x64_Version_dll.vcxproj /m /t:Build /p:Configuration=Release /p:Platform=x64
```

Release 输出：

```text
x64\Release\version.dll
```

Debug 输出：

```text
x64\Debug\version.dll
```

## 加载与启动参数

项目生成的 DLL 文件名为 `version.dll`。加载后会代理系统 `version.dll` 的导出函数，并在微信主进程内初始化 Hook 与 HTTP 服务。

支持的命令行参数：

| 参数 | 默认值 | 说明 |
| --- | --- | --- |
| `StartPort=` | `30001` | HTTP 服务监听端口 |
| `RecvType=` | `1` | 接收回调类型参数，当前保留解析 |
| `CallBackURL=` | 空 | 接收回调地址，当前用于保存配置和端口解析 |

示例：

```text
WeChat.exe StartPort=30001 CallBackURL="http://127.0.0.1:8080/callback"
```

## HTTP 接口

所有接口默认基地址：

```text
http://127.0.0.1:30001
```

### 发送文本消息

`POST /SendTextMsg`

请求：

```json
{
  "wxidorgid": "wxid_xxx",
  "msg": "hello"
}
```

响应：

```json
{
  "ret": 0,
  "retmsg": "success"
}
```

curl：

```bash
curl -X POST http://127.0.0.1:30001/SendTextMsg \
  -H "Content-Type: application/json" \
  -d "{\"wxidorgid\":\"wxid_xxx\",\"msg\":\"hello\"}"
```

### 发送图片消息

`POST /SendImgMsg`

请求：

```json
{
  "wxidorgid": "wxid_xxx",
  "path": "C:\\path\\image.jpg"
}
```

响应：

```json
{
  "ret": 0,
  "retmsg": "success"
}
```

### 转发 XML 消息

`POST /ForwardXMLMsg`

请求：

```json
{
  "to_wxid": "wxid_xxx",
  "content": "<msg>...</msg>"
}
```

响应：

```json
{
  "ret": 0,
  "retmsg": "success"
}
```

失败时：

```json
{
  "ret": 1,
  "retmsg": "fail"
}
```

### 解码图片

`POST /Decode_Pic`

请求：

```json
{
  "src_path": "C:\\path\\source.dat",
  "dst_path": "C:\\path\\output.jpg"
}
```

响应：

```json
{
  "ret": 0,
  "retmsg": "success"
}
```

### 获取当前账号资料

`POST /GetSelfProfile`

请求体可为空。

响应字段：

```json
{
  "wxid": "",
  "alias": "",
  "nickname": "",
  "email": "",
  "qq": 0,
  "phone": "",
  "proiv": "",
  "area": "",
  "signinfo": ""
}
```

### 查询数据库

`POST /QueryDB/execute`

请求：

```json
{
  "optDbName": "MicroMsg.db",
  "SQL": "SELECT * FROM ChatRoom LIMIT 10"
}
```

响应：

```json
{
  "status": 0,
  "desc": "",
  "data": []
}
```

### 获取数据库列表

`POST /QueryDB/GetAllDBName`

请求体需要是合法 JSON，可传空对象：

```json
{}
```

响应：

```json
[
  {
    "dbName": "MicroMsg.db",
    "dbHandle": 123456789
  }
]
```

### 查询运行状态

`GET /QueryDB/status`

响应：

```json
{
  "IsLogin": 1,
  "hWeixin": 123456789
}
```

## 返回约定

- JSON 解析失败时，接口返回：

```json
{
  "ret": -1,
  "msg": "invalid json"
}
```

- 数据库接口使用 `status` / `desc` / `data` 作为返回字段。
- 消息发送类接口使用 `ret` / `retmsg` 作为返回字段。

## 备注

本项目依赖微信内部偏移，微信升级后需要重新核对偏移和调用约定。当前 README 与 Release DLL 对应微信 `4.1.10.27`。
