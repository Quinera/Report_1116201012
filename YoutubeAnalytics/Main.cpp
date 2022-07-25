# include <Siv3D.hpp> // OpenSiv3D v0.6.4
#include <windows.h>
# include "YouTubeData.hpp"
# define BUFFSIZE 2048
# define API_KEY U"AIzaSyDwlanlD9Htug4vdVmChNmMqBz3tLAjLUA"  // 取得した API キー

void Main()
{
	char chStr[BUFFSIZE];
	DWORD dwLen = 0;
	GetPrivateProfileStringA("Data", "ChannelID", "0", chStr, BUFFSIZE - 1, ".\\Setting.ini");		//ChannelIDをchar変数に代入
	String CHANNEL_ID = Unicode::Widen(chStr);	 //SivのStringに変換
	YoutubeData ytdata(CHANNEL_ID, API_KEY);		 //YoutubeDataクラス作成

	Array<VideoItem> items;	//動画情報保存用配列
	ytdata.getNewItems(items); //動画情報取得
	if (ytdata.outputData("OutputData.csv", items)) //動画情報出力
	{
		Print << U"ファイル出力に成功しました。";
		Print << U"対象のチャンネルID:" << CHANNEL_ID;
		Print << U"終了するにはEscキーを押すか、ウィンドウを閉じてください...";
	}

	while (System::Update())
	{

	}
}
