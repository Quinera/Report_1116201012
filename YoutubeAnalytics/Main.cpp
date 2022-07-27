# include <Siv3D.hpp> // OpenSiv3D v0.6.4
# include <windows.h>	//GetPrivateProfileStringA用
# include "YouTubeData.hpp"
# define BUFFSIZE 2048	//char型配列のサイズ

void Main()	//メイン関数(main()の代替)
{
	char chStr[BUFFSIZE], apStr[BUFFSIZE];
	GetPrivateProfileStringA("Data", "ChannelID", "0", chStr, BUFFSIZE - 1, ".\\Setting.ini");		//iniファイルからChannelIDを取得
	GetPrivateProfileStringA("Data", "API_KEY", "0", apStr, BUFFSIZE - 1, ".\\Setting.ini");		//iniファイルからAPI_KEYを取得
	String CHANNEL_ID = Unicode::Widen(chStr);	 //SivのStringに変換
	String API_KEY = Unicode::Widen(apStr);	 //SivのStringに変換
	YoutubeData ytdata(CHANNEL_ID, API_KEY);		 //YoutubeDataクラスのインスタンスを作成

	Array<VideoItem> items;	//動画情報保存用配列
	Print << U"動画情報取得を開始します.";
	System::Update();
	ytdata.getNewItems(items); //動画情報取得
	if (ytdata.outputData("OutputData.csv", items)) //動画情報出力
	{
		Print << U"ファイル出力に成功しました.";
		Print << U"対象のチャンネルID:" << CHANNEL_ID;
	}
	Print << U"終了するにはEscキーを押すか、ウィンドウを閉じてください...";
	while (System::Update())	//画面表示
	{

	}
}
