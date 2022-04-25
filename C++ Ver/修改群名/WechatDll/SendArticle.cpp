#include "pch.h"

#define SendArticleCall1Offset 0x0F7454F0 - 0x0F6B0000
#define SendArticleCall2Offset 0x0FA41F80 - 0x0F6B0000
#define SendArticleCall3Offset 0x0F7794A0 - 0x0F6B0000
#define SendArticleCall4Offset 0x0FA42150 - 0x0F6B0000
#define SendArticleParamOffset 0x118EEC34 - 0x0F6B0000

#define SendArticleClearCacheCall1Offset 0x0FCEB4F0 - 0x0F6B0000
#define SendArticleClearCacheCall2Offset 0x0F744200 - 0x0F6B0000

void __stdcall SendArticle(wchar_t* wxid, wchar_t* title, wchar_t* description, wchar_t* url) {
	DWORD WeChatWinBase = GetWeChatWinBase();
	DWORD SendArticleCall1 = WeChatWinBase + SendArticleCall1Offset;
	DWORD SendArticleCall2 = WeChatWinBase + SendArticleCall2Offset;
	DWORD SendArticleCall3 = WeChatWinBase + SendArticleCall3Offset;
	DWORD SendArticleCall4 = WeChatWinBase + SendArticleCall4Offset;

	DWORD SendArticleParam = WeChatWinBase + SendArticleParamOffset;

	DWORD SendArticleClearCacheCall1 = WeChatWinBase + SendArticleClearCacheCall1Offset;
	DWORD SendArticleClearCacheCall2 = WeChatWinBase + SendArticleClearCacheCall2Offset;
	// 自己的wxid，发送者
	char* sselfwxid = (char*)(*(DWORD*)(WeChatWinBase + 0x222EB3C));
	wchar_t* wselfwxid = new wchar_t[strlen(sselfwxid) + 1];
	MultiByteToWideChar(CP_ACP, MB_COMPOSITE, sselfwxid, -1, wselfwxid, strlen(sselfwxid) + 1);
	// 构造xml数据
	wchar_t* xmlbuffer = new wchar_t[0x2000];
	ZeroMemory(xmlbuffer, 0x2000 * 2);
	swprintf_s(xmlbuffer, 0x2000, (wchar_t*)L"<msg>\n    <fromusername>%ws</fromusername>\n    <scene>0</scene>\n    <commenturl></commenturl>\n    <appmsg appid=\"\" sdkver=\"0\">\n        <title>%ws</title>\n        <des>%ws</des>\n        <action>view</action>\n        <type>5</type>\n        <showtype>0</showtype>\n        <content></content>\n        <url>%ws</url>\n        <dataurl></dataurl>\n        <lowurl></lowurl>\n        <lowdataurl></lowdataurl>\n        <recorditem>\n            <![CDATA[]]>\n        </recorditem>\n        <thumburl></thumburl>\n        <messageaction></messageaction>\n        <extinfo></extinfo>\n        <sourceusername></sourceusername>\n        <sourcedisplayname></sourcedisplayname>\n        <commenturl></commenturl>\n        <appattach>\n            <totallen>0</totallen>\n            <attachid></attachid>\n            <emoticonmd5></emoticonmd5>\n            <fileext></fileext>\n            <aeskey></aeskey>\n        </appattach>\n        <weappinfo>\n            <pagepath></pagepath>\n            <username></username>\n            <appid></appid>\n            <appservicetype>0</appservicetype>\n        </weappinfo>\n        <websearch />\n    </appmsg>\n    <appinfo>\n        <version>1</version>\n        <appname>Window wechat</appname>\n    </appinfo>\n</msg>",
		wselfwxid, title, description, url);

	DWORD sendtype = 0x5;
	WxBaseStruct pSender(wselfwxid);
	char nullbuffer[0x1C] = { 0 };
	WxBaseStruct pXml(xmlbuffer);
	WxBaseStruct pReceiver(wxid);
	WxString imgbuffer = { 0 };
	WxString nullStruct = { 0 };
	char buffer[0xF90] = { 0 };

	__asm {
		pushad;
		pushfd;
		lea ecx, buffer;
		call SendArticleCall1;
		mov eax, [sendtype];
		push eax;
		lea eax, nullbuffer;
		lea edx, pSender;
		push eax;
		lea eax, imgbuffer;
		push eax;
		lea eax, pXml;
		push eax;
		lea edi, pReceiver;
		push edi;
		lea ecx, buffer;
		call SendArticleCall2;
		add esp, 0x14;
		lea eax, nullStruct;
		push eax;
		lea ecx, buffer;
		call SendArticleCall3;
		mov dl, 0x0;
		lea ecx, buffer;
		push SendArticleParam;
		push SendArticleParam;
		call SendArticleCall4;
		add esp, 0x8;
		lea ecx, buffer;
		call SendArticleClearCacheCall1;
		lea ecx, buffer;
		call SendArticleClearCacheCall2;
		popfd;
		popad;
	}
	delete[] xmlbuffer;
	xmlbuffer = NULL;
	delete[] wselfwxid;
	wselfwxid = NULL;
}